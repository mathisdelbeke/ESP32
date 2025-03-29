#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "x";
const char* password = "x";
const int potmeter_pin = 34;

// MQTT broker
const char* mqtt_server = "192.168.0.154";
const int mqtt_port = 1883;
const char* mqtt_topic = "PotentioMeter";

static unsigned long lastMsg = 0;

// WiFi and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(on_topic_update);
}

void setup_wifi() {
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);  // Connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected. IP address: " + WiFi.localIP().toString());
}

void on_topic_update(char* topic, byte* message, unsigned int length) {
  Serial.print("[");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Client")) {
      Serial.println("Connected.");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    String msg = String(analogRead(potmeter_pin));
    client.publish(mqtt_topic, msg.c_str());
    Serial.println("Message sent: " + msg);
  }
}
