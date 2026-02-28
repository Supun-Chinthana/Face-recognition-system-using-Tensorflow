#include "esp32cam.h"
#include "WiFi.h"
#include "WebServer.h"
#include "ESP32_Servo.h"
#include "esp_camera.h"


const char* ssid = "My_AP";
int flash_pin = 4;

WebServer server(80);


void setup() {
  Serial.begin(9600);
  pinMode(4,OUTPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  server.begin();
  Serial.println("Network Setup completed");
  Serial.println(WiFi.softAPIP());

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);
    cfg.setResolution(Resolution::find(1500,1500));

    //cam innit
    if(!Camera.begin(cfg))
    {
      Serial.println("Camera initiation failed");
    }
  }

  sensor_t *s = esp_camera_sensor_get();
  if(s)
  {
    s->set_saturation(s, -2);
    s->set_brightness(s, 1.5);
    s->set_sharpness(s, 2);

  } 

  server.on("/capture", handle_root);
}

void handle_root()
{
  digitalWrite(4, HIGH);
  delay(300);
  auto img = esp32cam::capture();
  if(img == NULL)
  {
    Serial.println("capture failed");
    return;
  }
  digitalWrite(4,LOW);
  
  server.setContentLength(img->size());
  server.send(200, "image/jpeg");

  WiFiClient client = server.client();
  img->writeTo(client);
}

  // put your setup code here, to run once

void loop() {
  server.handleClient();
}
