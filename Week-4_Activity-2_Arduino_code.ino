int tmp36 = A0;      // TMP36 output connected to analogue pin A0

void setup() {
    Serial.begin(9600);          // Start UART communication for serial output
    while (!Serial);             // Wait until the Serial Monitor is opened

    analogReadResolution(12);    // Set ADC resolution to 12 bits (0–4095)
}

void loop() {
    uint16_t rawADC = analogRead(tmp36);          // Read 12-bit ADC value from TMP36
    float voltage = 3.3 * rawADC / 4095.0;        // Convert ADC value to actual voltage
                                                 // (3.3V reference, 12-bit range)

    float temperature = (voltage - 0.5) * 100;     // Convert voltage to °C using TMP36 formula:
                                                   // 0.5V = 0°C, 10mV = 1°C

    Serial.print("Room Temperature = ");           // Display temperature reading
    Serial.print(temperature);
    Serial.println(" Degree Celsius.");

    delay(1000);                                   // Read once every second
}
