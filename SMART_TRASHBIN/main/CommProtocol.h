#ifndef COMMPROTOCOL_H
#define COMMPROTOCOL_H

#include <Arduino.h>
#include <WiFi.h>
#include "SmartBin.h"

class CommProtocol {
public:
    CommProtocol(const char* ssid, const char* pass, int port = 80);

    void begin();
    void handleClient(SmartBin &smartBin);

private:
    const char* ssid;
    const char* password;
    WiFiServer server;
};

#endif
