#include "HMC5883L.h"
#include <cstdint>
#include <cstdio>

uint8_t constexpr HMC5883L::REGISTER_CONFIG_A   ;
uint8_t constexpr HMC5883L::REGISTER_CONFIG_B   ;
uint8_t constexpr HMC5883L::REGISTER_REG_MODE   ;
uint8_t constexpr HMC5883L::REGISTER_DO_X_MSB   ;
uint8_t constexpr HMC5883L::REGISTER_DO_X_LSB   ;
uint8_t constexpr HMC5883L::REGISTER_DO_Y_MSB   ;
uint8_t constexpr HMC5883L::REGISTER_DO_Y_LSB   ;
uint8_t constexpr HMC5883L::REGISTER_DO_Z_MSB   ;
uint8_t constexpr HMC5883L::REGISTER_DO_Z_LSB   ;
uint8_t constexpr HMC5883L::REGISTER_STATUS     ;
uint8_t constexpr HMC5883L::REGISTER_ID_A       ;
uint8_t constexpr HMC5883L::REGISTER_ID_B       ;
uint8_t constexpr HMC5883L::REGISTER_ID_C       ;

uint8_t constexpr HMC5883L::WHO_AM_I_ID_A       ;
uint8_t constexpr HMC5883L::WHO_AM_I_ID_B       ;
uint8_t constexpr HMC5883L::WHO_AM_I_ID_C       ;

uint8_t constexpr HMC5883L::RANGE_0_88_GA       ;
uint8_t constexpr HMC5883L::RANGE_1_33_GA       ;
uint8_t constexpr HMC5883L::RANGE_1_90_GA       ;
uint8_t constexpr HMC5883L::RANGE_2_50_GA       ;
uint8_t constexpr HMC5883L::RANGE_4_00_GA       ;
uint8_t constexpr HMC5883L::RANGE_4_70_GA       ;
uint8_t constexpr HMC5883L::RANGE_5_60_GA       ;
uint8_t constexpr HMC5883L::RANGE_8_10_GA       ;



/* Constructors */
HMC5883L::HMC5883L(I2C * i2c, uint8_t addr)
    : _i2c(i2c)
    , _address(addr)
{

}

/* Public functions */
bool HMC5883L::init(){
    return init(WHO_AM_I_ID_A, WHO_AM_I_ID_B, WHO_AM_I_ID_C);
}

xyzFloat HMC5883L::getMagneticField(){
    return readMagneticField();
}

/* Protected functions */
bool HMC5883L::init(uint8_t expectedID_A, uint8_t expectedID_B, uint8_t expectedID_C){
    // Device ID verification
    if (whoAmI(REGISTER_ID_A) != expectedID_A ||
        whoAmI(REGISTER_ID_B) != expectedID_B ||
        whoAmI(REGISTER_ID_C) != expectedID_C) {
        return false;
    }
    // A configuration (Nsamples averaged = 8, Data output rate = 75 Hz, Normal measurement) -> 0b01111000 (0x78)
    writeHMC5883LRegister(REGISTER_CONFIG_A, 0x78);
    // B configuration (Gain = 5) -> 0b10100000 (0xA0)
    uint8_t rangeType = RANGE_4_70_GA;
    writeHMC5883LRegister(REGISTER_CONFIG_B, rangeType << 5);
    updateResolution(rangeType);
    // Register mode (High speed I2C = false, Measurement mode = continuous) -> 0x00000000 (0x00)
    writeHMC5883LRegister(REGISTER_REG_MODE, 0x00);
    // printf("(a) MODE: 0x%2X\r\n", readHMC5883LRegister8(REGISTER_REG_MODE));
    // wait for measurement finished
    do {
        wait_us(1);
    }while (!dataReady());
    // Read first measurement (based on previous configuration). Next measurement will based on new configuration
    xyzFloat mag = readMagneticField();

    return true;
}

uint8_t HMC5883L::whoAmI(uint8_t ID){
    return readHMC5883LRegister8(ID);
}

void HMC5883L::writeHMC5883LRegister(uint8_t reg, uint8_t val){
    char msg[2] = {reg, val};
    _i2c->write(HMC5883L_WRITE_ADDRESS, msg, 2, false);
    wait_us(5e2);
}

uint8_t HMC5883L::readHMC5883LRegister8(uint8_t reg){
    char msg[1] = {reg};
    _i2c->write(HMC5883L_WRITE_ADDRESS, msg, 1, false);

    char recievedData[1];
    _i2c->read(HMC5883L_READ_ADDRESS, recievedData, 1, false);

    return recievedData[0];
}

bool HMC5883L::dataReady(){
    return readHMC5883LRegister8(REGISTER_STATUS) & 0x01;
}

/* Private functions */

xyzFloat HMC5883L::readMagneticField(){
    // Measurement in [gauss]
    // 10000 gauss = 1 tesla
    xyzFloat measurement;

    char request[1] = {REGISTER_DO_X_MSB};
    _i2c->write(HMC5883L_WRITE_ADDRESS, request, 1, false);

    char recievedData[6];
    _i2c->read(HMC5883L_READ_ADDRESS, recievedData, 6, false);

    measurement.x = (int16_t)((recievedData[0] << 8) | recievedData[1]) * _resolution;
    measurement.y = (int16_t)((recievedData[2] << 8) | recievedData[3]) * _resolution;
    measurement.z = (int16_t)((recievedData[4] << 8) | recievedData[5]) * _resolution;

    // Gauss to tesla
    measurement /= 10e3;

    return measurement;
}

void HMC5883L::updateResolution(uint8_t rangeType){
    switch (rangeType) {
        case RANGE_0_88_GA:
            _resolution = 0.73;
            break;
        case RANGE_1_33_GA:
            _resolution = 0.92;
            break;
        case RANGE_1_90_GA:
            _resolution = 1.22;
            break;
        case RANGE_2_50_GA:
            _resolution = 1.52;
            break;
        case RANGE_4_00_GA:
            _resolution = 2.27;
            break;
        case RANGE_4_70_GA:
            _resolution = 2.56;
            break;
        case RANGE_5_60_GA:
            _resolution = 3.03;
            break;
        case RANGE_8_10_GA:
            _resolution = 4.35;
            break;
    }
}