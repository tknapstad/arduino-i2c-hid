#include "HID-Project.h"
#include "MouseKeyboardDataTypes.h"

#define Y_AXIS_PIN A0
#define X_AXIS_PIN A1
#define BUTTON_PIN A2

#define JOYSTICK_JITTER 1
#define XY_RANGE 5

boolean buttonPressed = false;
uint8_t keyPressed = 0;

int8_t x_zero = 0;
int8_t y_zero = 0;
InputData prev_input = {0};
InputData input = {0};

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

void on_button_released() {
  Keyboard.write(KEY_S);
  Keyboard.write(KEY_P);
  Keyboard.write(KEY_R);
  Keyboard.write(KEY_E);
  Keyboard.write(KEY_S);
  Keyboard.write(KEY_S);
  Keyboard.write(KEY_SPACE);
  Keyboard.write(KEY_P);
  Keyboard.write(KEY_U);
  Keyboard.write(KEY_L);
  Keyboard.write(KEY_L);
  Keyboard.write(KEY_ENTER);
}

void on_key_pressed(const uint8_t key) {
    // TODO: handle keys    
}

void send_usb_input_reports(InputData const & input_) {
    // Release the mouse button if the joystick is released
    if (input_.x == 0 && input_.y == 0) {
      if (Mouse.isPressed(MOUSE_MIDDLE)) {
        Mouse.release(MOUSE_MIDDLE);
        //Serial.println(F("Middle mouse released"));
      }
    } else {
      // Press the middle mouse button if it's not allready
      if (!Mouse.isPressed(MOUSE_MIDDLE)) {
        Mouse.press(MOUSE_MIDDLE);
        //Serial.println(F("Middle mouse pressed"));
      }
      Mouse.move(input_.x, input_.y);
    }

    if (input_.btn) {
      buttonPressed = true;
    } else {
      if (buttonPressed) {
        on_button_released();
        buttonPressed = false;
      }
    }

    if (input_.key) {
      keyPressed = input_.key;
    } else {
      if (keyPressed) {
        on_key_pressed(input_.key);
        keyPressed = 0;
      }
    }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  calibrate_joystick();

  // Sends a clean report to the host. This is important on any Arduino type.
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  read_input(input);
  handle_input(input, prev_input, send_usb_input_reports);
  delay(5);
}

