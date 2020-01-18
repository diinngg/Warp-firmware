#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"


#define BUFFER_LENGTH 1
//#define USE_INBUFFER
#ifdef USE_INBUFFER
volatile uint8_t	inBuffer[BUFFER_LENGTH];
#endif
volatile uint8_t	payloadBytes[BUFFER_LENGTH];



/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 13),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};



int
devSSD1331Write(const uint8_t commandByte)
{
	spi_status_t status;

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	payloadBytes[0] = commandByte;
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[0],
#ifdef USE_INBUFFER
					(uint8_t * restrict)&inBuffer[0],
#else
					NULL,
#endif
					1		/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}



int
devSSD1331Init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	enableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 13u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio);

	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);

	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	devSSD1331Write(kSSD1331CommandDISPLAYOFF);	// 0xAE
	devSSD1331Write(kSSD1331CommandSETREMAP);		// 0xA0
	devSSD1331Write(0x72);				// RGB Color
	devSSD1331Write(kSSD1331CommandSTARTLINE);		// 0xA1
	devSSD1331Write(0x0);
	devSSD1331Write(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	devSSD1331Write(0x0);
	devSSD1331Write(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	devSSD1331Write(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	devSSD1331Write(0x3F);				// 0x3F 1/64 duty
	devSSD1331Write(kSSD1331CommandSETMASTER);		// 0xAD
	devSSD1331Write(0x8E);
	devSSD1331Write(kSSD1331CommandPOWERMODE);		// 0xB0
	devSSD1331Write(0x0B);
	devSSD1331Write(kSSD1331CommandPRECHARGE);		// 0xB1
	devSSD1331Write(0x31);
	devSSD1331Write(kSSD1331CommandCLOCKDIV);		// 0xB3
	devSSD1331Write(0xF0);				// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	devSSD1331Write(kSSD1331CommandPRECHARGEA);	// 0x8A
	devSSD1331Write(0x64);
	devSSD1331Write(kSSD1331CommandPRECHARGEB);	// 0x8B
	devSSD1331Write(0x78);
	devSSD1331Write(kSSD1331CommandPRECHARGEA);	// 0x8C
	devSSD1331Write(0x64);
	devSSD1331Write(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	devSSD1331Write(0x3A);
	devSSD1331Write(kSSD1331CommandVCOMH);		// 0xBE
	devSSD1331Write(0x3E);
	devSSD1331Write(kSSD1331CommandMASTERCURRENT);	// 0x87
	devSSD1331Write(0x06);
	devSSD1331Write(kSSD1331CommandCONTRASTA);		// 0x81
	devSSD1331Write(0x91);
	devSSD1331Write(kSSD1331CommandCONTRASTB);		// 0x82
	devSSD1331Write(0x50);
	devSSD1331Write(kSSD1331CommandCONTRASTC);		// 0x83
	devSSD1331Write(0x7D);
	devSSD1331Write(kSSD1331CommandDISPLAYON);		// Turn on oled panel

	/*
	 *	Clear Screen
	 */
	devSSD1331ClearFull();

	return 0;
}


void
devSSD1331ClearFull(void)
{
	devSSD1331Write(kSSD1331CommandCLEAR);
	devSSD1331Write(00); // Start column address
    devSSD1331Write(00); // Start row address
    devSSD1331Write(95); // End column address
    devSSD1331Write(63); // End row address
}



void
devSSD1331Clear(const screenCoord *start, const screenCoord *end)
{
	devSSD1331Write(kSSD1331CommandCLEAR);
	devSSD1331Write(min(start->x, end->x)); // Start column address
    devSSD1331Write(min(start->y, end->y)); // Start row address
    devSSD1331Write(max(start->x, end->x)); // end column address
    devSSD1331Write(max(start->y, end->y)); // end row address
}



void
devSSD1331FillScreen(const screenColor *color)
{
	devSSD1331DrawRect(&(screenCoord){00, 00}, &(screenCoord){95, 63}, color, true);
}



void
devSSD1331DrawLine(const screenCoord *start, const screenCoord *end, const screenColor *color)
{
	devSSD1331Write(kSSD1331CommandDRAWLINE);
    devSSD1331Write(start->x); // Start column address
    devSSD1331Write(start->y); // Start row address
    devSSD1331Write(end->x); // end column address
    devSSD1331Write(end->y); // end row address
    devSSD1331Write(color->r); // Line colour C
    devSSD1331Write(color->g); // Line colour B
    devSSD1331Write(color->b); // Line colour A
}



void
devSSD1331DrawRect(const screenCoord *start, const screenCoord *end, const screenColor *color, const uint8_t fill)
{
	devSSD1331Write(kSSD1331CommandFILL);
	devSSD1331Write(fill);
	devSSD1331Write(kSSD1331CommandDRAWRECT);
    devSSD1331Write(min(start->x, end->x)); // Start column address
    devSSD1331Write(min(start->y, end->y)); // Start row address
    devSSD1331Write(max(start->x, end->x)); // end column address
    devSSD1331Write(max(start->y, end->y)); // end row address
    devSSD1331Write(color->r); // Line colour C
    devSSD1331Write(color->g); // Line colour B
    devSSD1331Write(color->b); // Line colour A
    devSSD1331Write(color->r); // Fill colour C
    devSSD1331Write(color->g); // Fill colour B
    devSSD1331Write(color->b); // Fill colour A
}