
#ifndef BOARD_H
#define BOARD_H

#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>

#include "ServoWrapper.h"

class Board
{
    public:
        Board();

        void init(HardwareSerial *serialPort);

        void setLED(byte Rinput, byte Ginput, byte Binput);

        String rescan();

        ServoWrapper servos_;
    private:
        Adafruit_NeoPixel matrix;
        Adafruit_SSD1306 display;

        IPAddress ip_;
};
#endif