/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

MFModel.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <time.h>

static MFDomain_t *_MFDomain    = (MFDomain_t *) NULL;
static MFFunction *_MFFunctions = (MFFunction *) NULL;
static int _MFFunctionNum = 0;

MFVariable_t *MFVarSetPath (const char *,const char *, bool, int);

int MFModelAddFunction (MFFunction func) {

	if ((_MFFunctions = (MFFunction *) realloc (_MFFunctions, (_MFFunctionNum + 1) * sizeof (MFFunction))) == (MFFunction *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	_MFFunctions [_MFFunctionNum] = func;
	_MFFunctionNum++;
	return (CMsucceeded);
}

float MFModelGetXCoord (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].XCoord);
}

float MFModelGetYCoord (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].YCoord);
}

float MFModelGetLongitude (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Lon);
}

float MFModelGetLatitude (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Lat);
}

float MFModelGetArea (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Area * 1000000.0);
}

float MFModelGetLength (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Length * 1000.0);
}

int MFModelGetDownLink (int itemID,size_t linkNum) {
	if (itemID < 0) return (CMfailed);
	if (_MFDomain != (MFDomain_t *) NULL) return (CMfailed);
	if (_MFDomain->ObjNum <= itemID)  return (CMfailed);
	if (_MFDomain->Objects [itemID].DLinkNum <= linkNum) return (CMfailed);

	return (_MFDomain->Objects [itemID].DLinks [linkNum]);
}

float MFModelGet_dt () { return (86400.0); }

static struct output_s {
	char *Name;
	char *Path;
	bool State;
	} *_MFModelOutput = (struct output_s *) NULL;

static int _MFModelOutNum = 0;

static int _MFModelOutputNew (const char *name, const char *path, bool state) {
	_MFModelOutput = (struct output_s *) realloc (_MFModelOutput,sizeof (struct output_s)  * (_MFModelOutNum + 1));
	if (_MFModelOutput == (struct output_s *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	_MFModelOutput [_MFModelOutNum].Name  = (char *) malloc (strlen (name) + 1);
	_MFModelOutput [_MFModelOutNum].Path  = (char *) malloc (strlen (path) + 1);
	if ((_MFModelOutput [_MFModelOutNum].Name == (char *) NULL) ||
	    (_MFModelOutput [_MFModelOutNum].Path == (char *) NULL)) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	strcpy (_MFModelOutput [_MFModelOutNum].Name, name);
	strcpy (_MFModelOutput [_MFModelOutNum].Path, path);
	_MFModelOutput [_MFModelOutNum].Name  = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (_MFModelOutput [_MFModelOutNum].Name)));
	_MFModelOutput [_MFModelOutNum].Path  = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (_MFModelOutput [_MFModelOutNum].Path)));
	_MFModelOutput [_MFModelOutNum].State = state;
	_MFModelOutNum++;
	return (CMsucceeded);
}

static int _MFModelParse (int argc, char *argv [],int argNum, int (*conf) ()) {
	FILE *inFile;
	bool testOnly = false;
	int argPos, ret, help = false;
	int i, varID, intVal;
	float floatVal;
	char *startDate = (char *) NULL, *endDate = (char *) NULL;
	MFVariable_t *var;
	int MFLoadConfig (const char *,int (*) (const char *, const char *, bool));
	bool _MFOptionNew (char *,char *), _MFOptionTestInUse ();

	for (argPos = 1;argPos < argNum;) {
		if (CMargTest (argv [argPos],"-i","--input")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing input argument!\n");
				return (CMfailed);
			}
			for (i = 0;i < (int) strlen (argv[argPos]);++i) if (argv [argPos][i] == '=') break;
			if (i == (int) strlen (argv [argPos])) {
				CMmsgPrint  (CMmsgUsrError,"Illformed input variable [%s] in: %s:%d\n",argv [argPos],__FILE__,__LINE__);
				return (CMfailed);
			}
			argv [argPos][i] = '\0';
			if (MFVarSetPath (argv [argPos],argv [argPos] + i + 1, false, MFInput) == (MFVariable_t *) NULL)  return (CMfailed);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-o","--output")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing _MFModelOutput argument!\n");
				return (CMfailed);
			}
			for (i = 0;i < (int) strlen (argv[argPos]);++i) if (argv [argPos][i] == '=') break;
			if (i == (int) strlen (argv [argPos])) {
				CMmsgPrint (CMmsgUsrError,"Illformed _MFModelOutput variable [%s]!\n",argv [argPos]);
				return (CMfailed);
			}
			argv [argPos][i] = '\0';
			if (_MFModelOutputNew (argv [argPos],argv [argPos] + i + 1,false) == CMfailed) return (CMfailed);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
	 	}
		if (CMargTest (argv [argPos],"-t","--state")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing _MFModelOutput argument!\n");
				return (CMfailed);
			}
			for (i = 0;i < (int) strlen (argv[argPos]);++i) if (argv [argPos][i] == '=') break;
			if (i == (int) strlen (argv [argPos])) {
				CMmsgPrint (CMmsgUsrError,"Illformed _MFModelOutput variable [%s]!\n",argv [argPos]);
				return (CMfailed);
			}
			argv [argPos][i] = '\0';
			if (_MFModelOutputNew (argv [argPos],argv [argPos] + i + 1, true) == CMfailed) return (CMfailed);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
	 	}
		if (CMargTest (argv [argPos],"-s","--start")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing start time!\n");
				return (CMfailed);
			}
			startDate = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-n","--end")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing end time!\n");
				return (CMfailed);
			}
			endDate = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-T","--testonly")) {
			testOnly = true;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
		}
		if (CMargTest (argv [argPos],"-m","--message")) {
			const char *types [] = { "sys_error", "app_error", "usr_error", "debug", "warning", "info", (char *) NULL };
			CMmsgType msgTypes [] = { CMmsgSysError, CMmsgAppError, CMmsgUsrError, CMmsgDebug, CMmsgWarning, CMmsgInfo };
			int type;
			const char *modes [] = { "file:", "on", "off", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing message argument!\n");
				return (CMfailed);
			}

			if ((type = CMoptLookup (types,argv [argPos],false)) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Ignoring illformed message [%s]!\n",argv [argPos]);
			}
			else {
				switch (CMoptLookup (modes, argv [argPos] + strlen (types [type]) + 1, false)) {
					case 0: CMmsgSetStreamFile (msgTypes [type], argv [argPos] + strlen (types [type]) + 1 + strlen (modes [0]));
					case 1: CMmsgSetStatus     (msgTypes [type], true);  break;
					case 2: CMmsgSetStatus     (msgTypes [type], false); break;
					default:
						CMmsgPrint (CMmsgWarning,"Ignoring illformed message [%s]!\n",argv [argPos]);
						break;
				}
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-h","--help")) {
			help = true;
			CMmsgPrint (CMmsgInfo,"%s [options] <domain>\n",CMfileName (argv [0]));
			CMmsgPrint (CMmsgInfo,"     -s,  --start      [start date in the form of \"yyyy-mm-dd\"]\n");
			CMmsgPrint (CMmsgInfo,"     -n,  --end        [end date in the form of \"yyyy-mm-dd\"]\n");
			CMmsgPrint (CMmsgInfo,"     -i,  --input      [variable=source]\n");
			CMmsgPrint (CMmsgInfo,"     -o,  --output     [variable=destination]\n");
			CMmsgPrint (CMmsgInfo,"     -r,  --state      [variable=statefile]\n");
			CMmsgPrint (CMmsgInfo,"     -ol, --output-listfile [output listfile]\n");
			CMmsgPrint (CMmsgInfo,"     -p,  --option     [option=content]\n");
			CMmsgPrint (CMmsgInfo,"     -r,  --route      [variable]\n");
			CMmsgPrint (CMmsgInfo,"     -T,  --testonly\n");
			CMmsgPrint (CMmsgInfo,"     -m,  --message    [sys_error|app_error|usr_error|debug|warning|info]=[on|off|file=<filename>]\n");
			CMmsgPrint (CMmsgInfo,"     -h,  --help\n");
			break;
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1)) {
			CMmsgPrint (CMmsgUsrError,"Unknown option [%s]!\n",argv [argPos]);
			return (CMfailed);
		}
		argPos++;
	}

	if (startDate == (char *) NULL) startDate = "XXXX-01-01";
	if (endDate   == (char *) NULL) endDate   = "XXXX-12-31";

	if (!MFDateSetStart   (startDate)) { CMmsgPrint (CMmsgAppError,"Error: Invalid start date!\n"); ret = CMfailed; }
	if (!MFDateSetCurrent (startDate)) ret = CMfailed;
	if (!MFDateSetEnd     (endDate))   { CMmsgPrint (CMmsgAppError,"Error: Invalid end date!\n");   ret = CMfailed; }
	if (ret == CMfailed) return (CMfailed);

	ret = conf ();
	if (help) { MFOptionPrintList (); return (CMfailed); }
	if (ret == CMfailed) return (CMfailed);
	if (testOnly) {
		CMmsgPrint (CMmsgInfo, "ID  %10s %30s[%10s] %6s %5s NStep %3s %4s %8s Output",
			      "Start_Date", "Variable","Unit","Type", "TStep", "Set", "Flux", "Initial");
		for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID))
			if ((strncmp (var->Name,"__",2) != 0) || var->Initial)
				CMmsgPrint (CMmsgInfo, "%3i %10s %30s[%10s] %6s %5s %5d %3s %4s %8s %6s",
					varID,var->Header.Date,var->Name,var->Unit,MFVarTypeString (var->Header.DataType),MFDateTimeStepString (var->TStep),var->NStep,
					CMyesNoString (var->Set),CMyesNoString (var->Flux),CMyesNoString (var->Initial), CMyesNoString (var->OutPath != (char *) NULL));
		return (CMfailed);
	}

	if ((argNum) > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!");           return (CMfailed); }
	if ((argNum) < 2) { CMmsgPrint (CMmsgUsrError,"Missing Template Coverage!"); return (CMfailed); }

	if ((inFile = strcmp (argv [1],"-") != 0 ? fopen (argv [1],"r") : stdin) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError,"%s: Template Coverage [%s] Opening error!",CMfileName (argv [0]),argv [1]);
		return (CMfailed);
	}
	if ((_MFDomain = MFDomainGet (inFile)) == (MFDomain_t *) NULL)	return (CMfailed);

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
		if (var->InStream == (MFDataStream_t *) NULL) var->TStep = MFTimeStepDay;
		else if ((var->Initial == false) && (MFDateSetCurrent (var->Header.Date) == false))
			CMmsgPrint (CMmsgWarning,"Warning: Invalid date in input (%s)",var->Name);
		if (var->Flux) sprintf (var->Unit + strlen (var->Unit),"/%s",MFDateTimeStepUnit (var->TStep));
	}
	MFDateRewind ();

	if (strcmp (startDate,MFDateGetCurrent ()) != 0)//zero. strings are equal
		CMmsgPrint (CMmsgWarning,"Warning: Adjusting start date (%s,%s)!",startDate, MFDateGetCurrent ());

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
		switch (var->Header.DataType) {
			case MFInput:
				CMmsgPrint (CMmsgAppError, "Error: Unresolved variable (%s [%s] %s)!",var->Name,var->Unit, MFVarTypeString (var->Header.DataType));
				ret = CMfailed;
				break;
			case MFRoute:
			case MFOutput:
				var->Header.DataType = MFFloat;
				var->Header.Missing.Float = MFDefaultMissingFloat;
			default:
				if (var->Data == (void *) NULL) {
					var->Header.ItemNum = _MFDomain->ObjNum;
					if ((var->Data = (void *) calloc (var->Header.ItemNum,MFVarItemSize (var->Header.DataType))) == (void *) NULL) {
						CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
						ret = CMfailed;
					}
					switch (var->Header.DataType) {
						case MFByte:
						case MFShort:
						case MFInt:
							intVal   = var->InStream == (MFDataStream_t *) NULL ? 0 : var->InStream->Handle.Int;
							for (i = 0;i < var->Header.ItemNum;++i) MFVarSetInt   (varID,i,intVal);
							break;
						case MFFloat:
						case MFDouble:
							floatVal = var->InStream == (MFDataStream_t *) NULL ? 0.0 : var->InStream->Handle.Float;
							for (i = 0;i < var->Header.ItemNum;++i) MFVarSetFloat (varID,i,floatVal);
							break;
					}
				}
				else {
					if (var->Header.ItemNum != _MFDomain->ObjNum) {
						CMmsgPrint (CMmsgAppError,"Error: Inconsistent data stream (%s [%s])!",var->Name,var->Unit);
						ret = CMfailed;
					}
					else if (var->Initial) {
						while (MFDataStreamRead (var) == MFContinue);
						MFDataStreamClose (var->InStream);
						var->InPath = (char *) NULL;
						var->InStream = (MFDataStream_t *) NULL;
					}
					switch (var->TStep) {
						default:	var->NStep = 1; break;
						case MFTimeStepMonth: var->NStep = MFDateGetMonthLength (); break;
						case MFTimeStepYear:	 var->NStep = 365; break;
					}
				}
				if (!var->Initial && !var->Set) {
					CMmsgPrint (CMmsgWarning,"Warning: Ignoring unused variable (%s)!",var->Name);
					MFDataStreamClose (var->InStream);
					var->InPath = (char *) NULL;
					var->InStream = (MFDataStream_t *) NULL;
					var->Set = true;
					var->Initial = true;
				}
				break;
		}
	}

	for (i = 0;i < _MFModelOutNum;++i) MFVarSetPath (_MFModelOutput [i].Name, _MFModelOutput [i].Path, _MFModelOutput [i].State, MFOutput);
	free (_MFModelOutput);

	CMmsgPrint (CMmsgInfo, "ID  %10s %30s[%10s] %6s %5s NStep %3s %4s %8s Output", "Start_Date", "Variable","Unit","Type", "TStep", "Set", "Flux", "Initial");
	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID))
		if ((strncmp (var->Name,"__",2) != 0) || var->Initial)
			CMmsgPrint (CMmsgInfo, "%3i %10s %30s[%10s] %6s %5s %5d %3s %4s %8s %6s",
				varID,var->Header.Date,var->Name,var->Unit,MFVarTypeString (var->Header.DataType),MFDateTimeStepString (var->TStep),var->NStep,
				CMyesNoString (var->Set),CMyesNoString (var->Flux),CMyesNoString (var->Initial), CMyesNoString (var->OutPath != (char *) NULL));
	if (ret == CMfailed) return (CMfailed);
	_MFOptionTestInUse ();
	return (ret);
}

static bool _MFModelReadInput (char *time)

	{
	int varID;
	MFVariable_t *var;

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID))
		if (var->InStream == (MFDataStream_t *) NULL) var->Set = var->Initial;
		else {
			var->Set = true;
			if (MFDateCompare (time,var->Header.Date,var->Initial)) continue;
			do	{
				switch (MFDataStreamRead (var)) {
					case CMfailed:
						CMmsgPrint (CMmsgAppError,"Error: Variable [%s] reading error!",var->Name);
						return (MFStop);
					case MFStop:
						MFDataStreamClose (var->InStream);
						if (strncmp (var->Header.Date,MFDateClimatologyStr,strlen (MFDateClimatologyStr)) != 0) {
							var->InStream = (MFDataStream_t *) NULL;
							CMmsgPrint (CMmsgInfo, "time is %s, HeaderDate is %s", time, var->Header.Date) ;
							CMmsgPrint (CMmsgInfo, "Variable Name %s\n", var->Name);
							return (MFStop);
						}
					 	if ((var->InStream = MFDataStreamOpen (var->InPath,"r")) == (MFDataStream_t *) NULL) {
							CMmsgPrint (CMmsgAppError,"Error: Variable [%s] reopening error!",var->Name);
							return (MFStop);
						}
					default: break;
				}
			} while (!MFDateCompare (time,var->Header.Date,var->Initial));
			switch (var->TStep) {
				default:	var->NStep = 1; break;
				case MFTimeStepMonth: var->NStep = MFDateGetMonthLength (); break;
				case MFTimeStepYear:	 var->NStep = 365; break;
			}
		}
	return (MFContinue);
}

static void _MFUserFunc (void *commonPtr,void *threadData, size_t taskId) {
	int iFunc, varID, link, uLink, objectId;
	MFVariable_t *var;
	double value;

	objectId = _MFDomain->ObjNum - taskId - 1;
	
	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID))
		if (var->Route) {
			value = 0.0;
			for (link = 0; link < _MFDomain->Objects [objectId].ULinkNum; ++link) {
				uLink = _MFDomain->Objects [objectId].ULinks [link];
				value += MFVarGetFloat (varID,uLink,0.0);
			}
			MFVarSetFloat (varID, objectId, value);
		}
	for (iFunc = 0;iFunc < _MFFunctionNum; ++iFunc) (_MFFunctions [iFunc]) (objectId);
}

int MFModelRun (int argc, char *argv [], int argNum, int (*conf) ()) {
	int i, iFunc, varID, dlink, taskId;
	char *timeCur, timeWritten [MFDateStringLength];
	MFVariable_t *var;
	time_t sec;
	size_t threadsNum = CMthreadProcessorNum ();
	CMthreadTeam_p team = threadsNum > 0 ? CMthreadTeamCreate (threadsNum) : (CMthreadTeam_p) NULL;
	CMthreadJob_p  job  = (CMthreadJob_p) NULL;

	if (_MFModelParse (argc,argv,argNum,conf) == CMfailed) return (CMfailed);

	timeCur = MFDateGetCurrent ();

	time(&sec);
	CMmsgPrint (CMmsgInfo, "Model run started at... %s  started at %.24s", timeCur, ctime(&sec));
	if (_MFModelReadInput (timeCur) == MFStop) {
		CMmsgPrint (CMmsgInfo, "MFModelReadInput(%s) returned MFStop in: %s:%d",timeCur,__FILE__,__LINE__);
		return (CMfailed);
	}
	if (team != (CMthreadTeam_p) NULL) {
		if ((job  = CMthreadJobCreate (team, (void *) NULL, _MFDomain->ObjNum, _MFUserFunc)) == (CMthreadJob_p) NULL) {
			CMmsgPrint (CMmsgAppError, "Job creation error in %s:%d",__FILE__,__LINE__);
			CMthreadTeamDestroy (team);
			return (CMfailed);
			}
		for (i = 0;i < _MFDomain->ObjNum; ++i) {
			dlink  = _MFDomain->Objects [i].DLinkNum == 1 ? _MFDomain->Objects [i].DLinks [0] : i;
			dlink  = _MFDomain->ObjNum - dlink - 1;
			taskId = _MFDomain->ObjNum - i - 1;
			CMthreadJobTaskDependent (job, taskId, dlink);
		}
		do {
			CMmsgPrint (CMmsgDebug, "Computing: %s", timeCur);

			CMthreadJobExecute (team, job);

			for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
				if (var->OutStream != (MFDataStream_t *) NULL) {
					if (var->State != true) MFDataStreamWrite (var, timeCur);
				}
			}
		} while ((timeCur = MFDateAdvance ()) != (char *) NULL ? _MFModelReadInput (timeCur) : MFStop);
		CMthreadJobDestroy  (job);
		CMthreadTeamDestroy (team);
	}
	else // TODO Single CPU
		do	{
			int link, uLink;
			double value;

			CMmsgPrint (CMmsgDebug, "Computing: %s", timeCur);
			for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID))
				if (var->Route) { for (i = 0;i < _MFDomain->ObjNum; ++i)  MFVarSetFloat (varID, i, 0.0); }

			for (i = _MFDomain->ObjNum - 1;i >= 0; --i) {
				for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
					if (var->Route) {
						value = 0.0;
						for (link = 0; link < _MFDomain->Objects [i].ULinkNum; ++link) {
							uLink = _MFDomain->Objects [i].ULinks [link];
							value += MFVarGetFloat (varID,uLink,0.0);
							}
						MFVarSetFloat (varID, i, value);
					}
				}
				for (iFunc = 0;iFunc < _MFFunctionNum; ++iFunc) (_MFFunctions [iFunc]) (i);
			}

			for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
				if (var->OutStream != (MFDataStream_t *) NULL) {
					if (var->State != true) MFDataStreamWrite (var, timeCur);
				}
			strcpy (timeWritten,timeCur);
			}
		} while ((timeCur = MFDateAdvance ()) != (char *) NULL ? _MFModelReadInput (timeCur) : MFStop);

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
		if (var->InStream  != (MFDataStream_t *) NULL) MFDataStreamClose (var->InStream);
		if (var->OutStream != (MFDataStream_t *) NULL) {
			if (var->State) MFDataStreamWrite (var, timeWritten);
			MFDataStreamClose (var->OutStream);
		}
		free (var->Data);
	}
	return (CMsucceeded);
}
