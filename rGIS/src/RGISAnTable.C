/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - ASRC/CUNY

RGISAnTable.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <rgis.H>

extern void RGISAnGNUXYPlotCBK (Widget,RGISWorkspace *,XmAnyCallbackStruct *);

UIMenuItem RGISAnalyseTableMenu [] = {
	UIMenuItem ((char *) "GNU XY-Plot",				RGISTableGroup,	UIMENU_NORULE, 				(UIMenuCBK) RGISAnGNUXYPlotCBK,						(char *) NULL),
	UIMenuItem ()};
