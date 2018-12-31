/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - CUNY

RGlibTable.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <DB.H>
#include <RG.H>

DBInt RGLibTableJoin(DBObjTable *itemTable, DBObjTableField *relateField,
                     DBObjTable *joinTable, DBObjTableField *joinField) {
    DBInt itemID, joinID, fieldID, fieldNum = 0;
    DBObjectLIST<DBObjTableField> *fields = joinTable->Fields();
    DBObjRecord *itemRec, *joinRec;
    DBObjTableField *field, **newFields;

    if ((newFields = (DBObjTableField **) calloc(1, sizeof(DBObjTableField *))) == (DBObjTableField **) NULL) {
        CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: RGLibTableJoin ()");
        return (DBFault);
    }

    if ((newFields[fieldNum] = itemTable->Field(joinTable->Name())) == (DBObjTableField *) NULL) {
        newFields[fieldNum] = new DBObjTableField(joinTable->Name(), DBTableFieldString, "%s", DBStringLength);
        itemTable->AddField(newFields[fieldNum]);
    }
    for (fieldID = 0; fieldID < fields->ItemNum(); fieldID++) {
        field = fields->Item(fieldID);
        if (DBTableFieldIsVisible(field)) {
            fieldNum++;
            newFields = (DBObjTableField **) realloc(newFields, (fieldNum + 1) * sizeof(DBObjTableField *));
            if (newFields == (DBObjTableField **) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: RGLibTableJoin ()");
                return (DBFault);
            }
            if ((newFields[fieldNum] = itemTable->Field(field->Name())) == (DBObjTableField *) NULL) {
                itemTable->AddField(newFields[fieldNum] = new DBObjTableField(*field));
                newFields[fieldNum]->Required(false);
            }
        }
    }

    for (itemID = 0; itemID < itemTable->ItemNum(); itemID++) {
        itemRec = itemTable->Item(itemID);
        DBPause(itemID * 100 / itemTable->ItemNum());
        if ((itemRec->Flags() & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
        for (joinID = 0; joinID < joinTable->ItemNum(); joinID++) {
            joinRec = joinTable->Item(joinID);
            if ((joinRec->Flags() & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
            if (DBTableFieldMatch(relateField, itemRec, joinField, joinRec)) {
                fieldNum = 0;
                newFields[fieldNum]->String(itemRec, joinRec->Name());
                for (fieldID = 0; fieldID < fields->ItemNum(); fieldID++) {
                    field = fields->Item(fieldID);
                    if (DBTableFieldIsVisible(field)) {
                        fieldNum++;
                        if (newFields[fieldNum]->Required()) continue;
                        switch (newFields[fieldNum]->Type()) {
                            default:
                            case DBTableFieldString:
                                newFields[fieldNum]->String(itemRec, field->String(joinRec));
                                break;
                            case DBTableFieldInt:
                                newFields[fieldNum]->Int(itemRec, field->Int(joinRec));
                                break;
                            case DBTableFieldFloat:
                                newFields[fieldNum]->Float(itemRec, field->Float(joinRec));
                                break;
                            case DBTableFieldDate:
                                newFields[fieldNum]->Date(itemRec, field->Date(joinRec));
                                break;
                        }
                    }
                }
                break;
            }
        }
    }
    free(newFields);
    return (itemID < itemTable->ItemNum() ? DBFault : DBSuccess);
}

static const char *_RGlibSQLCaseChange (DBInt sqlCase, const char *str, char **bufferPtr, DBInt *bufferLen) {
    DBInt i, len;
    if (sqlCase == RGlibSQLCaseSensitive) return (str);
    len = strlen (str);

    if (len >= *bufferLen) { *bufferPtr = (char *) realloc (*bufferPtr,len + 1);}
    if (*bufferPtr == (char *) NULL) return ("Allocation Error");
    *bufferLen = len;

    for (i = 0;i < len; ++i) (*bufferPtr) [i] = sqlCase == RGlibSQLCaseLower ? tolower (str [i]) : toupper (str [i]);
    (*bufferPtr) [len] = 0;
    return (*bufferPtr);
}

DBInt RGlibTableToSQL (DBObjTable *table, const char *dbSchemaName, const char *dbTableName, DBInt mode, DBInt sqlCase, FILE *outFile) {
    DBObjectLIST<DBObjTableField> *fields = table->Fields();
    DBObjRecord *record;
    DBObjTableField *field;
    DBInt bufferLen [2] = {0,0};
    char *bufferPtr [2] = {(char *) NULL, (char *) NULL};

    if ((RGlibTableCopy == mode) || (RGlibTableBlank == mode)) {
        if (dbSchemaName == (char *) NULL) {
            fprintf (outFile, "-- Table: \"%s\"\n",             _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr, bufferLen));
            fprintf (outFile, "DROP TABLE IF EXISTS \"%s\";\n", _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr, bufferLen));
            fprintf (outFile, "CREATE TABLE \"%s\"\n",          _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr, bufferLen));
        }
        else {
            fprintf (outFile, "-- Table: \"%s\".\"%s\"\n",             _RGlibSQLCaseChange (sqlCase, dbSchemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr + 1, bufferLen + 1));
            fprintf (outFile, "DROP TABLE IF EXISTS \"%s\".\"%s\";\n", _RGlibSQLCaseChange (sqlCase, dbSchemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr + 1, bufferLen + 1));
            fprintf (outFile, "CREATE TABLE \"%s\".\"%s\"\n",          _RGlibSQLCaseChange (sqlCase, dbSchemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr + 1, bufferLen + 1));
        }

        fprintf (outFile,"(\n");
        fprintf (outFile,"\"%s\" SERIAL,\n",_RGlibSQLCaseChange (sqlCase, "ID", bufferPtr, bufferLen));
        fprintf (outFile,"\"%s\" CHARACTER VARYING(%d) COLLATE pg_catalog.\"default\",\n",_RGlibSQLCaseChange (sqlCase, "RecordName", bufferPtr, bufferLen),DBStringLength);
        for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
            if (DBTableFieldIsVisible (field))
                switch (field->Type()) {
                    default:
                    case DBTableFieldString:
                        fprintf(outFile, "\"%s\" CHARACTER VARYING(%d) COLLATE pg_catalog.\"default\",\n", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen),
                                field->Length());
                        break;
                    case DBTableFieldInt:
                        fprintf(outFile, "\"%s\" INTEGER,\n", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                        break;
                    case DBTableFieldFloat:
                        fprintf(outFile, "\"%s\" NUMERIC (%d,%d),\n", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen), field->FormatWidth(), field->FormatDecimals());
                        break;
                    case DBTableFieldDate:
                        fprintf(outFile, "\"%s\" CHARACTER VARYING(10) COLLATE pg_catalog.\"default\",\n", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                        break;
                }
        }
        fprintf (outFile,"CONSTRAINT \"%s:pkey\" PRIMARY KEY (\"%s\")\n", _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr, bufferLen),_RGlibSQLCaseChange (sqlCase, "ID", bufferPtr + 1, bufferLen + 1));
        fprintf (outFile,") WITH ( OIDS = FALSE )\n");
        fprintf (outFile,"TABLESPACE pg_default;\n");
    }

    if ((RGlibTableCopy == mode) || (RGlibTableAppend == mode)) {
        if (dbSchemaName == (char *) NULL) fprintf (outFile,"INSERT INTO  \"%s\" (",  _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr, bufferLen));
        else fprintf (outFile,"\nINSERT INTO  \"%s\".\"%s\" (", _RGlibSQLCaseChange (sqlCase, dbSchemaName, bufferPtr, bufferLen),  _RGlibSQLCaseChange (sqlCase, dbTableName, bufferPtr + 1, bufferLen + 1));
        fprintf (outFile,"\"%s\"",_RGlibSQLCaseChange (sqlCase, "RecordName", bufferPtr, bufferLen));
        for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
            if (DBTableFieldIsVisible (field)) fprintf (outFile,",\"%s\"",_RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
        }
        fprintf (outFile,") VALUES\n");
        for (record = table->First (); record != (DBObjRecord *) NULL; record = table->Next ()) {
            if (record->RowID () == 0) fprintf (outFile,    "('%s'", record->Name());
            else                       fprintf (outFile,"),\n('%s'", record->Name());
            for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
                if (DBTableFieldIsVisible (field))
                    switch (field->Type()) {
                        default:
                        case DBTableFieldString: fprintf(outFile,",$$%s$$", field->String(record));  break;
                        case DBTableFieldInt:    fprintf(outFile,",%d", field->Int (record));        break;
                        case DBTableFieldFloat:  fprintf(outFile,",%f", field->Float (record));      break;
                        case DBTableFieldDate:   fprintf(outFile,",\'%s\'", field->String (record)); break;
                    }
            }
        }
        fprintf (outFile,");");
    }
    if (bufferPtr [0] != (char *) NULL) free (bufferPtr [0]);
    if (bufferPtr [1] != (char *) NULL) free (bufferPtr [1]);
    return (CMsucceeded);
 }
