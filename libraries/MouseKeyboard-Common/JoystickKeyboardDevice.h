#ifndef __JOYSTICK_KEYBOARD_DEVICE_H
#define __JOYSTICK_KEYBOARD_DEVICE_H

#include "JoystickKeyboardDatatypes.h"
#include <map>

class JoystickKeyboardDevice {
public:
    enum IOPin {
        PIN_JOY_X, // ADC x axis input from joystick
        PIN_JOY_Y, // ADC y axis input from joystick
        PIN_JOY_BTN, // Digital IO for joystick button
        PIN_KEY_01, // Pin for key 01
        PIN_KEY_02, // Pin for key 02
        PIN_KEY_03, // Pin for key 03
        PIN_KEY_04, // Pin for key 04
        PIN_KEY_05, // Pin for key 05
        PIN_KEY_06, // Pin for key 06
        PIN_KEY_07, // Pin for key 07
        PIN_KEY_08, // Pin for key 08
        PIN_KEY_09, // Pin for key 09
        PIN_KEY_10, // Pin for key 10
        PIN_KEY_11, // Pin for key 11
        PIN_KEY_12 // Pin for key 12
    };

    JoystickKeyboardDevice();
    virtual ~JoystickKeyboardDevice();

    // Set the pin number for a specific function defined in
    // the IOPin enum
    void SetPin(const IOPin pin, const uint8_t num);

private:
    typedef std::map<IOPin, uint8_t> IoMap;
    IoMap ioMap;

    uint8_t joystickJitter;
    uint8_t joystickRange;
    int8_t joystickXZero = 0;
    int8_t joystickYZero = 0;

    InputData inputData;
    InputData prevInputData;

    typedef void (*ProcessInput)(InputData const &);

    // Return false if all zeros in current and prev
    boolean should_report_input(InputData const & current, InputData const & prev) {
      return (current.x + current.y + current.btn + current.key + prev.x + prev.y + prev.btn + prev.key);
    }

    void copy_input(InputData const & src, InputData & dst) {
      dst.x = src.x;
      dst.y = src.y;
      dst.btn = src.btn;
      dst.key = src.key;
    }

    // Send input data over serial on the format "%x,y,btn,key\n"
    void send_input_over_serial(const InputData& data) {
      char buf[128];
      int bytes = sprintf(buf, "%c%d%c%d%c%u%c%u%c",
                          SERIAL_FRAME_START,
                          data.x, SERIAL_DELIMITER,
                          data.y, SERIAL_DELIMITER,
                          data.btn, SERIAL_DELIMITER,
                          data.key,
                          SERIAL_FRAME_END);
      while (Serial.availableForWrite() < strlen(buf)) {
        delay(1);
      }
      Serial.print(buf);
    }

    void calibrate_joystick() {
      // Read the zero position of the joystick on startup for calibration
      // Fingers crosses that it's not moved at this time
      x_zero = map(analogRead(X_AXIS_PIN), 0, 1 << 10, -XY_RANGE, XY_RANGE);
      y_zero = map(analogRead(Y_AXIS_PIN), 0, 1 << 10, XY_RANGE, -XY_RANGE);
    }

    void read_input(InputData & input) {
      input.x = map(analogRead(X_AXIS_PIN), 0, 1 << 10, -XY_RANGE, XY_RANGE) - x_zero;
      input.y = map(analogRead(Y_AXIS_PIN), 0, 1 << 10, XY_RANGE, -XY_RANGE) - y_zero;

      // Zero x and/or y if below the jitter threshold
      if (abs(input.x) <= JOYSTICK_JITTER) {
        input.x = 0;
      }
      if (abs(input.y) <= JOYSTICK_JITTER) {
        input.y = 0;
      }

      input.btn = !digitalRead(BUTTON_PIN);
      // TODO: read actual keys when connected
      input.key = 0;
    }

    void handle_input(InputData const & input, InputData & prev_input, ProcessInput process) {
      if (should_report_input(input, prev_input)) {
        copy_input(input, prev_input);
        process(input);
      }
    }
};

#endif
