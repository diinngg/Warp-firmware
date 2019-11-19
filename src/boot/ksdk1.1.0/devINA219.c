
#include <stdlib.h>

#include "fsl_i2c_master_driver.h"

#include "SEGGER_RTT.h"
#include "warp.h"


extern volatile WarpI2CDeviceState	deviceINA219State;
extern volatile uint32_t		gWarpI2cBaudRateKbps;
extern volatile uint32_t		gWarpI2cTimeoutMilliseconds;

uint8_t registerPointer = 0;

#define shuntVoltageLSB 10  /* in microVolts uV */ 
#define busVoltageLSB 4     /* in milliVolts mV */
uint16_t powerLSB = 0;      /* in milliWatts mW */
uint16_t currentLSB = 0;    /* in microAmps uA  */


void
initINA219(const uint8_t i2cAddress, WarpI2CDeviceState volatile * deviceStatePointer)
{
	deviceStatePointer->i2cAddress	= i2cAddress;

	return;
}

WarpStatus
writeSensorRegisterINA219(uint8_t deviceRegister, uint16_t payload)
{
	uint8_t	payloadBytes[2], commandByte[1];
	i2c_status_t status;

	switch (deviceRegister)
	{
		case 0x00: case 0x05:
		{
			/* OK */
			break;
		}
		
		default:
		{
			return kWarpStatusBadDeviceCommand;
		}
	}

	i2c_device_t slave =
	{
		.address = deviceINA219State.i2cAddress,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

	commandByte[0] = deviceRegister;
    registerPointer = deviceRegister;
    payloadBytes[0] = (uint8_t)(payload >> 8);
	payloadBytes[1] = (uint8_t)payload;
	status = I2C_DRV_MasterSendDataBlocking(
							0 /* I2C instance */,
							&slave,
							commandByte,
							1,
							payloadBytes,
							2,
							gWarpI2cTimeoutMilliseconds);

	if (status != kStatus_I2C_Success)
	{
        registerPointer = -1; /* Register pointer might be unknown */
		return kWarpStatusDeviceCommunicationFailed;
    }

	return kWarpStatusOK;
}

WarpStatus
readSensorRegisterINA219(uint8_t deviceRegister, int numberOfBytes)
{
	uint8_t	commandByte[1] = {0xFF};
	i2c_status_t status;

	USED(numberOfBytes);
	switch (deviceRegister)
	{
		case 0x00: case 0x01: case 0x02: case 0x03: 
		case 0x04: case 0x05:
		{
			/* OK */
			break;
		}
		
		default:
		{
			return kWarpStatusBadDeviceCommand;
		}
	}

	i2c_device_t slave =
	{
		.address = deviceINA219State.i2cAddress,
		.baudRate_kbps = gWarpI2cBaudRateKbps
	};

    uint8_t commandSize = 0;
    if (deviceRegister != registerPointer)
    {
        /* Need to change register pointer */
        commandSize = 1;
        commandByte[0] = deviceRegister;
        registerPointer = deviceRegister;
    }

	status = I2C_DRV_MasterReceiveDataBlocking(
							0 /* I2C peripheral instance */,
							&slave,
							(commandSize > 0) ? commandByte : NULL, /* Needs to be NULL to work with commandSize = 0 */
							commandSize,
							(uint8_t *)deviceINA219State.i2cBuffer,
							numberOfBytes,
							gWarpI2cTimeoutMilliseconds);

	if (status != kStatus_I2C_Success)
	{
        registerPointer = -1; /* Register pointer might be unknown */
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}

WarpStatus
configureSensorINA219(uint16_t payloadCONFIG, uint16_t payloadCALIB, float rSHUNT)
{
    WarpStatus i2cWriteStatus1, i2cWriteStatus2;

	i2cWriteStatus1 = writeSensorRegisterINA219(0x00, payloadCONFIG);
	i2cWriteStatus2 = writeSensorRegisterINA219(0x05, payloadCALIB);

    currentLSB = (uint16_t)(40960 / (rSHUNT * payloadCALIB));
    powerLSB = (2 * currentLSB) / 100;

	return (i2cWriteStatus1 | i2cWriteStatus2);
}

WarpStatus
readConversionReadyINA219(bool * cnvr)
{
    WarpStatus status = readSensorRegisterINA219(0x02, 2);
    
    *cnvr = (bool)(deviceINA219State.i2cBuffer[1] & 0x02);

    return status;
}

WarpStatus
readMathOverflowINA219(bool * ovf)
{
    WarpStatus status = readSensorRegisterINA219(0x02, 2);
    
    *ovf = (bool)(deviceINA219State.i2cBuffer[1] & 0x01);

    return status;
}

WarpStatus
readShuntVoltageINA219(int16_t * uV)
{
    WarpStatus status = readSensorRegisterINA219(0x01, 2);

    *uV = deviceINA219State.i2cBuffer[1];
    *uV |= (uint16_t)deviceINA219State.i2cBuffer[0] << 8;
    *uV *= shuntVoltageLSB;

    return status;
}

WarpStatus
readBusVoltageINA219(uint16_t * mV)
{
    WarpStatus status = readSensorRegisterINA219(0x02, 2);

    *mV = deviceINA219State.i2cBuffer[1];
    *mV |= (uint16_t)deviceINA219State.i2cBuffer[0] << 8;
    *mV >>= 3; /* Get rid of CNVR and OVF bits */
    *mV *= busVoltageLSB;   

    return status;
}

WarpStatus
readPowerINA219(uint16_t * mW)
{
    WarpStatus status = readSensorRegisterINA219(0x03, 2);

    *mW = deviceINA219State.i2cBuffer[1];
    *mW |= (uint16_t)deviceINA219State.i2cBuffer[0] << 8;
    *mW *= powerLSB;

    return status;
}

WarpStatus
readCurrentINA219(uint16_t * uA)
{
    WarpStatus status = readSensorRegisterINA219(0x04, 2);

    *uA = deviceINA219State.i2cBuffer[1];
    *uA |= (uint16_t)deviceINA219State.i2cBuffer[0] << 8;
    *uA *= currentLSB;

    return status;
}