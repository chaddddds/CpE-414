#include "SmartBin.h"
#include <WiFi.h>

const char* ssid = "danieles";
const char* password = "daniel290312";

WiFiServer server(80);

// GAS SENSOR
const int GAS_LED_PIN = 14;
const int GAS_ANALOG_PIN = 34;

// IR COUNTER
const int IR_PIN = 4;
const int IR_BUTTON_PIN = 15;

// PROXIMITY 
const int SERVO_PIN = 13;
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

const int SHARED_BUZZER_PIN = 27;

SmartBin smartBin(
    GAS_LED_PIN,
    GAS_ANALOG_PIN,
    IR_PIN,
    IR_BUTTON_PIN,
    SHARED_BUZZER_PIN,
    SERVO_PIN,
    TRIG_PIN,
    ECHO_PIN
);

void setup() {
  Serial.begin(115200);
  smartBin.setup();
  
  Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("Web Server Started.");
}

void handleWebServer() {
    WiFiClient client = server.available();
    if (!client) return;

    unsigned long timeout = millis();
    while (!client.available()) {
        if (millis() - timeout > 20) return; 
    }

    String req = client.readStringUntil('\n');
    req.trim(); 

    String path = "";
    if (req.startsWith("GET ")) {
        int start = req.indexOf(' ') + 1;
        int end = req.indexOf(' ', start);
        path = req.substring(start, end);
    }

    if (path.indexOf("?mode=AUTO") != -1) {
        smartBin.setMode(SmartBin::AUTO);
    } else if (path.indexOf("?mode=MAINTENANCE") != -1) {
        smartBin.setMode(SmartBin::MAINTENANCE);
    } else if (path.indexOf("?mode=SLEEP") != -1) {
        smartBin.setMode(SmartBin::SLEEP);
    }

    if (smartBin.getMode() == SmartBin::MAINTENANCE) {
        if (path.indexOf("?cmd=openlid") != -1) {
            smartBin.proxLid.myServo.write(90);
            smartBin.lidState = "OPEN";
        } else if (path.indexOf("?cmd=closelid") != -1) {
            smartBin.proxLid.myServo.write(0);
            smartBin.lidState = "CLOSED";
        } else if (path.indexOf("?cmd=resetcounter") != -1) {
            smartBin.irCounter.resetAll();
        } else if (path.indexOf("?cmd=toggleled") != -1) {
            digitalWrite(smartBin.gasSensor.LED, !digitalRead(smartBin.gasSensor.LED));
        } else if (path.indexOf("?cmd=buzzertest") != -1) {
            digitalWrite(smartBin.buzzerPin, HIGH);
            delay(500);
            digitalWrite(smartBin.buzzerPin, LOW);
        }
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.print("<!DOCTYPE html><html><head>");
    client.print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    client.print("<title>SmartBin Dashboard</title>");
    client.print("<style>");
    client.print("body{font-family:Arial;text-align:center;padding:20px;}");
    client.print(".card{border:1px solid #ccc;border-radius:10px;padding:15px;margin:10px;}");
    client.print(".mode-btn{background:#4CAF50;color:white;border:none;padding:10px;margin:5px;border-radius:5px;}");
    client.print(".control-btn{background:#2196F3;color:white;border:none;padding:10px;margin:5px;border-radius:5px;}");
    client.print("</style>");

    if (smartBin.getMode() == SmartBin::AUTO) {
        client.print("<script>setTimeout(()=>location.reload(),2000);</script>");
    }

    client.print("</head><body>");

    client.print("<h2>SMART BIN DASHBOARD</h2>");

    client.print("<div>");
    client.print("<a href='/?mode=AUTO'><button class='mode-btn'>AUTO</button></a>");
    client.print("<a href='/?mode=MAINTENANCE'><button class='mode-btn'>MAINTENANCE</button></a>");
    client.print("<a href='/?mode=SLEEP'><button class='mode-btn'>SLEEP</button></a>");
    client.print("</div>");

    client.print("<h3>Current Mode: ");
    switch (smartBin.getMode()) {
        case SmartBin::AUTO: client.print("AUTO</h3>"); break;
        case SmartBin::MAINTENANCE: client.print("MAINTENANCE</h3>"); break;
        case SmartBin::SLEEP: client.print("SLEEP</h3>"); break;
    }

    if (smartBin.getMode() == SmartBin::AUTO || smartBin.getMode() == SmartBin::SLEEP) {
        client.print("<div class='card'><h3>Gas Sensor</h3>");
        client.print("Reading: <b>" + String(smartBin.gasSensor.value) + "</b><br>");
        client.print("Status: <b style='color:");
        client.print(smartBin.gasSensor.alert ? "red'>" : "green'>");
        client.print(smartBin.gasSensor.alert ? "GAS ALERT" : "Normal");
        client.print("</b></div>");

        client.print("<div class='card'><h3>Trash Counter</h3>");
        client.print("Count: <b>" + String(smartBin.irCounter.count) + "</b><br>");
        client.print("Bin Full: <b>" + String(smartBin.irCounter.binFull ? "YES" : "NO") + "</b>");
        client.print("</div>");

        client.print("<div class='card'><h3>Proximity Sensor</h3>");
        client.print("Distance: <b>" + String(smartBin.proxLid.lastDistance) + " cm</b><br>");
        client.print("Lid State: <b>" + smartBin.lidState + "</b>");
        client.print("</div>");

        client.print("<div class='card'><h3>Recommendations</h3>");
        if (smartBin.gasSensor.alert) {
            client.print("<b style='color:red;'>Ventilate the area immediately!</b><br>");
        }
        if (smartBin.irCounter.binFull) {
            client.print("<b style='color:orange;'>Empty the bin soon.</b><br>");
        }
        if (!smartBin.gasSensor.alert && !smartBin.irCounter.binFull) {
            client.print("<b style='color:green;'>All systems normal.</b>");
        }
        client.print("</div>");

        client.print("<div class='card'><h3>Recent Logs (Last 10)</h3>");
        for (int i = 0; i < 10; i++) {
            int idx = (smartBin.logIndex - 1 - i + 100) % 100; 
            if (smartBin.logs[idx].timestamp > 0) {
                client.print("Time: " + String(smartBin.logs[idx].timestamp / 1000) + "s, Gas: " + String(smartBin.logs[idx].gasValue) + ", Count: " + String(smartBin.logs[idx].count) + ", Dist: " + String(smartBin.logs[idx].distance) + ", Full: " + String(smartBin.logs[idx].binFull ? "Yes" : "No") + "<br>");
            }
        }
        client.print("</div>");
    } else if (smartBin.getMode() == SmartBin::MAINTENANCE) {
        client.print("<div class='card'><h3>Control Panel</h3>");
        client.print("<a href='/?cmd=openlid'><button class='control-btn'>Open Lid</button></a>");
        client.print("<a href='/?cmd=closelid'><button class='control-btn'>Close Lid</button></a>");
        client.print("<a href='/?cmd=resetcounter'><button class='control-btn'>Reset Counter</button></a>");
        client.print("<a href='/?cmd=toggleled'><button class='control-btn'>Toggle Gas LED</button></a>");
        client.print("<a href='/?cmd=buzzertest'><button class='control-btn'>Buzzer Test</button></a>");
        client.print("</div>");

        client.print("<div class='card'><h3>Current Readings</h3>");
        client.print("Gas: " + String(smartBin.gasSensor.value) + ", Count: " + String(smartBin.irCounter.count) + ", Distance: " + String(smartBin.proxLid.lastDistance) + " cm, Lid: " + smartBin.lidState);
        client.print("</div>");
    }

    client.print("</body></html>");
    client.stop();
}


void loop() {
    smartBin.loop();      
    handleWebServer();  
}


