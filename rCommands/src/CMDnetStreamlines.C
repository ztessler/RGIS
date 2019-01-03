/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2018, UNH - CCNY/CUNY

CMDnetStreamlines.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    DBInt minOrder = 3;
    int argPos, argNum = argc, ret, verbose = false;
    char *title = (char *) NULL, *subject = (char *) NULL;
    char *domain = (char *) NULL, *version = (char *) NULL;
    DBObjData *netData, *arcData;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-n", "--minorder")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing minimum order!");
                return (CMfailed);
            }
            if (sscanf (argv[argPos],"%d",&minOrder) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid minimum order!");
                return (CMfailed);
            }
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
        if (CMargTest (argv[argPos], "-d", "--domain")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing domain!");
                return (CMfailed);
            }
            domain = argv[argPos];
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
        if (CMargTest (argv[argPos], "-v", "--version")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing version!");
                return (CMfailed);
            }
            version = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <input network> <output stream lines>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -n,--minorder    [minimum order]");
            CMmsgPrint(CMmsgInfo, "     -t,--title       [dataset title]");
            CMmsgPrint(CMmsgInfo, "     -d,--domain      [domain]");
            CMmsgPrint(CMmsgInfo, "     -u,--subject     [subject]");
            CMmsgPrint(CMmsgInfo, "     -v,--version     [version]");
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

    netData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? netData->Read(argv[1]) : netData->Read(stdin);
    if ((ret == DBFault) || (netData->Type() != DBTypeNetwork)) {
        delete netData;
        return (CMfailed);
    }

    if (verbose) RGlibPauseOpen(argv[0]);

    if (title   == (char *) NULL) title   = (char *) "STNetwork Streamlines";
    if (domain  == (char *) NULL) domain  = netData->Document (DBDocGeoDomain);
    if (subject == (char *) NULL) subject = (char *) "StreamLines";
    if (version == (char *) NULL) version = (char *) "0.01pre";

    arcData = new DBObjData (title,DBTypeVectorLine);
    arcData->Document (DBDocGeoDomain,domain);
    arcData->Document (DBDocSubject,  subject);
    arcData->Document (DBDocVersion,  version);

    if ((ret = RGlibNetworkToStreamlines (netData, minOrder, arcData)) == DBSuccess)
        ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? arcData->Write(argv[2]) : arcData->Write(stdout);

    delete netData;
    delete arcData;
    if (verbose) RGlibPauseClose();
    return (ret);
}
