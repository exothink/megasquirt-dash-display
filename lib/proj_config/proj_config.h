/* ===========================================================================
Documents\PlatformIO\Projects\squirtDash\lib\proj_config\proj_config.h

 5-inch squirtDash                john eckert  12/12/20
 LCD graphics via the ft813 with touch 
 

 inspired by:  Mark Williams (2019-07-01)
 Distributed under the GNU GENERAL PUBLIC LICENSE v3.0 (see LICENCE file)

=========================================================================== */
#ifndef __PROJCFG_H
#define __PROJCFG_H

//PROJECT CONFIGURATION
#include <Arduino.h>

//project options
#define NODEMCU_ESP32S
#define CFAF800480E0_050SC_A1_1

#define EVE2_PIN_INT (4)
#define EVE2_PIN_PD (2)       //je works with Devkit v1 www.doit.am
#define EVE2_PIN_CS (5)
#define SPI_SPEED (20000000L) // FT813 up to 30Mhz ideally,  24MHz max for my fly lead proto
#define EVE2_WORKBUFSIZE (32)

//display options
#ifdef CFAF800480E0_050SC_A1_1
#define FT813_ENABLE (1)
#define EVE2_GT911 (1)

// CAN
#define BR250 250000
#define BR500 500000
#define CANBR BR250           // sets CAN data rate


// gauge parms from VanaDash
enum zoneIdx : uint8_t
{
  zone0,
  zone1,
  zone2,
  zone3
};

enum measAdr : uint8_t // CAN Bus measurement addr (ID)
{
  adrAuxBat,
  adrBattery,
  adrHeadT,
  adrCoolantT,
  adrCoolantP,
  adrFuelPres,
  adrFuelGal,
  adrOilPres,
  adrRPM, // 0x68
  adrSpeed,
  EOA
}; // 0x0 - 0x9

struct rgb
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

const rgb aqua = {0, 255, 255};
const rgb blk = {0, 0, 0};
const rgb blu = {0, 0, 255};
const rgb grn = {0, 255, 0};
const rgb red = {255, 0, 0};
const rgb wht = {255, 255, 255};
const rgb yel = {255, 255, 0};

struct dialFace
{
  uint8_t tag;
  const char *title[1]; // no other syntax works -> *title[1];
  const char *units[1];
  // uint8_t num;      // number of zones, 3 max
  float zone[4];  // zone start and stop values, zone[0] = dialMin, zone[3] = dialMax
  rgb zoneColor[3]; // zone colors for analog dial
  int8_t width[3];  // line width of zone
  rgb digiColor[3]; // zone colors for digital readout
};

const dialFace auxBat = {1, "Aux Batt", "v", {0, 13, 15, 16}, {red, aqua, red}, {1, 2, 1}, {red, grn, red}};
const dialFace battery = {2, "BATT", "v", {0, 12.2, 14.5, 15}, {red, aqua, red}, {1, 2, 1}, {red, grn, red}};  // 11.5, 12.0, 14.5, 15.0
const dialFace headT = {3, "CHT", "f", {85, 215, 325, 325}, {aqua, red, blk}, {1, 3, 0}, {grn, red, blk}};
const dialFace coolantT = {4, "CL-T", "f", {0, 210, 250, 270}, {aqua, yel, red}, {2, 3, 3}, {grn, red, blk}};
const dialFace coolantP = {5, "CL-P", "#", {0, 5, 10, 15}, {red, aqua, red}, {2, 2, 2}, {red, grn, red}};
const dialFace fuelPres = {6, "FUEL-P", "#", {0, 27, 32, 40}, {red, aqua, red}, {1, 2, 1}, {red, grn, red}};
const dialFace fuelGal = {7, "FUEL-G", "G", {0, 2, 3, 16}, {red, yel, aqua}, {3, 2, 1}, {red, yel, grn}};
const dialFace oilPres = {8, "OIL-P", "#", {20, 45, 55, 60}, {red, grn, red}, {3, 2, 3}, {red, grn, red}};
const dialFace rpm = {9, "RPM", "", {0, 6000, 7300, 8000}, {aqua, yel, red}, {1, 3, 3}, {grn, yel, red}};
const dialFace speed = {10, "MPH", "", {0, 80, 120, 120}, {aqua, yel, blk}, {1, 3, 0}, {grn, yel, blk}};


// // Horizontal timing (minimum values from ILI6122_SPEC_V008.pdf page 45)
// #define HPX   (800)    // Horizontal Pixel Width
// #define HSW   (1)      // Horizontal Sync Width (1~40)
// #define HBP   (46-HSW) // Horizontal Back Porch (must be 46, includes HSW)
// #define HFP   (16)     // Horizontal Front Porch (16~210~354)
// #define HPP   (116)    // Horizontal Pixel Padding (tot=863: 862~1056~1200)
//                        // FTDI needs at least 1 here
// // Vertical timing (minimum values from ILI6122_SPEC_V008.pdf page 46)
// #define VLH   (480)   // Vertical Line Height
// #define VS    (1)     // Vertical Sync (in lines)  (1~20)
// #define VBP   (23-VS) // Vertical Back Porch (must be 23, includes VS)
// #define VFP   (7)     // Vertical Front Porch (7~22~147)
// #define VLP   (1)     // Vertical Line Padding (tot=511: 510~525~650)
//                       // FTDI needs at least 1 here

// Horizontal timing (minimum values from ILI6122_SPEC_V008.pdf page 45)
#define HPX (800) // Horizontal Pixel Width
#define HSW (4)   // Horizontal Sync Width (1~40)
#define HBP (8)   // Horizontal Back Porch (must be 46, includes HSW)
#define HFP (8)   // Horizontal Front Porch (16~210~354)
#define HPP (178) // Horizontal Pixel Padding (tot=863: 862~1056~1200) 178 
                  // FTDI needs at least 1 here
// Vertical timing (minimum values from ILI6122_SPEC_V008.pdf page 46)
#define VLH (480) // Vertical Line Height
#define VS (4)    // Vertical Sync (in lines)  (1~20)
#define VBP (8)   // Vertical Back Porch (must be 23, includes VS)
#define VFP (8)   // Vertical Front Porch (7~22~147)
#define VLP (1)   // Vertical Line Padding (tot=511: 510~525~650) 
                  // FTDI needs at least 1 here

#define EVE2_DWIDTH (HPX)
#define EVE2_DHEIGHT (VLH)
#define EVE2_HCYCLE (HPX + HFP + HSW + HBP + HPP)
#define EVE2_HOFFSET (HFP + HSW + HBP)
#define EVE2_HSYNC0 (HFP)
#define EVE2_HSYNC1 (HFP + HSW)
#define EVE2_VCYCLE (VLH + VFP + VS + VBP + VLP)
#define EVE2_VOFFSET (VFP + VS + VBP)
#define EVE2_VSYNC0 (VFP)
#define EVE2_VSYNC1 (VFP + VS)
#define EVE2_PCLK 2L          // 60MHz/EVE2_PCLK = 30MHz
#define EVE2_SWIZZLE 0L
#define EVE2_PCLK_POL 1L
#define EVE2_CSPREAD 0L
#define EVE2_DITHER 1L
#endif

//extra
#define DPRINT_SIZE (64)
extern char _debugbuf[DPRINT_SIZE];
#define DPRINTF(...)                 \
  {                                  \
    sprintf(_debugbuf, __VA_ARGS__); \
    Serial.print(_debugbuf);         \
  }
#define DPRINT(...)            \
  {                            \
    Serial.print(__VA_ARGS__); \
  }

#endif
