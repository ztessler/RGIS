/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2017, UNH - CCNY/CUNY

CMDgrdMinMax.C

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
    DBInt doMin = true, reportValue = true;
    int shadeSet = DBDataFlagDispModeContGreyScale;
    bool changeShadeSet = false;
    DBObjData *tsData, *data;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-m", "--mode")) {
            int codes[] = {true, false};
            const char *strs[] = {"min", "max", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing min-max method!");
                return (CMfailed);
            }
            if ((doMin = CMoptLookup(strs, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid min-max method!");
                return (CMfailed);
            }
            doMin = codes[doMin];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-r", "--report")) {
            int codes[] = {true, false};
            const char *strs[] = {"value", "layer", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing report method!");
                return (CMfailed);
            }
            if ((reportValue = CMoptLookup(strs, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid report method!");
                return (CMfailed);
            }
            reportValue = codes[reportValue];
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
            changeShadeSet = true;
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
            CMmsgPrint(CMmsgInfo, "     -m,--mode      [min|max]");
            CMmsgPrint(CMmsgInfo, "     -r,--report    [value|layer]");
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

    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        return (CMfailed);
    }
    if (verbose) RGlibPauseOpen(argv[0]);

    tsData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? tsData->Read(argv[1]) : tsData->Read(stdin);
    if ((ret == DBFault) || (tsData->Type() != DBTypeGridContinuous)) {
        delete tsData;
        return (CMfailed);
    }

    if (title   == (char *) NULL) title   = tsData->Name();
    if (subject == (char *) NULL) subject = tsData->Document(DBDocSubject);
    if (domain  == (char *) NULL) domain  = tsData->Document(DBDocGeoDomain);
    if (version == (char *) NULL) version = tsData->Document(DBDocVersion);

    data = reportValue ? DBGridToGrid(tsData, DBTypeGridContinuous, DBTableFieldFloat, sizeof(DBFloat)) :
                         DBGridToGrid(tsData, DBTypeGridContinuous, DBTableFieldInt,   sizeof(DBInt));
    data->Name(title);
    data->Document(DBDocSubject,   subject);
    data->Document(DBDocGeoDomain, domain);
    data->Document(DBDocVersion,   version);
    if (changeShadeSet && (data->Type() == DBTypeGridContinuous)) {
        data->Flags(DBDataFlagDispModeContShadeSets, DBClear);
        data->Flags(shadeSet, DBSet);
    }

    if ((ret = RGlibMinMax(tsData, data, doMin,reportValue)) == DBSuccess)
        ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? data->Write(argv[2]) : data->Write(stdout);

    delete tsData;
    delete data;
    if (verbose) RGlibPauseClose();
    return (ret);
}
