/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2013, UNH - CCNY/CUNY

CMDnetcdf2ds.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	DBInt argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	DBInt shadeSet = DBFault;

	FILE *outFile;
	char *tmplName = (char *) NULL, *fieldName = (char *) NULL;
	DBObjData *grdData, *tmplData = (DBObjData *) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");       return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!");      return (CMfailed); }
			version  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
											DBDataFlagDispModeContGreyScale,
											DBDataFlagDispModeContBlueScale,
											DBDataFlagDispModeContBlueRed,
											DBDataFlagDispModeContElevation };
			const char *shadeSets [] = {	"standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!");		return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input netcdf> <output datastream>",CMfileName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
                        CMmsgPrint (CMmsgInfo,"     -m,--template     [template coverage]");
                        CMmsgPrint (CMmsgInfo,"     -f,--field        [fieldname]");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}

               if (CMargTest (argv [argPos],"-m","--template"))
                        {
                        if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
                                { CMmsgPrint (CMmsgUsrError,"Missing network!");      return (CMfailed); }
                        tmplName = argv [argPos];
                        if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
                        continue;
                        }
                if (CMargTest (argv [argPos],"-f","--field"))
                        {
                        if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
                                { CMmsgPrint (CMmsgUsrError,"Missing fieldname!");    return (CMfailed); }
                        fieldName = argv [argPos];
                        if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
                        continue;
                        }
                if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
                        { CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }


		argPos++;
		}

	if (argNum < 2) { CMmsgPrint (CMmsgUsrError,"Missing input file"); return (CMfailed); }
	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }



/******************************************************************************
CMDrgis2ds.C
*******************************************************************************/


	outFile = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? fopen (argv [2],"w") : stdout;
	if (outFile == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Output file Opening error in: %s %d",__FILE__,__LINE__); exit (DBFault); }

        //CMmsgPrint (CMmsgUsrError,"Processing argv1 %s, arv2 %s",argv [1],argv [2]);




	//grdData = new DBObjData ();
        grdData = new DBObjData ("Noname",DBTypeGridContinuous);

        ret = DBImportNetCDF (grdData,argv[1]);
        //if (title   != (char *) NULL) grdData->Name (title);
        //if (subject != (char *) NULL) grdData->Document (DBDocSubject,subject);
        //if (domain  != (char *) NULL) grdData->Document (DBDocGeoDomain,domain);
        //if (version != (char *) NULL) grdData->Document (DBDocVersion,version);

        //if (shadeSet != DBFault) { grdData->Flags (DBDataFlagDispModeContShadeSets,DBClear); grdData->Flags (shadeSet,DBSet); }


	if ((ret == DBFault) || ((grdData->Type () & DBTypeGrid) != DBTypeGrid))
		{ delete grdData; if (outFile != stdout) fclose (outFile); return (CMfailed); }
	
	if (tmplName != (char *) NULL)
		{ 
		tmplData = new DBObjData ();
		if (tmplData->Read (tmplName) == DBFault)
			{ delete grdData; if (outFile != stdout) fclose (outFile); delete tmplData; return (CMfailed); }
		}

	ret = RGlibRGIS2DataStream (grdData,tmplData,fieldName,outFile);
	if (tmplData != (DBObjData *) NULL) delete tmplData;
	delete grdData;
	if (outFile != stdout) fclose (outFile);
	return (ret);

	}
