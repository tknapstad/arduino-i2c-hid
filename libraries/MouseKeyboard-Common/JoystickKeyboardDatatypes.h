#ifndef __JOYSTICK_KEYBOARD_DATATYPES_H
#define __JOYSTICK_KEYBOARD_DATATYPES_H

#define INTERNAL_BAUDRATE 2000000

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
