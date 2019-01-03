/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2018, UNH - CCNY/CUNY

CMDrgis2sql.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    FILE *outFile;
    DBInt argPos, argNum = argc, ret, mode = RGlibTableCopy, sqlCase = RGlibSQLCaseSensitive;
    char *rgisTableName = (char *) "DBItems";
    char *dbSchemaName  = (char *) NULL;
    char *sqlTableName   = (char *) NULL;
    DBObjData *data = (DBObjData *) NULL;
    DBObjTable *table;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-a", "--rgistable")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network!");
                return (CMfailed);
            }
            sqlTableName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-c", "--case")) {
            int codes[] = {RGlibSQLCaseSensitive,
                           RGlibSQLCaseLower,
                           RGlibSQLCaseUpper};
            const char *options[] = {"sensitive", "lower", "upper", (char *) NULL};
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "option!");
                return (CMfailed);
            }
            if ((sqlCase = CMoptLookup(options, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid option!");
                return (CMfailed);
            }
            sqlCase = codes[sqlCase];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-q", "--sqltable")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network!");
                return (CMfailed);
            }
            sqlTableName = argv[argPos];
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
        if (CMargTest(argv[argPos], "-m", "--mode")) {
            const char *options[] = {"copy", "append", "blank", (char *) NULL};
            int codes[] = {RGlibTableCopy, RGlibTableAppend, RGlibTableBlank}, code;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            if ((code = CMoptLookup(options, argv[argPos], false)) == CMfailed) {
                CMmsgPrint(CMmsgWarning, "Ignoring illformed step option [%s]!", argv[argPos]);
            }
                else mode = codes[code];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <rgis file> <sqlfile>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -a,--rgistable [rgis table]");
            CMmsgPrint(CMmsgInfo, "     -c,--case      [sensitive|lower|upper");
            CMmsgPrint(CMmsgInfo, "     -s,--schema    [sql schema]");
            CMmsgPrint(CMmsgInfo, "     -q,--sqltable  [sql table]");
            CMmsgPrint(CMmsgInfo, "     -m,--mode      [copy|append|blank]");
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
    if ((outFile = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? fopen (argv[2],"w") : stdout) == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Invalid output!");
        delete data;
        return (CMfailed);
    }

    ret = RGlibTableToSQL (table, dbSchemaName, sqlTableName, mode, sqlCase, outFile);

    delete data;
    if (outFile != stdout) {
        fclose (outFile);
        if (ret == CMfailed) unlink (argv[2]);
    }
    return (ret);
}
