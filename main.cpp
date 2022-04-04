#include "mbed.h"
#include "HMC5883L.h"
#include <cstdint>
#include <cstdio>

/* 
ADXL345 -  0xA6 — Three axis acceleration 
ITG3205  - 0xD0 — Three axis gyroscope
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
        // hmc5883l.setCalibrationParameters(209.92, 103.68, 160.0);
    }
    else {
        printf("\nHMC5883L isn't found\r\n");
    }

    xyzFloat mag;
    float maxX = -1000;
    float minX = 1000;
    float maxY = -1000;
    float minY = 1000;
    float maxZ = -1000;
    float minZ = 1000;

    while (true) {
        mag = hmc5883l.getMagneticField();

        maxX = mag.x > maxX ? mag.x : maxX;
        maxY = mag.y > maxY ? mag.y : maxY;
        maxZ = mag.z > maxZ ? mag.z : maxZ;

        minX = mag.x < minX ? mag.x : minX;
        minY = mag.y < minY ? mag.y : minY;
        minZ = mag.z < minZ ? mag.z : minZ;
        printf("%.2f,%.2f,%.2f\r\n", mag.x, mag.y, mag.z);

        // printf("magX: %.2f [%.2f, %.2f]\tmagY: %.2f [%.2f, %.2f]\tmagZ: %.2f [%.2f, %.2f]\r\n", mag.x, minX, maxX, mag.y, minY, maxY, mag.z, minZ, maxZ);
        wait_us(20e3);
    }
}