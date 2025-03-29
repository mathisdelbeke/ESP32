#include <WiFi.h>
#include <WebServer.h>  

// Replace with your network credentials
const char* ssid = "x";
const char* password = "x";
const int ledPin = 2;

WebServer server(80);  // Create a web server on port 80

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  init_wifi();
  init_webserver();
}

void init_wifi() {
  Serial.print("Connecting to WiFi");
  
  WiFi.begin(ssid, password);  // Connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 
}

void init_webserver() {
  server.on("/", HTTP_GET, []() {
    String html = "<html><body>";
    html += "<h1>ESP32 Web Server</h1>";
    html += "<p>LED is currently ";
    html += (digitalRead(ledPin) == HIGH) ? "ON" : "OFF";
    html += "</p>";
    html += "<form method='get' action='/setled'>";
    html += "<input type='radio' id='on' name='led' value='on' " + String(digitalRead(ledPin) == HIGH ? "checked" : "") + ">";
    html += "<label for='on'>Turn LED On</label><br>";
    html += "<input type='radio' id='off' name='led' value='off' " + String(digitalRead(ledPin) == LOW ? "checked" : "") + ">";
    html += "<label for='off'>Turn LED Off</label><br>";
    html += "<input type='submit' value='Submit'>";
    html += "</form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/setled", HTTP_GET, []() {
    String ledState = server.arg("led");
    
    if (ledState == "on") {
      digitalWrite(ledPin, HIGH); 
    } else if (ledState == "off") {
      digitalWrite(ledPin, LOW);  
    }
    
    server.sendHeader("Location", "/");  // Redirect to main page
    server.send(303);                    // HTTP status for redirect
  });
  server.begin();  // Start the server
}

void loop() {
  // Check if there are incoming client requests
  server.handleClient(); 
}
