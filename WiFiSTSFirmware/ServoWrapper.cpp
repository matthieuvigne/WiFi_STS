#include "ServoWrapper.h"

#define MAX_ID 255

ServoWrapper::ServoWrapper()
{
    // Empty on purpose
}

void ServoWrapper::init(HardwareSerial *serialPort)
{
    driver_.init(serialPort);
}

String ServoWrapper::rescan()
{
    // Disable and reset all servos
    driver_.writeRegister(0xFE, STSRegisters::TORQUE_SWITCH, 0);
    driver_.setMode(0xFE, STSMode::POSITION);
    nServo_ = 0;
    for(int i = 0; i <= MAX_ID; i++)
    {
        if(driver_.ping(i))
        {
            servoList_[nServo_] = i;
            servoMode_[nServo_] = 0;
            enabled_[nServo_] = false;
            nServo_++;
        }
    }

    if (nServo_ == 0)
        return "";

    String result = "n";
    for(int i = 0; i < nServo_; i++)
        result += "@" + String(servoList_[i]);
    return result + ";";
}

String ServoWrapper::updateReadings()
{
    String message;
    for (int i = 0; i < nServo_; i++)
    {
        int const id = servoList_[i];
        message += String(id) + "@";
        bool isEnabled = driver_.readRegister(id, STSRegisters::TORQUE_SWITCH);
        message += (isEnabled ? "Enable" : "Disable");
        message += ", Pos " + String(driver_.getCurrentPosition(id)) + ", ";
        message += "Vel " + String(driver_.getCurrentSpeed(id)) + ";";
    }
    return message;
}

void ServoWrapper::enable(byte servoId, bool enable)
{
    enabled_[servoId] = enable;
    driver_.writeRegister(servoId, STSRegisters::TORQUE_SWITCH, (enable ? 1 : 0));
    if (!enable)
        driver_.setTargetVelocity(servoId, 0);
}

void ServoWrapper::setTarget(byte servoId, STSMode servoMode, int value)
{
    if (!enabled_[servoId])
        return;

    if (servoMode != servoMode_[servoId])
    {
        driver_.setMode(servoId, servoMode);
        if (servoMode == STSMode::POSITION)
            driver_.writeTwoBytesRegister(servoId, STSRegisters::MAXIMUM_ANGLE, 4095);
        else if (servoMode == STSMode::STEP)
            driver_.writeTwoBytesRegister(servoId, STSRegisters::MAXIMUM_ANGLE, 0);
        servoMode_[servoId] = servoMode;
    }
    if (servoMode_[servoId] == STSMode::POSITION)
    {
        driver_.setTargetPosition(servoId, value);
    }
    else if (servoMode_[servoId] == STSMode::VELOCITY)
    {
        driver_.setTargetVelocity(servoId, value);
    }
    else if (servoMode_[servoId] == STSMode::STEP)
    {
        driver_.setTargetPosition(servoId, value);
    }
}

bool ServoWrapper::updateId(byte servoId, byte newId)
{
    bool isUnique = true;
    for (int i = 0; i < nServo_; i++)
        isUnique &= newId != servoList_[i];
    if (isUnique)
        return driver_.setId(servoId, newId);
    return isUnique;
}

void ServoWrapper::setCenter(byte servoId)
{
    driver_.writeRegister(servoId, STSRegisters::TORQUE_SWITCH, 128);
}

void ServoWrapper::stop()
{
    driver_.writeRegister(0xFE, STSRegisters::TORQUE_SWITCH, 0);
    for (int i = 0; i < MAX_ID; i++)
        enabled_[i] = false;
}
