/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

MFDataStream.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>

MFDataStream_t *MFDataStreamOpen (const char *path, const char *mode) {
	MFDataStream_t *dStream;

	if (path == (char *) NULL) return ((MFDataStream_t *) NULL);
	if ((dStream = (MFDataStream_t *) malloc (sizeof (MFDataStream_t))) == (MFDataStream_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
		return (MFDataStream_t *) NULL;
	}
	if      (strncmp (path,MFconstStr,strlen (MFconstStr)) == 0) {
		if (strcmp (mode,"r") == 0) { dStream->Type = MFConst; return (dStream); }
		CMmsgPrint (CMmsgAppError,"Error: Invalid output data stream [%s] in: %s:%d\n",path + strlen (MFconstStr),__FILE__,__LINE__);
		free (dStream);
		dStream = (MFDataStream_t *) NULL;
	}
	if       (strncmp (path,MFpipeStr,strlen (MFpipeStr)) == 0) {
		dStream->Type = MFPipe;
		if ((dStream->Handle.File = popen (path + strlen (MFpipeStr),mode)) == (FILE *) NULL) {
			CMmsgPrint (CMmsgSysError,"Error: Opening datastream pipe [%s] in: %s:%d\n",path + strlen (MFpipeStr),__FILE__,__LINE__);
			free (dStream);
			dStream = (MFDataStream_t *) NULL;
		}
	}
	else if (strncmp (path,MFfileStr, strlen (MFfileStr)) == 0) {
		dStream->Type = MFFile;
		if ((dStream->Handle.File = fopen (path + strlen (MFfileStr),mode)) == (FILE *) NULL) {
			CMmsgPrint (CMmsgSysError,"Error: Opening datastream file [%s] in: %s:%d\n",path + strlen (MFfileStr),__FILE__,__LINE__);
			free (dStream);
			dStream = (MFDataStream_t *) NULL;
		}
	}
	else {
		CMmsgPrint (CMmsgAppError,"Error: Unknown datastream type [%s]!\n",path);
		free (dStream);
		dStream = (MFDataStream_t *) NULL;
	}
	return (dStream);
}

int MFDataStreamClose (MFDataStream_t *dStream)
	{
	if ((dStream == (MFDataStream_t *) NULL) || (dStream->Handle.File == (FILE *) NULL)) return (CMsucceeded);
	switch (dStream->Type) {
		case MFFile: return (fclose (dStream->Handle.File));
		case MFPipe: return (pclose (dStream->Handle.File));
	}
	return (CMsucceeded);
}

CMreturn MFdsHeaderRead (MFdsHeader_t *header,FILE *inFile) {
	MFdsHeader_t inHeader;

	if ((inFile == (FILE *) NULL) || (fread (&inHeader,sizeof (MFdsHeader_t),1,inFile) != 1)) return (CMfailed);

	memcpy (header,&inHeader,sizeof (MFdsHeader_t));
	if (header->Swap != 1) {
		MFSwapHalfWord (&(header->Type));
		MFSwapWord     (&(header->ItemNum));
		switch (header->Type) {
			case MFByte:
			case MFShort:
			case MFInt:		MFSwapWord     (&(header->Missing.Int));   break;
			case MFFloat:
			case MFDouble:	MFSwapLongWord (&(header->Missing.Float)); break;
		}
	}
	return (CMsucceeded);
}

CMreturn MFdsHeaderWrite (MFdsHeader_t *header,FILE *outFile) {

	if (outFile == (FILE *) NULL) return (CMfailed);

	header->Swap = 1;
	if (fwrite (header,sizeof (MFdsHeader_t),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"Header writing error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (CMsucceeded);
}

CMreturn MFdsRecordRead (MFVariable_t *var) {
	int i, sLen, readNum = 0, timeStep;
	MFdsHeader_t header;

    if (var->InStream->Type == MFConst) {
		if (var->InBuffer == (void *) NULL) {
			sLen = strlen (var->InputPath);
			for (i = strlen (MFconstStr);i < sLen;++i) if (var->InputPath [i] == '.') break;
			if (i == sLen) {
				if (sscanf (var->InputPath + strlen (MFconstStr),"%d",&(var->InStream->Handle.Int)) != 1) {
					CMmsgPrint (CMmsgAppError,"Error: Reading constant [%s]!\n",var->Name);
					return (CMfailed);
				}
				if (var->InStream->Handle.Int < 127) {
					var->Type    = MFByte;
					var->Missing.Int = MFDefaultMissingByte;
				}
				else if (var->InStream->Handle.Int < 32767) {
					var->Type    = MFShort;
					var->Missing.Int = MFDefaultMissingInt;
				}
				else {
					var->Type    = MFInt;
					var->Missing.Int = MFDefaultMissingInt;
				}
				if (var->Missing.Int == var->InStream->Handle.Int) var->Missing.Int = (int) 0;
			}
			else {
				var->Type = MFFloat;
				if (sscanf (var->InputPath + strlen (MFconstStr),"%lf",&var->InStream->Handle.Float) != 1) {
					CMmsgPrint (CMmsgAppError,"Error: Reading constant [%s]!\n",var->Name);
					return (CMfailed);
				}
				var->Missing.Float = CMmathEqualValues (var->InStream->Handle.Float,MFDefaultMissingFloat) ?
									 (float) 0.0 : MFDefaultMissingFloat;
			}
			var->InBuffer = (void *) malloc ((size_t) var->ItemNum * MFVarItemSize (var->Type));
			if (var->InBuffer == (void *) NULL) {
				CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d",__FILE__,__LINE__);
				return (CMfailed);
			}
		}
		switch (var->Type) {
			case MFByte:
				for (i = 0; i < var->ItemNum; ++i)
					((char *)  (var->InBuffer)) [i]  = (char)   (var->InStream->Handle.Int);
				break;
			case MFShort:
				for (i = 0; i < var->ItemNum; ++i)
					((short *) (var->InBuffer)) [i ] = (short)  (var->InStream->Handle.Int);
				break;
			case MFInt:
				for (i = 0; i < var->ItemNum; ++i)
					((int *)   (var->InBuffer)) [i]  = (int)    (var->InStream->Handle.Int);
				break;
			case MFFloat:
				for (i = 0; i < var->ItemNum; ++i)
					((float *) (var->InBuffer)) [i]  = (float)  (var->InStream->Handle.Float);
				break;
            default:
                break;
		}
	}
	else {
		if (var->InStream->Handle.File == (FILE *) NULL) return (CMfailed);
		if (MFDateCompare(var->CurDate, var->InDate) != 0) {
			do {
				if (MFdsHeaderRead(&header, var->InStream->Handle.File) == CMfailed) {
					if (readNum < 1) {
						CMmsgPrint(CMmsgSysError, "Data stream (%s %s %s) reading error", var->Name, var->CurDate, var->InDate);
						return (CMfailed);
					}
					break;
				}
				readNum++;
				if (var->ItemNum != header.ItemNum) {
					CMmsgPrint(CMmsgUsrError, "Variable [%] has inconsistent data stream (%d != %d)", header.ItemNum,
							   var->ItemNum);
					return (CMfailed);
				}

				if (var->InBuffer == (void *) NULL) {
					var->Type = header.Type;
					if ((var->InBuffer = (void *) malloc(var->ItemNum * MFVarItemSize(var->Type))) == (void *) NULL) {
						CMmsgPrint(CMmsgSysError, "Variable [%s] allocation error in: %s:%d", var->Name, __FILE__,
								   __LINE__);
						return (CMfailed);
					}

					switch (var->Type) {
						case MFByte:
						case MFShort:
						case MFInt:
							var->Missing.Int = header.Missing.Int;
							break;
						case MFFloat:
						case MFDouble:
							var->Missing.Float = header.Missing.Float;
							break;
					}
					switch (strlen(header.Date)) {
						case 4:
							var->TStep = MFTimeStepYear;
							break;
						case 7:
							var->TStep = MFTimeStepMonth;
							break;
						case 10:
							var->TStep = MFTimeStepDay;
							break;
						case 13:
							var->TStep = MFTimeStepHour;
							break;
						default:
							CMmsgPrint(CMmsgUsrError, "Invalid date in data stream %s", var->Name);
							return (CMfailed);
					}
				}
				else if (header.ItemNum != var->ItemNum) {
					CMmsgPrint(CMmsgUsrError, "Item number Missmatch %s != %d in varName %s", header.ItemNum,
							   var->ItemNum, var->Name);
					return (CMfailed);
				}
				else if (header.Type != var->Type) {
					CMmsgPrint(CMmsgAppError, "Record Type Missmatch [%d,%d] in: %s:%d varName %s", header.Type,
							   var->Type, __FILE__, __LINE__, var->Name);
					return (CMfailed);
				}
				else if (header.ItemNum != var->ItemNum) {
					CMmsgPrint(CMmsgAppError, "Record Size Missmatch [%d,%d] in: %s:%d", header.ItemNum, var->ItemNum,
							   __FILE__, __LINE__);
					return (CMfailed);
				}
				if ((int) fread(var->InBuffer, MFVarItemSize(var->Type), var->ItemNum, var->InStream->Handle.File) !=
					var->ItemNum) {
					CMmsgPrint(CMmsgSysError, "Data Reading error (%s:%d)!", __FILE__, __LINE__);
					return (CMfailed);
				}
				strcpy (var->CurDate, header.Date);
			} while (MFDateCompare(header.Date, var->InDate) < 0);
			if (header.Swap != 1)
				switch (var->Type) {
					case MFShort:  for (i = 0; i < var->ItemNum; ++i) MFSwapHalfWord((short *)  (var->InBuffer) + i); break;
					case MFInt:    for (i = 0; i < var->ItemNum; ++i) MFSwapWord((int *)        (var->InBuffer) + i); break;
					case MFFloat:  for (i = 0; i < var->ItemNum; ++i) MFSwapWord((float *)      (var->InBuffer) + i); break;
					case MFDouble: for (i = 0; i < var->ItemNum; ++i) MFSwapLongWord((double *) (var->InBuffer) + i); break;
					default: break;
				}
			if ((var->NStep = MFDateTimeStepLength(var->InDate, var->TStep)) == 0) {
				CMmsgPrint(CMmsgUsrError, "Invalid data stream in: %s, %d", __FILE__, __LINE__);
				return (CMfailed);
			}
		}
	}
	return (CMsucceeded);
}

CMreturn MFdsRecordWrite (MFVariable_t *var) {
	MFdsHeader_t header;

	header.Type    = var->Type;
	header.ItemNum = var->ItemNum;
	header.Swap    = 1;
	strncpy (header.Date, var->OutDate, sizeof (header.Date) - 1);
	switch (var->Type) {
		case MFByte:
		case MFShort:
		case MFInt:    header.Missing.Int   = var->Missing.Int;   break;
		case MFFloat:
		case MFDouble: header.Missing.Float = var->Missing.Float; break;
		default:	break;
	}
	if (MFdsHeaderWrite (&(header),var->OutStream->Handle.File) != CMsucceeded) return (CMfailed);
	if (fwrite (var->OutBuffer, (size_t) MFVarItemSize (var->Type), var->ItemNum, var->OutStream->Handle.File) != var->ItemNum) {
		CMmsgPrint (CMmsgSysError,"Data writing error (%s:%d)!"__FILE__,__LINE__);
		return (CMfailed);
	}
	return (CMsucceeded);
}