/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 C:\Users\EliteDesk\Documents\PlatformIO\Projects\squirtDash\src\main.cpp

 This project is based around:
 - NodeMCU-32s ESP32 (WROOM-32) module
   
 - FT813 EVE lcd graphics chip

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

https://github.com/exothink/megasquirt-dash-display								je 12/3/20

		// this is what broke the orig code in init().  Need div by 2
		FT81x_W8(REG_PCLK + RAM_REG, 5); // 60MHz/5 = 12MHz  need 60/2 = 30MHz

		takes 4.6 secs/demo loop
=========================================================================== */

#include <Arduino.h>
#include <SPI.h>
#include "proj_config.h"
#include "ft81x_io.h"
#include "ft81x.h"
#include "cf_gt911.h"
#include "ms_can.h"
#include "ledbar.h"
#include "screens.h"

// timing macro 
uint32_t a;
#define tic a = micros();
#define toc Serial.print(micros()-a-1); Serial.println("uS");

//globals
char _debugbuf[DPRINT_SIZE];

void core()
{
	uint16_t pos = 0;
	uint8_t touched = 0;
	uint8_t touch_check = 0;
	uint8_t screen_num = 0;
	DPRINT("CORE LOOP\n");

	while(1)
	{
//	tic		
		//gather data
		msCAN_Check();

		//check for screen touch
		touched = !(FT81x_R32(REG_CTOUCH_TOUCH_XY + RAM_REG) & 0x8000);

		//The EVE2 has a display list (a list of commands that it processes on every
		//scan line update, this runs continuously), and a co-processor command-list.
		//Either can be used to draw to the display, but using the co-processor has some advantages.
		//When signaled, the co-processor executes the command list in its RAM FIFO
		//buffer, and writes to the display list.

		//for detailed information on using the EVE2 (FT8xx) graphics Accelerator
		//see: https://www.ftdichip.com/Support/Documents/ProgramGuides/FT800%20Programmers%20Guide.pdf

		//check if EVE2 co-processor command list fifo is empty
		//we write commands into the EVE2 co-processor's fifo, which we then signal
		//it to process later. All FT81x_SendCommand() commands go onto the fifo.
		//They are not processed by the EVE2 co-processor until FT81x_UpdateFIFO() is called.
		FT81x_FIFO_WaitUntilEmpty();
		FT81x_SendCommand(CMD_DLSTART);  //tell the co-processor we are starting a new list of commands
		FT81x_SendCommand(CLEAR(1,1,1));
		//draw display
		//see screen1demo (s1demo.cpp) for detailed comments
		// timing macro 

		switch (screen_num)
		{
			case 0: gaugePage1(); break;	// gauges w/ can data	
			case 1: textPage1(); break;  	// text
			//case 2: demoPage(pos); break; 	// gauges w/ fake data
		}

		FT81x_SendCommand(DISPLAY());  		// fifo an end-of-commands marker
		FT81x_SendCommand(CMD_SWAP);
		FT81x_UpdateFIFO();  				//start excuting the commands in the FIFO.

		//debounce
		if (!touch_check && touched)
			//screen has been touched, but was not touched on last loop
			touch_check = 1;
		if (touch_check && !touched)
		{
			//no longer touched, but was on last loop = touched and released
			//rotate screens
			screen_num++;					//flip screens if display was touched
			if (screen_num >= SCREENS)
				screen_num = 0;
			//reset
			touch_check = 0;
		}

	//	demo screen counter
		pos++;
		if (pos > 100)  // dummy gauge data for demo
		{
			DPRINT("LOOP100  ");
			DPRINT(screen_num);
			DPRINT("\n");
			pos = 0;
		}
//	toc
	}
}

void setup()
{
	//max out the ESP32
	setCpuFrequencyMhz(240);

	//debug output
	Serial.begin(115200);
	DPRINT("\n\nSETUP STARTED\n");

	//led bar
//	LEDBar_Init();

	//can setup
	msCAN_Init();

	//lcd io setup
	DPRINT("LCD SETUP\n");
	SPI_Setup();
	FT81x_Init();
	FT81x_SetBacklight(1);

	//increase SPI rate after init
	SPI_FullSpeed();

	//inits done
	DPRINTF("SPI-CLK=%d\n", spiClockDivToFrequency(SPI.getClockDivider()));
	DPRINT("SETUP DONE\n");

	//main loop
	core();
}

void loop(void)
{
	//not used
}
