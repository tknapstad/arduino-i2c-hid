#include "MouseKeyboardDataTypes.h"

#define Y_AXIS_PIN A0
#define X_AXIS_PIN A1
#define BUTTON_PIN A2

#define JOYSTICK_JITTER 1
#define XY_RANGE 10

int8_t x_zero = 0;
int8_t y_zero = 0;
InputData prev_input = {0};

// Must send report if the joystick is off-center or the button/keys have changed state
boolean must_report_input(const InputData& current, const InputData& prev) {
//  return (abs(current.x) > JOYSTICK_JITTER ||
//          abs(current.y) > JOYSTICK_JITTER ||
//          current.btn != prev.btn ||
//          current.key != prev.key);
  boolean joystick_centered = (current.x == 0 && prev.x == 0 && current.y == 0 && prev.y == 0);
  return (!joystick_centered ||
          current.btn != prev.btn ||
          current.key != prev.key);
}

void copy_input(const InputData& src, InputData& dst) {
  dst.x == src.x;
  dst.y == src.y;
  dst.btn == src.btn;
  dst.key == src.key;
}

void send_input_data(const InputData& data) {
    char buf[128];
    int bytes = sprintf(buf, "%c%d%c%d%c%u%c%u%c", 
                        SERIAL_FRAME_START, 
                        data.x, SERIAL_DELIMITER,
                        data.y, SERIAL_DELIMITER,
                        data.btn, SERIAL_DELIMITER,
                        data.key, 
                        SERIAL_FRAME_END);
    while (Serial.availableForWrite() < strlen(buf)) { }
    Serial.print(buf);
    Serial.flush();
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
  if (input.x <= JOYSTICK_JITTER) {
    input.x = 0;
  }
  if (input.y <= JOYSTICK_JITTER) {
    input.y = 0;
  }
  input.btn = !digitalRead(BUTTON_PIN);
  // TODO: read actual keys when connected
  input.key = 0;

  if (must_report_input(input, prev_input)) {
    copy_input(input, prev_input);
    send_input_data(input);
  }
  delay(10);
}

