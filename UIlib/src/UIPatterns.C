/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

UIPatterns.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <UI.H>

#define UIWidth 8
#define UIHeight 8

static char _UITile00 [] = {((char) 0xff), ((char) 0xff), ((char) 0xff), ((char) 0xff), ((char) 0xff), ((char) 0xff), ((char) 0xff), ((char) 0xff)};
static char _UITile01 [] = {((char) 0x45), ((char) 0xaa), ((char) 0x11), ((char) 0x88), ((char) 0x55), ((char) 0x22), ((char) 0x11), ((char) 0xaa)};
static char _UITile02 [] = {((char) 0x44), ((char) 0xaa), ((char) 0x11), ((char) 0x88), ((char) 0x55), ((char) 0x22), ((char) 0x11), ((char) 0xaa)};
static char _UITile03 [] = {((char) 0xff), ((char) 0x04), ((char) 0x04), ((char) 0x04), ((char) 0xff), ((char) 0x20), ((char) 0x20), ((char) 0x20)};
static char _UITile04 [] = {((char) 0x03), ((char) 0x66), ((char) 0xf6), ((char) 0x93), ((char) 0x30), ((char) 0x66), ((char) 0x6f), ((char) 0x39)};
static char _UITile05 [] = {((char) 0x92), ((char) 0x49), ((char) 0x64), ((char) 0x92), ((char) 0x19), ((char) 0x24), ((char) 0x46), ((char) 0x89)};
static char _UITile06 [] = {((char) 0xa8), ((char) 0x45), ((char) 0x2a), ((char) 0x51), ((char) 0x8a), ((char) 0x54), ((char) 0xa2), ((char) 0x15)};
static char _UITile07 [] = {((char) 0xdf), ((char) 0xb5), ((char) 0xda), ((char) 0xbf), ((char) 0xfd), ((char) 0x5b), ((char) 0xad), ((char) 0xfb)};
static char _UITile08 [] = {((char) 0x6f), ((char) 0x24), ((char) 0x42), ((char) 0x6f), ((char) 0xf6), ((char) 0x42), ((char) 0x24), ((char) 0xf6)};
static char _UITile09 [] = {((char) 0x00), ((char) 0x7e), ((char) 0x42), ((char) 0x5a), ((char) 0x5a), ((char) 0x42), ((char) 0x7e), ((char) 0x00)};
static char _UITile10 [] = {((char) 0x00), ((char) 0xee), ((char) 0xaa), ((char) 0xee), ((char) 0x00), ((char) 0xee), ((char) 0xaa), ((char) 0xee)};

static char *_UIPatterns [] = {
				_UITile00,
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

Pixmap UIPattern (int style,int foreground,int background)

	{
	Pixmap pixmap;
	if (style >= (int) (sizeof (_UIPatterns) / sizeof (char *))) return ((Pixmap) NULL);

	pixmap = XCreatePixmapFromBitmapData (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),
					      _UIPatterns [style],UIWidth,UIHeight,foreground,background,
					      DefaultDepth (XtDisplay (UITopLevel ()),DefaultScreen (XtDisplay (UITopLevel ())) ));

	return (pixmap);
	}
