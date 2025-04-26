#ifndef STSSERVO_WRAPPER_H
#define STSSERVO_WRAPPER_H

#include <Arduino.h>
#include <STSServoDriver.h>

class ServoWrapper
{
    public:
        ServoWrapper();

        void init(HardwareSerial *serialPort);

        String rescan();

        String updateReadings();

        void enable(byte servoId, bool enable);

        void setTarget(byte servoId, STSMode servoMode, int value);

        void setCenter(byte servoId);

        bool updateId(byte servoId, byte newId);

        int nServo() {return nServo_;};

        void stop();

    private:
        STSServoDriver driver_;
        byte servoList_[255] = {0};
        byte enabled_[255] = {false};
        byte servoMode_[255] = {0};
        int nServo_ = 0;
};
#endif