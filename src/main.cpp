/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 C:\Users\EliteDesk\Documents\PlatformIO\Projects\squirtDash\src\main.cpp

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

https://github.com/exothink/megasquirt-dash-display								je 12/3/20

		//All FT81x_SendCommand() commands go onto the fifo(RAM_DL).
		//They are not processed by the EVE2 co-processor until FT81x_UpdateFIFO() is called.

		// this is what broke the orig code in init().  Need div by 2
		FT81x_W8(REG_PCLK + RAM_REG, 5); // 60MHz/5 = 12MHz  need 60/2 = 30MHz

		takes 4.6 secs/demo loop
	added color zone to gauge_rdial.cpp	for dials.								12/10	

	adding static/dynamic gauges												12/15
	screen update rate 16.55mS.  60 frames/Sec
	added exoGauge(parms...) constructor of kind Gauge_RDial(parms...)
	five gauges on one screen working											12/16
	RAM:   [=         ]   5.2% (used 17084 bytes from 327680 bytes)
	Flash: [==        ]  18.8% (used 246060 bytes from 1310720 bytes)
	touch screen increments through the pages									12/17
	zones are colored to reflect proj_config.h using "dialFace"	type			12/18
	added shadows, digi vals with color, using config.h data					12/23
	RAM:   [=         ]   5.3% (used 17388 bytes from 327680 bytes)
	Flash: [==        ]  19.0% (used 249088 bytes from 1310720 bytes)
	created macros:
				COLOR_STRUCT(rgb)
				COLOR_EQ(x,y)
	sometimes the lcd doesn't restart.  Added time in 'EVE_Reset'.
		300mS total as per ftdi AE			
=========================================================================== */

#include <Arduino.h>
#include <SPI.h>
#include "proj_config.h"
#include "ft81x_io.h"
#include "ft81x.h"
#include "cf_gt911.h"
#include "ms_can.h"
//#include "ledbar.h"
#include "exoGauge.h"
#include "screens.h"

// execution time macro
uint32_t a;
#define tic a = micros();
#define toc                         \
	Serial.print(micros() - a - 1); \
	Serial.println("uS");

void core();
uint16_t pos = 0;
int staticRendered = 0;
char _debugbuf[DPRINT_SIZE];

void setup()
{
	setCpuFrequencyMhz(240); // max = 240MHz
	Serial.begin(921600);	 // for debug
	//delay(5);				 // esp32 reset is intermittent without delay
	DPRINT("\n\nSETUP STARTED\n");
	msCAN_Init(); //can setup

	DPRINT("LCD SETUP\n");
	SPI_Setup(); //lcd io setup
	FT81x_Init();
	FT81x_SetBacklight(1); // 0 = off, 1 = dimmest, 128 = brightest
	SPI_FullSpeed();	   // 20MHz, 30M with errors, 24M is ok. Increase SPI rate after init
	DPRINTF("SPI-CLK=%d\n", spiClockDivToFrequency(SPI.getClockDivider()));
	DPRINT("SETUP DONE\n");

	FT81x_SendCommand(CMD_DLSTART);				 // a new list of commands, pointer location == 1
	FT81x_SendCommand(CLEAR_COLOR_RGB(0, 0, 0)); // init screen to black
	FT81x_SendCommand(CLEAR(1, 1, 1));			 //
	staticRendered = gDrawStaticPg1();			 // draw gauges without needle and digital value
	FT81x_UpdateFIFO();							 //
	FT81x_FIFO_WaitUntilEmpty();				 // wait for coproc to finish. We have created the static image
	dlSize = FT81x_R16(RAM_REG + REG_CMD_DL);	 // Copy this DL into RAM_G instead of sending over SPI each time
	FT81x_Memcpy(3000L, RAM_DL, dlSize);		 // dest, src, cnt. copy DL to GL
	DPRINTF("dlSize=%d\n", dlSize);				 // 0d448 for one dial
}

void loop(void)
{
	uint8_t touched = 0;
	uint8_t touch_check = 0;
	uint8_t screen_num = 0;
	DPRINT("CORE LOOP\n");

	// Memory block base addresses
	//  RAM_G                    0x0
	//  RAM_DL                   0x300000
	//  RAM_REG                  0x302000
	//  RAM_CMD                  0x308000  FT81x_SendCommand()

	while (1)
	{
		//  tic

		msCAN_Check(); // check for CAN data
		touched = !(FT81x_R32(REG_CTOUCH_TOUCH_XY + RAM_REG) & 0x8000);
		FT81x_FIFO_WaitUntilEmpty();	// wait for DL is empty
		FT81x_SendCommand(CMD_DLSTART); // a new list of commands, pointer location == 1

		switch (screen_num) // screen page to disply
		{
		case 0:
			updateGauges(); // always refreshes static part from RAM_G to RAM_DL
			break;			// and updates vals of gauges with CAN data
		case 1:
			textPage1();
			break;
			//case 2: demoPage(pos); break; 	// gauges w/ fake data
		}

		//	if (screen_num == 0)
		//		updateGauges();  		  // show current measurements
		FT81x_SendCommand(DISPLAY()); // fifo an end-of-commands marker
		FT81x_SendCommand(CMD_SWAP);  // disp new list
		FT81x_UpdateFIFO();			  // start excuting the commands in the FIFO.

		//debounce
		if (!touch_check && touched)
			touch_check = 1;		 // pen was down, but was not touched on last loop
		if (touch_check && !touched) // pen up, but was on last loop = touched and released
		{
			staticRendered = 0;
			screen_num++; // next screen
			if (screen_num >= SCREENS)
				screen_num = 0;
			touch_check = 0; // reset
		}

		//	demo screen counter
		pos++;
		if (pos > 100) // dummy gauge data for demo
		{
			DPRINT("LOOP100  ");
			DPRINT(screen_num);
			DPRINT("\n");
			pos = 0;
		}

		// toc
	}
}