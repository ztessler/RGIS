/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

UIMarkers.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <UI.H>

#define UIWidth  11 
#define UIHeight 11 

static char _UITile00 [] = {
	((char) 0xff), ((char) 0x07), ((char) 0xff), ((char) 0x07), ((char) 0x03), ((char) 0x06), ((char) 0x03), ((char) 0x06),
  	((char) 0x73), ((char) 0x06), ((char) 0x73), ((char) 0x06), ((char) 0x73), ((char) 0x06), ((char) 0x03), ((char) 0x06),
	((char) 0x03), ((char) 0x06), ((char) 0xff), ((char) 0x07), ((char) 0xff), ((char) 0x07)};
static char _UITile01 [] = {
	((char) 0xf8), ((char) 0x00), ((char) 0xfc), ((char) 0x01), ((char) 0x06), ((char) 0x03), ((char) 0x03), ((char) 0x06),
	((char) 0x23), ((char) 0x06), ((char) 0x73), ((char) 0x06), ((char) 0x23), ((char) 0x06), ((char) 0x03), ((char) 0x06),
	((char) 0x06), ((char) 0x03), ((char) 0xfc), ((char) 0x01), ((char) 0xf8), ((char) 0x00)};
static char _UITile02 [] = {
	((char) 0xf8), ((char) 0x00), ((char) 0x04), ((char) 0x01), ((char) 0x06), ((char) 0x03), ((char) 0x8f), ((char) 0x07),
	((char) 0xdf), ((char) 0x07), ((char) 0xff), ((char) 0x07), ((char) 0xdf), ((char) 0x07), ((char) 0x8f), ((char) 0x07),
	((char) 0x06), ((char) 0x03), ((char) 0x04), ((char) 0x01), ((char) 0xf8), ((char) 0x00)};
static char _UITile03 [] = {
	((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00),
	((char) 0x20), ((char) 0x00), ((char) 0xff), ((char) 0x07), ((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00),
	((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00)};
static char _UITile04 [] = {
	((char) 0x01), ((char) 0x04), ((char) 0x02), ((char) 0x02), ((char) 0x04), ((char) 0x01), ((char) 0x88), ((char) 0x00),
	((char) 0x50), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x50), ((char) 0x00), ((char) 0x88), ((char) 0x00),
	((char) 0x04), ((char) 0x01), ((char) 0x02), ((char) 0x02), ((char) 0x01), ((char) 0x04)};
static char _UITile05 [] = {
	((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x70), ((char) 0x00), ((char) 0xa8), ((char) 0x00),
	((char) 0x24), ((char) 0x01), ((char) 0xff), ((char) 0x07), ((char) 0x24), ((char) 0x01), ((char) 0xa8), ((char) 0x00),
	((char) 0x70), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00)};
static char _UITile06 [] = {
	((char) 0x20), ((char) 0x00), ((char) 0x3c), ((char) 0x00), ((char) 0x3f), ((char) 0x00), ((char) 0x3c), ((char) 0x00),
	((char) 0x20), ((char) 0x00), ((char) 0x20), ((char) 0x00), ((char) 0x70), ((char) 0x00), ((char) 0xf8), ((char) 0x00),
	((char) 0xfc), ((char) 0x01), ((char) 0xfe), ((char) 0x03), ((char) 0xff), ((char) 0x07)};
static char _UITile07 [] = {
	((char) 0x20), ((char) 0x00), ((char) 0x50), ((char) 0x00), ((char) 0x88), ((char) 0x00), ((char) 0x04), ((char) 0x01),
	((char) 0x02), ((char) 0x02), ((char) 0x07), ((char) 0x07), ((char) 0x04), ((char) 0x01), ((char) 0x04), ((char) 0x01),
	((char) 0x04), ((char) 0x01), ((char) 0x04), ((char) 0x01), ((char) 0xfc), ((char) 0x01)};
static char _UITile08 [] = {
	((char) 0xff), ((char) 0x07), ((char) 0x3f), ((char) 0x04), ((char) 0x3f), ((char) 0x04), ((char) 0x3f), ((char) 0x04),
	((char) 0x3f), ((char) 0x04), ((char) 0xff), ((char) 0x07), ((char) 0xe1), ((char) 0x07), ((char) 0xe1), ((char) 0x07),
	((char) 0xe1), ((char) 0x07), ((char) 0xe1), ((char) 0x07), ((char) 0xff), ((char) 0x07)};
static char _UITile09 [] = {
	((char) 0xf8), ((char) 0x00), ((char) 0xfc), ((char) 0x01), ((char) 0xfe), ((char) 0x03), ((char) 0x77), ((char) 0x07),
	((char) 0xaf), ((char) 0x07), ((char) 0xdf), ((char) 0x07), ((char) 0xaf), ((char) 0x07), ((char) 0x77), ((char) 0x07),
	((char) 0xfe), ((char) 0x03), ((char) 0xfc), ((char) 0x01), ((char) 0xf8), ((char) 0x00)};
static char _UITile10 [] = {
	((char) 0x20), ((char) 0x00), ((char) 0x70), ((char) 0x00), ((char) 0xf8), ((char) 0x00), ((char) 0xfc), ((char) 0x01),
	((char) 0xfe), ((char) 0x03), ((char) 0xff), ((char) 0x07), ((char) 0xfe), ((char) 0x03), ((char) 0xfc), ((char) 0x01),
	((char) 0xf8), ((char) 0x00), ((char) 0x70), ((char) 0x00), ((char) 0x20), ((char) 0x00)};

static char *_UIMarkers [] = { _UITile00,
				_UITile01,
				_UITile02,
				_UITile03,
				_UITile04,
				_UITile05,
				_UITile06,
				_UITile07,
				_UITile08,
				_UITile09,
				_UITile10};

Pixmap UIMarker (int style,int foreground,int background)

	{
	if (style >= (int) (sizeof (_UIMarkers) / sizeof (char *))) return ((Pixmap) NULL);

	return (XCreatePixmapFromBitmapData (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),
					      _UIMarkers [style],UIWidth,UIHeight,foreground,background,
					      DefaultDepth (XtDisplay (UITopLevel ()),DefaultScreen (XtDisplay (UITopLevel ())) )));
	}
