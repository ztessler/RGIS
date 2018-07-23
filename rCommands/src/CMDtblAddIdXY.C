/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2018, UNH - CCNY/CUNY

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
    DBObjRecord *record, *cellRec;
    DBNetworkIF *netIF;
	DBCoordinate coord;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <input file> <output file>", CMfileName(argv[0]));
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

    tableName = (char *) "DBCells";
    fieldIDName = (char *) "CellID";
    fieldXName = (char *) "CellXCoord";
    fieldYName = (char *) "CellYCoord";

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

    for (recID = 0; recID < table->ItemNum(); ++recID) {
        record = table->Item(recID); // record and cellRec seem to point to the same data, but I'm not sure this is always true
		cellRec = netIF->Cell(recID);
		coord = netIF->Center(cellRec);

        fieldID->Int(record, recID+1);
		fieldX->Float(cellRec, coord.X);
		fieldY->Float(cellRec, coord.Y);
    }

    ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? data->Write(argv[2]) : data->Write(stdout);

    delete data;
    if (verbose) RGlibPauseClose();
    return (ret);
}
