#ifndef __MOUSE_KEYBOARD_DATATYPES_H
#define __MOUSE_KEYBOARD_DATATYPES_H

#define INTERNAL_BAUDRATE 115200
#define NACK 0
#define ACK 1

static const char SERIAL_FRAME_START = '$';
static const char SERIAL_FRAME_END = '\n';
static const char SERIAL_DELIMITER = ',';

typedef struct _InputData
{
    int8_t      x;
    int8_t      y;
    uint8_t     btn;
    uint8_t     key;
} InputData;

#endif
