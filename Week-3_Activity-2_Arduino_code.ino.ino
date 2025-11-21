int myLed = 6;      // External LED connected to digital pin 6
int input = 0;      // Variable to store incoming UART data

void setup() {

  pinMode(myLed, OUTPUT);          // Configure LED pin as output

  Serial.begin(115200);            // UART over USB for printing messages to PC
  Serial1.begin(115200);           // UART1 used for communication with STM32

  while (!Serial);                 // Wait for the Serial Monitor to open
  Serial.println("Serial is ready!");
}

void loop() {

  // Check if the STM32 has sent any data to the Arduino via Serial1
  if (Serial1.available()) {

    input = Serial1.read();        // Read the incoming byte (ASCII character)

    // Check if the byte is an alphabetic character (A–Z or a–z)
    if ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
      Serial.println("Character Pressed ");  // Print result to Arduino Serial Monitor
      digitalWrite(myLed, HIGH);             // Turn LED ON
    }

    // Check if the byte is a numeric digit (0–9)
    else if (input >= '0' && input <= '9') {
      Serial.println("Number Pressed ");     // Print result
      digitalWrite(myLed, LOW);              // Turn LED OFF
    }

    Serial.flush();   // Ensure that any pending serial messages are sent out
  }

  delay(5);           // Small delay to slow the loop slightly
}
