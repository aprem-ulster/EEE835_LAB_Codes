int pin = A0;     // Analogue input pin connected to the potentiometer

void setup() {
    Serial.begin(9600);     // Start UART communication at 9600 baud
    while (!Serial);        // Wait for the Serial Monitor to open
}

void loop() {
    uint16_t rawADC = analogRead(pin);           // Read 10-bit ADC value (0–1023)
    float voltage = 3.3 * rawADC / 1023.0;       // Convert ADC value to voltage (0–3.3V)

    Serial.print("ADC Value = ");                // Print raw ADC reading
    Serial.println(rawADC);

    Serial.print("Voltage = ");                  // Print converted voltage
    Serial.print(voltage);
    Serial.println(" V");

    delay(1000);                                 // Read once per second
}

