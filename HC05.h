#ifndef HC05_H
#define HC05_H

#include <Arduino.h>
#include <Stream.h>
#include "SoftwareSerial.h"
#include "Adafruit_ATParser.h"

#define DEFAULT_DATA_BAUD_RATE 38400
#define DEFAULT_AT_BAUD_RATE 38400

typedef enum hc05_role
{
    ROLE_SLAVE = 0,
    ROLE_MASTER,
    ROLE_SLAVE_LOOP,
} hc05_role;

class HC05 : public Adafruit_ATParser
{
public:
    HC05(HardwareSerial &serial, int cmdPin, int pwrPin);
    HC05(SoftwareSerial &serial, int cmdPin, int pwrPin);

    //powercycle
    void restartModule(uint8_t mode = BLUEFRUIT_MODE_DATA);

    //default 38400 bits/s; Stop bit: 1 bit; Parity bit: None.
    bool setBaud(uint32_t baud);
    // Change between master slave etc
    bool setRole(hc05_role role);
    bool setName(const char name[]);
    bool setPass(const char name[]);

    virtual bool setMode(uint8_t mode);
    // Class Print virtual function Interface
    virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buffer, size_t size);

    // pull in write(str) and write(buf, size) from Print
    using Print::write;

    // Class Stream interface
    virtual int  available(void);
    virtual int  read(void);
    virtual void flush(void);
    virtual int  peek(void);

protected:
    void serialBegin(uint32_t baud);
private:
    uint32_t _dataBaud;

    const int _cmdPin, _pwrPin;
    // Stupid Serial inherits from Stream instead of an abstract Serial class
    SoftwareSerial *const _swSerial;
    HardwareSerial *const _hwSerial;
};
#endif
