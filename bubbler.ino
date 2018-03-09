#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

#define TOMILLIGAUSS 1953L
#define RELAY_PIN D4
#define MQTT_BASE   "FERMENTER/BUBBLER/"
#define DEVICE_NAME   "BUB4"

char MQTT_TOPIC[80];
char MQTT_LASTWILL_TOPIC[80];

int lastread=0;
AsyncMqttClient mqttClient;

void onMqttConnect(bool sessionPresent) {
  Serial.println("** Connected to the broker **");
//use if you want to send to esp device
}
void DoMeasurement()
{
  String mqttTOPIC = String(MQTT_BASE) + String(DEVICE_NAME);
  mqttTOPIC.toCharArray(MQTT_TOPIC,80);
// measure magnetic field
  int raw = analogRead(0);   // Range : 0..1024

//  Uncomment this to get a raw reading for calibration of no-field point
  //Serial.print("Raw reading: ");
  //Serial.println(raw);

//  long compensated = raw - NOFIELD;                 // adjust relative to no applied field 
  long gauss = raw * TOMILLIGAUSS / 1000;   // adjust scale to Gauss

  //Serial.print(gauss);
  //Serial.print(" Gauss ");

  if (gauss > lastread) lastread=gauss; //Serial.println("(South pole)");
  else if(gauss < lastread - 80) {
          Serial.println("onebubble");
          lastread=gauss;
          Serial.println();
          mqttClient.publish(MQTT_TOPIC, 1, false, "1");
  }
} 

void setup() {
  String mqttLASTWILL_TOPIC = String (MQTT_BASE);
  mqttLASTWILL_TOPIC.toCharArray(MQTT_LASTWILL_TOPIC,80);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin("SSID", "password");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" OK");
  mqttClient.setServer(IPAddress(192, 168, 0, 200), 1883);
  mqttClient.setKeepAlive(5).setCleanSession(false).setWill(MQTT_LASTWILL_TOPIC, 1, true, "something has gone wrong").setCredentials("username", "password").setClientId("m3");
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void loop() {
      delay(250);
    DoMeasurement();
}
