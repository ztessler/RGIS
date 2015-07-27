/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

RGISEditPoly.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <rgis.H>

void RGISEditPolyFourColorCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBVPolyIF *polyIF = new DBVPolyIF (dataset->Data ());

	UIPauseDialogOpen ((char *) "Four Coloring Polygons");
	polyIF->FourColoring ();
	UIPauseDialogClose ();
	}
