#include "HC05.h"

HC05::HC05()
{
    _dataBaud = DEFAULT_DATA_BAUD_RATE;
}


void HC05::begin(SoftwareSerial &serial, int cmdPin, int pwrPin1, int pwrPin2)
{
    _swSerial = &serial;
    _hwSerial = NULL;
    _cmdPin = cmdPin;
    _pwrPin1 = pwrPin1;
    _pwrPin2 = pwrPin2;
    pinMode(_pwrPin1, OUTPUT);
    pinMode(_pwrPin2, OUTPUT);
    pinMode(_cmdPin, OUTPUT);
    restartModule(BLUEFRUIT_MODE_DATA);
}


void HC05::begin(HardwareSerial &serial, int cmdPin, int pwrPin1, int pwrPin2)
{
    _hwSerial = &serial;
    _swSerial = NULL;
    _cmdPin = cmdPin;
    _pwrPin1 = pwrPin1;
    _pwrPin2 = pwrPin2;
    pinMode(_pwrPin1, OUTPUT);
    pinMode(_pwrPin2, OUTPUT);
    pinMode(_cmdPin, OUTPUT);
    restartModule(BLUEFRUIT_MODE_DATA);
}

void HC05::serialBegin(uint32_t baud)
{
    if (_hwSerial)
        _hwSerial->begin(baud);
    else
        _swSerial->begin(baud);
}

bool HC05::reset()
{
    bool ret = atcommand_0(F("AT+RESET"));
    if (!ret)
        return ret;
    delay(1000);
    return ret;
}


void HC05::restartModule(uint8_t mode)
{
    if (mode != BLUEFRUIT_MODE_COMMAND && mode != BLUEFRUIT_MODE_DATA)
        return;

    if (mode == BLUEFRUIT_MODE_COMMAND) {
        digitalWrite(_cmdPin, HIGH);
        serialBegin(_dataBaud);
    } else {
        digitalWrite(_cmdPin, LOW);
        serialBegin(DEFAULT_AT_BAUD_RATE);
    }

    _mode = mode;

    // power cycle
    digitalWrite(_pwrPin1, LOW);
    digitalWrite(_pwrPin2, LOW);
    delay(10);
    digitalWrite(_pwrPin1, HIGH);
    digitalWrite(_pwrPin2, HIGH);
    delay(1000); //bootup
    while(available()) read();
}


bool HC05::setMode(uint8_t mode)
{
    if (mode != BLUEFRUIT_MODE_COMMAND && mode != BLUEFRUIT_MODE_DATA)
        return false;

    if (getMode() != mode)
        restartModule(mode);

    return true;
}

bool HC05::setInqMode(uint8_t mode, uint8_t maxDevices, uint8_t timeout)
{
    uint16_t argtype[] = {AT_ARGTYPE_UINT8, AT_ARGTYPE_UINT8,
        AT_ARGTYPE_UINT8 };

    uint32_t args[] = {mode, maxDevices, timeout};
    return atcommand_full(F("AT+INQM"), NULL, 3, argtype, args);
}

bool HC05::setBaud(uint32_t baud)
{
    uint16_t argtype[] = {AT_ARGTYPE_UINT32, AT_ARGTYPE_UINT32,
        AT_ARGTYPE_UINT32 };

    uint32_t args[] = {baud, 1, 0};
    //AT+UART
    if(atcommand_full(F("AT+UART"), NULL, 3, argtype, args)) {
        _dataBaud = baud;
        //serialBegin(baud); //not sure if AT baud rate is changed
        return true;
    }
    return false;
}

bool HC05::setRole(hc05_role_t role)
{
    return atcommand_1(F("AT+ROLE"), (uint16_t)role);
}

bool HC05::setName(const char name[])
{
    return atcommand_1(F("AT+NAME"), name);
}

bool HC05::setPass(const char name[])
{
    return atcommand_1(F("AT+PSWD"), name);
}

bool HC05::setCmdMode(hc05_cmd_mode_t mode)
{
    return atcommand_1(F("AT+CMODE"), (uint16_t)mode);
}

bool HC05::initSPP()
{
    return atcommand_0(F("AT+INIT"));
}


bool HC05::link(const char addr[])
{
    return atcommand_1(F("AT+LINK"), addr, 5000);
}

bool HC05::releaseModule()
{
    return atcommand_0(F("AT+RMAAD"));
}

bool HC05::pair(const char addr[], uint8_t timeout)
{
    uint16_t type[] = { AT_ARGTYPE_STRING, AT_ARGTYPE_UINT8 };
    uint32_t args[] = { (uint32_t)addr, timeout };

    return atcommand_full(F("AT+PAIR"), NULL, 2, type, args, (timeout+1)*1000);
}

bool HC05::bind(const char addr[])
{
    return atcommand_1(F("AT+BIND"), addr);
}

bool HC05::originalState()
{
    return atcommand_0(F("AT+ORGL"));
}

int32_t HC05::getNumAuthDevices()
{
    int32_t num;
    if(this->atcommandIntReply_0(F("AT+ADCN?"), &num))
        return num;
    return 0;
}

hc05_status_t HC05::getStatus()
{
    uint8_t current_mode = _mode;
    hc05_status_t ret = NUKNOW;
    char *start;

    // switch mode if necessary to execute command
    if ( current_mode == BLUEFRUIT_MODE_DATA ) setMode(BLUEFRUIT_MODE_COMMAND);

    // Execute command with parameter and get response
    println(F("AT+STATE?"));

    uint16_t len = readline(100);

    if(len <= sizeof("+STATE:")) {
        ret =  NUKNOW;
        goto end;
    }

    ret = NUKNOW;
    start = &buffer[7];
    len -= 7;

    if (!strncmp("INITIALIZED", start, len)) {
        ret = INITIALIZED;
    } else if (!strncmp("READY", start, len)) {
        ret = READY;
    } else if (!strncmp("PAIRABLE", start, len)) {
        ret = PAIRABLE;
    } else if (!strncmp("PAIRED", start, len)) {
        ret = PAIRED;
    } else if (!strncmp("INQUIRING", start, len)) {
        ret = INQUIRING;
    } else if (!strncmp("CONNECTING", start, len)) {
        ret = CONNECTING;
    } else if (!strncmp("CONNECTED", start, len)) {
        ret = CONNECTED;
    } else if (!strncmp("DISCONNECTED", start, len)) {
        ret = DISCONNECTED;
    } else {
        ret = NUKNOW;
    }

    if(!waitForOK(100)) {
        ret =  NUKNOW;
        goto end;
    }

    //return status
    // switch back if necessary
end:
    if ( current_mode == BLUEFRUIT_MODE_DATA ) setMode(BLUEFRUIT_MODE_DATA);
    return ret;
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
