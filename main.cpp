#include "mbed.h"
#include "HMC5883L.h"
#include <cstdint>
#include <cstdio>

/* 
HMC5883L - 0x1E — Three axis magnetic field
*/

Serial  pc(USBTX, USBRX, 9600);   // TX,  RX, baudrate
I2C     i2c(PA_10, PA_9);           // SDA, SCL

HMC5883L hmc5883l(&i2c);

// main() runs in its own thread in the OS
int main()
{
    if (hmc5883l.init()) {
        printf("\nHMC5883L is detected\r\n");
        // No calibration algorithm implemented
    }
    else {
        printf("\nHMC5883L isn't found\r\n");
    }

    // Measurement in [T] (tesla)
    xyzFloat mag;

    while (true) {
        mag = hmc5883l.getMagneticField();
        printf("%.2f,%.2f,%.2f\r\n", mag.x, mag.y, mag.z);
        wait_us(50e3);
    }
}