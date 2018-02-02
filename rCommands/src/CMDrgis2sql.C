/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2018, UNH - CCNY/CUNY

CMDrgis2sql.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    FILE *outFile;
    DBInt argPos, argNum = argc, ret;
    char *rgisTableName = (char *) "DBItems";
    char *dbSchemaName  = (char *) NULL;
    char *dbTableName   = (char *) NULL;
    DBObjData *data = (DBObjData *) NULL;
    DBObjTable *table;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-a", "--rgistable")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network!");
                return (CMfailed);
            }
            dbTableName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-d", "--dbtable")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network!");
                return (CMfailed);
            }
            dbTableName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-s", "--schema")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network!");
                return (CMfailed);
            }
            dbSchemaName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <rgis file> <sqlfile>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -a,--rgistable [rgis table]");
            CMmsgPrint(CMmsgInfo, "     -s,--schema    [sql schema]");
            CMmsgPrint(CMmsgInfo, "     -d,--dbtable   [sql table]");
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

    data = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? data->Read(argv[1]) : data->Read(stdin);
    if ((ret == DBFault) || ((table = data->Table (rgisTableName)) == (DBObjTable *) NULL)) {
        CMmsgPrint(CMmsgUsrError, "Wrong rgis data: %s!");
        delete data;
        return (CMfailed);
    }
    if ((outFile = (argNum > 1) && (strcmp(argv[2], "-") != 0) ? fopen (argv[2],"w") : stdout) == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Invalid output!");
        delete data;
        return (CMfailed);
    }

    ret = RGlibTableToSQL (table, dbSchemaName, dbTableName, outFile);

    delete data;
    if (outFile != stdout) {
        fclose (outFile);
        if (ret == CMfailed) unlink (argv[2]);
    }
    return (ret);
}
