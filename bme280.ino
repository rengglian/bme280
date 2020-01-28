#include <ESP8266WiFi.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#ifndef STASSID
#define STASSID "WHATEVER"
#define STAPSK  "BETTERNOTTOMENTIONHERE"
#endif

#define SEALEVELPRESSURE_HPA (1013.25)

#define DEEPSLEEP 5*60e6

ADC_MODE(ADC_VCC);

Adafruit_BME280 bme; // I2C

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "192.168.0.60";
const uint16_t port = 12345;

void setup()
{
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  unsigned status;
  
  status = bme.begin(0x76);  
   
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    ESP.deepSleep(DEEPSLEEP);
  }
  
  float temperature = 0.00f;
  float pressure = 0.00f;
  float altidute = 0.00f;
  float humidity = 0.00f;
  float voltage=0.00f;
  
  if (status)
  {
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 100.0F;
    altidute = bme.readAltitude(SEALEVELPRESSURE_HPA);
    humidity = bme.readHumidity();
  }
  voltage = ESP.getVcc();

  String json =String("{ \"Location\":\"Office\"," \
                      " \"Temperature\":" + String(temperature, 2) +  \
                      ", \"Pressure\":" + String(pressure, 2) +  \
                      ", \"Humidity\":" + String(humidity, 2) +  \
                      ", \"Altitude\":" + String(altidute, 2) +  \
                      ", \"Voltage\":" + String(voltage, 2) +  \
                      ", \"Status\":" + String(status, HEX) +  \
                      " }");
                      
  // This will send a string to the server
  if (client.connected()) {
    client.println(json);
  }   
  delay(500);
  // Close the connection
  client.stop();  
  ESP.deepSleep(DEEPSLEEP);
}

void loop() {

}