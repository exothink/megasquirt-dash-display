/*   Universal EVE Gauge Library
  
  Copyright (c) 2020; John Eckert
  All rights reserved 
*/

#ifndef _EXOGLIB_HH
#define _EXOCGLIB_HH

#include <Arduino.h>
#include "ft81x.h"

#define COLOR_STRUCT(rgb) ((4UL<<24)|(((rgb.r)&255UL)<<16)|(((rgb.g)&255UL)<<8)|(((rgb.b)&255UL)<<0))
#define COLOR_EQ(x,y) (((x.r)==(y.r))&&((x.g)==(y.g))&&((x.b) == (y.b)))

class exoGauge
{
public:
   exoGauge();
   exoGauge(uint16_t cx, uint16_t cy, uint16_t circle_radius,
				 float angle_start, float angle_end,
				 float value_start, float value_end,
				 uint8_t circle_weight,
				 float major_spacing, float major_iradius, float major_width,
				 float minor_spacing, float minor_iradius, float minor_width,
				 float major_num_radius, const char *major_format, float major_multiplier, uint8_t major_font,
				 float dial_weight, float value,
				 const char *gauge_name_label, uint8_t gauge_label_font,
				 const char *value_format, float value_multiplier, uint8_t value_font,
         const dialFace& properties);
    void drawGauge();
    void setValue(float val);

public:
    uint16_t cx = 100;
    uint16_t cy = 100;
    uint16_t circle_radius = 100;

    float angle_start = 135;
    float angle_end = 45;
    float value_start = 0;
    float value_end = 100;

    uint8_t circle_weight = 3;

    float major_spacing = 20;
    float major_iradius = 80;
    float major_width = 2;

    float minor_spacing = 5;
    float minor_iradius = 85;
    float minor_width = 1;

    float major_num_radius = 65;
    const char *major_format = "%0.0f";
    float major_multiplier = 1;
    uint8_t major_font = 26;

    float dial_weight = 2.5;
    float value = 0;

    const char *gauge_name_label = "TEST";
    uint8_t gauge_label_font = 28;

    const char *value_format = "%0.0f";
    float value_multiplier = 1;
    uint8_t value_font = 29;

    dialFace properties;

    char tf[16]; //value text buffer
    
    void point_at(float cx, float cy, float radius, float angle);
    void radial_line(float cx, float cy, float start_rad, float end_rad, float angle); // angle in degrees
    void angular_line(float cx, float cy, float radius, float start_angle, float end_angle);
    void text_at(float cx, float cy, float radius, float angle, uint8_t font, char *text);

   // #define COLOR_RGB(red,green,blue) ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
    private:
    float angle;
    float incr_angle;

 };

/* 	Gauge_RDial(
		255;370; 100; 		// x;y; radius
	    180.0f; 90.0f;      // start; end angle
		coolantT.zone[zone0]; coolantT.zone[zone3];  // start; end vals
		3;					// circle weight
		25; 80; 2;			// major spacing; iradius; width
		5; 85; 0.8;			// minor spacing; iradius; width
		65; "%0.0f"; 1; 26; // major_num_radius; major_format; multiplier; major_font
		2.5; CLT;			// dial_weight; float value
		"CL-T"; 28;			// gauge_name_label; gauge_label_font
		"%0.0f"; 1; 29		// value_format; value_multiplier; value_font 
	); */

#endif /* _EXOGLIB_HH */