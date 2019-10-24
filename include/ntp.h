#include <ESP8266WiFi.h>
#include <Task.h>
#include <WiFiUdp.h>

class NTP : public Task
{
public:
    NTP();
    void requestTime();
    bool checkTime();

    unsigned int localPort;
    int hoursOFGMT;

    int hours;
    int mins;
    int seconds;
    int mils;

    unsigned int state;

protected:
    virtual bool OnStart();
    virtual void OnUpdate(uint32_t deltaTime);
    void decodeEpoch(unsigned long currentTime);
    void sendNTPpacket(const char *host);

    unsigned long epoch;
    unsigned long lastNTP;

private:
};