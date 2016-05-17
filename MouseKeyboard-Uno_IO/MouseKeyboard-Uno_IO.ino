#include "MouseKeyboardDataTypes.h"

#define Y_AXIS_PIN A0
#define X_AXIS_PIN A1
#define BUTTON_PIN A2

#define JOYSTICK_JITTER 1
#define XY_RANGE 10

int8_t x_zero = 0;
int8_t y_zero = 0;
InputData prev_input = {0};

// Send report if the joystick is off-center, has just been released or the button/keys have changed state
//boolean should_report_input(const InputData& current, const InputData& prev) {
//  boolean joystick_active = (current.x != 0 || current.y != 0);
//  boolean joystick_released = (current.x == 0 && current.y == 0) && (prev.x != 0 || prev.y != 0);
//  boolean button_changed = (current.btn != prev.btn);
//  boolean key_changed = (current.key != prev.key);
//  
//  return (joystick_active || joystick_released || button_changed || key_changed);
//}

// Return false if all zeros in current and prev
boolean should_report_input(const InputData& current, const InputData& prev) {
  return (current.x + current.y + current.btn + current.key + prev.x + prev.y + prev.btn + prev.key);
}

void copy_input(const InputData& src, InputData& dst) {
  dst.x = src.x;
  dst.y = src.y;
  dst.btn = src.btn;
  dst.key = src.key;
}

// Send input data on the format "%x,y,btn,key\n"
void send_input_data(const InputData& data) {
    char buf[128];
    int bytes = sprintf(buf, "%c%d%c%d%c%u%c%u%c", 
                        SERIAL_FRAME_START, 
                        data.x, SERIAL_DELIMITER,
                        data.y, SERIAL_DELIMITER,
                        data.btn, SERIAL_DELIMITER,
                        data.key, 
                        SERIAL_FRAME_END);
    while (Serial.availableForWrite() < strlen(buf)) { delay(1); }
    Serial.print(buf);
    //Serial.flush();
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Read the zero position of the joystick on startup for calibration
  // Fingers crosses that it's not moved at this time
  x_zero = map(analogRead(X_AXIS_PIN), 0, 1 << 10, -XY_RANGE, XY_RANGE);
  y_zero = map(analogRead(Y_AXIS_PIN), 0, 1 << 10, XY_RANGE, -XY_RANGE);
  
  // Start the Serial which is connected with the USB MCU.
  // Make sure both baud rates are the same
  Serial.begin(INTERNAL_BAUDRATE);
}

void loop() {
  InputData input;
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

  if (should_report_input(input, prev_input)) {
    copy_input(input, prev_input);
    send_input_data(input);
  }
  delay(10);
}

