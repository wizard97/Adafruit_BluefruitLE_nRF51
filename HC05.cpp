#include "HC05.h"

HC05::HC05()
{
    _dataBaud = DEFAULT_DATA_BAUD_RATE;
}


void HC05::begin(SoftwareSerial &serial, int cmdPin, int pwrPin)
{
    _swSerial = &serial;
    _hwSerial = NULL;
    _cmdPin = cmdPin;
    _pwrPin = pwrPin;
    restartModule(BLUEFRUIT_MODE_DATA);
}


void HC05::begin(HardwareSerial &serial, int cmdPin, int pwrPin)
{
    _hwSerial = &serial;
    _swSerial = NULL;
    _cmdPin = cmdPin;
    _pwrPin = pwrPin;
    restartModule(BLUEFRUIT_MODE_DATA);
}

void HC05::serialBegin(uint32_t baud)
{
    if (_hwSerial)
        _hwSerial->begin(baud);
    else
        _swSerial->begin(baud);
}


void HC05::restartModule(uint8_t mode)
{
    if (mode != BLUEFRUIT_MODE_COMMAND && mode != BLUEFRUIT_MODE_DATA)
        return;

    if (mode == BLUEFRUIT_MODE_COMMAND) {
        pinMode(_cmdPin, OUTPUT);
        digitalWrite(_pwrPin, HIGH);
        serialBegin(_dataBaud);
    } else {
        pinMode(_cmdPin, INPUT); //high-z
        serialBegin(DEFAULT_AT_BAUD_RATE);
    }
    _mode = mode;

    // power cycle
    digitalWrite(_pwrPin, LOW);
    delay(10);
    digitalWrite(_pwrPin, HIGH);
}


bool HC05::setMode(uint8_t mode)
{
    if (mode != BLUEFRUIT_MODE_COMMAND && mode != BLUEFRUIT_MODE_DATA)
        return false;

    if (getMode() != mode)
        restartModule(mode);

    return true;
}

bool HC05::setBaud(uint32_t baud)
{
    uint16_t argtype[] = {AT_ARGTYPE_UINT32, AT_ARGTYPE_UINT32,
        AT_ARGTYPE_UINT32 };

    uint32_t args[] = {baud, 1, 0};
    //AT+UART
    if(atcommand_full("AT+UART", NULL, 3, argtype, args)) {
        _dataBaud = baud;
        serialBegin(baud);
        return true;
    }
    return false;
}

bool HC05::setRole(hc05_role role)
{
    return atcommandIntReply("AT+ROLE", NULL, role);
}

bool HC05::setName(const char name[])
{
    return atcommand("AT+NAME", name);
}

bool HC05::setPass(const char name[])
{
    return atcommand("AT+PSWD", name);
}

// Class Print virtual function Interface
size_t HC05::write(uint8_t c)
{
    if (_hwSerial)
        return _hwSerial->write(c);
    else
        return _swSerial->write(c);
}


size_t HC05::write(const uint8_t *buffer, size_t size)
{
    if (_hwSerial)
        return _hwSerial->write(buffer, size);
    else
        return _swSerial->write(buffer, size);

}


// Class Stream interface
int HC05::available(void)
{
    if (_hwSerial)
        return _hwSerial->available();
    else
        return _swSerial->available();
}


int HC05::read(void)
{
    if (_hwSerial)
        return _hwSerial->read();
    else
        return _swSerial->read();
}

void HC05::flush(void)
{
    if (_hwSerial)
        return _hwSerial->flush();
    else
        return _swSerial->flush();
}

int HC05::peek(void)
{
    if (_hwSerial)
        return _hwSerial->peek();
    else
        return _swSerial->peek();
}
