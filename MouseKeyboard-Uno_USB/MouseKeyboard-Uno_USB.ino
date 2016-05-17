#include "HID-Project.h"
#include "MouseKeyboardDataTypes.h"

InputData input;
boolean buttonPressed = false;

void print_input_data(const InputData& input) {
      Serial.print(F("X="));
      Serial.print(input.x);
      Serial.print(F(", Y="));
      Serial.print(input.y);
      Serial.print(F(", Button="));
      Serial.print(input.btn);
      Serial.print(F(", Key="));
      Serial.println(input.key); 
}

void parse_input_data() {
    Serial1.readStringUntil(SERIAL_FRAME_START);
    input.x = Serial1.parseInt();
    input.y = Serial1.parseInt();
    input.btn = Serial1.parseInt();
    input.key = Serial1.parseInt();
    Serial1.readStringUntil(SERIAL_FRAME_END); 
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

void setup() {
  // Start the USB Serial for debugging
  Serial.begin(115200);

  // Start the Serial1 which is connected with the IO MCU.
  // Make sure both baud rates are the same
  Serial1.begin(INTERNAL_BAUDRATE);

  // Sends a clean report to the host. This is important on any Arduino type.
  Keyboard.begin();
  Mouse.begin();

}

void loop() {
  if (Serial1.available()) {
    parse_input_data();
    //print_input_data(input);

    // Release the mouse button if the joystick is released
    if (input.x == 0 && input.y == 0) {
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
      Mouse.move(input.x, input.y);
    }

    if (input.btn) {
      buttonPressed = true;
    } else {
      if (buttonPressed) {
        on_button_released();
      }
    }

    // TODO: handle keys
  } else {
    delay(1);
  }
}

