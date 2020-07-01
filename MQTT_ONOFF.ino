
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Define NodeMCU D3 
#define PIN D3

//  network.
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
    delay(100);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Command from MQTT: [");
  Serial.print(topic);
  int p =(char)payload[0]-'0';
  // if MQTT comes a 0 turn off 
  if(p==0) 
  {
    digitalWrite(PIN, LOW); 
    Serial.println(" Turn Off" );
    client.publish("esp/greenhouse/ONOF/back", "Off");

  } 
  // if MQTT comes a 1, turn on 
  if(p==1)
  {
     digitalWrite(PIN, HIGH); 
    Serial.println(" Turn On" );
    client.publish("esp/greenhouse/ONOF/back", "On");

  }
  Serial.println();
} 

void reconnect() {
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    //if MQTT broker has clientID,username and password
    //if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("esp/greenhouse/ONOF");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(6000);
    }
  }
} 

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
    pinMode(PIN, OUTPUT);
 digitalWrite(PIN, LOW);   
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
