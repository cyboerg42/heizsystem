#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "max6675.h"

// Pins for SPI | MAX6675

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 14;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// WiFi Credentials

#ifndef STASSID
#define STASSID "WIFI-SSID"
#define STAPSK  "WIFI-PASS"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Pin for 1-Wire | DS18B20

OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer;

// Internaly used

int deviceCount = 0;
float tempC;
int counter = 0;

// Port

ESP8266WebServer server(80);

// LED Pin

const int led = 13;

// Function needed for glitching OneWire, reboot when device count changes.
void checkDeviceCount() {
  if (deviceCount != sensors.getDeviceCount()) {
     ESP.restart();
  }
}

// OneWire DeviceAddress to String
String printAddress(DeviceAddress deviceAddress)
{ 
  String address = "0x";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 0x10) address = address + "0";
    address = address + String(deviceAddress[i], HEX);
  }
  Serial.println("");
  return address;
}

// DS18B20
void handle18b20() {
  String text = "";
  
  digitalWrite(led, 1);
  counter++;
  sensors.requestTemperatures();
   
  for (int i = 0;  i < deviceCount;  i++)
  {
    tempC = sensors.getTempCByIndex(i);
    if (tempC == -127) ESP.restart();
    sensors.getAddress(Thermometer, i);
    text = text + printAddress(Thermometer) + " " + String(tempC) + "\n";
  }
  if (counter > 10) {
    checkDeviceCount();
    counter = 0;
  }
  server.send(200, "text/plain", text);
  digitalWrite(led, 0);
}

// Thermocouple via MAX6675
void handle6675() {
  String readout = String(thermocouple.readCelsius());
  if (readout != "nan") {
    server.send(200, "text/plain", String(thermocouple.readCelsius()));
    delay(500);
  } else {
    server.send(200, "text/plain", "");
  }
}

// Bin | 404
void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

// Remote Reboot
void reboot() {
  server.send(200, "text/plain", "bye bye...");
  delay(250);
  ESP.restart();
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  sensors.begin();
  deviceCount = sensors.getDeviceCount();
  Serial.println(deviceCount);
  WiFi.mode(WIFI_STA);
  WiFi.setOutputPower(10); 
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/ds18b20", handle18b20);
  server.on("/reboot", reboot);
  server.on("/max6675", handle6675);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
