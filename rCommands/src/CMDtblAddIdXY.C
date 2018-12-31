/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - CUNY

CMDtblAddIdXY.C

zachary.tessler@asrc.cuny.edu
bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    DBInt recID;
    DBObjData *data;
    char *tableName = (char *) NULL;
    char *fieldIDName = (char *) NULL;
    char *fieldXName = (char *) NULL;
    char *fieldYName = (char *) NULL;
    DBObjTable *table;
    DBObjTableField *fieldID, *fieldX, *fieldY;
    DBObjRecord *record;
    DBNetworkIF *netIF;
	DBCoordinate coord;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-a", "--table")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing table name!");
                return (CMfailed);
            }
            tableName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-f", "--IDfield")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing field name!");
                return (CMfailed);
            }
            fieldIDName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-x", "--Xfield")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing field name!");
                return (CMfailed);
            }
            fieldXName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-y", "--Yfield")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing field name!");
                return (CMfailed);
            }
            fieldYName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <input file> <output file>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -a, --table   [ [DBCells] | DBItems ]");
            CMmsgPrint(CMmsgInfo, "     -f, --IDfield [ [CellID]  | BasinID ]");
            CMmsgPrint(CMmsgInfo, "     -x, --Xfield  [ [CellXCoord] | MouthXCoord ]");
            CMmsgPrint(CMmsgInfo, "     -y, --Yfield  [ [CellYCoord] | MouthYCoord ]");
            CMmsgPrint(CMmsgInfo, "     -V, --verbose");
            CMmsgPrint(CMmsgInfo, "     -h, --help");
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

    if (tableName == (char *) NULL)   tableName = (char *) "DBCells";
    if (fieldIDName == (char *) NULL) fieldIDName = (char *) "CellID";
    if (fieldXName == (char *) NULL)  fieldXName = (char *) "CellXCoord";
    if (fieldYName == (char *) NULL)  fieldYName = (char *) "CellYCoord";

    data = new DBObjData();
    if (((argNum > 1) && (strcmp(argv[1], "-") != 0) ? data->Read(argv[1]) : data->Read(stdin)) == DBFault) {
        delete data;
        return (CMfailed);
    }

    if ((table = data->Table(tableName)) == (DBObjTable *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Invalid table: %s!", tableName);
        delete data;
        return (CMfailed);
    }

    netIF = new DBNetworkIF(data);

    fieldID = new DBObjTableField(fieldIDName, DBTableFieldInt,   (char *) "%8d",    sizeof (DBInt));
    fieldX  = new DBObjTableField (fieldXName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
    fieldY  = new DBObjTableField (fieldYName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));

    table->AddField(fieldID);
    table->AddField(fieldX);
    table->AddField(fieldY);

    if (strcmp(fieldIDName, "CellID") == 0) {
        for (recID = 0; recID < table->ItemNum(); ++recID) {
            record = netIF->Cell(recID);
            coord = netIF->Center(record);
            fieldID->Int(record, recID+1);
            fieldX->Float(record, coord.X);
            fieldY->Float(record, coord.Y);
        }
    } else if (strcmp(fieldIDName, "BasinID") == 0) {
        for (recID = 0; recID < netIF->BasinNum(); ++recID) {
            record = netIF->Basin(recID);
            coord = netIF->Center(netIF->MouthCell(record));
            fieldID->Int(record, recID+1);
            fieldX->Float(record,coord.X);
            fieldY->Float(record,coord.Y);
        }
    } else {
        CMmsgPrint(CMmsgUsrError, "Invalid field name: %s!", fieldIDName);
        delete data;
        return (CMfailed);
    }

    ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? data->Write(argv[2]) : data->Write(stdout);

    delete data;
    if (verbose) RGlibPauseClose();
    return (ret);
}
