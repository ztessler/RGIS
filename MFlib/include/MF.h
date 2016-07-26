/******************************************************************************

GHAAS Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

MF.h

bfekete@ccny.cuny.edu

*******************************************************************************/

#ifndef MF_H_INCLUDED
#define MF_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

enum {
    MFStop = false,
    MFContinue = true,
    MFState = false,
    MFFlux = true,
    MFBoundary = false,
    MFInitial = true,
    MFUnset = -2
};

#define MFDefaultMissingFloat -9999.0
#define MFDefaultMissingByte  -99
#define MFDefaultMissingInt   -9999
#define MFPrecision 0.0001
#define MFTolerance 0.001

enum {
    MFInput = 1, MFRoute = 2, MFOutput = 3, MFByte = 4, MFShort = 5, MFInt = 6, MFFloat = 7, MFDouble = 8
};

enum {
    MFTimeStepHour  = (0x01 << 0x00),
    MFTimeStepDay   = (0x01 << 0x01),
    MFTimeStepMonth = (0x01 << 0x02),
    MFTimeStepYear  = (0x01 << 0x03)
};

#define MFDateClimatologyYearStr  "XXXX"
#define MFDateClimatologyMonthStr "XXXX-XX"
#define MFDateClimatologyDayStr   "XXXX-XX-XX"
#define MFDateClimatologyHourStr  "XXXX-XX-XX XX"
#define MFNoUnit " "

enum {
    MFDateStringLength = 24, MFNameLength = 64
};

void MFSwapHalfWord(void *);
void MFSwapWord(void *);
void MFSwapLongWord(void *);

typedef struct MFDataStream_s {
    int Type;
    union {
        FILE  *File;
        int    Int;
        double Float;
    } Handle;
    pthread_t Thread;
    pthread_mutex_t Mutex;
    pthread_cond_t Cond;
} MFDataStream_t;

typedef struct MFdsHeader_s {
    short Swap, Type;
    int ItemNum;
    union {
        int Int;
        double Float;
    } Missing;
    char Date[MFDateStringLength];
} MFdsHeader_t;

#define MFconstStr "const:"
#define MFfileStr  "file:"
#define MFpipeStr  "pipe:"

enum {
    MFConst, MFFile, MFPipe
};

typedef struct MFVariable_s {
    int  ID;
    char Name[MFNameLength];
    char Unit[MFNameLength];
    char InDate[MFDateStringLength], CurDate[MFDateStringLength], OutDate[MFDateStringLength];
    bool Flux, Initial, Set, Route, State;
    int  Type;
    int  ItemNum;
    union {
        int Int;
        double Float;
    } Missing;
    short TStep;
    void *InBuffer,  *OutBuffer,  *ProcBuffer;
    char *InputPath, *OutputPath, *StatePath;
    int NStep;
    MFDataStream_t *InStream, *OutStream;
    pthread_t       InThread,  OutThread;
    pthread_mutex_t InMutex,   OutMutex;
    pthread_cond_t  InCond,    OutCond;
    int             ReadRet,   WriteRet;
    bool            Read,      Write;
} MFVariable_t;

typedef void (*MFFunction)(int);

MFDataStream_t *MFDataStreamOpen(const char *, const char *);
int MFDataStreamClose (MFDataStream_t *);
CMreturn MFdsHeaderRead    (MFdsHeader_t *,FILE *);
CMreturn MFdsHeaderWrite   (MFdsHeader_t *,FILE *);
CMreturn MFdsRecordRead    (MFVariable_t *);
CMreturn MFdsRecordWrite   (MFVariable_t *);

int MFVarGetID(char *, char *, int, bool, bool);
MFVariable_t *MFVarGetByID(int);
MFVariable_t *MFVarGetByName(const char *);
void   MFVarSetInt(int, int, int);
int    MFVarGetInt(int, int, int);
void   MFVarSetFloat(int, int, double);
double MFVarGetFloat(int, int, double);
int   MFVarGetTStep(int);
bool   MFVarTestMissingVal(int, int);
void   MFVarSetMissingVal(int, int);
char  *MFVarTypeString(int);
int    MFOptionParse(int, char *[]);
const char *MFOptionGet(const char *);
void   MFOptionPrintList();
void   MFOptionMessage(const char *, const char *, const char *[]);


int MFModelRun(int, char *[], int, int (*)());
int MFModelAddFunction(MFFunction);
float MFModelGetXCoord(int);
float MFModelGetYCoord(int);
float MFModelGetLongitude(int);
float MFModelGetLatitude(int);
int   MFModelGetDownLink(int, size_t);
float MFModelGetArea(int);
float MFModelGetLength(int);
float MFModelGet_dt();
void _MFDefEntering(const char *, const char *);
void _MFDefLeaving(const char *, const char *);
#define MFDefEntering(msg) _MFDefEntering(msg,__FILE__)
#define MFDefLeaving(msg)  _MFDefLeaving(msg,__FILE__)

size_t MFVarItemSize(int);

typedef struct MFObject_s {
    int ID;
    short DLinkNum, ULinkNum;
    float XCoord, YCoord, Lon, Lat;
    float Area, Length;
    size_t *DLinks, *ULinks;
} MFObject_t;

typedef struct MFDomain_s {
    short Swap, Type;
    int ObjNum;
    MFObject_t *Objects;
} MFDomain_t;

MFDomain_t *MFDomainRead (FILE *);
int  MFDomainWrite(MFDomain_t *, FILE *);
void MFDomainFree(MFDomain_t *);

bool  MFDateCompare (const char *, const char *);
char *MFDateGetCurrent ();
bool  MFDateSetCurrent (char *);
char *MFDateGetNext ();
int   MFDateGetDayOfYear ();
int   MFDateGetDayOfMonth ();
int   MFDateGetMonthLength ();
int   MFDateGetCurrentDay ();
int   MFDateGetCurrentMonth ();
int   MFDateGetCurrentYear ();

float MFModelGetArea(int);

char *MFDateTimeStepString(int);
char *MFDateTimeStepUnit(int);
int   MFDateTimeStepLength(const char *,int);

float MFRungeKutta(float, float, float, float (*deltaFunc)(float, float));

#if defined(__cplusplus)
}
#endif

#endif /* MF_H_INCLUDED */
