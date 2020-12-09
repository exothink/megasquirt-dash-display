/* ===========================================================================

 5-inch MEGASQUIRT ECU CANBus Dashboard Display
 Simple graphic renderers

 Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#include <Arduino.h>
#include <math.h>
#include "proj_config.h"
#include "ft81x.h"

static const float pi = 3.1415926535f;

void point_at(float cx, float cy, float radius, float angle)
{
	// float r = ((angle - 90) / 360.0f) * 2 * pi;
	float r = (angle / 360.0f) * 2 * pi;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void radial_line(float cx, float cy, float start_rad, float end_rad, float angle) // angle in degrees
{
	//float r = ((angle - 90) / 360.0f) * 2 * pi;  // angle in degrees -> radians. Zero is 12:00
	float r = (angle / 360.0f) * 2 * pi;
	float x = (start_rad * cos(r)) + cx;
	float y = (start_rad * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
	x = (end_rad * cos(r)) + cx;
	y = (end_rad * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void angular_line(float cx, float cy, float radius, float start_angle, float end_angle)
{
	// float r = ((start_angle - 90) / 360.0f) * 2 * pi;
	float r = (start_angle / 360.0f) * 2 * pi;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
	// r = ((end_angle - 90) / 360.0f) * 2 * PI;
	r = (end_angle / 360.0f) * 2 * PI;
	x = (radius * cos(r)) + cx;
	y = (radius * sin(r)) + cy;
	FT81x_SendCommand(VERTEX2F(((int)x) * 16, ((int)y) * 16));
}

void text_at(float cx, float cy, float radius, float angle, uint8_t font, char *text)
{
	// float r = ((angle - 90) / 360.0f) * 2 * pi;
	float r = (angle / 360.0f) * 2 * pi;
	float x = (radius * cos(r)) + cx;
	float y = (radius * sin(r)) + cy;
	FT81x_Text(x, y, font, OPT_CENTER, text);
}

void Gauge_RDial(uint16_t cx, uint16_t cy, uint16_t circle_radius,
				 float angle_start, float angle_end,
				 float value_start, float value_end,
				 uint8_t circle_weight,
				 float major_spacing, float major_iradius, float major_width,
				 float minor_spacing, float minor_iradius, float minor_width,
				 float major_num_radius, const char *major_format, float major_multiplier, uint8_t major_font,
				 float dial_weight, float value,
				 const char *gauge_name_label, uint8_t gauge_label_font,
				 const char *value_format, float value_multiplier, uint8_t value_font)
{
	//create a custom gauge background
	//ported from (ewwwww) PHP
	char tf[16];

	//pre-calc
	// float angleRng = 0;
	// if (angle_start < angle_end)
	// 	angleRng = angle_end - angle_start;
	// else
	// 	angleRng = (360 - angle_start) + angle_end;

	if (angle_start > angle_end)  // je
		angle_start -= 360;
	float angleRng = angle_end - angle_start;

	//setup
	FT81x_SendCommand(COLOR_A(255));
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));

	//major circle
	FT81x_SendCommand(BEGIN(POINTS));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius + 1) * 16));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius - circle_weight - 1) * 16));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));

	//minor ticks
	// ================================
	float incr_angle = (angleRng / (value_end - value_start)) * minor_spacing;
	float tickcnt = (value_end - value_start) / minor_spacing;
	float angle = angle_start;
	uint16_t s;
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16)));
	// for (s = 0; s < tickcnt + 1; s++)
	for (angle = angle_start; angle <= angle_end; angle += incr_angle)
	{
		// if (angle >= 360)
		// 	angle -= 360;
		radial_line(cx, cy, minor_iradius, circle_radius - circle_weight - 1, angle); // angle in degrees
																					  // if (angle >= 180 && angle <= 190)
																					  // 	angular_line(cx, cy, circle_radius - circle_weight - 5, angle, angle + incr_angle); //je
																					  //angle += incr_angle;
	}
	//=============================

	//---------------------------
	// float incr_angle = (angleRng / (value_end - value_start)) * minor_spacing;
	// float tickcnt = (value_end - value_start) / minor_spacing;
	// float angle = angle_start;
	// uint16_t s;
	// FT81x_SendCommand(BEGIN(LINES));
	// FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16)));
	// for (s = 0; s < tickcnt + 1; s++)
	// {
	// 	// if (angle >= 360)
	// 	// 	angle -= 360;
	// 	radial_line(cx, cy, minor_iradius, circle_radius - circle_weight - 1, angle);  // angle in degrees
	// 	// if (angle >= 180 && angle <= 190)
	// 	// 	angular_line(cx, cy, circle_radius - circle_weight - 5, angle, angle + incr_angle); //je
	// 	angle += incr_angle;
	// }
	//-------------------------------

	//zones
	//	float zValue;
	float zValue_start = 170;
	float zValue_end = 210;

	// if (zValue > zValue_end)
	// 	zValue = zValue_end;
	// if (zValue < zValue_start)
	// 	zValue = zValue_start;

	//	zValue = zValue - zValue_start;
	tickcnt = (zValue_end - zValue_start) / minor_spacing;
	float dangle = (angleRng / (value_end - value_start)) * zValue_start;
	//dangle += angle_start;
	if (dangle >= 360)
		dangle -= 360;
	float deltaDegrees = ((dangle - 90) / 360.0f) * 2 * PI;
	// float dx = ((circle_radius - (circle_weight * 4)) * cos(deltaDegrees)) + cx;
	// float dy = ((circle_radius - (circle_weight * 4)) * sin(deltaDegrees)) + cy;
	FT81x_SendCommand(COLOR_RGB(255, 0, 0));
	FT81x_SendCommand(BEGIN(LINE_STRIP));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16 * 4)));
	//	FT81x_SendCommand(
	for (s = 0; s < 4 + 1; s++)
	{
		if (angle >= 360)
			angle -= 360;
		// radial_line(cx, cy, minor_iradius, circle_radius - circle_weight - 1, angle);
		//if (angle >= 180 && angle <= 190)
		//	angular_line(cx, cy, circle_radius - circle_weight - 5, angle, angle + incr_angle); //je
		float dx = ((circle_radius - (circle_weight * 4)) * cos(deltaDegrees * 2 * pi)) + cx;
		float dy = ((circle_radius - (circle_weight * 4)) * sin(deltaDegrees * 2 * pi)) + cy;
		FT81x_SendCommand(VERTEX2F(((int)dx) * 16, ((int)dy) * 16));
		if (value > 170)
		{
			Serial.print(s);
			Serial.print(" ");
			Serial.print(deltaDegrees * 2 * pi);
			Serial.print(" ");
			Serial.print(dx);
			Serial.print(" ");
			Serial.println(dy);
		}
		deltaDegrees += incr_angle / (2 * PI);
	}

	//major ticks
	incr_angle = (angleRng / (value_end - value_start)) * major_spacing;
	tickcnt = (value_end - value_start) / major_spacing;
	angle = angle_start;
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(major_width * 16)));
	for (s = 0; s < tickcnt + 1; s++)
	{
		if (angle >= 360)
			angle -= 360;
		radial_line(cx, cy, major_iradius, circle_radius - circle_weight - 1, angle);
		angle += incr_angle;
	}

	//majors text
	angle = angle_start;
	float tvalue = value_start;
	for (s = 0; s < tickcnt + 1; s++)
	{
		if (angle >= 360)
			angle -= 360;
		sprintf(tf, major_format, tvalue * major_multiplier);
		text_at(cx, cy, major_num_radius, angle, major_font, tf);
		angle += incr_angle;
		tvalue += major_spacing;
	}

	//gauge label
	if (gauge_name_label[0] != 0)
		FT81x_Text(cx, cy - (circle_radius / 4), gauge_label_font, OPT_CENTER, gauge_name_label);

	//value text
	if (value_format[0] != 0)
	{
		sprintf(tf, value_format, value_multiplier * value);
		FT81x_Text(cx, cy + (circle_radius / 4), value_font, OPT_CENTER, tf);
	}

	//dial (center dot)
	if (value > value_end)
		value = value_end;
	if (value < value_start)
		value = value_start;
	value = value - value_start;
	// float
	dangle = (angleRng / (value_end - value_start)) * value; // y = (angleRng / valueRange)*x
	dangle += angle_start;									 // y = mx + b
	// if (dangle >= 360)je
	// 	dangle -= 360;
	// float
	// deltaDegrees = ((dangle - 90) / 360.0f) * 2 * PI;
	deltaDegrees = ((dangle) / 360.0f) * 2 * PI;
	float dx = ((circle_radius - (circle_weight * 4)) * cos(deltaDegrees)) + cx; // abs x location on disp
	float dy = ((circle_radius - (circle_weight * 4)) * sin(deltaDegrees)) + cy; // abs y location on disp
	FT81x_SendCommand(COLOR_RGB(255, 255, 0));									 // of yellow needle tip
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint8_t)(dial_weight * 16)));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(VERTEX2F((int)(dx * 16.0f), (int)(dy * 16.0f)));

	//center circle
	FT81x_SendCommand(BEGIN(POINTS));
	float ccsize = (dial_weight * 3) * 16;
	FT81x_SendCommand(POINT_SIZE((uint8_t)ccsize));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(POINT_SIZE((uint8_t)(ccsize - (dial_weight * 16))));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));
}
