#include <Arduino.h>
#include <math.h>
#include "proj_config.h"
//#include "ft81x.h"
#include "exoGauge.h"

//static const float pi = 3.1415926535f;
exoGauge::exoGauge()
{
}

exoGauge::exoGauge(uint16_t _cx, uint16_t _cy, uint16_t _circle_radius,
				   float _angle_start, float _angle_end,
				   float _value_start, float _value_end,
				   uint8_t _circle_weight,
				   float _major_spacing, float _major_iradius, float _major_width,
				   float _minor_spacing, float _minor_iradius, float _minor_width,
				   float _major_num_radius, const char *_major_format, float _major_multiplier, uint8_t _major_font,
				   float _dial_weight, float _value,
				   const char *_gauge_name_label, uint8_t _gauge_label_font,
				   const char *_value_format, float _value_multiplier, uint8_t _value_font,
				   const dialFace& _properties):
					cx(_cx),						//initializer list
					cy(_cy),
					circle_radius(_circle_radius),
					angle_start(_angle_start),
					angle_end(_angle_end),
					value_start(_value_start),
					value_end(_value_end),
					circle_weight(_circle_weight),
					major_spacing(_major_spacing),
					major_iradius(_major_iradius),
					major_width(_major_width),
					minor_spacing(_minor_spacing),
					minor_iradius(_minor_iradius),
					minor_width(_minor_width),
					major_num_radius(_major_num_radius),
					major_format(_major_format),
					major_multiplier(_major_multiplier),
					major_font(_major_font),
					dial_weight(_dial_weight),
					value(_value),
					gauge_name_label(_gauge_name_label),
					gauge_label_font(_gauge_label_font),
					value_format(_value_format), 
					value_multiplier(_value_multiplier), 
					value_font(_value_font),
					properties(_properties)  {};



/* {
	cx = _cx;
	cy = _cy;
	circle_radius = _circle_radius;
	angle_start = _angle_start;
	angle_end = _angle_end;
	value_start = _value_start;
	value_end = _value_end;
	circle_weight = _circle_weight;
	major_spacing = _major_spacing;
	major_iradius = _major_iradius;
	major_width = _major_width;
	minor_spacing = _minor_spacing;
	minor_iradius = _minor_iradius;
	minor_width = _minor_width;
	major_num_radius = _major_num_radius;
	major_format = _major_format;
	major_multiplier = _major_multiplier;
	major_font = _major_font;
	dial_weight = _dial_weight;
	value = _value;
	gauge_name_label = _gauge_name_label;
	gauge_label_font = _gauge_label_font;
	value_format = _value_format, value_multiplier = _value_multiplier, value_font = _value_font;
	properties = _properties;
} */

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
				 const char *value_format, float value_multiplier, uint8_t value_font,
				 dialFace props) 

				struct dialFace
				{
				uint8_t tag;
				const char *title[1]; // no other syntax works -> *title[1];
				const char *units[1];
				// uint8_t num;      // number of zones, 3 max
				int16_t zone[4];  // zone start and stop values, zone[0] = dialMin, zone[3] = dialMax
				rgb zoneColor[3]; // zone colors for analog dial
				int8_t width[3];  // line width of zone
				rgb digiColor[3]; // zone colors for digital readout
				};
*/
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
	//FT81x_SendCommand(COLOR_RGB(200, 255, 230));
	// FT81x_SendCommand(COLOR_RGBx(red));

	//major circle
	FT81x_SendCommand(BEGIN(POINTS));

	FT81x_SendCommand(COLOR_RGB(115, 115, 115));
	//FT81x_SendCommand(COLOR_RGBX(red));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius + 1) * 16)); // shaddow 2
	FT81x_SendCommand(VERTEX2F((cx * 16) + 130, (cy * 16) + 130));

	FT81x_SendCommand(COLOR_RGB(85, 85, 100));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius + 1) * 16)); // shaddow 1
	FT81x_SendCommand(VERTEX2F((cx * 16) + 100, (cy * 16) + 100));
	
    FT81x_SendCommand(COLOR_RGB(255, 255, 255));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius + 1) * 16)); // large ball, dial outer diameter
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));

	FT81x_SendCommand(COLOR_RGB(0, 0, 0));
	FT81x_SendCommand(POINT_SIZE(((uint16_t)circle_radius - circle_weight - 1) * 16));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16)); 						// smaller black dot, yields a circle from the ball
	FT81x_SendCommand(COLOR_RGB(255, 255, 255));

	// //minor ticks
	// float incr_angle = (angleRng / (value_end - value_start)) * minor_spacing;
	// float angle = angle_start;
	// FT81x_SendCommand(BEGIN(LINES));
	// FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16)));
	// for (angle = angle_start; angle <= angle_end; angle += incr_angle)
	// 	radial_line(cx, cy, minor_iradius, circle_radius - circle_weight - 1, angle); // angle in degrees

	//zones
	for (size_t i = 0; i < 3; i++)
	{
		float zValue_start = properties.zone[i];
		float zValue_end = properties.zone[i + 1];

		float angleStart = angle_start + ((angle_end - angle_start) / (value_end - value_start)) * (zValue_start);
		float angleStop = angle_start + ((angle_end - angle_start) / (value_end - value_start)) * (zValue_end);
		//	incr_angle = ((angle_end - angle_start) / (value_end - value_start)) * minor_spacing;
		//	 FT81x_SendCommand(COLOR_RGB(255, 0, 0));
		FT81x_SendCommand(COLOR_STRUCT(properties.zoneColor[i]));
		FT81x_SendCommand(BEGIN(LINE_STRIP));
		FT81x_SendCommand(LINE_WIDTH((uint16_t)(properties.width[i] * 16)));
		for (float angle = angleStart; angle <= angleStop; angle += 5) // stitch an arc
		{
			float r = (angle / 360.0f) * 2 * PI;
			float dx = ((circle_radius - (circle_weight + properties.width[i] + 1)) * cos(r)) + cx;
			float dy = ((circle_radius - (circle_weight + properties.width[i] + 1)) * sin(r)) + cy;
			FT81x_SendCommand(VERTEX2F(((int)dx) * 16, ((int)dy) * 16));
		}
	}

	//minor ticks
	incr_angle = (angleRng / (value_end - value_start)) * minor_spacing;
	angle = angle_start;
	FT81x_SendCommand(COLOR_STRUCT(wht));
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint16_t)(minor_width * 16)));
	for (angle = angle_start; angle <= angle_end; angle += incr_angle)
		radial_line(cx, cy, minor_iradius, circle_radius - circle_weight - 1, angle); // angle in degrees

	//major ticks
	incr_angle = (angleRng / (value_end - value_start)) * major_spacing;
	angle = angle_start;
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
		FT81x_Text(cx, cy - (circle_radius / 4), gauge_label_font, OPT_CENTER, properties.title[0]);

	// //center circle
	FT81x_SendCommand(BEGIN(POINTS)); // draw center needle circle
	float ccsize = (dial_weight * 3) * 16;
	FT81x_SendCommand(POINT_SIZE((uint8_t)ccsize));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	//````````````````````````````````````````````````````````````````````````````````````````
}

void exoGauge::setValue(float value)
{
	//value text
	int vColor = 0;
	if (value_format[0] != 0)
	{
		if (value >= properties.zone[0] && value < properties.zone[1]) // digital readouts vary with zones
			vColor = 0;
		else if (value >= properties.zone[1] && value < properties.zone[2])
			vColor = 1;
		else if (value >= properties.zone[2] && value < properties.zone[3])
			vColor = 2;

	 	
		sprintf(tf, value_format, value_multiplier * value);
	if (COLOR_EQ(properties.digiColor[vColor], red))
		{
			FT81x_SendCommand(COLOR_STRUCT(yel));
			FT81x_Text(cx + 2, cy + 1 + (circle_radius / 4), value_font + 1, OPT_CENTER, tf);
			FT81x_SendCommand(COLOR_STRUCT(properties.digiColor[vColor]));
			FT81x_Text(cx, cy + (circle_radius / 4), value_font + 1, OPT_CENTER, tf);
		}
		else
		{
		FT81x_SendCommand(COLOR_STRUCT(properties.digiColor[vColor]));
		FT81x_Text(cx, cy + (circle_radius / 4), value_font, OPT_CENTER, tf);
		}
	}

	//needle
	if (value > value_end) // keep needle between min and max
		value = value_end;
	if (value < value_start)
		value = value_start;
	value = value - value_start;

	float dangle = ((angle_end - angle_start) / (value_end - value_start)) * value; // y = (angleRng / valueRange)*x
	dangle += angle_start;															// y = mx + b
	float deltaDegrees = ((dangle) / 360.0f) * 2 * PI;
	float dx = ((circle_radius - (circle_weight * 4)) * cos(deltaDegrees)) + cx; // abs x location on disp
	float dy = ((circle_radius - (circle_weight * 4)) * sin(deltaDegrees)) + cy; // abs y location on disp
	FT81x_SendCommand(COLOR_STRUCT(yel));									 // of yellow needle tip
	FT81x_SendCommand(BEGIN(LINES));
	FT81x_SendCommand(LINE_WIDTH((uint8_t)(dial_weight * 16)));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));				 // needle origin
	FT81x_SendCommand(VERTEX2F((int)(dx * 16), (int)(dy * 16))); // needle tip

	FT81x_SendCommand(BEGIN(POINTS)); // this blacks out center dot
	float ccsize = (dial_weight * 3) * 16;
	FT81x_SendCommand(COLOR_STRUCT(wht));
	FT81x_SendCommand(POINT_SIZE((uint8_t)(ccsize - (dial_weight * 16))));
	FT81x_SendCommand(VERTEX2F(cx * 16, cy * 16));
	FT81x_SendCommand(COLOR_STRUCT(blk));
}