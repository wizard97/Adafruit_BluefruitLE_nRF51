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
    ROLE_MASTER = 1,
    ROLE_SLAVE_LOOP,
} hc05_role_t;

typedef enum hc05_status
{
    INITIALIZED = 0,
    READY,
    PAIRABLE,
    PAIRED,
    INQUIRING,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    NUKNOW
} hc05_status_t;

typedef enum hc05_cmd_mode
{
    CMD_MODE_CONNECT_BOUND = 0, //only bound addresses
    CMD_MODE_CONNECT_ANY = 1, //any address
} hc05_cmd_mode_t;

class HC05 : public Adafruit_ATParser
{
public:
    HC05();

    void begin(HardwareSerial &serial, int cmdPin, int pwrPin1, int pwrPin2);
    void begin(SoftwareSerial &serial, int cmdPin, int pwrPin1, int pwrPin2);

    /*********** BEGIN AT COMMAND METHODS *****************/
    //powercycle
    void restartModule(uint8_t mode = BLUEFRUIT_MODE_DATA);

    //default 38400 bits/s; Stop bit: 1 bit; Parity bit: None.
    bool setBaud(uint32_t baud);
    // Change between master slave etc
    bool setRole(hc05_role_t role);
    bool setName(const char name[]);
    bool setPass(const char name[]);

    bool setCmdMode(hc05_cmd_mode_t mode);
    bool setInqMode(uint8_t mode, uint8_t maxDevices, uint8_t timeout);

    bool releaseModule();

    bool initSPP();
    bool pair(const char addr[], uint8_t timeout);
    bool bind(const char addr[]);
    bool link(const char addr[]);

    bool reset();
    bool originalState();

    //getters
    int32_t getNumAuthDevices();
    hc05_status_t getStatus();

    /*********** END AT COMMAND METHODS *****************/

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

    int _cmdPin, _pwrPin1, _pwrPin2;
    // Stupid Serial inherits from Stream instead of an abstract Serial class
    SoftwareSerial *_swSerial;
    HardwareSerial *_hwSerial;
};
#endif
