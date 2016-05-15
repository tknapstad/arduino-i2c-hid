#include "HID-Project.h"
#include "MouseKeyboardDataTypes.h"

InputData input;
int lastMove = 0;

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
    boolean neg = false;
    Serial1.readStringUntil(SERIAL_FRAME_START);
    if (Serial1.peek() == '-') {
      neg = true;
    }
    input.x = Serial1.parseInt();
    if (neg) {
      input.x *= -1;
      neg = false;
    }
    Serial1.read();
    if (Serial1.peek() == '-') {
      neg = true;
    }
    input.y = Serial1.parseInt();
    if (neg) {
      input.y *= -1;
      neg = false;
    }
    Serial1.read();
    input.btn = Serial1.parseInt();
    Serial1.read();
    input.key = Serial1.parseInt();
    Serial1.readStringUntil(SERIAL_FRAME_END); 
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
    print_input_data(input);
    if (!Mouse.isPressed(MOUSE_MIDDLE)) {
      Mouse.press(MOUSE_MIDDLE);
      Serial.println(F("Middle mouse pressed"));
    }
    Mouse.move(input.x, input.y);
    lastMove = millis();
  } else {
    if ( (millis() - lastMove > 100) && Mouse.isPressed(MOUSE_MIDDLE)) {
       Mouse.release(MOUSE_MIDDLE);
      Serial.println(F("Middle mouse released"));
    }
  }
}

