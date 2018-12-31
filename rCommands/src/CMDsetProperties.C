/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - CUNY

CMDsetProperties.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    char *title = (char *) NULL, *subject = (char *) NULL, *domain = (char *) NULL, *version = (char *) NULL;
    int argPos, argNum = argc, verbose = false, ret;
    int display = DBFault, projection = DBFault, precision = DBFault, minScale = DBFault, maxScale = DBFault;
    DBObjData *dbData = new DBObjData();
    DBGridIF    *gridIF;
    DBNetworkIF *netIF;

    if (argc == 1) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-p", "--projection")) {
            int projectionModes[] = {DBProjectionSpherical, DBProjectionCartesian};
            const char *projectionCodes[] = {"geographic", "cartesian", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing projection option!");
                return (CMfailed);
            }
            if ((projection = CMoptLookup(projectionCodes, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid projection option!");
                return (CMfailed);
            }
            projection = projectionModes[projection];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-r", "--precision")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing precision");
                return (CMfailed);
            }
            if (sscanf (argv[argPos],"%d",&precision) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid precision!");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-a", "--max_scale")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing maximum scale");
                return (CMfailed);
            }
            if (sscanf (argv[argPos],"%d",&maxScale) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid maximum scale!");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-i", "--min_scale")) {
            argNum = CMargShiftLeft(argPos, argv, argNum);
            if (argNum <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing minimum scale");
                return (CMfailed);
            }
            if (sscanf (argv[argPos],"%d",&minScale) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid minimum scale!");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
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
        if (CMargTest (argv[argPos], "-y", "--display")) {
            int displayModes[] = {DBSet, DBClear};
            const char *displayCodes[] = {"on", "off", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing display option!");
                return (CMfailed);
            }
            if ((display = CMoptLookup(displayCodes, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid display option!");
                return (CMfailed);
            }
            display = displayModes[display];
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
            CMmsgPrint(CMmsgInfo, "       -p,--projection [geographic|cartesian]");
            CMmsgPrint(CMmsgInfo, "       -r,--precision [ten-base exponent]");
            CMmsgPrint(CMmsgInfo, "       -a,--max_scale [ten-base exponent]");
            CMmsgPrint(CMmsgInfo, "       -i,--mix_scale [ten-base exponent]");
            CMmsgPrint(CMmsgInfo, "       -t,--title   <title>");
            CMmsgPrint(CMmsgInfo, "       -d,--domain  <domanin>");
            CMmsgPrint(CMmsgInfo, "       -u,--subject <subject>");
            CMmsgPrint(CMmsgInfo, "       -v,--version <version>");
            CMmsgPrint(CMmsgInfo, "       -y,--display [on|off]");
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
    if (display   != DBFault)       dbData->Flags   (DBDataFlagDisplay, display);
    if (precision != DBFault)       dbData->Precision (precision);
    if (maxScale  != DBFault)       dbData->MaxScale  (maxScale);
    if (minScale  != DBFault)       dbData->MinScale  (minScale);

    if ((projection != DBFault) && (projection != dbData->Projection())) {
        dbData->Projection(projection);
        switch (dbData->Type ()) {
            case DBTypeGridDiscrete:
                gridIF = new DBGridIF (dbData);
                gridIF->DiscreteStats();
                delete gridIF;
                break;
            case DBTypeGridContinuous:
                gridIF = new DBGridIF (dbData);
                gridIF->RecalcStats();
                delete gridIF;
                break;
            case DBTypeNetwork:
                netIF = new DBNetworkIF (dbData);
                netIF->Build ();
                delete netIF;
                break;
            default: break;
        }
    }

    ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? dbData->Write(argv[2]) : dbData->Write(stdout);
    if (verbose) RGlibPauseClose();
    return (ret);
}
