
// Board is ESP32-Dev module
#include <WiFi.h>
#include <WebServer.h>

#include "Board.h"
#include "Constants.h"
#include "WEBPAGE.h"

Board board;
WebServer server(80);
String message;

void handleRoot()
{
  server.send(200, "text/html", index_html); //Send web page
  message = board.rescan();
}


void webpageUpdate()
{
  message += board.servos_.updateReadings();
  server.send(200, "text/plane", message);
  message = "";
}

String lastCommand = "";
unsigned long lastTime = 0;

void webServerSetup()
{
  server.on("/", handleRoot);
  server.on("/update", webpageUpdate);

  server.on("/cmd", [](){
    // Filter echo (Firefox seems to send the request several times)
    String cmd = server.uri();
    for (int i = 0; i < server.args(); i++)
      cmd += server.arg(i);
    if (millis() - lastTime < 5000 && cmd == lastCommand)
      return;
    lastCommand = cmd;
    lastTime = millis();
    
    Serial.println("Got command");
    Serial.println(cmd);
    int servoId = server.arg(0).toInt();
    String command = server.arg(1);
    int value = server.arg(2).toInt();

    if (command == "enable")
    {
      board.servos_.enable(servoId, value);
    }
    else if (command == "setTarget")
    {
      int mode = server.arg(3).toInt();
      board.servos_.setTarget(servoId, static_cast<STSMode>(mode), value);
    }
    else if (command == "updateId")
    {
      int newId = server.arg(3).toInt();
    Serial.println("newId");
    Serial.println(newId);
      if (newId > -1 && newId < 0xFE)
      {
        bool updated = board.servos_.updateId(servoId, static_cast<byte>(newId));
        if (updated)
        {
          // Give some time to the servo to fully change ID
          delay(20);
          message = board.rescan();
        }
      }
    }
    else if (command == "setCenter")
    {
      board.servos_.setCenter(servoId);
    }
    else if (command == "rescan")
    {
      message = board.rescan();
    }
    else if (command == "stop")
    {
      board.servos_.stop();
    }
  });

  // Start server
  server.begin();
  Serial.println("Server Starts.");
}

void clientThreading(void *pvParameter)
{
  while(true)
  {
    server.handleClient();
    delay(10);
  }
}


void setup()
{
  // Start communication
  Serial.begin(115200);
  Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  Wire.begin(S_SDA, S_SCL);

  // Setup objects
  board.init(&Serial1);
  board.setLED(2, 4, 0);

  webServerSetup();
  board.setLED(0, 0, 0);
  board.rescan();

  xTaskCreate(&clientThreading, "Client", 4000, NULL, 5, NULL);
}


void loop()
{
  delay(300000);
}
