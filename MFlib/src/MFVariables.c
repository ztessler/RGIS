/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

MFVariables.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>

static MFVariable_t *_MFVariables = (MFVariable_t *) NULL;
static int _MFVariableNum = 0;

MFVariable_t *MFVarGetByID (int id) {
	return ((id > 0) && (id <= _MFVariableNum) ? _MFVariables + id - 1: (MFVariable_t *) NULL);
}

char *MFVarTypeString (int type) {
	switch (type) {
		case MFInput:  return ("input");
		case MFOutput: return ("output");
		case MFRoute:  return ("route");
		case MFByte:   return ("byte");
		case MFShort:  return ("short");
		case MFInt:    return ("int");
		case MFFloat:  return ("float");
		case MFDouble: return ("double");
		default: CMmsgPrint (CMmsgAppError,"Error: Invalide Type [%d] in: %s:%d\n",__FILE__,__LINE__); break;
	}
	return ("");
}

static MFVariable_t *_MFVarNewEntry (const char *name) {
	MFVariable_t *var;
	_MFVariables = (MFVariable_t *) realloc (_MFVariables,(_MFVariableNum + 1) * sizeof (MFVariable_t));
	if (_MFVariables == (MFVariable_t *) NULL) {
	 	CMmsgPrint (CMmsgSysError,"Error: Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
		return ((MFVariable_t *) NULL);
	}
	var = _MFVariables + _MFVariableNum;
	var->ID = _MFVariableNum + 1;
	strncpy (var->Name,name,sizeof (var->Name) - 1);
	strcpy  (var->Unit,MFNoUnit);
	strcpy  (var->InDate,  "NOT SET");
	strcpy  (var->OutDate, "NOT SET");
	strcpy  (var->CurDate, "NOT SET");
	var->ItemNum  = 0;
	var->Type = MFInput;
	var->InBuffer   = (void *) NULL;
	var->OutBuffer  = (void *) NULL;
	var->ProcBuffer = (void *) NULL;
	var->InputPath  = (char *) NULL;
	var->OutputPath = (char *) NULL;
	var->StatePath  = (char *) NULL;
	var->InStream   = (MFDataStream_t *) NULL;
	var->OutStream  = (MFDataStream_t *) NULL;
	var->TStep      = MFTimeStepYear;
    var->NStep      = 1;
	var->Set        = false;
	var->Flux       = false;
	var->Initial    = false;
	var->Route      = false;
    var->ReadRet    = CMfailed;
    var->WriteRet   = CMfailed;
    var->Read       = false;
    var->Write      = false;
	_MFVariableNum++;
	return (var);
}

static MFVariable_t *_MFVarFindEntry (const char *name) {
	int i;

	for (i = 0;i < _MFVariableNum;++i) if (strcmp (_MFVariables [i].Name,name) == 0) break;
	if (i < _MFVariableNum) return (_MFVariables + i);
	return ((MFVariable_t *) NULL);
}


int MFVarGetID (char *name,char *unit,int type, bool flux, bool initial) {
	MFVariable_t *var;

	if ((var = _MFVarFindEntry (name)) == (MFVariable_t *) NULL) {
		if ((var = _MFVarNewEntry (name)) == (MFVariable_t *) NULL)
			return (CMfailed);
		if (type == MFRoute) var->Route = true;
		var->Type = type == MFInput ? MFInput : MFOutput;
		var->Set  = type == MFInput ? initial : true;
	}
	switch (var->Type) {
		case MFInput:		break;
		case MFRoute:
		case MFOutput:
			switch (type) {
				case MFInput: if (!var->Set) type = MFInput; break;
				default: 
					var->Type = type;
					switch (var->Type) {
						case MFByte:	var->Missing.Int   = MFDefaultMissingByte;  break;
						case MFShort:
						case MFInt:		var->Missing.Int   = MFDefaultMissingInt;   break;
						case MFFloat:
						case MFDouble:	var->Missing.Float = MFDefaultMissingFloat; break;
					}
				case MFRoute:
				case MFOutput: break;
			}
			break;
		default:
			switch (type) {
				default:
					if (type != var->Type)
						CMmsgPrint (CMmsgWarning,"Warning: Ignoring type redefinition (%s,%s)\n",var->Name,MFVarTypeString (type));
				case MFInput:
				case MFRoute:
				case MFOutput:	break;
			}
	}

	if (initial != var->Initial) {
		if (initial) var->Initial = initial;
		else CMmsgPrint (CMmsgWarning,"Warning: Ignoring initial redefinition (%s [%s])!\n",var->Name,var->Unit);
	}

	if (strncmp (unit,var->Unit,strlen (unit)) != 0) {
		if (strcmp (var->Unit,MFNoUnit) == 0) strncpy (var->Unit,unit,sizeof (var->Unit) - 1);
		else CMmsgPrint (CMmsgWarning,"Warning: Ignoring Unit redefinition (%s [%s],%d)!\n",var->Name,var->Unit,type);
	}
	if (flux != var->Flux) {
		if (flux)  var->Flux = flux;
		else CMmsgPrint (CMmsgWarning,"Warning: Ignoring flux redefinition (%s [%s])!\n",var->Name,var->Unit);
	}
	return (var->ID);
}

int MFVarGetTStep (int id) {
	MFVariable_t *var;
	return ((var = MFVarGetByID (id)) != (MFVariable_t *) NULL ? var->NStep : MFTimeStepDay);
} 

static bool _MFVarTestMissingVal (MFVariable_t *var,int itemID)
	{
	switch (var->Type) {
		case MFByte:	return ((int) (((char *)  var->ProcBuffer) [itemID]) == var->Missing.Int);
		case MFShort:	return ((int) (((short *) var->ProcBuffer) [itemID]) == var->Missing.Int);
		case MFInt:		return ((int) (((int *)   var->ProcBuffer) [itemID]) == var->Missing.Int);
		case MFFloat:
			return (CMmathEqualValues ((((float *)  var->ProcBuffer) [itemID]),var->Missing.Float));
		case MFDouble:
			return (CMmathEqualValues ((((double *) var->ProcBuffer) [itemID]),var->Missing.Float));
	}
	CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Type,__FILE__,__LINE__);
	return (true);
}

bool MFVarTestMissingVal (int id,int itemID)
	{
	MFVariable_t *var;

	if ((var = MFVarGetByID (id)) == (MFVariable_t *) NULL)  {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d] in: %s:%d\n",id,__FILE__,__LINE__);
		return (true);
	}
	if ((itemID < 0) || (itemID >= var->ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid item [%s,%d] in: %s:%d\n",var->Name,itemID,__FILE__,__LINE__);
		return (true);
	}
	return  (_MFVarTestMissingVal (var,itemID));
}

void MFVarSetMissingVal (int id, int itemID)
	{
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: %s:%d\n",id,itemID,__FILE__,__LINE__);
		return;
	}
	switch (var->Type) {
		case MFByte:	((char *)   var->ProcBuffer) [itemID] = (char)   var->Missing.Int;		break;
		case MFShort:	((short *)  var->ProcBuffer) [itemID] = (short)  var->Missing.Int;		break;
		case MFInt:		((int *)    var->ProcBuffer) [itemID] = (int)    var->Missing.Int;		break;
		case MFFloat:	((float *)  var->ProcBuffer) [itemID] = (float)  var->Missing.Float;	break;
		case MFDouble:	((double *) var->ProcBuffer) [itemID] = (double) var->Missing.Float;	break;
	}
}

void MFVarSetFloat (int id,int itemID,double val) {
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: MFVarSetFloat ()\n",id,itemID);
		return;
	}

	var->Set = true;
	if (var->Flux) val = val * (double) var->NStep;
	switch (var->Type) {
		case MFByte:	((char *)   var->ProcBuffer) [itemID] = (char)  val; break;
		case MFShort:	((short *)  var->ProcBuffer) [itemID] = (short) val; break;
		case MFInt:		((int *)    var->ProcBuffer) [itemID] = (int)   val; break;
		case MFFloat:	((float *)  var->ProcBuffer) [itemID] = (float) val; break;
		case MFDouble:	((double *) var->ProcBuffer) [itemID] =         val; break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Type,__FILE__,__LINE__);
			break;
	}
}

double MFVarGetFloat (int id,int itemID,double missingVal) {
	double val;
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: MFVarGetFloat ()\n",id,itemID);
		return (MFDefaultMissingFloat);
	}
	if ((itemID == 0) && (var->Set != true)) CMmsgPrint (CMmsgWarning,"Warning: Unset variable [%s]!\n",var->Name);
	if (_MFVarTestMissingVal (var,itemID)) return (missingVal);

	switch (var->Type) {
		case MFByte:	val = (double) (((char *)   var->ProcBuffer) [itemID]); break;
		case MFShort:	val = (double) (((short *)  var->ProcBuffer) [itemID]); break;
		case MFInt:		val = (double) (((int *)    var->ProcBuffer) [itemID]); break;
		case MFFloat:	val = (double) (((float *)  var->ProcBuffer) [itemID]); break;
		case MFDouble:	val = (double) (((double *) var->ProcBuffer) [itemID]); break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Type,__FILE__,__LINE__);
			return (MFDefaultMissingFloat);
	}
 	return (var->Flux ? val = val / (double) var->NStep : val);
}

void MFVarSetInt (int id,int itemID,int val) {
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: %s:%d\n",id,itemID,__FILE__,__LINE__);
		return;
	}

	var->Set = true;
	if (var->Flux) val = val * var->NStep;
	switch (var->Type) {
		case MFByte:	((char *)   var->ProcBuffer) [itemID] = (char)   val;	break;
		case MFShort:	((short *)  var->ProcBuffer) [itemID] = (short)  val;	break;
		case MFInt:		((int *)    var->ProcBuffer) [itemID] = (int)    val;	break;
		case MFFloat:	((float *)  var->ProcBuffer) [itemID] = (float)  val;	break;
		case MFDouble:	((double *) var->ProcBuffer) [itemID] = (double) val;	break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Type,__FILE__,__LINE__);
			break;
	}
}

int MFVarGetInt (int id,int itemID, int missingVal) {
	int val;
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: %s:%d\n",id,itemID,__FILE__,__LINE__);
		return (MFDefaultMissingInt);
	}

	if (var->Set != true) CMmsgPrint (CMmsgWarning,"Warning: Unset variable %s\n",var->Name);
	if (_MFVarTestMissingVal (var,itemID)) return (missingVal);
	
	switch (var->Type) {
		case MFByte:	val = (int) (((char *)   var->ProcBuffer) [itemID]); break;
		case MFShort:	val = (int) (((short *)  var->ProcBuffer) [itemID]); break;
		case MFInt:		val = (int) (((int *)    var->ProcBuffer) [itemID]); break;
		case MFFloat:	val = (int) (((float *)  var->ProcBuffer) [itemID]); break;
		case MFDouble:	val = (int) (((double *) var->ProcBuffer) [itemID]); break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Type,__FILE__,__LINE__);
			return (MFDefaultMissingInt);
	}
	return (var->Flux ? (val / var->NStep) : val);
}	

size_t MFVarItemSize (int type) {
	switch (type) {
		case MFByte:	return (sizeof (char));
		case MFShort:	return (sizeof (short));
		case MFInt:		return (sizeof (int));
		case MFFloat:	return (sizeof (float));
		case MFDouble:	return (sizeof (double));
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid type [%d] in: %s:%d\n",type,__FILE__,__LINE__);
			break;
	}
	return (0);
}