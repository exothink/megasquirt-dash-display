#include <Arduino.h>
#include <math.h>
#include "proj_config.h"
//#include "ft81x.h"
#include "exoGauge.h"

//static const float pi = 3.1415926535f;
 exoGauge::exoGauge()
 {}

 exoGauge::exoGauge(uint16_t _cx, uint16_t _cy, uint16_t _circle_radius,
				 float _angle_start, float _angle_end,
				 float _value_start, float _value_end,
				 uint8_t _circle_weight,
				 float _major_spacing, float _major_iradius, float _major_width,
				 float _minor_spacing, float _minor_iradius, float _minor_width,
				 float _major_num_radius, const char *_major_format, float _major_multiplier, uint8_t _major_font,
				 float _dial_weight, float _value,
				 const char *_gauge_name_label, uint8_t _gauge_label_font,
				 const char *_value_format, float _value_multiplier, uint8_t _value_font)
				 {
					  cx = _cx;  cy = _cy;  circle_radius = _circle_radius;
					  angle_start = _angle_start; angle_end = _angle_end;
					  value_start = _value_start; value_end = _value_end;
					  circle_weight = _circle_weight;
					  major_spacing = _major_spacing; major_iradius = _major_iradius; major_width = _major_width;
					  minor_spacing = _minor_spacing; minor_iradius = _minor_iradius; minor_width = _minor_width;
					  major_num_radius = _major_num_radius; major_format = _major_format; major_multiplier = _major_multiplier; major_font = _major_font;
					  dial_weight = _dial_weight; value = _value;
					  gauge_name_label = _gauge_name_label;  gauge_label_font = _gauge_label_font;
					  value_format = _value_format, value_multiplier = _value_multiplier, value_font = _value_font;
				 }		

void exoGauge::point_at(float cx, float cy, float radius, float angle)
{
	// float r = ((angle - 90) / 360.0f) * 2 * PI;
	float r = (angle / 360.0f) * 2 * PI;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void exoGauge::radial_line(float cx, float cy, float start_rad, float end_rad, float angle) // angle in degrees
{
	//float r = ((angle - 90) / 360.0f) * 2 * PI;  // angle in degrees -> radians. Zero is 12:00
	float r = (angle / 360.0f) * 2 * PI;
	float x = (start_rad * cos(r)) + cx;
	float y = (start_rad * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
	x = (end_rad * cos(r)) + cx;
	y = (end_rad * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void exoGauge::angular_line(float cx, float cy, float radius, float start_angle, float end_angle)
{
	// float r = ((start_angle - 90) / 360.0f) * 2 * PI;
	float r = (start_angle / 360.0f) * 2 * PI;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
	// r = ((end_angle - 90) / 360.0f) * 2 * PI;
	r = (end_angle / 360.0f) * 2 * PI;
	x = (radius * cos(r)) + cx;
	y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void exoGauge::text_at(float cx, float cy, float radius, float angle, uint8_t font, char *text)
{
	// float r = ((angle - 90) / 360.0f) * 2 * PI;
	float r = (angle / 360.0f) * 2 * PI;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_Text(x, y, font, OPT_CENTER, text);
}

/* void Gauge_RDial(uint16_t cx, uint16_t cy, uint16_t circle_radius,
				 float angle_start, float angle_end,
				 float value_start, float value_end,
				 uint8_t circle_weight,
				 float major_spacing, float major_iradius, float major_width,
				 float minor_spacing, float minor_iradius, float minor_width,
				 float major_num_radius, const char *major_format, float major_multiplier, uint8_t major_font,
				 float dial_weight, float value,
				 const char *gauge_name_label, uint8_t gauge_label_font,
				 const char *value_format, float value_multiplier, uint8_t value_font) */
void exoGauge::drawGauge()
{
	//create a custom gauge background
	//ported from (ewwwww) PHP
	// char tf[16]; //value text buffer

	//pre-calc
	if (angle_start > angle_end) // je
		angle_start -= 360.0f;
	float angleRng = angle_end - angle_start;

	//setup
	FT81x_SendCommand(COLOR_A(255));
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));

	//major circle
	FT81x_SendCommand(BEGIN(POINTS));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius + 1) * 16)); // large white dot
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius - circle_weight - 1) * 16));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16)); // smaller black dot
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));

	//minor ticks
	float incr_angle = (angleRng / (value_end - value_start)) * minor_spacing;
	float angle = angle_start;
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16)));
	for (angle = angle_start; angle <= angle_end; angle += incr_angle)
		radial_line(cx, cy, minor_iradius, circle_radius - circle_weight - 1, angle); // angle in degrees

	//zones
	if (cx == 255) // cl-t gauge
	{
		float zValue_start = coolantT.zone[zone1];
		float zValue_end = coolantT.zone[zone2];

		float angleStart = angle_start + ((angle_end - angle_start) / (value_end - value_start)) * (zValue_start);
		float angleStop = angle_start + ((angle_end - angle_start) / (value_end - value_start)) * (zValue_end);
		incr_angle = ((angle_end - angle_start) / (value_end - value_start)) * minor_spacing;
		FT81x_SendCommand(COLOR_RGB(255, 0, 0));
		FT81x_SendCommand(BEGIN(LINE_STRIP));
		FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16 * 4)));
		for (angle = angleStart; angle <= angleStop; angle += incr_angle) // stitch an arc
		{
			float r = (angle / 360.0f) * 2 * PI;
			float dx = ((circle_radius - (circle_weight * 2)) * cos(r)) + cx;
			float dy = ((circle_radius - (circle_weight * 2)) * sin(r)) + cy;
			FT81x_SendCommand(VERTEX2F(((int)dx) * 16, ((int)dy) * 16));
		}
	}
	//major ticks
	incr_angle = (angleRng / (value_end - value_start)) * major_spacing;
	angle = angle_start;
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(major_width * 16)));
	for (angle = angle_start; angle <= angle_end + 1; angle += incr_angle)
		radial_line(cx, cy, major_iradius, circle_radius - circle_weight - 1, angle);

	//major text
	angle = angle_start;
	float tvalue = value_start;
	for (angle = angle_start; angle <= angle_end + 1; angle += incr_angle)
	{
		sprintf(tf, major_format, tvalue * major_multiplier);
		text_at(cx, cy, major_num_radius, angle, major_font, tf);
		tvalue += major_spacing;
	}

	//gauge label
	if (gauge_name_label[0] != 0)
		FT81x_Text(cx, cy - (circle_radius / 4), gauge_label_font, OPT_CENTER, gauge_name_label);

	// //value text ````````````````````````````````````````````````````````````````````
	// if (value_format[0] != 0)
	// {
	// 	sprintf(tf, value_format, value_multiplier * value);
	// 	FT81x_Text(cx, cy + (circle_radius / 4), value_font, OPT_CENTER, tf);
	// }

	// //dial (center dot)
	// if (value > value_end) // keep needle between min and max
	// 	value = value_end;
	// if (value < value_start)
	// 	value = value_start;
	// value = value - value_start;
	// float dangle = ((angle_end - angle_start) / (value_end - value_start)) * value; // y = (angleRng / valueRange)*x
	// dangle += angle_start;										   // y = mx + b
	// // if (dangle >= 360)je
	// // 	dangle -= 360;
	// // float
	// // deltaDegrees = ((dangle - 90) / 360.0f) * 2 * PI;
	// float deltaDegrees = ((dangle) / 360.0f) * 2 * PI;
	// float dx = ((circle_radius - (circle_weight * 4)) * cos(deltaDegrees)) + cx; // abs x location on disp
	// float dy = ((circle_radius - (circle_weight * 4)) * sin(deltaDegrees)) + cy; // abs y location on disp
	// FT81x_SendCommand(COLOR_RGB(255, 255, 0));									 // of yellow needle tip
	// FT81x_SendCommand(BEGIN(LINES));
	// FT81x_SendCommand(LINE_WIDTH((uint8_t)(dial_weight * 16)));
	// FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));					  // needle origin
	// FT81x_SendCommand(VERTEX2F((int)(dx * 16.0f), (int)(dy * 16.0f))); // needle tip

	// //center circle
	FT81x_SendCommand(BEGIN(POINTS)); // draw center needle circle
	float ccsize = (dial_weight * 3) * 16;
	FT81x_SendCommand(POINT_SIZE((uint8_t)ccsize));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	// // FT81x_SendCommand(COLOR_RGB(0, 0, 0));  						// this blacks out center dot
	// // FT81x_SendCommand(POINT_SIZE((uint8_t)(ccsize - (dial_weight * 16))));
	// // FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	// // FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	// //````````````````````````````````````````````````````````````````````````````````````````
}

void exoGauge::setValue(float value)
{
	//value text
	if (value_format[0] != 0)
	{
		sprintf(tf, value_format, value_multiplier * value);
		FT81x_Text(cx, cy + (circle_radius / 4), value_font, OPT_CENTER, tf);
	}

	//dial (center dot)
	if (value > value_end) // keep needle between min and max
		value = value_end;
	if (value < value_start)
		value = value_start;
	value = value - value_start;
	// if (angle_start > angle_end) // je
	// 	angle_start -= 360.0f;
	float dangle = ((angle_end - angle_start) / (value_end - value_start)) * value; // y = (angleRng / valueRange)*x
	dangle += angle_start;															// y = mx + b
	// if (dangle >= 360)je
	// 	dangle -= 360;
	// float
	// deltaDegrees = ((dangle - 90) / 360.0f) * 2 * PI;
	float deltaDegrees = ((dangle) / 360.0f) * 2 * PI;
	float dx = ((circle_radius - (circle_weight * 4)) * cos(deltaDegrees)) + cx; // abs x location on disp
	float dy = ((circle_radius - (circle_weight * 4)) * sin(deltaDegrees)) + cy; // abs y location on disp
	FT81x_SendCommand(COLOR_RGB(255, 255, 0));									 // of yellow needle tip
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint8_t)(dial_weight * 16)));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));					   // needle origin
	FT81x_SendCommand(VERTEX2F((int)(dx * 16.0f), (int)(dy * 16.0f))); // needle tip

	FT81x_SendCommand(BEGIN(POINTS)); // this blacks out center dot
	float ccsize = (dial_weight * 3) * 16;
	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(POINT_SIZE((uint8_t)(ccsize - (dial_weight * 16))));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
}