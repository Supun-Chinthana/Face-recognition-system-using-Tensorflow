#include <LiquidCrystal_I2C.h>
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "Servo.h"


int button_pin = 16;
ESP8266WebServer server(80);
WiFiClient client;
LiquidCrystal_I2C display(0x27, 16, 2);

const char* ssid = "My_AP";

const char* laptop_IP_address = "192.168.4.2";
const char* esp32cam_IP_address = "192.168.4.1";

void setup() {
  Serial.begin(9600);
  pinMode(button_pin, INPUT);
  display.init();
  display.backlight();
  display.clear();


  WiFi.begin(ssid);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  Serial.print("network connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("Starting server at ");
  Serial.println(WiFi.localIP());
  display.print("IP = ");
  display.print(WiFi.localIP());
  delay(500);

  server.begin();


  server.on("/image_process_result", handle_results);
}


void loop() {
  server.handleClient();
  bool button_value = digitalRead(button_pin);
  if(button_value == HIGH){
    Serial.print(button_value);
  }
  

  if(button_value == HIGH)
  {
    Serial.println();

    display.clear();
    bool ok = client.connect(laptop_IP_address, 8080);
    if(!ok) {
        Serial.println("Connection to IP failed");

    }
    else {
      Serial.print("Connected to ");
      Serial.print(client.remoteIP());
      Serial.print(" at port ");
      Serial.println(client.remotePort());

      display.clear();
      display.setCursor(0, 0);
      display.print("Client connected to ");
      display.setCursor(0, 1);
      display.print(client.remoteIP());

      String request_data = "GET /capture_images HTTP/1.1";

      // Corrected HTTP Request Format
      Serial.println("Sending data");

      client.println(request_data);
      client.print("Host: ");
      client.println(laptop_IP_address);
      client.println("Connection: close");

      client.println();

      
      Serial.println("Data sent, awaiting response");
      display.clear();
      display.setCursor(0, 0);
      display.print("cap request sent to");
      display.setCursor(0, 1);
      display.print(client.remoteIP());

      // Await Response
      while(!client.available()) {
        delay(100);
        Serial.print(".");
      }

      Serial.println();
      String data = client.readString();
      Serial.println(data);

      display.clear();
      display.setCursor(0, 0);
      display.print("capture request approved...");

    }
  }
}



void handle_results()
{
  display.clear();

  String arg = server.arg("result");
  Serial.println(arg);

  if(arg == "1")
  {
    Serial.print("Face recognized. output = ");
    Serial.println(arg);

    display.clear();
    display.setCursor(0, 0);
    display.print("Face recognized");
    display.setCursor(0, 1);
    display.print("Access alowed");


  }

  else if(arg == "0")
  {
    Serial.print("Face NOT recognized. output = ");
    Serial.println(arg);

    display.clear();
    display.setCursor(0, 0);
    display.print("NOT recognized");
    display.setCursor(0, 1);
    display.print("Access denied");

  }

  else if(arg == "2")
  {
    Serial.println("Face not detected");
    display.print("Face not detected.");
    delay(400);
    display.scrollDisplayLeft();
  }


  else
  {
    Serial.println("invalid data");
  }
  delay(1500);
  display.clear();
  display.setCursor(0,0);
  display.print("Waiting for ");
  display.setCursor(0, 1);
  display.print("commands..");
}
