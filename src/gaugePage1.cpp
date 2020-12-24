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

//MS INI data in this example is for MS3-pre1.5.2beta4 firmware

void gaugePage1(void)
{
	//top RPM bar
	//rpm = scalar, U16, 6, "RPM", 1.000, 0.0
	float RPM = msCAN_U16(msCAN_Data[adrRPM].U16[0]);  //je
	Gauge_HDashBar(20, 10, 800-20, 20,
		8, 15,
		COLOR_RGB(10,10,30),
		COLOR_RGB(0,255,0),
		COLOR_RGB(255,153,0), 6400,
		COLOR_RGB(255,0,0), 7000,
		BARS_SHOWTEXT,
		0, 7200, RPM,
		140, 22,
		"", "%0.0f", 1, 30);  // 31


	//AFR gauge
	//called AFR0 in TunerStudio, but AFR1 in MS ini
	//afr1 = scalar, U08,  252, "AFR", 0.1, 0.0
	float AFR = msCAN_Data[adrSpeed].U08[0] * 0.1f;
	Gauge_RDial(
		110,220, 100,
		225-360, 135,  // orig 225, 135
		10, 20,
		3,
		2, 80, 2,
		0.5, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, AFR,
		"AFR", 28,
		"%0.1f", 1, 29
	);

	//MAP gauge
	//map = scalar, S16,   18, "kPa", 0.100, 0.0
	float MAP = msCAN_S16(msCAN_Data[2].S16[1]) * 0.1f;
	//convert from KPA to PSI
	MAP /= 6.895f;
	Gauge_RDial(
		400,220, 100,
		225, 135,
		-10, 25,
		3,
		5, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, MAP,
		"MAP", 28,
		"%0.1f", 1, 29
	);

	//Oil Pressure
	//I have set this up as a generic input on sensor7
	//sensor07 = scalar, S16,  116, "", 0.1000, 0.0
	float OILP = msCAN_S16(msCAN_Data[adrOilPres].U08[0]);
	Gauge_RDial(
		690,220, 100,
		0, 270,
		0, 80,
		3,
		10, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, OILP,
		"OIL-P", 28,
		"%0.0f", 1, 29
	);

	//Coolant Temperature (celcius)
	//coolant = scalar, S16,   22, "°C",  0.05555, -320.0
	//float CLT = (msCAN_S16(msCAN_Data[2].S16[3]) - 320) * 0.05555f;
	float CLT = ((msCAN_S16(msCAN_Data[adrCoolantT].S16[0]) - 40) * 9 / 5) + 32;  //ok
	Gauge_RDial(
		255,370, 100, 		// x,y, radius
	    180.0f, 90.0f, //angle
		coolantT.zone[zone0], coolantT.zone[zone3],  // start, end vals
		3,					// circle weight
		25, 80, 2,			// major spacing, iradius, width
		5, 85, 0.8,			// minor spacing, iradius, width
		65, "%0.0f", 1, 26, // major_num_radius, major_format, multiplier, major_font
		2.5, CLT,			// dial_weight, float value
		"CL-T", 28,			// gauge_name_label, gauge_label_font
		"%0.0f", 1, 29		// value_format, value_multiplier, value_font 
	);

	// //Coolant Pressure
	float CLP = msCAN_S16(msCAN_Data[adrCoolantP].U08[0]);
	Gauge_RDial(
		545,370, 100,
		135, 45,
		0, 25,
		3,
		5, 80, 2,
		1, 85, 0.8,
		65, "%0.0f", 1, 26,
		2.5, CLP,
		"CL-P", 28,
		"%0.1f", 1, 29
	);

}

// void Gauge_RDial(uint16_t cx, uint16_t cy, uint16_t circle_radius,
// 	float angle_start, float angle_end,
// 	float value_start, float value_end,
// 	uint8_t circle_weight,
// 	float major_spacing, float major_iradius, float major_width,
// 	float minor_spacing, float minor_iradius, float minor_width,
// 	float major_num_radius, const char *major_format, float major_multiplier, uint8_t major_font,
// 	float dial_weight, float value,
// 	const char *gauge_name_label, uint8_t gauge_label_font,
// 	const char *value_format, float value_multiplier, uint8_t value_font
// )
