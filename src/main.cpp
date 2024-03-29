#define TASK_MICRO_RESOLUTION
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Stream.h>
#include <WiFiManager.h>
#include <Task.h>
// #include "ntp.h"
#include <ezTime.h>

const int SER = 0;
const int CLK = 5;
const int RCLK = 16;
const int OUTPUT_ENABLE = 4;
const int debug = 1;
const int CLK_DELAY = 20 * debug;       // in microsecnds;
const int SHOW_DELAY = CLK_DELAY * 100; // in Micros
const int DIGIT_1 = 14;
const int DIGIT_2 = 12;
const int DIGIT_3 = 13;
const int DIGIT_4 = 10;
const int LED = 2;

const int A = 1 << 6;
const int B = 1 << 5;
const int C = 1 << 4;
const int D = 1 << 3;
const int E = 1 << 2;
const int F = 1 << 1;
const int G = 1 << 0;
const int DP = 1 << 7;

const int digitOff = HIGH;
const int digitOn = LOW;

const int ledOff = HIGH;
const int ledOn = LOW;

bool twelveHourMode = true;

TaskManager taskManager;
Timezone myTz;


// TODO: PROGMEM? pgm_read_word_near
const int digits[10] = {
    A | B | C | D | E | F,
    B | C,
    A | B | G | E | D,
    B | C | A | G | D,
    B | C | F | G,
    A | F | G | C | D,
    A | F | E | G | D | C,
    A | B | C,
    A | B | C | D | E | F | G,
    A | B | C | G | F};

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(2000);
  pinMode(SER, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(OUTPUT_ENABLE, OUTPUT);
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledOff);

  digitalWrite(CLK, LOW);
  digitalWrite(RCLK, LOW);
  digitalWrite(SER, LOW);
  digitalWrite(OUTPUT_ENABLE, HIGH);

  digitalWrite(DIGIT_1, digitOff);
  digitalWrite(DIGIT_2, digitOff);
  digitalWrite(DIGIT_3, digitOff);
  digitalWrite(DIGIT_4, digitOff);

  Serial.println(F("Starting..."));

  WiFiManager wifiManager;
  String ssid = "aw-clock-" + ESP.getChipId();
  wifiManager.autoConnect(ssid.c_str(), "xxx1234");

  setDebug(INFO);
  waitForSync();

  // Set timezone
  myTz.setLocation("America/New_York");

  // taskManager.StartTask(&ntp);
}

void clockTick(int clk, int rclk)
{
  digitalWrite(CLK, clk);
  digitalWrite(RCLK, rclk);
  delayMicroseconds(CLK_DELAY);
}

void sendDigit(const int num, const bool dp, const int pin)
{
  // clockTick(LOW, LOW);
  digitalWrite(OUTPUT_ENABLE, HIGH);
  // clockTick(HIGH, HIGH);

  int segments = digits[num];

  clockTick(LOW, LOW);
  digitalWrite(SER, dp ? HIGH : LOW);
  clockTick(HIGH, HIGH);

  for (int i = 0; i < 7; i++)
  {
    int mask = 1 << i;
    int bit = segments & mask ? HIGH : LOW;
    clockTick(LOW, LOW);
    digitalWrite(SER, bit);
    clockTick(HIGH, HIGH);
  }

  clockTick(LOW, LOW);

  digitalWrite(OUTPUT_ENABLE, LOW);
  digitalWrite(pin, digitOn);
  for (int i = 0; i < 50; i++)
  {
    clockTick(HIGH, LOW);
    clockTick(LOW, LOW);
  }
  // delayMicroseconds(SHOW_DELAY - CLK_DELAY);
  digitalWrite(pin, digitOff);
  digitalWrite(OUTPUT_ENABLE, HIGH);
}

void sendNumber(const int fullNum)
{
  int seconds = myTz.second();
  bool showTHMdot = twelveHourMode ? fullNum >= 1200 : false;
  int twhour = twelveHourMode && fullNum >= 1300 ? fullNum % 1200 : fullNum;

  int showZero = 0;
  int tmp = (twhour / 1000) % 10;
  if (showZero || tmp > 0)
  {
    showZero = 1;
    sendDigit(tmp, false,  DIGIT_4);
  }

  tmp = (twhour / 100) % 10;
  if (showZero || tmp > 0)
  {
    showZero = 1;
    sendDigit(tmp, showTHMdot, DIGIT_3);
  }

  tmp = (fullNum / 10) % 10;
  if (showZero || tmp > 0)
  {
    showZero = 1;
    sendDigit(tmp, false, DIGIT_2);
  }

  tmp = (fullNum / 1) % 10;
  if (tmp >= 0)
  {
    showZero = 1;
    sendDigit(tmp, seconds % 2 == 1, DIGIT_1);
  }
}

void loop()
{
  taskManager.Loop();
  events();
  digitalWrite(LED, myTz.second() % 2 == 1 ? ledOn : ledOff);

  int count = myTz.hour() * 100 + myTz.minute();

  sendNumber(count);

  
}