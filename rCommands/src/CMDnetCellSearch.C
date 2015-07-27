/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

CMDnetCellSearch.C

bfekete@ccny.cuny.edu

*******************************************************************************/
#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

static bool _CMDnetCellSearchPrintID (DBNetworkIF *netIF, DBObjRecord *cellRec,void *dataPtr)
	{ printf ("%d\n",cellRec->RowID () + 1); return (true); }

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret;
   bool upStream = true;
   DBCoordinate coord;
	DBObjData *data;

	for (argPos = 1;argPos < argNum; ) {
		if (CMargTest (argv [argPos],"-c","--coordinates")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing sampling coordinates!"); return (CMfailed); }
			if (sscanf (argv [argPos],"%lf,%lf", &(coord.X),&(coord.Y)) != 2)
            { CMmsgPrint (CMmsgUsrError,"Invalid sampling coordinates!"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-d","--direction")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing aggregate method!"); return (CMfailed); }
			else {
				const char *options [] = { "upstream", "downstream", (char *) NULL };

				if ((ret = CMoptLookup (options,argv [argPos],false)) == CMfailed) {
					CMmsgPrint (CMmsgWarning,"Ignoring illformed direction [%s]!",argv [argPos]);
				}
				else upStream = ret == 0 ? true : false;
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-h","--help")) {
			CMmsgPrint (CMmsgInfo,"%s [options] <input network> <output list>",CMfileName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -c,--coordinates");
         CMmsgPrint (CMmsgInfo,"     -d,--direction [upstream|downstream]");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }

	data = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin);
	
  
   if (data->Type () == DBTypeNetwork) {
      DBNetworkIF *netIF = new DBNetworkIF (data);   
      DBObjRecord *cellRec = netIF->Cell (coord);
      
      if (upStream) netIF->UpStreamSearch   (cellRec,_CMDnetCellSearchPrintID);
      else          netIF->DownStreamSearch (cellRec,_CMDnetCellSearchPrintID);
      
      delete netIF;
   }
	delete data;
	return (ret);
}
