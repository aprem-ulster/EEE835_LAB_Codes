#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include "adxl345.h"

// Variables to store raw 16-bit accelerometer output from each axis
int16_t x, y, z;

// Variables to store converted acceleration values in units of 'g'
float xg, yg, zg;

// Buffer used by the ADXL345 driver to store 6 bytes (X0, X1, Y0, Y1, Z0, Z1)
extern uint8_t rec_data[6];

int main(void)
{
    // Initialise the ADXL345 sensor:
    // - Configure I2C communication
    // - Set measurement range and data format
    // - Enable measurement mode
    adxl_init();

    while (1)
    {
        // Read 6 consecutive data registers starting from DATAX0
        // This provides the low and high bytes for X, Y, and Z axes
        adxl_read_values(DATA_START_ADD);

        // Reconstruct 16-bit signed raw acceleration values
        // High byte is shifted left by 8 bits and combined with the low byte
        x = ((rec_data[1] << 8) | rec_data[0]);
        y = ((rec_data[3] << 8) | rec_data[2]);
        z = ((rec_data[5] << 8) | rec_data[4]);

        // Convert the raw values to acceleration in 'g'
        // ADXL345 scale factor ≈ 3.9 mg/LSB → 0.0039 g/LSB
        // Using 0.0078 accounts for ±2g full-resolution (13-bit) mode
        xg = x * 0.0078f;
        yg = y * 0.0078f;
        zg = z * 0.0078f;

        // Loop continuously to update readings
    }
}
