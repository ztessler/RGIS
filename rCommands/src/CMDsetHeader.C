/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - ASRC/CUNY

CMDsetHeader.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <DB.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    char *title = (char *) NULL, *subject = (char *) NULL, *domain = (char *) NULL, *version = (char *) NULL;
    char *citation = (char *) NULL, *institute = (char *) NULL, *source = (char *) NULL, *person = (char *) NULL;
    char *comment = (char *) NULL;
    int argPos, argNum = argc, verbose = false, ret;
    int shadeSet = DBFault, display = DBFault;
    DBObjData *dbData = new DBObjData();

    if (argc == 1) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-t", "--title")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing title!");
                return (CMfailed);
            }
            title = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-d", "--domain")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing domain!");
                return (CMfailed);
            }
            domain = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-u", "--subject")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing subject!");
                return (CMfailed);
            }
            subject = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-v", "--version")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing version!");
                return (CMfailed);
            }
            version = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-s", "--shadeset")) {
            int shadeCodes[] = {DBDataFlagDispModeContStandard,
                                DBDataFlagDispModeContGreyScale,
                                DBDataFlagDispModeContBlueScale,
                                DBDataFlagDispModeContBlueRed,
                                DBDataFlagDispModeContElevation};
            const char *shadeSets[] = {"standard", "grey", "blue", "blue-to-red", "elevation", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing shadeset!");
                return (CMfailed);
            }
            if ((shadeSet = CMoptLookup(shadeSets, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid shadeset!");
                return (CMfailed);
            }
            shadeSet = shadeCodes[shadeSet];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-y", "--display")) {
            int displayModes[] = {DBSet, DBClear};
            const char *displayCodes[] = {"on", "off", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing shadeset!");
                return (CMfailed);
            }
            if ((display = CMoptLookup(displayCodes, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid shadeset!");
                return (CMfailed);
            }
            display = displayModes[display];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-n", "--institute")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing institute!");
                return (CMfailed);
            }
            institute = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-p", "--person")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing person!");
                return (CMfailed);
            }
            person = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-i", "--citation")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing citation!");
                return (CMfailed);
            }
            citation = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-c", "--comment")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing comment!");
                return (CMfailed);
            }
            comment = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-o", "--source")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum < argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing source!");
                return (CMfailed);
            }
            source = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            Help:
            CMmsgPrint(CMmsgInfo, "%s [options] <input file> <output file>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "       -c,--comment");
            CMmsgPrint(CMmsgInfo, "       -i,--citation");
            CMmsgPrint(CMmsgInfo, "       -d,--domain");
            CMmsgPrint(CMmsgInfo, "       -n,--institute");
            CMmsgPrint(CMmsgInfo, "       -p,--person");
            CMmsgPrint(CMmsgInfo, "       -u,--subject");
            CMmsgPrint(CMmsgInfo, "       -o,--source");
            CMmsgPrint(CMmsgInfo, "       -t,--title");
            CMmsgPrint(CMmsgInfo, "       -y,--display [on|off]");
            CMmsgPrint(CMmsgInfo, "       -v,--version");
            CMmsgPrint(CMmsgInfo, "       -V,--verbose");
            CMmsgPrint(CMmsgInfo, "       -h,--help");
            return 0;
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        ++argPos;
    }
    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        return (CMfailed);
    }

    if (verbose) RGlibPauseOpen(argv[0]);

    if (((argNum > 1) && (strcmp(argv[1], "-") != 0) ? dbData->Read(argv[1]) : dbData->Read(stdin)) == DBFault) {
        delete dbData;
        return (CMfailed);
    }

    if (title     != (char *) NULL) dbData->Name(title);
    if (domain    != (char *) NULL) dbData->Document(DBDocGeoDomain,    domain);
    if (subject   != (char *) NULL) dbData->Document(DBDocSubject,      subject);
    if (version   != (char *) NULL) dbData->Document(DBDocVersion,      version);
    if (institute != (char *) NULL) dbData->Document(DBDocCitationInst, institute);
    if (person    != (char *) NULL) dbData->Document(DBDocSourcePerson, person);
    if (citation  != (char *) NULL) dbData->Document(DBDocCitationRef,  citation);
    if (comment   != (char *) NULL) dbData->Document(DBDocComment,      comment);
    if (source    != (char *) NULL) dbData->Document(DBDocSourceInst,   source);
    if (shadeSet  != DBFault)  { dbData->Flags (DBDataFlagDispModeContShadeSets,DBClear); dbData->Flags(shadeSet, DBSet); }
    if (display   != DBFault)       dbData->Flags(DBDataFlagDisplay,display);

    ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? dbData->Write(argv[2]) : dbData->Write(stdout);
    if (verbose) RGlibPauseClose();
    return (ret);
}
