/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// NOMAGE MQTT
#define ModuleName "Light"
#define TopicLx "Light/1/lux"
#define ESP8266Client ModuleName

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
float lastValue = 0;
const char* ssid = "WifiName";   // replace with the SSID of your WiFi
const char* password = "Password";  // replace with your WiFi password
const char* host = "IP ou Host MQTT Broker";       // IP address or name of your MQTT broker


void setup() {
  Serial.begin(115200);

  pinMode(A0, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Wifi connection");
  while (WiFi.status() != WL_CONNECTED) {
    //digitalWrite(1, !digitalRead(1));
    Serial.print(".");
    delay(500);
  } 
  client.setServer(host, 1883);
  Serial.println(". Started !");
}

void loop() {
  int attempts = 0;
  char ch_status[1];
  float ratio;
  int lxValue;


  // Read Analog Value
  lxValue = analogRead(A0);
  
  // Check if any reads failed and exit early (to try again).
  while (isnan(lxValue)) {
    Serial.print(".");
    delay(1000);
    lxValue = analogRead(A0);
  }
  ratio = (float)lxValue / 8;

  if (ratio >= (lastValue * 1.10) || ratio <= (lastValue * 0.90)) {
    Serial.println("Value Trigger. Attempt to publish");
    // Loop until we're reconnected
    while (!client.connected() && attempts < 5) {
      Serial.println("Attempt to connect");
      if (client.connect(ESP8266Client)) {
        Serial.println("Once connected, publish an announcement...");
        sprintf(ch_status, "%.2f", ratio);
        Serial.print("Publish :");
        Serial.println(ch_status);
        Serial.println(client.publish(TopicLx, ch_status));
        lastValue = ratio;
      } else {
        Serial.println("Wait 5 seconds before retrying");
        //digitalWrite(1, !digitalRead(1));
        attempts++;
        delay(5000);
      }
    }
  }
  delay(3000);
}
