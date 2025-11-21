int myLed = 6;      // GPIO pin connected to the external LED
int input = 0;      // Stores the character received over UART

void setup() {
    pinMode(myLed, OUTPUT);         // Configure LED pin as an output
    Serial.begin(115200);           // Initialise UART communication at 115200 baud
    while(!Serial);                 // Wait until the Serial Monitor is opened
    Serial.println("Serial is ready!");  // Confirmation message
}

void loop() {

    // Check if a character has been received through UART
    if (Serial.available() > 0) {
        input = Serial.read();      // Read the incoming byte (ASCII value)

        // If input is an alphabetic character (either uppercase or lowercase)
        if ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z')) {
            Serial.println("Character Pressed ");
            digitalWrite(myLed, HIGH);   // Turn LED ON
        }

        // If input is a numeric digit (0–9)
        else if (input >= '0' && input <= '9') {
            Serial.println("Number Pressed ");
            digitalWrite(myLed, LOW);    // Turn LED OFF
        }

        // Ensures queued serial data is transmitted before continuing
        Serial.flush();
    }

    // Small delay to prevent the loop from running too quickly
    delay(5);
}
