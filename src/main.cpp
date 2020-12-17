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
=========================================================================== */

#include <Arduino.h>
#include <SPI.h>
#include "proj_config.h"
#include "ft81x_io.h"
#include "ft81x.h"
#include "cf_gt911.h"
#include "ms_can.h"
//#include "ledbar.h"
#include "screens.h"
#include "exoGauge.h"

// timing macro
uint32_t a;
#define tic a = micros();
#define toc                         \
	Serial.print(micros() - a - 1); \
	Serial.println("uS");

uint16_t dlSize;

// float MPH = msCAN_Data[adrSpeed].U08[0] * 0.1f;
exoGauge gMPH(
	110, 220, 100,
	135, 45, // orig 225, 135
	0, 120,
	3,
	10, 80, 2,
	5, 85, 0.8,
	65, "%0.0f", 1, 26,
	2.5, 0,
	"MPH", 28,
	"%0.0f", 1, 29);

exoGauge gRPM(
	400, 220, 100,
	135, 45,
	0, 8000,
	3,
	1000, 80, 2,
	250, 85, 0.8,
	65, "%0.0f", 0.001, 26,
	2.5, 0,
	"RPM", 28,
	"%0.0f", 1, 29);

exoGauge gCLT(
	255, 370, 100,								// x,y, radius
												//	0, 270,				// start, end angle
												//  -160, 110,
	180.0f, 90.0f,								//angle
	coolantT.zone[zone0], coolantT.zone[zone3], // start, end vals
	3,											// circle weight
	30, 80, 2,									// major spacing, iradius, width
	15, 85, 0.8,								// minor spacing, iradius, width
	65, "%0.0f", 1, 26,							// major_num_radius, major_format, multiplier, major_font
	2.5, 210,									// dial_weight, float value
	"CL-T", 28,									// gauge_name_label, gauge_label_font
	"%0.0f", 1, 29								// value_format, value_multiplier, value_font
);

exoGauge gCLP(
	545, 370, 100,
	0, 270,
	0, 15,
	3,
	5, 80, 2,
	1, 85, 0.8,
	65, "%0.0f", 1, 26,
	2.5, 0, // 2.5, CLP,
	"CL-P", 28,
	"%0.1f", 1, 29);

//Oil Pressure
//I have set this up as a generic input on sensor7
//sensor07 = scalar, S16,  116, "", 0.1000, 0.0

//it's in KPA, convert to PSI
//OILP /= 6.895f;
exoGauge gOILP(
	690, 220, 100,
	0, 270,
	0, 60,
	3,
	10, 80, 2,
	5, 85, 0.8,
	65, "%0.0f", 1, 26,
	2.5, 0,
	"OIL-P", 28,
	"%0.0f", 1, 29);

//globals
char _debugbuf[DPRINT_SIZE];

void core()
{
	uint16_t pos = 0;
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
		tic

		msCAN_Check(); // check for CAN data

		//check for screen touch
		touched = !(FT81x_R32(REG_CTOUCH_TOUCH_XY + RAM_REG) & 0x8000);


		FT81x_FIFO_WaitUntilEmpty();	// wait for DL is empty
		FT81x_SendCommand(CMD_DLSTART); // a new list of commands, pointer location == 1
		////////	 	FT81x_SendCommand(CLEAR(1,0,0));
		//draw display
		//see screen1demo (s1demo.cpp) for detailed comments

		// switch (screen_num)
		// {
		// 	case 0: gaugePage1(); break;	// gauges w/ can data
		// 	case 1: textPage1(); break;  	// text
		// 	//case 2: demoPage(pos); break; 	// gauges w/ fake data
		// }
		// gCLT.cx = 400;
		// gCLT.cy = 166;
		// gCLT.value_end = 260;
		//	gCLT.value = ((msCAN_S16(msCAN_Data[adrgCLT].S16[0]) - 40) * 9 / 5) + 32;
		//gCLT.drawGauge();  // draws gauge minus needle and digital value

		FT81x_SendCommand(CMD_APPEND);  // draws needle and digital value
		FT81x_SendCommand(3000L);
		FT81x_SendCommand(dlSize);
		float MPH = msCAN_S16(msCAN_Data[adrSpeed].S16[0]);		// get CAN bus value 
		gMPH.setValue(MPH);
		float RPM = msCAN_S16(msCAN_Data[adrRPM].S16[0]);
		gRPM.setValue(RPM);
		float CLT = ((msCAN_S16(msCAN_Data[adrCoolantT].S16[0]) - 40) * 9 / 5) + 32;
		gCLT.setValue(CLT);											  
		float CLP = msCAN_S16(msCAN_Data[adrCoolantP].S16[0]) * 0.1f; 
		gCLP.setValue(CLP);											 
		float OILP = msCAN_S16(msCAN_Data[adrOilPres].U08[0]);
		gOILP.setValue(OILP);

		FT81x_SendCommand(DISPLAY()); 	// fifo an end-of-commands marker
		FT81x_SendCommand(CMD_SWAP);	// disp new list
		FT81x_UpdateFIFO(); 			// start excuting the commands in the FIFO.

		//debounce
		if (!touch_check && touched)
			//screen has been touched, but was not touched on last loop
			touch_check = 1;
		if (touch_check && !touched)
		{
			//no longer touched, but was on last loop = touched and released
			//rotate screens
			screen_num++; //flip screens if display was touched
			if (screen_num >= SCREENS)
				screen_num = 0;
			//reset
			touch_check = 0;
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
		toc
	}
}

void setup()
{
	setCpuFrequencyMhz(240); // max = 240MHz
	Serial.begin(115200);	 // for debug
	DPRINT("\n\nSETUP STARTED\n");

	msCAN_Init(); //can setup

	DPRINT("LCD SETUP\n");
	SPI_Setup(); //lcd io setup
	FT81x_Init();
	FT81x_SetBacklight(1); // 0 = off, 1 = dimmest, 128 = brightest
	SPI_FullSpeed();	   // 20MHz, 30M with errors, 24M is ok. Increase SPI rate after init
	DPRINTF("SPI-CLK=%d\n", spiClockDivToFrequency(SPI.getClockDivider()));
	DPRINT("SETUP DONE\n");

	FT81x_SendCommand(CMD_DLSTART); // a new list of commands, pointer location == 1
	FT81x_SendCommand(CLEAR_COLOR_RGB(0, 0, 0));  // init screen to black
	FT81x_SendCommand(CLEAR(1, 1, 1));
	gMPH.drawGauge();				// draw gauges minus needle and digital value
	gRPM.drawGauge();
	gCLT.drawGauge(); 
	gCLP.drawGauge();
	gOILP.drawGauge();
	FT81x_UpdateFIFO();
	FT81x_FIFO_WaitUntilEmpty(); // wait for coproc to finish. We have created the static image 							
	dlSize = FT81x_R16(RAM_REG + REG_CMD_DL); // Copy this DL into RAM_G instead of sending over SPI each time
	FT81x_Memcpy(3000L, RAM_DL, dlSize);	  // dest, src, cnt. copy DL to GL

	//DPRINTF("dlSize=%d\n", dlSize); // 0d448 for one dial

	core(); //main loop
}

void loop(void)
{
	//not used
}