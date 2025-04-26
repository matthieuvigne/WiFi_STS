
#include <WiFi.h>

#include "Board.h"
#include "ServoWrapper.h"
#include "Constants.h"

// GPIO used to control the leds
#define RGB_LED   23

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels, 32 as default.
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Board::Board():
     matrix(Adafruit_NeoPixel(10, RGB_LED, NEO_GRB + NEO_KHZ800)),
     display(Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET))

{

}

void Board::init(HardwareSerial *serialPort)
{
    servos_.init(serialPort);
    matrix.setBrightness(255);
    matrix.begin();
    setLED(0, 0, 0);

    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    display.println("Booting...");
    display.display();

    WiFi.softAP(AP_SSID, AP_PWD);
    ip_ = WiFi.softAPIP();

}

void Board::setLED(byte Rinput, byte Ginput, byte Binput)
{
  matrix.setPixelColor(0, matrix.Color(Rinput, Ginput, Binput));
  matrix.setPixelColor(1, matrix.Color(Rinput, Ginput, Binput));
  matrix.show();
}

String Board::rescan()
{
  setLED(0, 5, 5);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(F("AP: ")); display.println(AP_SSID);
  display.print(F("IP: ")); display.println(ip_);
  display.println();
  display.println(F("Searching Servos..."));
  display.display();

  String result = servos_.rescan();

  if (result == "")
    setLED(5, 0, 0);
  else
    setLED(0, 0, 0);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  display.print(F("AP: ")); display.println(AP_SSID);
  display.print(F("IP: ")); display.println(ip_);
  display.println();

  display.print(servos_.nServo()); display.println(F(" servos detected."));
  display.display();

  return result;
}