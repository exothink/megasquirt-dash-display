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

void gaugePage1(void);
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
    speed);

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
	50, 340,										// start, end angle
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
	135, 45,  // angles
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
			float MPH = msCAN_S16(msCAN_Data[adrSpeed].S16[0]); // get CAN bus value
			gMPH.setValue(MPH);
			float RPM = msCAN_S16(msCAN_Data[adrRPM].S16[0]);
			gRPM.setValue(RPM);
			float CLT = ((msCAN_S16(msCAN_Data[adrCoolantT].S16[0]) - 40) * 9 / 5) + 32;
			gCLT.setValue(CLT);
			float CLP = msCAN_S16(msCAN_Data[adrCoolantP].U08[0]) * 0.1f;
			gCLP.setValue(CLP);
			float OILP = msCAN_S16(msCAN_Data[adrOilPres].U08[0]);
			gOILP.setValue(OILP);
		}

#endif
