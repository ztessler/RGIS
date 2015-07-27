/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

CMDgrdCalculate.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>
#include <CMDexp.H>

class CMDgrdVariable
	{
	private:
		DBGridIF *GridIF;
		DBObjTableField *SourceFLD;
		DBObjTableField *TargetFLD;
		DBObjRecord *LayerRec;
	public:
	CMDgrdVariable (char *varName)
		{
		GridIF = (DBGridIF *) NULL;
		SourceFLD = (DBObjTableField *) NULL;
		TargetFLD = new DBObjTableField (varName,DBVariableFloat,"%10.3f",sizeof (DBFloat),false);
		}
	~CMDgrdVariable ()
		{
		DBObjData *data;

		if (GridIF != (DBGridIF *) NULL)
			{
			data = GridIF->Data ();
			delete GridIF;
			delete data;
			}
		}
	DBInt Configure (DBObjTable *table, bool flat)
		{
		DBInt i;
		char *dataName;
		char *fieldName = (char *) NULL;
		DBObjData *data;

		dataName = TargetFLD->Name ();

		if ((dataName = (char *) malloc (strlen (dataName) + 1)) == (char *) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (CMfailed); }
		strcpy (dataName, TargetFLD->Name ());

		for (i = 0;i < (DBInt) strlen (dataName);++i)
			if (dataName [i] == ':') { fieldName = dataName + i + 1; dataName [i] = '\0'; i = 0; break;}

		data = new DBObjData ();
		if ((strcmp (dataName,"stdin") == 0 ? data->Read (stdin) : data->Read (dataName)) == DBFault)
			{ CMmsgPrint (CMmsgUsrError,"%s",dataName); return (CMfailed); }

		if (data->Type () == DBTypeGridDiscrete)
			{
			DBObjTable *table = data->Table (DBrNItems);

			if (fieldName == (char *) NULL) fieldName = DBrNGridValue;
			if ((SourceFLD = table->Field (fieldName)) == (DBObjTableField *) NULL)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid field [%s]!",fieldName);
				return (CMfailed);
				}
			strcpy (dataName, TargetFLD->Name ());
			delete TargetFLD;
			TargetFLD = new DBObjTableField (*SourceFLD);
			TargetFLD->Name (dataName);
			}
		else
			{
			if (fieldName != (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Continuous grid field is referenced!"); free (dataName); return (CMfailed); }
			}
		table->AddField (TargetFLD);
		GridIF = new DBGridIF (data, flat);
		free (dataName);
		return (DBSuccess);
		}
	DBInt    Projection () const { return (GridIF->Data ()->Projection ()); };
	DBRegion Extent () const { DBObjData *data = GridIF->Data (); return (data->Extent ()); }
	DBFloat CellWidth  () const { return (GridIF->CellWidth ()); }
	DBFloat CellHeight () const { return (GridIF->CellWidth ()); }
	DBInt LayerNum () const { return (GridIF->LayerNum ()); }

	char *CurrentLayer (DBInt layerID)
		{
		if (layerID == DBFault) LayerRec = GridIF->Layer (0);
		else	LayerRec = layerID < GridIF->LayerNum () ? GridIF->Layer (layerID) :
								GridIF->Layer (layerID % GridIF->LayerNum ());
		return (LayerRec->Name ());
		}

	DBInt FindLayer (char *layerName)
		{
		DBInt layerID, i = 0;
		DBObjRecord *layerRec;
		char *name;

		for (layerID = 0;layerID < GridIF->LayerNum ();++layerID)
			{
			layerRec = GridIF->Layer (layerID);

			name = layerRec->Name ();
			if (strncmp (name,"XXXX-",5) == 0)
				{ i = 5; if ((strlen (name) - 5) == 0) return (layerID); }
			if (strncmp (name + i,layerName + i,strlen (name) - i) == 0) return (layerID);
			}
		return (CMfailed);
		}
	DBInt LayerIsDated (DBInt layerID)
		{
		DBObjRecord *layerRec;
		DBDate date;
		layerRec = GridIF->Layer (layerID);
		date.Set (layerRec->Name ());
		if (date.Year () != DBDefaultMissingIntVal) return (true);
		return (date.Month () != DBDefaultMissingIntVal ? true : false);
		}
	void GetVariable (DBObjRecord *record,DBCoordinate coord)
		{
		switch ((GridIF->Data ())->Type ())
			{
			case DBTypeGridContinuous:
				{
				DBFloat value;
				if (GridIF->Value (LayerRec,coord,&value))
						TargetFLD->Float (record,value);
				else	TargetFLD->Float (record,TargetFLD->FloatNoData ());
				} break;
			case DBTypeGridDiscrete:
				{
				DBObjRecord *grdRec;
				if ((grdRec = GridIF->GridItem (LayerRec,coord)) != (DBObjRecord *) NULL)
					switch (SourceFLD->Type ())
						{
						case DBVariableString:
							TargetFLD->String (record,SourceFLD->String (grdRec));	break;
						case DBVariableInt:
							TargetFLD->Int (record,SourceFLD->Int (grdRec));			break;
						case DBVariableFloat:
							TargetFLD->Float (record,SourceFLD->Float (grdRec));		break;
						default:	break;
						}
				else
					switch (SourceFLD->Type ())
						{
						case DBVariableString:
							TargetFLD->String (record,"");									break;
						case DBVariableInt:
							TargetFLD->Int (record,SourceFLD->IntNoData ());			break;
						case DBVariableFloat:
							TargetFLD->Float (record,SourceFLD->FloatNoData ());		break;
						default:	break;
						}
				} break;
			default:
				CMmsgPrint (CMmsgUsrError,"Invalid grid type in: CMDgrdVariable:GetVariable ()");
				break;
			}
		}
	};

class CMDgrdThreadData
	{
	private:
		size_t ExpNum, VarNum;
		DBInt  LayerNum, MasterVar;
		DBCoordinate CellSize;
		DBRegion Extent;
		DBObjRecord *LayerRec;
		DBGridIF *GridIF;
		CMDExpression **Expressions;
		CMDgrdVariable **GrdVar;
		DBObjectLIST<DBObject>	*Variables;
		DBObjTable *Table;
		DBMathOperand *Operand;
	public:
	CMDgrdThreadData ()
		{
		Expressions = (CMDExpression **) NULL;
		GrdVar      = (CMDgrdVariable **) NULL;
		ExpNum   = 0;
		VarNum   = 0;
		LayerNum = 0;
		Variables = new DBObjectLIST<DBObject>  ("Variables");
		Table     = new DBObjTable ("TEMPTable");
		}
	~CMDgrdThreadData ()
		{
		size_t i;
		if (ExpNum > 0) { for (i = 0;i < ExpNum; ++i) delete Expressions [i]; free (Expressions); }
		delete Variables;
		delete Table;
		}
	
	CMreturn AddExpression (char *varName, char *exprStr)
		{
		DBObjTableField *fieldPTR = new DBObjTableField (varName,DBVariableFloat,"%10.3f",sizeof (DBFloat),false);
		Table->AddField (fieldPTR);

		if ((Expressions = (CMDExpression **) realloc (Expressions,(ExpNum + 1) * sizeof (CMDExpression *))) == (CMDExpression **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); return (CMfailed); }
		Expressions [ExpNum] = new CMDExpression (varName, exprStr);
		if ((Expressions [ExpNum])->Expand (Variables) == DBFault)
			{ CMmsgPrint (CMmsgUsrError,"Invalid Expression!"); return (CMfailed); }
		ExpNum++;
		return (CMsucceeded);
		}
	CMreturn Configure (bool shrink, bool flat, char *expStr)
		{
		size_t i;
		DBInt recID, layerID;
		DBFloat floatVal;
		DBObject *obj;
		DBObjTableField *fieldPTR;
	
		Operand = new DBMathOperand (expStr);
		if (Operand->Expand (Variables) == DBFault) return (CMfailed);

		CellSize.X = CellSize.Y = DBHugeVal;
		for (recID = 0;recID < Variables->ItemNum ();++recID)
			{
			obj = Variables->Item (recID);
			if ((fieldPTR = Table->Field (obj->Name ())) != (DBObjTableField *) NULL) continue;

			if ((GrdVar = (CMDgrdVariable **) realloc (GrdVar,sizeof (CMDgrdVariable *) * (VarNum + 1))) == (CMDgrdVariable **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); return (CMfailed); }
			GrdVar [VarNum] = new CMDgrdVariable (obj->Name ());
			if (GrdVar [VarNum]->Configure (Table,flat) == DBFault)
				{
				for (i = 0;i <= VarNum;++i) delete GrdVar [i];
				free (GrdVar);
				return (CMfailed);
				}
			Extent.Expand (GrdVar [VarNum]->Extent ());
			floatVal = GrdVar [VarNum]->CellWidth ();  if (CellSize.X > floatVal) CellSize.X = floatVal;
			floatVal = GrdVar [VarNum]->CellHeight (); if (CellSize.Y > floatVal) CellSize.Y = floatVal;
			layerID = (GrdVar [VarNum])->LayerNum ();
			if (LayerNum < layerID) { LayerNum = layerID; MasterVar = VarNum; }
			VarNum++;
			}

		for (i = 0;i < ExpNum;++i) if (Expressions [i]->Configure (Table) == DBFault) return (CMfailed);
		Operand->Configure (Table->Fields ());
		if (shrink) for (i = 0;i < VarNum;++i) Extent.Shrink (GrdVar [i]->Extent ());
		return (CMsucceeded);
		}
	DBObjData *Compute (char *title, CMthreadUserExecFunc userFunc)
		{
		DBInt i, layerID, dataLayerID;
		size_t threadId;
		DBPosition pos;
		DBCoordinate coord;
		char *layerName;
		DBObjData *data;
		DBObjRecord *record;
		size_t threadsNum = CMthreadProcessorNum ();
		CMthreadTeam_p team = threadsNum > 0 ? CMthreadTeamCreate (threadsNum) : (CMthreadTeam_p) NULL;
		CMthreadJob_p  job  = (CMthreadJob_p) NULL;


		if ((data = DBGridCreate (title,Extent,CellSize)) == (DBObjData *) NULL) return ((DBObjData *) NULL);
		data->Projection(GrdVar [0]->Projection ()); // Taking projection from first grid variable

		GridIF = new DBGridIF (data);
			
		if (team != (CMthreadTeam_p) NULL)
			{
				if ((job  = CMthreadJobCreate (team, (void *) this,GridIF->RowNum () * GridIF->ColNum (), userFunc)) == (CMthreadJob_p) NULL)
					{
					CMmsgPrint (CMmsgAppError, "Job creation error in %s:%d",__FILE__,__LINE__);
					CMthreadTeamDestroy (team);
					return ((DBObjData *) NULL);
					}
				for (threadId = 0; threadId < job->ThreadNum; ++threadId)
					if ((job->Data [threadId] = (void *) (Table->Add ("TEMPRecord"))) == (void *) NULL)
						{ CMthreadTeamDestroy (team); return ((DBObjData *) NULL); }
			}
		else
			{
			if ((record = Table->Add ("TEMPRecord")) == (DBObjRecord *) NULL) return ((DBObjData *) NULL);
			}
		for (layerID = 0;layerID < LayerNum;++layerID)
			{
			layerName = GrdVar [MasterVar]->CurrentLayer (layerID);
			for (i = 0;i < (DBInt) VarNum;++i)
				{
				if ((dataLayerID = GrdVar [i]->FindLayer (layerName)) != DBFault)
					GrdVar [i]->CurrentLayer (dataLayerID);
				else
					{
					if (GrdVar [i]->LayerIsDated (0)) continue;
					GrdVar [i]->CurrentLayer (dataLayerID);
					}
				}
			if (layerID > 0) GridIF->AddLayer ((char *) "New Layer");
			LayerRec = GridIF->Layer (layerID);
			GridIF->RenameLayer (LayerRec,layerName);

			if (job != (CMthreadJob_p) NULL) CMthreadJobExecute (team, job);
			else
				for (pos.Row = 0;pos.Row < GridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < GridIF->ColNum ();++pos.Col)
						{
						GridIF->Pos2Coord (pos,coord);
						for (i = 0;i < (DBInt) VarNum;++i) GrdVar [i]->GetVariable (record,coord);
						for (i = 0;i < (DBInt) ExpNum;++i) Expressions [i]->Evaluate (record);
						GridIF->Value (LayerRec,pos,Operand->Float (record));
						}
			GridIF->RecalcStats (LayerRec);
			}
		delete GridIF;
		return (data);
		}
	void ComputeTask (DBObjRecord *record, size_t taskId)
		{
		size_t i;
		DBPosition pos;
		DBCoordinate coord;
		
		pos.Row = taskId / GridIF->ColNum ();
		pos.Col = taskId % GridIF->ColNum ();

		GridIF->Pos2Coord (pos,coord);
		for (i = 0;i < VarNum;++i) GrdVar [i]->GetVariable (record,coord);
		for (i = 0;i < ExpNum;++i) Expressions [i]->Evaluate (record);
		GridIF->Value (LayerRec,pos,Operand->Float (record));
		}
	};

static void _CMDgrdCalculateUserFunc (void *commonPtr,void *dataPtr, size_t taskId)
	{
	DBPosition pos;
	CMDgrdThreadData *threadData = (CMDgrdThreadData *) commonPtr;
	DBObjRecord *record = (DBObjRecord *) dataPtr;
	
	threadData->ComputeTask(record, taskId);
	}

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *expStr = (char *) NULL;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	int shadeSet = DBDataFlagDispModeContGreyScale;
	bool shrink = true, flat = false;
	DBObjData *data;
	CMDgrdThreadData *threadData = new CMDgrdThreadData ();

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-c","--calculate"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!");  delete threadData;  return (CMfailed); }
			if (expStr != (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Expression is already specified!"); delete threadData; return (CMfailed); }
			expStr = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--variable"))
			{
			char *varName;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing variable name!"); delete threadData; return (CMfailed); }
			varName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!");   delete threadData; return (CMfailed); }
			if (threadData->AddExpression (varName,argv [argPos]) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid Expression!"); delete threadData; return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-x","--extent"))
			{
			int codes [] = { true, false };
			const char *names [] = { "minimum","maximum", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing extent mode!");     delete threadData; return (CMfailed); }
			if ((ret = CMoptLookup (names,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid extent mode!");     delete threadData; return (CMfailed); }
			shrink = codes [ret];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--interpolate"))
			{
			int codes [] = { false, true };
			const char *names [] = { "surface","flat", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing interpolation mode!");     delete threadData; return (CMfailed); }
			if ((ret = CMoptLookup (names,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid interpolation mode!");     delete threadData; return (CMfailed); }
			flat = codes [ret];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!");        delete threadData; return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!");      delete threadData; return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");            delete threadData; return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!");      delete threadData; return (CMfailed); }
			version  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
			                        DBDataFlagDispModeContGreyScale,
			                        DBDataFlagDispModeContBlueScale,
			                        DBDataFlagDispModeContBlueRed,
			                        DBDataFlagDispModeContElevation };
			const char *shadeSets [] = { "standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     delete threadData; return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!");     delete threadData; return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <output file>",CMfileName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -c,--calculate   [expression]");
			CMmsgPrint (CMmsgInfo,"     -r,--variable    [variable expression]");
			CMmsgPrint (CMmsgInfo,"     -n,--interpolate [surface|flat]");
			CMmsgPrint (CMmsgInfo,"     -x,--extent      [maximum minimum]");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset    [standard|grey|blue|blue-to-red|elevation]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); delete threadData; return (CMfailed); }
		argPos++;
		}

	if (expStr == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Missing expression!"); delete threadData; return (CMfailed); }

	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); delete threadData; return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (threadData->Configure(shrink, flat, expStr) == CMfailed) { delete threadData; return (CMfailed); }
	
	if (title	== (char *) NULL)	title   = (char *) "Grid Calculate Result";
	if (subject == (char *) NULL) subject = (char *) "GridCalc";
	if (domain  == (char *) NULL) domain  = (char *) "Non-specified";
	if (version == (char *) NULL) version = (char *) "0.01pre";
	if (shadeSet == DBFault)     shadeSet = DBDataFlagDispModeContGreyScale;

	if ((data = threadData->Compute (title, _CMDgrdCalculateUserFunc)) == (DBObjData *) NULL) { delete threadData; return (CMfailed); }
		
	delete threadData; 

	data->Document (DBDocSubject,  subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,  version);
	data->Flags    (DBDataFlagDispModeContShadeSets,DBClear);
	data->Flags (shadeSet, DBSet);

	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Write (argv [1]) : data->Write (stdout);

	if (verbose) RGlibPauseClose ();
	return (ret);
	}
