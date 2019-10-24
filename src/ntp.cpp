#include "ntp.h"
const char *host = "time.nist.gov";

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;


NTP::NTP(): Task(MsToTaskTime(20)) {
    epoch = 0;
    lastNTP = 0;
    state = 0;
    localPort = 2390;
    hoursOFGMT = -4;
}

bool NTP::OnStart() {
    if (WiFi.status() == WL_CONNECTED)
    {
        // Assume connected to network.
        udp.begin(localPort);
        requestTime();
        return true;
    }
    // Not connected to wifi. fail task
    return false;
}

void NTP::requestTime()
{
    if (state == 0)
    {
        sendNTPpacket(host);
    }
}

void NTP::sendNTPpacket(const char *host)
{
    state = 1;
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    packetBuffer[0] = 0b11100011; // LI Version, Mode
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;    // Polling Interval
    packetBuffer[3] = 0xEC; // Peer clock precision
    // 8 byter os zeo for root delay & root dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    udp.beginPacket(host, 123);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
    Serial.println(F("(Sending packet to NTP"));
}

bool NTP::checkTime()
{
    if (state != 1) {
        return false;
    }
    int cb = udp.parsePacket();
    if (cb)
    {
        Serial.printf("Udp packet bytes avail %d\n", cb);
        udp.read(packetBuffer, NTP_PACKET_SIZE);
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        Serial.printf("Got Packet %u %lx %lx\n", packetBuffer[0], highWord, lowWord);
        const unsigned long seventyYears = 2208988800UL;
        epoch = secsSince1900 - seventyYears;
        lastNTP = millis();
        state = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void NTP::decodeEpoch(unsigned long currentTime) {
    // update for local zone;
    currentTime = currentTime + (hoursOFGMT * 60 * 60);
    hours = (currentTime % 86400L) / 3600;
    mins = (currentTime % 3600) / 60;
    seconds = currentTime % 60;
    mils = 0;
}

void NTP::OnUpdate(uint32_t deltaTime) {
    unsigned long delta = millis() - lastNTP;
    decodeEpoch(epoch + delta/1000);
    mils = delta % 1000;
    if (state == 0 && delta > 100000L) {
        requestTime();
    } else if (state == 1) {
        bool updated = checkTime();
        if (updated) {
            decodeEpoch(epoch);
        } else if ( delta > 1000000L) {
            Serial.println(F("UDP Packet timedout. Reset state."));
            state = 0;
        }
    }
}
