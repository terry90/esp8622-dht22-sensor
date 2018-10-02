#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "conf.h"
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  WiFi.persistent(false);
  WiFi.begin(SSID, PASS);
  //IPAddress ip(192,168,1,200);   
  //IPAddress gateway(192,168,1,254);
  //IPAddress subnet(255,255,255,0);
  //WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);

  Serial.println("Waiting for connection");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  send_data();
  delay(2000);
  ESP.deepSleep(SLEEP_TIME); 
}

void loop() {}
 
void send_data()
{
 float h = dht.readHumidity();
 float t = dht.readTemperature();
 
 if (isnan(h) || isnan(t))
 {
   Serial.println("Failed to read from DHT sensor!");
   return;
 }
 
 float hi = dht.computeHeatIndex(t, h, false);
 
 Serial.print("Humidity: ");
 Serial.print(h);
 Serial.print(" %\t");
 Serial.print("Temp: ");
 Serial.print(t);
 Serial.print(" *C\t");
 Serial.print("Heat index: ");
 Serial.print(hi);
 Serial.println(" *C\n");

  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& data = jsonBuffer.createObject();

  data["thing_name"] = SENSOR_NAME;
  data["thing_type"] = "temp_sensor";

  char tmpBuf[150];
  sprintf(tmpBuf, "{\"temperature\":%f,\"humidity\":%f,\"heat_index\":%f}",t,h,hi);
  data["value"] = tmpBuf;
  data.printTo(Serial);
  Serial.println();
  
  String jsonStr;
  data.printTo(jsonStr);

  post_data(jsonStr);
}

void post_data(String data) {
 HTTPClient http;
 
 http.begin(URL);
 http.addHeader("Content-Type", "application/json");
 
 int httpCode = http.POST(data);
 String payload = http.getString();

 Serial.println("Response Code");
 Serial.println(httpCode);
 Serial.println(payload);
 
 http.end();
}

