/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2017, UNH - CCNY/CUNY

CMDgrdRemovePits.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    char *title = (char *) NULL, *subject = (char *) NULL;
    char *domain = (char *) NULL, *version = (char *) NULL;
    char *network = (char *) NULL;
    int shadeSet = DBDataFlagDispModeContGreyScale;
    bool setShadeSet = false;
    DBObjData *netData, *grdData;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-n", "--network")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing merge grid!");
                return (CMfailed);
            }
            network = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-t", "--title")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing title!");
                return (CMfailed);
            }
            title = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-u", "--subject")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing subject!");
                return (CMfailed);
            }
            subject = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-d", "--domain")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing domain!");
                return (CMfailed);
            }
            domain = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-v", "--version")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
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
            if ((shadeSet = CMoptLookup(shadeSets, argv[argPos], true)) == CMfailed) {
                CMmsgPrint(CMmsgUsrError, "Invalid shadeset!");
                return (CMfailed);
            }
            shadeSet = shadeCodes[shadeSet];
            setShadeSet = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <input grid> <output grid>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -n,--network   [network]");
            CMmsgPrint(CMmsgInfo, "     -t,--title     [dataset title]");
            CMmsgPrint(CMmsgInfo, "     -u,--subject   [subject]");
            CMmsgPrint(CMmsgInfo, "     -d,--domain    [domain]");
            CMmsgPrint(CMmsgInfo, "     -v,--version   [version]");
            CMmsgPrint(CMmsgInfo, "     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]");
            CMmsgPrint(CMmsgInfo, "     -V,--verbose");
            CMmsgPrint(CMmsgInfo, "     -h,--help");
            return (DBSuccess);
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }

    if (argNum > 3) { CMmsgPrint(CMmsgUsrError, "Extra arguments!"); return (CMfailed); }
    if (network == (char *) NULL) { CMmsgPrint(CMmsgUsrError, "Missing network!"); return (CMfailed); }
    netData = new DBObjData ();
    if (netData->Read (network) != CMsucceeded) { delete netData; return (CMfailed); }
    if (verbose) RGlibPauseOpen(argv[0]);

    grdData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? grdData->Read(argv[1]) : grdData->Read(stdin);
    if ((ret == DBFault) || ((grdData->Type() & DBTypeGrid) != DBTypeGridContinuous)) {
        CMmsgPrint(CMmsgUsrError, "Invalid input grid!");
        delete grdData;
        delete netData;
        return (CMfailed);
    }

    if (title   != (char *) NULL) grdData->Name(title);
    if (subject != (char *) NULL) grdData->Document(DBDocSubject, subject);
    if (domain  != (char *) NULL) grdData->Document(DBDocGeoDomain, domain);
    if (version != (char *) NULL) grdData->Document(DBDocVersion, version);
    if (setShadeSet) {
        grdData->Flags(DBDataFlagDispModeContShadeSets, DBClear);
        grdData->Flags(shadeSet, DBSet);
    }

    if ((ret = RGlibGridRemovePits (netData, grdData)) == CMsucceeded) {
        ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? grdData->Read(argv[2]) : grdData->Write(stdout);
    }

    delete grdData;
    delete netData;
    if (verbose) RGlibPauseClose();
    return (ret);
}
