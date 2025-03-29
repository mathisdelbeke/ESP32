#include <WiFi.h>
#include <WebServer.h>  

// Replace with your network credentials
const char* ssid = "x";
const char* password = "x";

WebServer server(80);  // Create a web server on port 80

int ledPin = 2;  // LED connected to GPIO2 (onboard LED for most ESP32 boards)

void setup() {
  // Start the Serial monitor for debugging
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print the IP address of the ESP32
  
  // Set LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Start with the LED off

  // Define a simple HTTP route (this will serve a webpage)
  server.on("/", HTTP_GET, []() {
    String html = "<html><body>";
    html += "<h1>ESP32 Web Server</h1>";
    html += "<p>LED is ";
    html += (digitalRead(ledPin) == HIGH) ? "ON" : "OFF";
    html += "</p>";
    html += "<a href='/toggle'>Toggle LED</a>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // Define a route to toggle the LED state
  server.on("/toggle", HTTP_GET, []() {
    int ledState = digitalRead(ledPin);  // Read current LED state
    digitalWrite(ledPin, (ledState == LOW) ? HIGH : LOW);  // Toggle LED state
    
    // Redirect back to the main page
    server.sendHeader("Location", "/");
    server.send(303);  // HTTP status code for redirect
  });

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();  // Check if there are incoming client requests
}
