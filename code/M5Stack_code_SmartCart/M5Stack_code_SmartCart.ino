#include "M5Stack.h"
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define DHTTYPE DHT22
#define DHTPIN 5

//Update below fields as per current WiFi hotspot and MQTT config
const char* ssid = "";
const char* wpd = "";
const char* mqtt_server = "";
const char* mqtt_username = "";
const char* mqtt_password = "";

#define MQTT_TEMP_TOPIC "node1/dht/temperature"
#define MQTT_HUM_TOPIC "node1/dht/humidity"
#define MQTT_RFID_TOPIC "node1/dht/rfid"

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
PubSubClient psClient(wifiClient);

unsigned long lastMsg = 0;
int line_count = 0;
int lastX = 0;
int lastY = 0;

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();

void setup() {
   M5.begin();
   M5.Power.begin();
   Serial.begin(115200);

   dht.begin();

   setupWifi();

   psClient.setServer(mqtt_server, 1883);
   psClient.setCallback(callback);
}

void loop() {
  if(!psClient.connected()) {
    reConnect();
  }
  psClient.loop();

  unsigned long now = millis();
  if(now - lastMsg > 10000) {
    lastMsg = now;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    psClient.publish(MQTT_TEMP_TOPIC, String(temperature).c_str());
    psClient.publish(MQTT_HUM_TOPIC, String(humidity).c_str());

//    M5.Lcd.clear();
    M5.Lcd.fillRect(0, 0, 320, 48, WHITE);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextSize(2);
    
    if(temperature >= 20) {
      M5.Lcd.setTextColor(RED);
    } else {
      M5.Lcd.setTextColor(BLUE);
    }
    
    M5.Lcd.printf("ICE BOX STATE at %d\nTemperature:%3.2f\nHumidity:%3.2f\n", now, temperature, humidity);

    psClient.subscribe(MQTT_RFID_TOPIC);
  }
}

void setupWifi() {
  delay(10);
  M5.Lcd.printf("Connecting to %s", ssid);
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wpd);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
    Serial.print(".");
  }

  M5.Lcd.printf("\nSuccess. WiFi Connected.\n");
  Serial.print("WiFi Connected. IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(lastX != 0 or lastY != 0) {
    M5.Lcd.setCursor(lastX, lastY);
  }
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);

  M5.Lcd.print("Added to cart: ");
  int uidS = 0;
  for(int i = 0, chr = 0; i < length && chr < 4; i++) {
    
    if((char) payload[i] == '"') {
      chr++;
    }
    else if(chr == 3) {
      M5.Lcd.print((char) payload[i]);
    }
  }
  M5.Lcd.println();
  
  lastX = M5.Lcd.getCursorX();
  lastY = M5.Lcd.getCursorY();
}

void reConnect() {
  while(!psClient.connected()) {
    M5.Lcd.print("Attempting MQTT connection...");

    String clientId = "M5Stack-";
    clientId += String(random(0xffff), HEX);

    if(psClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      M5.Lcd.printf("\nSuccess\n");
      psClient.publish("M5Stack", "hello world");
      psClient.subscribe("M5Stack");
    }
    else {
      M5.Lcd.print("failed, rc=");
      M5.Lcd.print(psClient.state());
      M5.Lcd.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
