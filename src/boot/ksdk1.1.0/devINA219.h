
#ifndef WARP_BUILD_ENABLE_DEVINA219
#define WARP_BUILD_ENABLE_DEVINA219
#endif

void		initINA219(const uint8_t i2cAddress, WarpI2CDeviceState volatile * deviceStatePointer);
WarpStatus	readSensorRegisterINA219(uint8_t deviceRegister, int numberOfBytes);
WarpStatus	writeSensorRegisterINA219(uint8_t deviceRegister, uint16_t payload);
WarpStatus	configureSensorINA219(uint16_t payloadCONFIG, uint16_t payloadCALIB, float rSHUNT);

WarpStatus  readConversionReadyINA219(bool * cnvr);
WarpStatus  readMathOverflowINA219(bool * ovf);

WarpStatus  readShuntVoltageINA219(uint16_t * voltage);
WarpStatus  readBusVoltageINA219(uint16_t * voltage);
WarpStatus  readPowerINA219(uint16_t * power);
WarpStatus  readCurrentINA219(uint16_t * current);