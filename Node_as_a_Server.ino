#include "ArduinoJson.h"

#include "DHT.h"
// 63104632 ธีรภูมิ คูศิริวานิชกร
// 63118343 จิรวัฒน์ ชาตรี
#include "Wire.h"

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "LiquidCrystal_I2C.h"

const char *ssid = "All_for_one";
const char *pass = "ggwp1234";
#define LEDPIN D6
#define BRIGHTPIN A0
#define SERVER_PORT 80
#define BYTE_LENGTH 1024

ESP8266WebServer server(SERVER_PORT);
DynamicJsonDocument doc(BYTE_LENGTH);
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht;

void setup()
{
    Serial.begin(115200);
    lcd.begin();
    lcd.backlight();
    wifiConnect();
    server.on("/", root);
    server.on("/api", handleGetData);
    server.on("/led_on", handleGetLedOn);
    server.on("/led_off", handleGetLedOff);
    server.begin();
    dht.setup(D4);
    pinMode(LEDPIN, OUTPUT);
}

void loop()
{
    server.handleClient();
}

void wifiConnect()
{
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting... ");
        Serial.printf("Connection Status: %d\n", WiFi.status());
        delay(1000);
    }

    Serial.print("Wi-Fi connected.");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 0);
    lcd.print(WiFi.localIP());
}

void root()
{
    static String responseHTML = F("<!doctype html>"
                                   "<html>"
                                   "<head>"
                                   "<title>Hello IoT</title>"
                                   "</head>"
                                   "<body>"
                                   "<h1>HELLO IOT WORLD</h1>"
                                   "</body>"
                                   "</html>");
    server.send(200, "text/html", responseHTML.c_str());
}

void fixCore()
{
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
}

void handleGetData()
{
    String output;
    int led_status = digitalRead(LEDPIN);
    doc["BRIGHT_RANGE"] = analogRead(BRIGHTPIN);
    doc["DHT_TEMP"] = dht.getTemperature();
    doc["DHT_HUMIDITY"] = dht.getHumidity();
    if (led_status == 1)
    {
        doc["LED_STATUS"] = "ON";
    }
    else
    {
        doc["LED_STATUS"] = "OFF";
    }
    serializeJson(doc, output);
    fixCore();
    server.send(200, "application/json", output);
    lcd.setCursor(0, 1);
    lcd.print("PATH: /api    ");
}

void handleGetLedOn()
{
    digitalWrite(LEDPIN, HIGH);
    fixCore();
    server.send(200, "application/json", "");
    lcd.setCursor(0, 1);
    lcd.print("PATH: /led_on ");
}

void handleGetLedOff()
{
    digitalWrite(LEDPIN, LOW);
    fixCore();
    server.send(200, "application/json", "");
    lcd.setCursor(0, 1);
    lcd.print("PATH: /led_off");
}
