/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Screens Setup

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#ifndef __SCREENS_H
#define __SCREENS_H

#define SCREENS 3

uint16_t dlSize;

//void gaugePage1(void);
void textPage1(void);
// void demoPage(int pos);

// struct dialFace
// {
//   uint8_t tag;
//   const char *title[1]; // no other syntax works -> *title[1];
//   const char *units[1];
//   // uint8_t num;      // number of zones, 3 max
//   int16_t zone[4];  // zone start and stop values, zone[0] = dialMin, zone[3] = dialMax
//   rgb zoneColor[3]; // zone colors for analog dial
//   int8_t width[3];  // line width of zone
//   rgb digiColor[3]; // zone colors for digital readout
// };

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
	"%0.0f", 1, 30,
	speed); // struct dialFace in proj_config.h

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
	"%0.0f", 1, 30,
	rpm);

exoGauge gCLT(									// Coolant Temp
	255, 370, 100,								// x,y, radius
	50, 340,									// start, end angle
	coolantT.zone[zone0], coolantT.zone[zone3], // start, end vals    coolantT.zone[zone3]
	3,											// circle weight
	30, 80, 2,									// major spacing, iradius, width
	15, 85, 0.8,								// minor spacing, iradius, width
	65, "%0.0f", 1, 26,							// major_num_radius, major_format, multiplier, major_font
	2.5, 210,									// dial_weight, float value
	"CL-T", 28,									// gauge_name_label, gauge_label_font
	"%0.0f", 1, 30,								// value_format, value_multiplier, value_font
	coolantT);

exoGauge gCLP( // Coolant Pressure
	545, 370, 100,
	135, 45, // angles
	0, 15,
	3,
	5, 80, 2,
	1, 85, 0.8,
	65, "%0.0f", 1, 26,
	2.5, 0, // 2.5, CLP,
	"CL-P", 28,
	"%0.1f", 1, 30,
	coolantP);

exoGauge gOILP( //Oil Pressure
	690, 220, 100,
	45, 315,
	0, 60,
	3,
	10, 80, 2,
	2.5, 85, 0.8,
	65, "%0.0f", 1, 26,
	2.5, 0,
	"OIL-P", 28,
	"%0.0f", 1, 30,
	oilPres);

int gDrawStaticPg1()
{
	// FT81x_SendCommand(CMD_(1, 34)); CMD_SETFONT
	// cmd_text(60, 32, 1, 0, "34");
	FT81x_Text(0, 0, 31, 0, "<"); // for next/back to page
	FT81x_Text(780, 0, 31, 0, ">");
	gMPH.drawGauge(); // draw gauges minus needle and digital value
	gRPM.drawGauge();
	gCLT.drawGauge();
	gCLP.drawGauge();
	gOILP.drawGauge();
	return 1;
}

void updateGauges()
{
	FT81x_SendCommand(CMD_APPEND); // draws needle and digital value
	FT81x_SendCommand(3000L);
	FT81x_SendCommand(dlSize);
	// float MPH = msCAN_S16(msCAN_Data[adrSpeed].S16[0]); // get CAN bus value
	float MPH = msCAN_Data[adrSpeed].S16[0]; // get CAN bus value
	gMPH.setValue(MPH);
	// float RPM = msCAN_S16(msCAN_Data[adrRPM].S16[0]);
	float RPM = msCAN_Data[adrRPM].S16[0];
	gRPM.setValue(RPM);
	// float CLT = ((msCAN_S16(msCAN_Data[adrCoolantT].S16[0]) - 40) * 9 / 5) + 32;
	float CLT = ((msCAN_Data[adrCoolantT].S16[0] - 40) * 9 / 5) + 32;
	gCLT.setValue(CLT);
	// float CLP = msCAN_S16(msCAN_Data[adrCoolantP].U08[0]) * 0.1f;
	float CLP = msCAN_Data[adrCoolantP].U08[0] * 0.1f;
	gCLP.setValue(CLP);
	// float OILP = msCAN_S16(msCAN_Data[adrOilPres].U08[0]);
	float OILP = msCAN_Data[adrOilPres].U08[0];
	gOILP.setValue(OILP);

	//time
	char temps[32];
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	int secs = msCAN_Data[adrTime].U08[0];
	sprintf(temps, "%d:%02d:%02d", msCAN_Data[adrTime].U08[2], msCAN_Data[adrTime].U08[1], secs); // hh:mm:ss
	FT81x_Text(10, 450, 30, 0, temps);

	//in/outdoor temp
	//	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	sprintf(temps, "%d/%df", 72, 80);
	FT81x_Text(700, 450, 29, 0, temps);
}

int FT_DispWidth = 800;
int FT_DispHeight = 272;
int xoff = 0;
int yoff = 0;
uint8_t arrow[] = {0x78, 0x9C, 0xBD, 0xD0, 0xA9, 0x11, 0x00, 0x30, 0x0C, 0x04, 0x31, 0xF7, 0x5F, 0xAD, 0x3B, 0xD8,
				   0x70, 0xCD, 0x24, 0x30, 0xC7, 0x2C, 0xE0, 0x6F, 0xE6, 0x7B, 0x96, 0x3A, 0x20, 0x20, 0x20, 0x20,
				   0x20, 0x20, 0x20, 0xE0, 0x52, 0xEF, 0xB3, 0x1F, 0xF3, 0xD8, 0x87, 0x7D, 0xB9, 0x87, 0x7B, 0xFD, 0xC7, 0xD7, 0x1C, 0x6F, 0x57, 0x58, 0x99};
uint8_t arrow1[] = {0x78, 0x9C, 0xDD, 0xD2, 0x4F, 0x0E, 0xC1, 0x50, 0x10, 0x06, 0xF0, 0x47, 0x68, 0x48, 0x69, 0xDD,
					0x80, 0x1B, 0xF4, 0x08, 0xDC, 0xA0, 0x6E, 0xA0, 0x07, 0x91, 0x70, 0x03, 0x6E, 0xE0, 0x08, 0xEC, 0x6D, 0xEC, 0xAC, 0x24, 0x0E,
					0x20, 0xE1, 0x06, 0x28, 0xE2, 0x4F, 0x52, 0xA3, 0xF3, 0xDE, 0x1B, 0x33, 0x2F, 0x6C, 0x6D, 0x7C, 0xBB, 0xDF, 0xA6, 0xDF, 0xBC,
					0xE9, 0x28, 0xF5, 0x9B, 0xCC, 0x5C, 0xD6, 0xCE, 0xAE, 0x23, 0x70, 0xBD, 0x87, 0x44, 0xD2, 0x03, 0x38, 0x48, 0x87, 0x00, 0x57,
					0xE9, 0x09, 0x00, 0x34, 0x84, 0x73, 0x82, 0x68, 0xAC, 0xA3, 0x8F, 0xEC, 0x36, 0x3A, 0x93, 0x6D, 0x98, 0x2E, 0xB1, 0xA2, 0xC9,
					0x8D, 0xA1, 0xF1, 0x8D, 0x3C, 0x35, 0xA6, 0x91, 0x8B, 0x96, 0x30, 0x36, 0x0E, 0xC8, 0xA9, 0x71, 0x4C, 0xCE, 0x78, 0x38, 0x93,
					0x0E, 0xB2, 0xCA, 0xDE, 0x89, 0x36, 0xCC, 0x05, 0xDD, 0x9F, 0xBF, 0xB3, 0x52, 0x7F, 0x90, 0x0D, 0xBF, 0x67, 0x89, 0xEE, 0xF1,
					0x7B, 0xF5, 0xC6, 0x03, 0xB6, 0x5E, 0x68, 0x89, 0x6D, 0x3E, 0xB0, 0x25, 0xDA, 0x1B, 0x8A, 0x9C, 0xF5, 0x89, 0x85, 0xB6, 0x6C,
					0xA3, 0xE5, 0x83, 0x26, 0x18, 0x7D, 0xFF, 0xBF, 0x43, 0x72, 0x59, 0xF3, 0xC9, 0x23, 0xEB, 0xC6, 0x13, 0xBB, 0x89, 0x5E, 0xB3,
					0x7D, 0x71, 0x0D, 0x98, 0x42, 0xCE, 0xBB, 0x7C, 0xF3, 0xE0, 0xF3, 0x7E, 0x13, 0x69, 0x4F, 0xB6, 0xE9, 0x91, 0x53, 0xD7, 0xF1,
					0xC2, 0xB5, 0xAF, 0x7E, 0x94, 0x17, 0x03, 0x64, 0xFA, 0x39};

/* void incdec_setup()
{
  Ft_Gpu_Hal_WrCmd32(phost,CMD_INFLATE);
  Ft_Gpu_Hal_WrCmd32(phost,4585L);
  Ft_Gpu_Hal_WrCmdBuf(phost,arrow1,sizeof(arrow1)); 
  Ft_DlBuffer_Index = 0;
  FT81x_SendCommand(BITMAP_HANDLE(13));    // handle for background arrows
  FT81x_SendCommand(BITMAP_SOURCE(4585L));      // Starting address in g-ram
  FT81x_SendCommand(BITMAP_LAYOUT(L4, 32, 16));  // format 
  FT81x_SendCommand(BITMAP_SIZE(NEAREST, BORDER, BORDER, 32, 16  ));

FT81x_SendCommand(DISPLAY()); // fifo end-of-list
FT81x_SendCommand(CMD_SWAP);  // disp new list
FT81x_UpdateFIFO();			  // start excuting the commands in the FIFO.
  FT81x_FIFO_WaitUntilEmpty();	// wait for DL is empty  Ft_App_Flush_DL_Buffer(phost);
 // Ft_Gpu_DLSwap(DLSWAP_FRAME);

} */

#define CALENDER (0x04)
#define DATE_INCR 5
#define MONTH_INCR 6
#define YEAR_INCR 7
#define AM_PM 11
#define DATE_DEC 8
#define MONTH_DEC 9
#define YEAR_DEC 10
struct RTC
{
	int date = 26;
	int month = 12;
	int year = 20;
};

RTC Rtc;

void calender()
{
	xoff = (FT_DispWidth / 2) - 75;
	yoff = (FT_DispHeight) - (50);
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	FT81x_SendCommand(LINE_WIDTH(3 * 16));
	FT81x_SendCommand(BEGIN(RECTS));
	FT81x_SendCommand(VERTEX2F(xoff * 16, yoff * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 150) * 16, (yoff + 40) * 16));
	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(TAG_MASK(1));
	FT81x_SendCommand(TAG(CALENDER));
	FT81x_SendCommand(VERTEX2F((xoff + 2) * 16, (yoff + 2) * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 148) * 16, (yoff + 38) * 16));
	FT81x_SendCommand(TAG_MASK(0));
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	FT81x_SendCommand(BEGIN(BITMAPS));
	// FT81x_SendCommand(VERTEX2II((xoff+0),(yoff+2),3,0));
	//FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	xoff += 6;
	FT81x_SendCommand(VERTEX2II((xoff), (yoff + 5), 29, (Rtc.month / 10) + '0'));
	FT81x_SendCommand(VERTEX2II((xoff + 13), (yoff + 5), 29, (Rtc.month % 10) + '0')); // width 14+2      //26
	xoff += 41;
	FT81x_SendCommand(VERTEX2II((xoff), (yoff + 5), 29, (Rtc.date / 10) + '0'));
	FT81x_SendCommand(VERTEX2II((xoff + 13), (yoff + 5), 29, (Rtc.date % 10) + '0')); // width 14+2
	xoff += 39;
	FT81x_SendCommand(VERTEX2II((xoff), (yoff + 5), 29, 2 + '0'));
	FT81x_SendCommand(VERTEX2II((xoff + 13), (yoff + 5), 29, 0 + '0')); // width 14+2
	FT81x_SendCommand(VERTEX2II((xoff + 26), (yoff + 5), 29, (Rtc.year / 10) + '0'));
	FT81x_SendCommand(VERTEX2II((xoff + 39), (yoff + 5), 29, (Rtc.year % 10) + '0')); // width 14+2

	FT81x_SendCommand(LINE_WIDTH(1 * 16));
	xoff = (FT_DispWidth / 2) - 75;
	yoff = (FT_DispHeight) - (50);

	FT81x_SendCommand(COLOR_RGB(100, 100, 100));
	FT81x_SendCommand(BEGIN(LINES));
	xoff += 38;
	FT81x_SendCommand(VERTEX2F((xoff)*16, (yoff + 8) * 16));
	FT81x_SendCommand(VERTEX2F((xoff)*16, (yoff + 32) * 16)); // width 14+2
	xoff += 42;
	FT81x_SendCommand(VERTEX2F((xoff)*16, (yoff + 8) * 16));
	FT81x_SendCommand(VERTEX2F((xoff)*16, (yoff + 32) * 16)); // width 14+2
}

void Date_Setting(uint8_t date, uint8_t month, uint8_t year)
{
	xoff = (FT_DispWidth / 2) - 75;
	yoff = (FT_DispHeight) - (50);
	FT81x_SendCommand(COLOR_A(150));
	FT81x_SendCommand(LINE_WIDTH(1 * 16));
	FT81x_SendCommand(COLOR_RGB(0, 255, 0)); // box boarder (big box)
	FT81x_SendCommand(BEGIN(RECTS));
	FT81x_SendCommand(VERTEX2F((xoff - 10) * 16, 25 * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 160) * 16, 155 * 16));
	FT81x_SendCommand(COLOR_RGB(255, 0, 0)); // box fill  (smaller box)
	FT81x_SendCommand(LINE_WIDTH(4 * 16));
	FT81x_SendCommand(VERTEX2F(xoff * 16, 35 * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 150) * 16, 145 * 16));
	FT81x_SendCommand(COLOR_A(255));
	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(LINE_WIDTH(1 * 16));
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(VERTEX2F((xoff + 38) * 16, 38 * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 38) * 16, 142 * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 80) * 16, 38 * 16));
	FT81x_SendCommand(VERTEX2F((xoff + 80) * 16, 142 * 16));
	FT81x_SendCommand(BEGIN(BITMAPS));
	FT81x_SendCommand(VERTEX2II(xoff + 6, 75, 29, (date / 10) + '0'));	//
	FT81x_SendCommand(VERTEX2II(xoff + 19, 75, 29, (date % 10) + '0')); // width 14+2      //26

	FT81x_SendCommand(VERTEX2II(xoff + 47, 75, 29, (month / 10) + '0'));
	FT81x_SendCommand(VERTEX2II(xoff + 60, 75, 29, (month % 10) + '0')); // width 14+2
	FT81x_SendCommand(VERTEX2II(xoff + 90, 75, 29, 2 + '0'));
	FT81x_SendCommand(VERTEX2II(xoff + 103, 75, 29, 0 + '0')); // width 14+2
	FT81x_SendCommand(VERTEX2II(xoff + 116, 75, 29, (year / 10) + '0'));
	FT81x_SendCommand(VERTEX2II(xoff + 129, 75, 29, (year % 10) + '0')); // width 14+2

	FT81x_SendCommand(TAG_MASK(1));
	FT81x_SendCommand(TAG(DATE_INCR));
	FT81x_SendCommand(VERTEX2II(xoff + 3, 45, 25, '+')); // width 14+2  13, 0
														 // FT81x_SendCommand(TAG(MONTH_INCR));
														 // FT81x_SendCommand(VERTEX2II(xoff + 44, 45, 13, 0)); // width 14+2
														 // FT81x_SendCommand(TAG(YEAR_INCR));
														 // FT81x_SendCommand(VERTEX2II(xoff + 100, 45, 13, 0)); // width 14+2*/
														 // FT81x_SendCommand(TAG(DATE_DEC));
														 // FT81x_SendCommand(VERTEX2II(xoff + 3, 120, 13, 1)); // width 14+2
														 // FT81x_SendCommand(TAG(MONTH_DEC));
														 // FT81x_SendCommand(VERTEX2II(xoff + 44, 120, 13, 1)); // width 14+2
														 // FT81x_SendCommand(TAG(YEAR_DEC));
														 // FT81x_SendCommand(VERTEX2II(xoff + 100, 120, 13, 1)); // width 14+2*/
														 // FT81x_SendCommand(TAG_MASK(0));

	FT81x_SendCommand(COLOR_RGB(255, 168, 64));
	FT81x_SendCommand(BEGIN(RECTS));
	FT81x_SendCommand(VERTEX2F(60 * 16, 50 * 16));
	FT81x_SendCommand(VERTEX2F(120 * 16, 110 * 16));

	FT81x_Romfont(1, 34);			// set font handle 1 to font 34
	FT81x_Text(10, 300, 1, 0, "^"); // use handle 1 (font 34)
	FT81x_Romfont(1, 33);			// set font handle 1 to font 33
	FT81x_Text(11, 350, 1, 0, "v"); // use handle 1 (font 33)
}

void timePage()
{
	FT81x_SendCommand(CMD_DLSTART);				  // a new list of commands, pointer location == 1
	FT81x_SendCommand(CLEAR_COLOR_RGB(0, 0, 77)); // init screen to black
	FT81x_SendCommand(CLEAR_TAG(21));
	FT81x_SendCommand(CLEAR(1, 0, 1)); // set color, tag
									   //	FT81x_SendCommand(CLEAR(1, 1, 0));			  //
	FT81x_Text(360, 40, 30, 0, "Setup");
	calender();
	Date_Setting(9, 6, 54);
}

#endif
