


// wifi related libraries

#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

// blynk related libraries

#include <BlynkSimpleEsp32.h>

// global variables

#define analog_pin_thermo_resistor 4 
#define resolution_analoge_port 4090 
#define voltage_analoge_port 3.3
#define resistance 10 //the value of the pull-down resistor

// wifi configuration

uint64_t chipid;
char ssid[] = "IoT-Projekt";     //  your network SSID (name)
char pass[] = "0112358132134";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// blynk configuration

char auth[] = "6b2a34e0fa234448b6c946b3d7e2529d";

// thingspeak configuration

char server[] = "api.thingspeak.com";
String writeAPIKey = "2XK2T7DU1SYEK0ZU";

// setup

void setup() {
  Serial.begin(9600);
  listNetworks();
   while (status != WL_CONNECTED) {
    tryToConnectToWifi();
  }
 printWifiData();
 Blynk.begin(auth, ssid, pass);
}

int measurementNumber = 1;
void loop() {
  while (status != WL_CONNECTED) {
       tryToConnectToWifi();
  }

 Blynk.run();

 //read thermo resistor value 
 // int reading_temp_sensor  = analogRead(analog_pin_thermo_resistor);
  //the calculating formula of temperature
 // float tempC = ((((reading_temp_sensor * voltage_analog_port) / resolution_analog_port) - 0.5) * 10);
 int tempC = 21.0;
 Blynk.virtualWrite(V0, tempC);
 httpRequest(tempC, measurementNumber);
  delay(200); //wait for 100 milliseconds
 measurementNumber++;
  
}

void tryToConnectToWifi() {
  Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    //printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void httpRequest(float field1Data, int field2Data) {

    WiFiClient client;
    
    if (!client.connect(server, 80)){
      
        Serial.println("connection failed");
        client.stop();
        return;     
    }
    
    else{
        
        // create data string to send to ThingSpeak
        String data = "field1=" + String(field1Data) + "&field2=" + String(field2Data); //shows how to include additional field data in http post
        
        // POST data to ThingSpeak
        if (client.connect(server, 80)) {
          
            client.println("POST /update HTTP/1.1");
            client.println("Host: api.thingspeak.com");
            client.println("Connection: close");
            client.println("User-Agent: ESP32WiFi/1.1");
            client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
            client.println("Content-Type: application/x-www-form-urlencoded");
            client.print("Content-Length: ");
            client.print(data.length());
            client.print("\n\n");
            client.print(data);
            
            Serial.println("RSSI = " + String(field1Data));
        }
    }
    client.stop();
}
