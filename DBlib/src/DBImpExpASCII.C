/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2012, UNH - CCNY/CUNY

DBExpASCII.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

#define BLOCKSIZE 128

static char *_DBGetLine(char *buffer, int *size, FILE *inFile)
	{
	char *ptr;
	int cont = false;

	if (buffer == (char *) NULL)
		{
		if ((buffer = (char *) malloc (BLOCKSIZE * sizeof (char))) == (char *) NULL)
			{
			CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
			*size = 0;
			return ((char *) NULL);
			}
		else	*size = BLOCKSIZE;
		}

	ptr = buffer;
	do
		{
		if (fgets (ptr,*size - (ptr - buffer),inFile) == NULL)
			{ free (buffer); *size = 0; return ((char *) NULL); }
		if (((int) strlen (buffer) == *size - 1) && (buffer [strlen (buffer) - 1] != '\n'))
			{
			if ((buffer = (char *) realloc (buffer,(*size + BLOCKSIZE) * sizeof (char))) == (char *) NULL)
				{
				CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
				*size = 0;
				return ((char *) NULL);
				}
			else *size += BLOCKSIZE;
			ptr = buffer + strlen (buffer);
			cont = true;
			}
		else cont = false;
		} while (cont);

	if (buffer [strlen (buffer) - 1] == '\n') buffer [strlen (buffer) - 1] = '\0';
	return (buffer);
	}


DBInt DBImportASCIITable (DBObjTable *table, const char *fileName)
	{
	DBInt ret;
	FILE *fp;
	if ((fp = fopen (fileName,"r")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	ret = DBImportASCIITable (table,fp);
	fclose (fp);
	return (ret);	
	}

class _DBImportASCIIRecord
	{
	private:
		char *Buffer, **FieldARRAY;
		int   FieldNumVAR;
	public:
	_DBImportASCIIRecord (const char *buffer)
		{
		int ret = DBFault, i, field = 0, bufferLen;

		Buffer      = (char *) NULL;
		FieldARRAY  = (char **) NULL;
		FieldNumVAR = 1;
		bufferLen = strlen (buffer);
		if ((Buffer = (char *) malloc (bufferLen + 1)) != (char *) NULL)
			{
			strcpy (Buffer,buffer);

			for (i = 0;i < bufferLen; ++i) if (Buffer [i] == '\t') FieldNumVAR++; 
			if ((FieldARRAY = (char **) calloc (FieldNumVAR,sizeof (char *))) == (char **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); goto Stop; }
			Fields [field] = Buffer;
			for (i = 0;i < bufferLen; ++i)
				if (Buffer [i] == '\t')
					{
					FieldARRAY [field] = Buffer + i;
					Buffer [i] = '\0';
					field++;
					}
			}
		Stop:
		if (ret == DBFault)
			{
			if (Buffer != (char *)  NULL) free (Buffer);
			if (Fields != (char **) NULL) free (Fields);
			Buffer = (char *) NULL;
			Fields = (char **) NULL;
			FieldNumVAR = 0;
			}
		}
	int FieldNum () const { return (FieldNumVAR); }
	const char *Field (int fieldID) const { if (fieldID >= FieldNumVAR) return ((char *) NULL); else return (FieldARRAY [fieldID]); }
	~_DBImportASCIIRecord ()
		{
		free (Buffer);
		free (Fields);
		}
	};

class _DBImportASCIIHeader : public _DBImportASCIIRecord
	{
	private:
		int Types;
		int DLengths;
		int Decimals;
	public:
		_DBImportASCIIHeader (const char *buffer) : _DBImportASCIIRecord (buffer)
			{
			
			}
		~_DBImportASCIIHeader ()
			{
			
			}
	};

DBInt DBImportASCIITable (DBObjTable *table, FILE *fp)

	{
	DBObjectLIST<DBObjTableField> *fields = table->Fields (); 
	DBObjRecord *record;
	DBObjTableField *field;
	char *buffer = (char *) NULL, *fieldSTR;
	int   bufferSize = 0;
	int   recordNum  = 0, recordID, fieldID;
	_DBImportASCIIHeader *header;
	_DBImportASCIIRecord **records = (_DBImportASCIIRecord **) NULL;
	
	if ((buffer = _DBGetLine (buffer, &bufferSize, fp)) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Reading Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	header = new _DBImportASCIIHeader (buffer);

	if (header->FieldNum () < 1)
	{
		
	}
	while ((buffer = _DBGetLine (buffer, &bufferSize, fp)) != (char *) NULL)
		{
		if ((records = (_DBImportASCIIRecord **) realloc (records, sizeof (_DBImportASCIIRecord *) * (recordNum + 1))) == (_DBImportASCIIRecord **) NULL)
			{
			CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
			delete header;
			return (DBFault);
			}
		records [recordNum] = new _DBImportASCIIRecord (buffer);
		recordNum++;
		}
	for (fieldID = 0; fieldID < header->FieldNum(); field++)
		{
		for (recordID = 0;recordID < recordNum: recordID++)
			{
			if ((fieldSTR = records[recordID]->Field(fieldID)) == (char *) NULL) continue;
			for (i = 0;i < strlen (record); i++)
				
			}
		}
	printf ("%d\n",recordNum);
	return (DBSuccess);
	}

DBInt DBExportASCIITable (DBObjTable *table, const char *fileName)

	{
	DBInt ret;
	FILE *fp;
	if ((fp = fopen (fileName,"w")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	ret = DBExportASCIITable (table,fp);
	fclose (fp);
	return (ret);
	}

DBInt DBExportASCIITable (DBObjTable *table, FILE *fp)

	{
	DBObjectLIST<DBObjTableField> *fields = table->Fields (); 
	DBObjRecord *record;
	DBObjTableField *field;
	
	fprintf (fp,"\"ID\"%c\"Name\"",DBASCIISeparator);
	for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
		if ((strcmp (field->Name (),DBrNSymbol) == 0) || DBTableFieldIsVisible (field))
			fprintf (fp,"%c\"%s\"",DBASCIISeparator,field->Name ());
	fprintf (fp,"\n");
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		{
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		fprintf (fp,"%d%c\"%s\"",record->RowID () + 1,DBASCIISeparator,record->Name ());
		for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
			if ((strcmp (field->Name (),DBrNSymbol) == 0) || DBTableFieldIsVisible (field))
				switch	(field->Type ())
					{
					default:
					case DBTableFieldString:
						fprintf (fp,"%c\"%s\"",DBASCIISeparator,field->String (record));
						break;
					case DBTableFieldInt:
						if (field->Int (record) == field->IntNoData ())
								fprintf (fp,"%c",DBASCIISeparator);
						else	fprintf (fp,"%c%d",DBASCIISeparator,field->Int (record));
						break;
					case DBTableFieldFloat:
						if (CMmathEqualValues (field->Float (record),field->FloatNoData ()))
								fprintf (fp,"%c",DBASCIISeparator);
						else	fprintf (fp,"%c%f",DBASCIISeparator,field->Float (record));
						break;
					case DBTableFieldDate:
						fprintf (fp,"%c\"%s\"",DBASCIISeparator,field->String(record));
						break;
					}
		fprintf (fp,"\n");
		}
	return (DBSuccess);
	}
