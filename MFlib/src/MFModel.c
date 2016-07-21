/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

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

typedef struct varEntry_s {
	bool InUse;
	char *Name;
	char *Path;
} varEntry_t;

static varEntry_t *_MFModelVarEntryNew (varEntry_t *varEntries, int entryID, const char *name, const char *path) {
	varEntries = (varEntry_t *) realloc (varEntries,sizeof (varEntry_t)  * (entryID + 1));
	if (varEntries == (varEntry_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
		return ((varEntry_t *) NULL);
	}
	varEntries [entryID].Name  = (char *) malloc (strlen (name) + 1);
	varEntries [entryID].Path  = (char *) malloc (strlen (path) + 1);
	if ((varEntries [entryID].Name == (char *) NULL) || (varEntries [entryID].Path == (char *) NULL)) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
        return ((varEntry_t *) NULL);
	}
	strcpy (varEntries [entryID].Name, name);
	strcpy (varEntries [entryID].Path, path);
	varEntries [entryID].Name  = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (varEntries [entryID].Name)));
	varEntries [entryID].Path  = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (varEntries [entryID].Path)));
	varEntries [entryID].InUse = false;
	return (varEntries);
}

static varEntry_t *_MFModelVarEntryFind (varEntry_t *varEntries, int varEntryNum, const char *name) {
	int i;

	for (i = 0; i < varEntryNum; ++i) if (strcmp (varEntries [i].Name,name) == 0) return (varEntries + i);
	return ((varEntry_t *) NULL);
}

static void _MFModelVarEntriesFree (varEntry_t *varEntries, int varEntryNum) {
	int i;
	if (varEntryNum > 0) {
		for (i = 0; i < varEntryNum; ++i) {
			free (varEntries [i].Name);
			if (varEntries [i].InUse == false) free (varEntries [i].Path);
		}
		free (varEntries);
	}
}

static void _MFModelVarPrintOut (const char *label) {
    int varID;
    MFVariable_t *var;

    CMmsgPrint (CMmsgInfo, "ID  %10s %30s[%10s] %6s %5s NStep %3s %4s %8s Output", label, "Variable","Unit","Type", "TStep", "Set", "Flux", "Initial");
    for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID))
        if ((strncmp (var->Name,"__",2) != 0) || var->Initial)
            CMmsgPrint (CMmsgInfo, "%3i %10s %30s[%10s] %6s %5s %5d %3s %4s %8s %6s",
                        varID,var->Date,var->Name,var->Unit,MFVarTypeString (var->Type),MFDateTimeStepString (var->TStep),var->NStep,
                        CMyesNoString (var->Set),CMyesNoString (var->Flux),CMyesNoString (var->Initial), CMyesNoString (var->OutputPath != (char *) NULL));
}

static int _MFModelParse (int argc, char *argv [],int argNum, int (*mainDefFunc) (), char **domainFile, char **startDate, char **endDate) {
	bool testOnly = false, resolved = true;
	int argPos, ret, help = false;
	int i, varID;
	varEntry_t *inputVars  = (varEntry_t *) NULL;
	varEntry_t *outputVars = (varEntry_t *) NULL;
	varEntry_t *stateVars  = (varEntry_t *) NULL;
	varEntry_t *varEntry;
	int inputVarNum = 0, outputVarNum = 0, stateVarNum = 0;
	MFVariable_t *var;
    bool _MFOptionTestInUse ();

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
			inputVars = _MFModelVarEntryNew (inputVars, inputVarNum, argv [argPos],argv [argPos] + i + 1);
			if (inputVars == (varEntry_t *) NULL) return (CMfailed); else inputVarNum++;
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
			outputVars = _MFModelVarEntryNew (outputVars, outputVarNum, argv [argPos],argv [argPos] + i + 1);
			if (outputVars == (varEntry_t *) NULL) return (CMfailed); else outputVarNum++;
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
			stateVars = _MFModelVarEntryNew (stateVars, stateVarNum, argv [argPos],argv [argPos] + i + 1);
			if (stateVars == (varEntry_t *) NULL) return (CMfailed); else stateVarNum++;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
	 	}
		if (CMargTest (argv [argPos],"-s","--start")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing start time!\n");
				return (CMfailed);
			}
			*startDate = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-n","--end")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing end time!\n");
				return (CMfailed);
			}
			*endDate = argv [argPos];
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
			CMmsgPrint (CMmsgInfo,"     -t,  --state      [variable=statefile]\n");
			CMmsgPrint (CMmsgInfo,"     -p,  --option     [option=content]\n");
			CMmsgPrint (CMmsgInfo,"     -T,  --testonly\n");
			CMmsgPrint (CMmsgInfo,"     -m,  --message    [sys_error|app_error|usr_error|debug|warning|info]=[on|off|file=<filename>]\n");
			CMmsgPrint (CMmsgInfo,"     -h,  --help\n");
			break;
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1)) {
			CMmsgPrint (CMmsgUsrError,"Unknown option [%s]!\n",argv [argPos]);
			_MFModelVarEntriesFree(inputVars,  inputVarNum);
			_MFModelVarEntriesFree(outputVars, outputVarNum);
			_MFModelVarEntriesFree(stateVars,  stateVarNum);
			return (CMfailed);
		}
		argPos++;
	}

	if (*startDate == (char *) NULL) *startDate = "XXXX-01-01";
	if (*endDate   == (char *) NULL) *endDate   = "XXXX-12-31";

    if (!MFDateSetCurrent (*startDate)) { CMmsgPrint (CMmsgUsrError,"Error: Invalid start date!\n"); ret = CMfailed; }
    if (!MFDateSetCurrent (*endDate))   { CMmsgPrint (CMmsgAppError,"Error: Invalid end date!\n");   ret = CMfailed; }
	if (ret == CMfailed) {
		_MFModelVarEntriesFree(inputVars,  inputVarNum);
		_MFModelVarEntriesFree(outputVars, outputVarNum);
		_MFModelVarEntriesFree(stateVars,  stateVarNum);
		return (CMfailed);
	}

	ret = mainDefFunc ();

	if ((ret == CMfailed) || (help)) {
		MFOptionPrintList ();
		_MFModelVarEntriesFree(inputVars,  inputVarNum);
		_MFModelVarEntriesFree(outputVars, outputVarNum);
		_MFModelVarEntriesFree(stateVars,  stateVarNum);
		return (CMfailed);
	}

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
        if (var->Type != MFInput) strcpy (var->Date,"Calculated");
		if ((varEntry = _MFModelVarEntryFind (inputVars,  inputVarNum,  var->Name))  != (varEntry_t *) NULL) {
            if (var->Type != MFInput) var->Set = true;
            if ((var->Type == MFOutput) && (var->Initial == false))
				CMmsgPrint (CMmsgInfo,"Input variable [%s] is used for output before reading the input!\n",var->Name);
			varEntry->InUse = true;
			var->InputPath = varEntry->Path;
            strcpy (var->Date,"From input");
            var->Set = true;
		}
		else {
			if ((var->Type == MFInput) && (var->Initial == false)) {
				CMmsgPrint (CMmsgInfo,"Unresolved variable: %s", var->Name);
				resolved = false;
			}
		}
		if ((varEntry = _MFModelVarEntryFind (outputVars, outputVarNum, var->Name)) != (varEntry_t *) NULL) {
			varEntry->InUse = true;
			var->OutputPath = varEntry->Path;
		}
		if ((varEntry = _MFModelVarEntryFind (stateVars, stateVarNum,   var->Name)) != (varEntry_t *) NULL) {
			varEntry->InUse = true;
			var->StatePath = varEntry->Path;
		}
	}
    _MFOptionTestInUse ();
	for (i = 0; i < inputVarNum;  ++i)
		if (inputVars [i].InUse  == false) CMmsgPrint(CMmsgInfo,"Unused input variable : %s",  inputVars [i].Name);
	for (i = 0; i < outputVarNum; ++i)
		if (outputVars [i].InUse == false) CMmsgPrint(CMmsgInfo,"Unused output variable : %s", outputVars [i].Name);
	for (i = 0; i < stateVarNum;  ++i)
		if (stateVars [i].InUse  == false) CMmsgPrint(CMmsgInfo,"Unused state variable : %s",  stateVars [i].Name);

	_MFModelVarEntriesFree(inputVars,  inputVarNum);
	_MFModelVarEntriesFree(outputVars, outputVarNum);
	_MFModelVarEntriesFree(stateVars,  stateVarNum);

	if ((argNum) > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!");           return (CMfailed); }
	if ((argNum) < 2) { CMmsgPrint (CMmsgUsrError,"Missing Template Coverage!"); return (CMfailed); }
	*domainFile = argv [1];

	if (testOnly) { _MFModelVarPrintOut ("Source"); return (CMfailed); }
	return (resolved ? CMsucceeded : CMfailed);
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

int MFModelRun (int argc, char *argv [], int argNum, int (*mainDefFunc) ()) {
	FILE *inFile;
	int i, iFunc, varID, dlink, taskId, intVal, ret, timeStep = MFTimeStepDay;
	float floatVal;
	char *startDate = (char *) NULL, *endDate = (char *) NULL, *domainFileName = (char *) NULL;
	char dateCur [MFDateStringLength], dateNext [MFDateStringLength], *climatologyStr;
    void *buffer;
	MFVariable_t *var;
	time_t sec;
	size_t threadsNum = CMthreadProcessorNum ();
	CMthreadTeam_p team = threadsNum > 0 ? CMthreadTeamCreate (threadsNum) : (CMthreadTeam_p) NULL;
	CMthreadJob_p  job;
    bool parallelIO;

    if ((buffer = getenv ("GHAASparallelIO")) != (char *) NULL) {
        parallelIO = strcmp (buffer, "true") != 0 ? true : false;
    }
    else parallelIO = false;

	if (_MFModelParse (argc,argv,argNum, mainDefFunc, &domainFileName, &startDate, &endDate) == CMfailed) return (CMfailed);

    switch (strlen (startDate)) {
        case  4: timeStep = MFTimeStepYear;  climatologyStr = MFDateClimatologyYearStr;  break;
        case  7: timeStep = MFTimeStepMonth; climatologyStr = MFDateClimatologyMonthStr; break;
        case 10: timeStep = MFTimeStepDay;   climatologyStr = MFDateClimatologyDayStr;   break;
        case 13: timeStep = MFTimeStepHour;  climatologyStr = MFDateClimatologyHourStr;  break;
        default:
            CMmsgPrint (CMmsgUsrError,"Invalid date in model run %s\n", startDate);
            return (CMfailed);
    }

    if ((inFile = strcmp (domainFileName,"-") != 0 ? fopen (argv [1],"r") : stdin) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError,"%s: Template Coverage [%s] Opening error!",CMfileName (argv [0]),argv [1]);
		return (CMfailed);
	}
	if ((_MFDomain = MFDomainRead (inFile)) == (MFDomain_t *) NULL)	return (CMfailed);

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
		var->ItemNum = _MFDomain->ObjNum;
		if (var->InputPath != (char *) NULL) {
			if ((var->InStream = MFDataStreamOpen(var->InputPath,"r")) == (MFDataStream_t *) NULL) {
				return (CMfailed);
			}
			if (parallelIO) {
				// TODO
			}
			else {
				if (var->Initial) {
					if (MFdsRecordRead (var,climatologyStr) == CMfailed) return (CMfailed);
					if (MFDataStreamClose (var->InStream)   == CMfailed) return (CMfailed);
					var->InStream = (MFDataStream_t *) NULL;
				}
				else {
					if (MFdsRecordRead(var, startDate) == CMfailed) return (CMfailed);
				}
			}
		}
		else var->TStep = timeStep;

		if (var->Flux) sprintf (var->Unit + strlen(var->Unit), "/%s", MFDateTimeStepUnit(var->TStep));
		switch (var->Type) {
			case MFInput:
				CMmsgPrint (CMmsgAppError, "Error: Unresolved variable (%s [%s] %s)!",var->Name,var->Unit, MFVarTypeString (var->Type));
				ret = CMfailed;
				break;
			case MFRoute:
			case MFOutput:
				var->Type = MFFloat;
				var->Missing.Float = MFDefaultMissingFloat;
                strcpy (var->Date,"calculated");
			default:
                if (parallelIO || (var->InBuffer == (void *) NULL)) {
                    if ((var->ProcBuffer = (void *) calloc (var->ItemNum,MFVarItemSize (var->Type))) == (void *) NULL) {
                        CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
                        ret = CMfailed;
                    }
                }
                else var->ProcBuffer = var->InBuffer;
                break;
        }
        if (var->OutputPath != (char *) NULL) {
            if ((var->OutStream = MFDataStreamOpen(var->OutputPath,"w")) == (MFDataStream_t *) NULL) return (CMfailed);
            if (parallelIO) {
                if ((var->OutBuffer = (void *) malloc(var->ItemNum * MFVarItemSize(var->Type))) == (void *) NULL) {
                    CMmsgPrint(CMmsgSysError, "Variable [%s] allocation error in: %s:%d\n", var->Name, __FILE__,__LINE__);
                    return (CMfailed);
                }
            }
            else var->OutBuffer = var->ProcBuffer;
        }
	}
    _MFModelVarPrintOut ("Start date");
	if (ret == CMfailed) return (CMfailed);

	time(&sec);
    strcpy (dateCur,MFDateGetCurrent ());
	CMmsgPrint (CMmsgInfo, "Model run started at... %s  started at %.24s", dateCur, ctime(&sec));

	if (team != (CMthreadTeam_p) NULL) /* Multiple CPU */ {
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
		while (strcmp (dateCur,endDate) <= 0) {
            CMmsgPrint (CMmsgDebug, "Computing: %s", dateCur);
			strcpy (dateNext,MFDateGetNext ());
            if (parallelIO) {
                // TODO
            }
			CMthreadJobExecute (team, job);

            if (parallelIO) {
                // TODO
            }
            else {
                for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
                    if (var->OutStream != (MFDataStream_t *) NULL) {
                        if (MFdsRecordWrite (var, dateCur) == CMfailed) {
                            CMmsgPrint (CMmsgAppError,"Variable (%s) writing error!\n",var->Name);
                            ret = CMfailed;
                        }
                    }
                }
            }
            strcpy (dateCur,dateNext);
		}
		CMthreadJobDestroy  (job);
		CMthreadTeamDestroy (team);
	}
	else {/* Single CPU */
        int link, uLink;
        double value;
        while (strcmp(dateCur, endDate) <= 0) {
            CMmsgPrint(CMmsgDebug, "Computing: %s", dateCur);
            strcpy (dateNext,MFDateGetNext ());
            if (parallelIO) {
                // TODO
            }

            for (var = MFVarGetByID(varID = 1); var != (MFVariable_t *) NULL; var = MFVarGetByID(++varID))
                if (var->Route) { for (i = 0; i < _MFDomain->ObjNum; ++i) MFVarSetFloat(varID, i, 0.0); }

            for (i = _MFDomain->ObjNum - 1; i >= 0; --i) {
                for (var = MFVarGetByID(varID = 1); var != (MFVariable_t *) NULL; var = MFVarGetByID(++varID)) {
                    if (var->Route) {
                        value = 0.0;
                        for (link = 0; link < _MFDomain->Objects[i].ULinkNum; ++link) {
                            uLink = _MFDomain->Objects[i].ULinks[link];
                            value += MFVarGetFloat(varID, uLink, 0.0);
                        }
                        MFVarSetFloat(varID, i, value);
                    }
                }
                for (iFunc = 0; iFunc < _MFFunctionNum; ++iFunc) (_MFFunctions[iFunc])(i);
            }
            if (parallelIO) {

            }
            else {
                for (var = MFVarGetByID(varID = 1); var != (MFVariable_t *) NULL; var = MFVarGetByID(++varID)) {
                    if (var->OutStream != (MFDataStream_t *) NULL) {
                        if (MFdsRecordWrite(var, dateCur) == CMfailed) {
                            CMmsgPrint (CMmsgAppError,"Variable (%s) writing error!\n",var->Name);
                            ret = CMfailed;
                        }
                    }
                }
            }
            strcpy (dateCur,dateNext);
        }
    }
	for (var = MFVarGetByID (varID = 1);var != (MFVariable_t *) NULL;var = MFVarGetByID (++varID)) {
		if (var->InStream  != (MFDataStream_t *) NULL) MFDataStreamClose (var->InStream);
		if (var->OutStream != (MFDataStream_t *) NULL) MFDataStreamClose (var->OutStream);
        if (var->StatePath != (char *) NULL) {
            if ((var->OutStream = MFDataStreamOpen (var->StatePath,"w")) == (MFDataStream_t *) NULL) ret = CMfailed;
			if (MFdsRecordWrite(var, dateCur) == CMfailed) {
                CMmsgPrint (CMmsgAppError,"Variable (%s) writing error!\n",var->Name);
                ret = CMfailed;
            }
			MFDataStreamClose (var->OutStream);
		}
		free (var->ProcBuffer);
	}
	return (ret);
}
