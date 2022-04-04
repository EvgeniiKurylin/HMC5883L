#ifndef HMC5883L_H
#define HMC5883L_H

#include "mbed.h"
#include <stdint.h>

#define HMC5883L_DEFAULT_ADDRESS    0x1E
#define HMC5883L_WRITE_ADDRESS      (_address << 1)
#define HMC5883L_READ_ADDRESS       ((_address << 1) | 0b00000001)

struct xyzFloat{
    float x;
    float y;
    float z;

    xyzFloat();
    xyzFloat(float const x, float const y, float const z);
};

class HMC5883L{
    public:
        /* Registers */
        static uint8_t constexpr REGISTER_CONFIG_A      = 0x00;     // R/W
        static uint8_t constexpr REGISTER_CONFIG_B      = 0x01;     // R/W
        static uint8_t constexpr REGISTER_REG_MODE      = 0x02;     // R/W
        static uint8_t constexpr REGISTER_DO_X_MSB      = 0x03;     // R
        static uint8_t constexpr REGISTER_DO_X_LSB      = 0x04;     // R
        static uint8_t constexpr REGISTER_DO_Y_MSB      = 0x05;     // R
        static uint8_t constexpr REGISTER_DO_Y_LSB      = 0x06;     // R
        static uint8_t constexpr REGISTER_DO_Z_MSB      = 0x07;     // R
        static uint8_t constexpr REGISTER_DO_Z_LSB      = 0x08;     // R
        static uint8_t constexpr REGISTER_STATUS        = 0x09;     // R
        static uint8_t constexpr REGISTER_ID_A          = 0x0A;     // R
        static uint8_t constexpr REGISTER_ID_B          = 0x0B;     // R
        static uint8_t constexpr REGISTER_ID_C          = 0x0C;     // R

        /* Register values */
        static uint8_t constexpr WHO_AM_I_ID_A          = 0x48;
        static uint8_t constexpr WHO_AM_I_ID_B          = 0x34;
        static uint8_t constexpr WHO_AM_I_ID_C          = 0x33;

        static uint8_t constexpr RANGE_0_88_GA          = 0x00;
        static uint8_t constexpr RANGE_1_33_GA          = 0x01;
        static uint8_t constexpr RANGE_1_90_GA          = 0x02;
        static uint8_t constexpr RANGE_2_50_GA          = 0x03;
        static uint8_t constexpr RANGE_4_00_GA          = 0x04;
        static uint8_t constexpr RANGE_4_70_GA          = 0x05;
        static uint8_t constexpr RANGE_5_60_GA          = 0x06;
        static uint8_t constexpr RANGE_8_10_GA          = 0x07;

        /* Constructors */
        HMC5883L(I2C * i2c, uint8_t addr = HMC5883L_DEFAULT_ADDRESS);

        /* Methods */
        bool init();
        xyzFloat getMagneticField();
        void setCalibrationParameters(float offX, float offY, float offZ);

    protected:
        bool init(uint8_t expectedID_A, uint8_t expectedID_B, uint8_t expectedID_C);
        uint8_t whoAmI(uint8_t ID);

        void writeHMC5883LRegister(uint8_t reg, uint8_t val);
        uint8_t readHMC5883LRegister8(uint8_t reg);

        bool dataReady();

    private:
        I2C * _i2c;
        uint8_t _address;
        float   _resolution;
        xyzFloat _offset;

        void updateResolution(uint8_t rangeType);
        xyzFloat readMagneticField();
};

void printByte(char byte);

#endif