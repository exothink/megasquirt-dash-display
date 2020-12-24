/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple Display Screen

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#include <Arduino.h>
#include "proj_config.h"
#include "ft81x.h"
#include "gauge_bars.h"
#include "gauge_rdial.h"
#include "ms_can.h"

#define VALUEPAD 95  //130
#define FONTSIZE 30
#define FONTHEIGHT 42

static void textDataf(uint16_t x, uint16_t y, float value, const char *label, const char *format,
					  float low_warn, float low_err, float high_warn, float high_err)
{
	char temps[32];
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	sprintf(temps, format, value);
	FT81x_Text(x + VALUEPAD, y, FONTSIZE, 0, temps);
	if (value < low_err) // red
		FT81x_SendCommand(COLOR_RGB(255, 50, 50));
	else if (value < low_warn) // yel
		FT81x_SendCommand(COLOR_RGB(255, 200, 100));
	else if (value > high_err) // red
		FT81x_SendCommand(COLOR_RGB(255, 50, 50));
	else if (value > high_warn) // yel
		FT81x_SendCommand(COLOR_RGB(255, 200, 100));
	else
		FT81x_SendCommand(COLOR_RGB(120, 255, 120)); // green OK
	FT81x_Text(x, y, FONTSIZE, 0, label);
}

static void textDatai(uint16_t x, uint16_t y, int value, const char *label, const char *format,
					  int low_warn, int low_err, int high_warn, int high_err)
{
	char temps[32];
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	sprintf(temps, format, value);
	FT81x_Text(x + VALUEPAD, y, FONTSIZE, 0, temps);
	if (value < low_err)
		FT81x_SendCommand(COLOR_RGB(255, 50, 50));
	else if (value < low_warn)
		FT81x_SendCommand(COLOR_RGB(255, 255, 50));
	else if (value > high_err)
		FT81x_SendCommand(COLOR_RGB(255, 50, 50));
	else if (value > high_warn)
		FT81x_SendCommand(COLOR_RGB(255, 255, 50));
	else
		FT81x_SendCommand(COLOR_RGB(100, 255, 100));
	FT81x_Text(x, y, FONTSIZE, 0, label);
}

void textPage1(void)
{
	FT81x_SendCommand(CLEAR(1, 1, 1));
	//thin RPM bar
	float RPM = msCAN_U16(msCAN_Data[adrRPM].U16[0]); //je

	Gauge_HDashBar(20, 10, 800 - 20, 40,
				   4, 9,
				   COLOR_RGB(30, 30, 140),
				   COLOR_RGB(0, 255, 0),
				   COLOR_RGB(255, 153, 0), rpm.zone[1],
				   COLOR_RGB(255, 0, 0), rpm.zone[2],
				   BARS_SHOWTEXT,
				   0, rpm.zone[3], RPM,
				   100, 16,
				   "", "%0.0f", 1, 30);

	uint16_t xPos = 100;	
	uint16_t yPos = 20;


	//Seconds
	// uint16_t Seconds = msCAN_U16(msCAN_Data[0].U16[0]);
	// textDatai(xPos, yPos += FONTHEIGHT, Seconds, "UpT:", "%d s", 30, 1, 65536, 65536);

	//Aux Battery Voltage
	float auxV = msCAN_U16(msCAN_Data[adrAuxBat].U08[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, auxV, "ABAT:", "%0.1f V", 11.5, auxBat.zone[1], auxBat.zone[2], 15.0);

	//Battery Voltage
	float BATTV = msCAN_S16(msCAN_Data[adrBattery].U08[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, BATTV, "BATT:", "%0.1f V", 11.5, battery.zone[1], battery.zone[2], 15.0); // red, yel, yel, red

	//Coolant Pressure
	float CLPres = msCAN_S16(msCAN_Data[adrCoolantP].U08[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, CLPres, "CLP:", "%0.1f psi", 0, coolantP.zone[1], coolantP.zone[3], coolantP.zone[2]);
	//																y				r			y					r		0, 5, 10, 15
	//Coolant Temperature (fahrenheit)
	float CLT = ((msCAN_S16(msCAN_Data[adrCoolantT].S16[0]) - 40) * 9 / 5) + 32; //ok
	textDataf(xPos, yPos += FONTHEIGHT, CLT, "CLT:", "%0.0f F", 0, 0, coolantT.zone[1], coolantT.zone[2]);

	//RPM
	textDatai(xPos, yPos += FONTHEIGHT, RPM, "RPM:", "%d", 0, 0, rpm.zone[1], rpm.zone[2]);
	//													<val=yel, 	<val=red, 	>val=yel, 		>val=red    else green

	//MAP gauge
	float MAP = msCAN_S16(msCAN_Data[2].S16[1]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, MAP, "MAP:", "%0.1f kPa", 5, 5, 240, 260);

	//TPS
	float TPS = msCAN_S16(msCAN_Data[3].S16[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, TPS, "TPS:", "%0.1f %%", -1, -20, 100, 500);

	//AFR gauge
	float EGO = msCAN_Data[3].U08[0] * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, EGO, "EGO:", "%0.1f AFR", 11, 10.5, 15.5, 17);

	//Manifold Temperature (celcius)
	float MAT = (msCAN_S16(msCAN_Data[2].S16[2]) - 320) * 0.05555f;
	textDataf(xPos, yPos += FONTHEIGHT, MAT, "MAT:", "%0.0f C", 0, -10, 60, 80);

	//Injector PW1
	float PW1 = msCAN_U16(msCAN_Data[0].U16[1]) * 0.001f;
	textDataf(xPos, yPos += FONTHEIGHT, PW1, "PW1:", "%0.3f mS", 0.01, -10, 10, 11);

	xPos = 460;	// next col
	yPos = 20; 
	
	//Ignition Advance
	//advance = scalar, S16,    8, "deg", 0.100, 0.0
	float ADV = msCAN_S16(msCAN_Data[1].S16[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, ADV, "ADV:", "%0.1f deg", -50, -50, 50, 50);

	//AFR Target
	//afrtgt1 = scalar, U08, 12, "AFR", 0.1, 0.0
	float AFRT = msCAN_Data[1].U08[3] * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, AFRT, "AFRT:", "%0.1f AFR", -50, -50, 50, 50);

	//EGO Correction %
	//egoCorrection1   = scalar, S16,   34, "%",   0.1000, 0.0
	float EGOCOR = msCAN_S16(msCAN_Data[4].S16[1]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, EGOCOR, "COR:", "%0.1f %%", 75, 85, 115, 125);

	//Baro gauge
	//barometer = scalar, S16,   16, "kPa", 0.100, 0.0
	float BARO = msCAN_S16(msCAN_Data[2].S16[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, BARO, "BARO:", "%0.1f kpa", 5, 5, 240, 260);

	//Boost Duty 1
	//boostduty = scalar, U08,  139 , "%",  1.0, 0.0
	//2,2,1,1,2
	float BOOSTDTY = msCAN_S16(msCAN_Data[7].U08[4]) * 0.392f;
	textDataf(xPos, yPos += FONTHEIGHT, BOOSTDTY, "BST:", "%0.1f %%", -10, -10, 100, 100);

	//Idle PWM %
	//idleDC = scalar, S16,   54, "%",    0.392, 0.0
	float IDLE = msCAN_S16(msCAN_Data[6].S16[3]) * 0.392f;
	textDataf(xPos, yPos += FONTHEIGHT, IDLE, "IDL:", "%0.1f %%", -10, -10, 100, 100);

	//fuel tank
	float fuelTank = msCAN_S16(msCAN_Data[adrFuelGal].U08[0]) * 0.1f;
	textDataf(xPos, yPos += FONTHEIGHT, fuelTank, "FUEL:", "%0.1f Gal", fuelGal.zone[1], fuelGal.zone[1] - 1 , fuelGal.zone[3], fuelGal.zone[3]);
	//																				0, 2, 3, 16
	//fuel pump pressure
	float fuelPSI = msCAN_S16(msCAN_Data[adrFuelPres].U08[0]) * 1.0f;
	textDataf(xPos, yPos += FONTHEIGHT, fuelPSI, "FP:", "%0.1f psi", fuelPres.zone[1], fuelPres.zone[1] - 5 , fuelPres.zone[2], fuelPres.zone[2] + 5);
	//																	y	r	y	r		0, 27, 32, 40
}
