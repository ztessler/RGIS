/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2017, UNH - CCNY/CUNY

RGISEditGrid.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <rgis.H>

static void _RGISEditGridLayersYearButtonCBK (Widget widget,DBInt step,XmAnyCallbackStruct *callData)

	{
	DBInt *timeStep;

	XtVaGetValues (widget,XmNuserData, &timeStep, NULL);
	*timeStep = step;
	}

static void _RGISEditGridLayersYearTextCBK (Widget widget,char *yearText,XmAnyCallbackStruct *callData)

	{
	char *text = XmTextFieldGetString (widget);
	strncpy (yearText,text,DBStringLength);
	XtFree (text);
	}

void RGISEditGridDateLayersCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	static char yearText [DBStringLength];
	static DBInt timeStep = DBTimeStepYear;
	static DBInt proc;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	static Widget dShell = (Widget) NULL;
	static Widget yearTextF;
	static Widget menu, button;

	if (dShell == (Widget) NULL)
		{
		Widget mainForm;
		XmString string;

		dShell = UIDialogForm ((char *) "Date Layers",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		menu = XmCreatePulldownMenu (mainForm,(char *) "RGISEditGridDateLayersTimeStepMenu",NULL,0);
		string = XmStringCreate ((char *) "Year",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISEditGridDateLayersTimeStepButton",xmPushButtonWidgetClass,menu,
												XmNlabelString,				string,
												XmNuserData,					&timeStep,
												NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditGridLayersYearButtonCBK,(XtPointer) DBTimeStepYear);
		string = XmStringCreate ((char *) "Month",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISEditGridDateLayersTimeStepButton",xmPushButtonWidgetClass,menu,
												XmNlabelString,				string,
												XmNuserData,					&timeStep,
												NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditGridLayersYearButtonCBK,(XtPointer) DBTimeStepMonth);
		string = XmStringCreate ((char *) "Day",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISEditGridDateLayersTimeStepButton",xmPushButtonWidgetClass,menu,
												XmNlabelString,				string,
												XmNuserData,					&timeStep,
												NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditGridLayersYearButtonCBK,(XtPointer) DBTimeStepDay);
		string = XmStringCreate ((char *) "Hour",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISEditGridDateLayersTimeStepButton",xmPushButtonWidgetClass,menu,
												XmNlabelString,				string,
												XmNuserData,					&timeStep,
												NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditGridLayersYearButtonCBK,(XtPointer) DBTimeStepHour);
		string = XmStringCreate ((char *) "Minute",UICharSetNormal);
		button = XtVaCreateManagedWidget ("RGISEditGridDateLayersTimeStepButton",xmPushButtonWidgetClass,menu,
												XmNlabelString,				string,
												XmNuserData,					&timeStep,
												NULL);
		XmStringFree (string);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditGridLayersYearButtonCBK,(XtPointer) DBTimeStepMinute);
		string = XmStringCreate ((char *) "Time Step:",UICharSetBold);
		menu = XtVaCreateManagedWidget ("RGISEditGridDateLayersTimeStepMenu",xmRowColumnWidgetClass,mainForm,
												XmNtopAttachment,				XmATTACH_FORM,
												XmNtopOffset,					10,
												XmNrightAttachment,			XmATTACH_FORM,
												XmNrightOffset,				10,
												XmNbottomAttachment,			XmATTACH_FORM,
												XmNbottomOffset,				10,
												XmNrowColumnType,				XmMENU_OPTION,
												XmNlabelString,				string,
												XmNsubMenuId,					menu,
												NULL);
		XmStringFree (string);

		yearTextF = XtVaCreateManagedWidget ("RGISEditGridDateLayersYearTextF",xmTextFieldWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				menu,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			menu,
								XmNrightOffset,			10,
								XmNbottomAttachment,		XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			menu,
								XmNmaxLength,				4,
								XmNcolumns,					4,
								NULL);
		XtAddCallback (yearTextF,XmNvalueChangedCallback,(XtCallbackProc) _RGISEditGridLayersYearTextCBK,yearText);
		string = XmStringCreate ((char *) "Begin Year",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditGridDateLayersYearLabel",xmLabelWidgetClass,mainForm,
								XmNtopAttachment,			XmATTACH_OPPOSITE_WIDGET,
								XmNtopWidget,				menu,
								XmNleftAttachment,		XmATTACH_FORM,
								XmNleftOffset,				10,
								XmNrightAttachment,		XmATTACH_WIDGET,
								XmNrightWidget,			yearTextF,
								XmNrightOffset,			10,
								XmNbottomAttachment,	  XmATTACH_OPPOSITE_WIDGET,
								XmNbottomWidget,			menu,
								XmNlabelString,			string,
								NULL);
		XmStringFree (string);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),true);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&proc);
		}

	proc = false;
	UIDialogFormPopup (dShell);
	while (UILoop ())
		{ }
	UIDialogFormPopdown (dShell);
	if (proc)
		{
		DBInt layerID, year;
		DBDate stepDate;
		DBGridIF *gridIF = new DBGridIF (dbData);
		DBObjRecord *layerRec;
		DBDate date;

		switch (timeStep)
			{
			case DBTimeStepYear:		stepDate.Set (1);				break;
			case DBTimeStepMonth:	stepDate.Set (0,1);			break;
			case DBTimeStepDay:		stepDate.Set (0,0,1);		break;
			case DBTimeStepHour:		stepDate.Set (0,0,0,1);		break;
			case DBTimeStepMinute:	stepDate.Set (0,0,0,0,1);	break;
			}
		if (sscanf (yearText,"%d",&year) != 1)
			{
			year = DBDefaultMissingIntVal;
			if (((stepDate.Year () > 0)	&& (gridIF->LayerNum () > 1)) ||
				 ((stepDate.Month () > 0)	&& (gridIF->LayerNum () > 12)) ||
				 ((stepDate.Day () > 0)		&& (gridIF->LayerNum () > 365)) ||
				 ((stepDate.Hour () > 0)	&& (gridIF->LayerNum () > 365 * 24)) ||
				 ((stepDate.Minute () > 0) && (gridIF->LayerNum () > 365 * 24 * 60)))
				 {
				 UIMessage ((char *) "Too Many Layers in Dataset");
				 }
			}
		UIPauseDialogOpen ((char *) "Labeling Layers");
		date.Set (year);
		if (stepDate.Month () > 0)		date.Set (year,0);
		if (stepDate.Day () > 0) 		date.Set (year,0,0);
		if (stepDate.Hour () > 0) 		date.Set (year,0,0,0);
		if (stepDate.Minute () > 0) 	date.Set (year,0,0,0,0);

		for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
			{
			layerRec = gridIF->Layer (layerID);
			if (UIPause (layerRec->RowID () * 100 / gridIF->LayerNum ()))  goto Stop;
			gridIF->RenameLayer (layerRec,date.Get ());
			date = date + stepDate;
			}
Stop:
		UIPauseDialogClose ();
		delete gridIF;
		}
	}

void RGISEditGridRenameLayerCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	char *layerName;
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBGridIF *gridIF;

	if ((layerName = UIGetString ((char *) "Layer Name",DBStringLength)) == (char *) NULL) return;
	gridIF= new DBGridIF (dbData);
	gridIF->RenameLayer (layerName);
	delete gridIF;
	workspace->CurrentData (dbData);
	}

void RGISEditGridStatsCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData = dataset->Data ();
	DBGridIF *gridIF = new DBGridIF (dbData);
	UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,dbData->Table (DBrNItems)));

	if (dbData->Type () == DBTypeGridContinuous)
			gridIF->RecalcStats ();
	else	gridIF->DiscreteStats ();
	if (tableCLS != (UITable *) NULL) tableCLS->Draw ();
	}

void RGISEditGridRemovePitsCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();
	DBObjData *netData = grdData->LinkedData ();

	UIPauseDialogOpen ((char *) "Removing Pits");
	RGlibGridRemovePits (netData,grdData);
	UIPauseDialogClose ();
	}

void RGISEditGridNetFilterCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *grdData = dataset->Data ();
	DBObjData *netData = grdData->LinkedData ();

	UIPauseDialogOpen ((char *) "Network Filtering");
    RGlibGridNetFilter (netData,grdData);
	UIPauseDialogClose ();
	}
