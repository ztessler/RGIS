/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2018, UNH - CCNY/CUNY

CMDgrdHeatIndex.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>
#include <math.h>

static DBFloat _CMDheatIndex (DBFloat airT, DBFloat dewT) {
    DBFloat eSat, eAir, H, T, A, B, hi;
    /* Anderson, G. B., M. L. Bell, and R. D. Peng (2013), Review Methods to Calculate the Heat Index as an
     * Exposure Metric in Environmental Health Research, Environmental Health Perspectives, 121(10), 1111–1119. */


    eSat = airT >= (DBFloat) 0.0 ? 0.611 * exp ( 17.27 * airT / (237.3 + airT)) : 0.611 * exp ( 21.87 * airT / (265.5 + airT));
    eAir = dewT >= (DBFloat) 0.0 ? 0.611 * exp ( 17.27 * dewT / (237.3 + dewT)) : 0.611 * exp ( 21.87 * dewT / (265.5 + dewT));
    H = eAir / eSat * 100.0;

    T = 1.8 * airT + 32.0;

    if (T < 40.0) hi = T;
    else {
        A = -10.3 + 1.1 * T + 0.047 * H;
        if (A < 79.0) hi = A;
        else {
            B = - 42.379 + 2.04901523 * T + 10.14333127 * H - 0.22475541 * T * H - 6.83783 / 1000.0 * pow (T,2.0)
                - 5.481717 / 100.0 * pow (H,2.0) + 1.22874 / 1000.0 * pow (T,2.0) * H + 8.5282 / 10000.0 * T * pow (H,2.0)
                - 1.99 / 1000000.0 * pow (T,2.0) * pow (H,2.0);
            if       ((H < 13.0) && (T > 80.0) && (T < 112.0)) hi = B - ((13.0  - H)/4.0) * sqrt ((17 - fabs (T - 95.0)) / 17.0);
            else  if ((H > 85.0) && (T > 80.0) && (T <  87.0)) hi = B + 0.02 * (H - 85) * (87 - T);
            else hi = B;
        }
    }
    return ((hi - 32.0) / 1.8);
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    char *title  = (char *) NULL,  *subject = (char *) NULL;
    char *domain = (char *) NULL,  *version = (char *) NULL;
    char  *dewpT = (char *) NULL;
    int shadeSet = DBFault;
    DBObjData *airData, *dewData, *outData;
    DBGridIF  *airIF,   *dewIF, *outIF;
    DBInt layerID;
    DBObjRecord *airLayerRec, *dewLayerRec, *outLayerRec;
    DBPosition pos;
    DBFloat airT, dewT, hi;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-e", "--dewpoint")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing air temperature file!");
                return (CMfailed);
            }
            dewpT = argv[argPos];
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
            if ((shadeSet = CMoptLookup(shadeSets, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid shadeset!");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            shadeSet = shadeCodes[shadeSet];
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <air temperature grid> <output grid>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -e,--dewpoint  [dewpoint temperature grid]");
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

    if (argNum > 3) { CMmsgPrint(CMmsgUsrError, "Extra arguments!"); return (CMfailed);  }
    if (dewpT == (char *) NULL)  { CMmsgPrint(CMmsgUsrError, "Dewpoint temperature is missing!"); return (CMfailed);  }
    if (verbose) RGlibPauseOpen(argv[0]);

    airData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? airData->Read(argv[1]) : airData->Read(stdin);
    if ((ret == DBFault) || (airData->Type() != DBTypeGridContinuous)) { delete airData; return (CMfailed); }
    dewData = new DBObjData ();
    if (dewData->Read(dewpT) == DBFault) { delete airData; delete dewData; return (CMfailed); }

    outData = DBGridToGrid(airData);

    if (title    == (char *) NULL) title   = (char *) "NOAA Heat Index";
    if (subject  == (char *) NULL) subject = (char *) "Heat-Index";
    if (domain   == (char *) NULL) domain  = outData->Document(DBDocGeoDomain);
    if (version  == (char *) NULL) version = outData->Document(DBDocVersion);
    if (shadeSet == DBFault) shadeSet = DBDataFlagDispModeContBlueRed;

    outData->Name(title);
    outData->Document(DBDocSubject, subject);
    outData->Document(DBDocGeoDomain, domain);
    outData->Document(DBDocVersion, version);
    outData->Flags(DBDataFlagDispModeContShadeSets, DBClear);
    outData->Flags(shadeSet, DBSet);

    airIF = new DBGridIF (airData);
    dewIF = new DBGridIF (dewData);
    outIF = new DBGridIF (outData);

    if (airIF->LayerNum () != dewIF->LayerNum()) {
        CMmsgPrint (CMmsgUsrError,"Air temperature and Dewpoint temperature grids have different number of layers!");
        ret = CMfailed;
        goto Stop;
    }
    for (layerID = 0; layerID < airIF->LayerNum(); ++layerID) {
        airLayerRec = airIF->Layer(layerID);
        dewLayerRec = dewIF->Layer(layerID);
        if (layerID == 0) {
            outLayerRec = outIF->Layer (layerID);
            outIF->RenameLayer (outLayerRec,airLayerRec->Name());
        }
        else {
            if ((outLayerRec = outIF->AddLayer(airLayerRec->Name())) == (DBObjRecord *) NULL) {
                CMmsgPrint(CMmsgSysError, "Creating new layer");
                ret = CMfailed;
                goto Stop;
            }
        }

        for (pos.Row = 0; pos.Row < airIF->RowNum(); pos.Row++) {
            if (DBPause((layerID * airIF->RowNum() + pos.Row) * 100 / (airIF->LayerNum() * airIF->RowNum())))
                goto Stop;
            for (pos.Col = 0; pos.Col < airIF->ColNum(); pos.Col++) {
                if (airIF->Value(airLayerRec, pos, &airT) && dewIF->Value(dewLayerRec, pos, &dewT))
                    outIF->Value(outLayerRec, pos, hi = _CMDheatIndex (airT,dewT));
                else
                    outIF->Value(outLayerRec, pos, outIF->MissingValue());
            }
        }
    }
    outIF->RecalcStats();
    ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? outData->Write(argv[2]) : outData->Write(stdout);
Stop:
    delete airIF;
    delete dewIF;
    delete outIF;
    delete airData;
    delete dewData;
    delete outData;
    if (verbose) RGlibPauseClose();
    return (ret);
}
