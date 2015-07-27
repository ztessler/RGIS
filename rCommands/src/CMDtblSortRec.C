/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2015, UNH - CCNY/CUNY

CMDtblSortRec.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>
#include <stdlib.h>

class CMDtblSortField
	{
	private:
		const char *FieldNameSTR;
		bool  AscendingVAR;
	public:
		void FieldName (const char *fieldName) { FieldNameSTR = fieldName; };
		const char *FieldName () { return (FieldNameSTR); }
		void Ascending (bool ascending) { AscendingVAR = ascending; }
		bool Ascending () { return (AscendingVAR); };
	};

int main (int argc,char *argv [])
	{
	int argPos, argNum = argc, ret, fieldNum = 0, verbose = false;
	char *tableName = (char *) NULL;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	DBObjData   *data;
	DBObjectLIST<DBObjTableField> *fieldList;
	DBObjTable  *table;
	DBObjTableField *fieldPTR;
	CMDtblSortField *fields = (CMDtblSortField *) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--table"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{
				CMmsgPrint (CMmsgUsrError,"Missing table name!");
				if (fields != (CMDtblSortField *) NULL) free (fields);
				return (CMfailed);
				}
			tableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-A","--ascending"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{
				CMmsgPrint (CMmsgUsrError,"Missing sort field!");
				if (fields != (CMDtblSortField *) NULL) free (fields);
				return (CMfailed);
				}
			if ((fields = (CMDtblSortField *) realloc (fields, (fieldNum + 1) * sizeof (CMDtblSortField))) == (CMDtblSortField *) NULL)
				{
				CMmsgPrint (CMmsgSysError,"Memory reallocation error in: %s, %n",__FILE__,__LINE__);
				return (CMfailed);
				}
			fields [fieldNum].FieldName(argv [argPos]);
			fields [fieldNum].Ascending(true);
			fieldNum++;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-D","--descending"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{
				CMmsgPrint (CMmsgUsrError,"Missing selection mode!");
				if (fields != (CMDtblSortField *) NULL) free (fields);
				return (CMfailed);
				}
			if ((fields = (CMDtblSortField *) realloc (fields, (fieldNum + 1) * sizeof (CMDtblSortField))) == (CMDtblSortField *) NULL)
				{
				CMmsgPrint (CMmsgSysError,"Memory reallocation error in: %s, %n",__FILE__,__LINE__);
				return (CMfailed);
				}
			fields [fieldNum].FieldName(argv [argPos]);
			fields [fieldNum].Ascending(false);
			fieldNum++;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
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
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>",CMfileName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--table      [ table name ]");
			CMmsgPrint (CMmsgInfo,"     -A,--ascending  [ fieldname ]");
			CMmsgPrint (CMmsgInfo,"     -D,--descending [ fieldname ]");
			CMmsgPrint (CMmsgInfo,"     -t,--title      [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject    [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain     [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version    [version]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			if (fields != (CMDtblSortField *) NULL) free (fields);
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]);
			if (fields != (CMDtblSortField *) NULL) free (fields);
			return (CMfailed); }
		argPos++;
		}

	if (argNum > 3)
		{
		CMmsgPrint (CMmsgUsrError,"Extra arguments!");
		if (fields != (CMDtblSortField *) NULL) free (fields);
		return (CMfailed);
		}
	if (verbose) RGlibPauseOpen (argv[0]);


	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ if (fields != (CMDtblSortField *) NULL) free (fields); delete data; return (CMfailed); }

	if (title	!= (char *) NULL) data->Name (title);
	if (subject != (char *) NULL) data->Document (DBDocSubject,subject);
	if (domain	!= (char *) NULL)	data->Document (DBDocGeoDomain,domain);
	if (version != (char *) NULL) data->Document (DBDocVersion,version);	

	if (tableName == (char *) NULL) tableName = DBrNItems;

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{
		CMmsgPrint (CMmsgUsrError,"Invalid table!");
		if (fields != (CMDtblSortField *) NULL) free (fields);
		delete data;
		return (CMfailed);
		}

	if (fields != (CMDtblSortField *) NULL)
		{
		DBInt fieldID;
		fieldList = new DBObjectLIST<DBObjTableField> ("Fields");
		for (fieldID = 0; fieldID < fieldNum; ++fieldID)
			{
			if ((fieldPTR = table->Field(fields [fieldID].FieldName())) == (DBObjTableField *) NULL)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid field: %s!",fields [fieldID].FieldName());
				free (fields);
				return (CMfailed);
				}
			if (fields [fieldID].Ascending())
				  fieldPTR->Flags (DBObjectFlagSortReversed, DBClear);
			else fieldPTR->Flags (DBObjectFlagSortReversed, DBSet);
			fieldList->Add (new DBObjTableField (*fieldPTR));
			}
		table->ListSort (fieldList);
		table->ItemSort ();
		delete fieldList;
		free (fields);
		}
	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
