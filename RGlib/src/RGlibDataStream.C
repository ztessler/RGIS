/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2017, UNH - CCNY/CUNY

RGlibDataStream.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include<cm.h>
#include<DB.H>
#include<DBif.H>
#include<MF.h>

DBInt RGlibRGIS2DataStream(DBObjData *grdData, DBObjData *tmplData, char *fieldName, FILE *outFile) {
    DBInt layerID, ret = DBSuccess, itemSize, itemID;
    DBInt intValue;
    DBFloat floatValue;
    void *data;
    MFdsHeader_t dsHeader;
    DBObjRecord *layerRec, *gridRec;
    DBObjTableField *fieldPTR = (DBObjTableField *) NULL;
    DBGridIF *gridIF;
    DBGridSampler *sampler = (DBGridSampler *) NULL;
    DBVPointIF *tmplPntIF = (DBVPointIF *) NULL;
    DBGridIF *tmplGrdIF = (DBGridIF *) NULL;
    DBNetworkIF *tmplNetIF = (DBNetworkIF *) NULL;

    gridIF = new DBGridIF(grdData);

    dsHeader.Swap = 1;
    if (grdData->Type() == DBTypeGridDiscrete) {
        DBObjTable *itemTable = grdData->Table(DBrNItems);

        if (fieldName == (char *) NULL) fieldName = DBrNGridValue;
        if ((fieldPTR = itemTable->Field(fieldName)) == (DBObjTableField *) NULL) {
            CMmsgPrint(CMmsgAppError, "Error: Invalid field [%s] in: %s %d", fieldName, __FILE__, __LINE__);
            return (DBFault);
        }
        itemSize = fieldPTR->Length();
        switch (fieldPTR->Type()) {
            case DBTableFieldInt:
                switch (itemSize) {
                   case sizeof(DBByte):
                        dsHeader.Type = MFByte;
                        break;
                    case sizeof(DBShort):
                        dsHeader.Type = MFShort;
                        break;
                    case sizeof(DBInt):
                        dsHeader.Type = MFInt;
                        break;
                    default:
                        CMmsgPrint(CMmsgAppError, "Error: Invalid field size in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                }
                dsHeader.Missing.Int = fieldPTR->IntNoData();
                break;
            case DBTableFieldFloat:
                switch (itemSize) {
                    case sizeof(DBFloat4):
                        dsHeader.Type = MFFloat;
                        break;
                    case sizeof(DBFloat):
                        dsHeader.Type = MFDouble;
                        break;
                    default:
                        CMmsgPrint(CMmsgAppError, "Error: Invalid field size in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                }
                dsHeader.Missing.Float = fieldPTR->FloatNoData();
                break;
            default:
                CMmsgPrint(CMmsgAppError, "Error: Invalid field type in: %s %d", __FILE__, __LINE__);
                return (DBFault);
        }
    }
    else {
        if (fieldName != (char *) NULL) CMmsgPrint(CMmsgUsrError, "Warning: Fieldname ignored for continuous grid!");
        itemSize = gridIF->ValueSize();
        switch (gridIF->ValueType()) {
            case DBVariableInt:
                switch (itemSize) {
                    case 1:
                        dsHeader.Type = MFByte;
                        break;
                    case 2:
                        dsHeader.Type = MFShort;
                        break;
                    case 4:
                        dsHeader.Type = MFInt;
                        break;
                    default:
                        CMmsgPrint(CMmsgAppError, "Error: Invalid field size in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                }
                dsHeader.Missing.Int = (int) gridIF->MissingValue();
                break;
            case DBVariableFloat:
                switch (itemSize) {
                    case 4:
                        dsHeader.Type = MFFloat;
                        break;
                    case 8:
                        dsHeader.Type = MFDouble;
                        break;
                    default:
                        CMmsgPrint(CMmsgAppError, "Error: Invalid field size in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                }
                dsHeader.Missing.Float = gridIF->MissingValue();
                break;
            default:
                CMmsgPrint(CMmsgAppError, "Error: Invalid field type in: %s %d", __FILE__, __LINE__);
                return (DBFault);
        }
    }

    if (tmplData == (DBObjData *) NULL) {
        tmplGrdIF = gridIF;
        dsHeader.ItemNum = gridIF->RowNum() * gridIF->ColNum();
    }
    else {
        switch (tmplData->Type()) {
            case DBTypeVectorPoint:
                tmplPntIF = new DBVPointIF(tmplData);
                dsHeader.ItemNum = tmplPntIF->ItemNum();
                break;
            case DBTypeGridContinuous:
            case DBTypeGridDiscrete:
                tmplGrdIF = new DBGridIF(tmplData);
                dsHeader.ItemNum = gridIF->RowNum() * gridIF->ColNum();
                break;
            case DBTypeNetwork:
                tmplNetIF = new DBNetworkIF(tmplData);
                dsHeader.ItemNum = tmplNetIF->CellNum();
                break;
            default:
                delete gridIF;
                return (DBFault);
        }
    }
    if ((data = calloc((size_t) (dsHeader.ItemNum), (size_t) itemSize)) == (void *) NULL) {
        CMmsgPrint(CMmsgSysError, "Error! Allocating %d items of %d size in: %s %d", dsHeader.ItemNum, itemSize,
                   __FILE__, __LINE__);
        return (DBFault);
    }

/**************************************************************
*                                                             *
* Point template                                              *
*                                                             *
**************************************************************/

    if (tmplPntIF != (DBVPointIF *) NULL) {
        DBObjRecord *pntRec;

        if ((fieldPTR == (DBObjTableField *) NULL) && ((dsHeader.Type == MFFloat) || (dsHeader.Type == MFDouble))) {
            if ((sampler = (DBGridSampler *) calloc(sizeof(DBGridSampler), (size_t) tmplPntIF->ItemNum())) ==
                (DBGridSampler *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                return (DBFault);
            }
            for (itemID = 0; itemID < tmplPntIF->ItemNum(); ++itemID) {
                pntRec = tmplPntIF->Item(itemID);
                gridIF->Coord2Sampler(tmplPntIF->Coordinate(pntRec), sampler[itemID]);
            }
        }

        if (fieldPTR == (DBObjTableField *) NULL) {
            for (layerID = 0; layerID < gridIF->LayerNum(); ++layerID) {
                layerRec = gridIF->Layer(layerID);
                strncpy(dsHeader.Date, layerRec->Name(), MFDateStringLength - 1);
                for (itemID = 0; itemID < dsHeader.ItemNum; ++itemID) {
                    pntRec = tmplPntIF->Item(itemID);
                    if ((dsHeader.Type == MFByte) || (dsHeader.Type == MFShort) ||
                        (dsHeader.Type == MFInt)) {
                        if (gridIF->Value(layerRec, tmplPntIF->Coordinate(pntRec), &intValue) == false)
                            intValue = dsHeader.Missing.Int;
                        switch (dsHeader.Type) {
                            case MFByte:
                                ((char *) data)[itemID] = (char) intValue;
                                break;
                            case MFShort:
                                ((short *) data)[itemID] = (short) intValue;
                                break;
                            case MFInt:
                                ((int *) data)[itemID] = (short) intValue;
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                    }
                    else {
                        if (gridIF->Value(layerRec, sampler [pntRec->RowID()], &floatValue) == false)
                            floatValue = dsHeader.Missing.Float;
                        switch (dsHeader.Type) {
                            case MFFloat:
                                ((float *) data)[itemID] = (float) floatValue;
                                break;
                            case MFDouble:
                                ((double *) data)[itemID] = (double) floatValue;
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                    }
                }
                if ((DBInt) fwrite(&dsHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
                if ((DBInt) fwrite(data, (size_t) itemSize, (size_t) (dsHeader.ItemNum), outFile) != dsHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
            }
        }
        else {
            for (layerID = 0; layerID < gridIF->LayerNum(); ++layerID) {
                layerRec = gridIF->Layer(layerID);
                strncpy(dsHeader.Date, layerRec->Name(), MFDateStringLength - 1);
                for (itemID = 0; itemID < dsHeader.ItemNum; ++itemID) {
                    pntRec = tmplPntIF->Item(itemID);
                    gridRec = gridIF->GridItem(layerRec, tmplPntIF->Coordinate(pntRec));
                    switch (dsHeader.Type) {
                        case MFByte:
                            ((char *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? (char) fieldPTR->Int(gridRec) : (char) fieldPTR->IntNoData();
                            break;
                        case MFShort:
                            ((short *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? (short) fieldPTR->Int(gridRec) : (short) fieldPTR->IntNoData();
                            break;
                        case MFInt:
                            ((int *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? fieldPTR->Int(gridRec) : fieldPTR->IntNoData();
                            break;
                        case MFFloat:
                            ((float *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? (float) fieldPTR->Float(gridRec) : (float) fieldPTR->FloatNoData();
                            break;
                        case MFDouble:
                            ((double *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? (double) fieldPTR->Float(gridRec) : (double) fieldPTR->FloatNoData();
                            break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                    }
                }
                if ((DBInt) fwrite(&dsHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
                if ((DBInt) fwrite(data, (size_t) itemSize, (size_t) (dsHeader.ItemNum), outFile) != dsHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
            }
        }
        delete tmplPntIF;
    }

/**************************************************************
*                                                             *
* Grid Template (default when no template coverage is given.  *
*                                                             *
**************************************************************/
    else if (tmplGrdIF != (DBGridIF *) NULL) {
        DBPosition pos;
        DBCoordinate coord;

        if ((tmplGrdIF != gridIF) && (fieldPTR == (DBObjTableField *) NULL) && ((dsHeader.Type == MFFloat) || (dsHeader.Type == MFDouble))) {
            if ((sampler = (DBGridSampler *) calloc(sizeof(DBGridSampler),
                                                    (size_t) tmplGrdIF->RowNum() * (size_t) tmplGrdIF->ColNum())) ==
                (DBGridSampler *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                return (DBFault);
            }
            for (pos.Row = 0; pos.Row < tmplGrdIF->RowNum(); ++pos.Row)
                for (pos.Col = 0; pos.Col < tmplGrdIF->ColNum(); ++pos.Col) {
                    tmplGrdIF->Pos2Coord(pos, coord);
                    gridIF->Coord2Sampler(coord, sampler[pos.Row * tmplGrdIF->RowNum() + pos.Col]);
                }
        }

        if (fieldPTR == (DBObjTableField *) NULL) {
            for (layerID = 0; layerID < gridIF->LayerNum(); ++layerID) {
                layerRec = gridIF->Layer(layerID);
                strncpy(dsHeader.Date, layerRec->Name(), MFDateStringLength - 1);
                for (pos.Row = 0; pos.Row < tmplGrdIF->RowNum(); ++pos.Row)
                    for (pos.Col = 0; pos.Col < tmplGrdIF->ColNum(); ++pos.Col) {
                        itemID = pos.Row * tmplGrdIF->ColNum() + pos.Col;
                        if ((dsHeader.Type == MFByte) || (dsHeader.Type == MFShort) ||
                            (dsHeader.Type == MFInt)) {
                            if (tmplGrdIF != gridIF) {
                                tmplGrdIF->Pos2Coord(pos, coord);
                                if (gridIF->Value(layerRec, coord, &intValue) == false)
                                    intValue = dsHeader.Missing.Int;
                            }
                            else {
                                if (gridIF->Value(layerRec, pos, &intValue) == false) intValue = dsHeader.Missing.Int;
                            }
                            switch (dsHeader.Type) {
                                case MFByte:
                                    ((char *) data)[itemID] = (char) intValue;
                                    break;
                                case MFShort:
                                    ((short *) data)[itemID] = (short) intValue;
                                    break;
                                case MFInt:
                                    ((int *) data)[itemID] = (short) intValue;
                                    break;
                                default:
                                    CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                    return (DBFault);
                            }
                        }
                        else {
                            if (tmplGrdIF != gridIF) {
                                if (gridIF->Value(layerRec, sampler[pos.Row * tmplGrdIF->RowNum() + pos.Col], &floatValue) == false)
                                    floatValue = dsHeader.Missing.Float;
                            }
                            else {
                                if (gridIF->Value(layerRec, pos, &floatValue) == false)
                                    floatValue = dsHeader.Missing.Float;
                            }
                            switch (dsHeader.Type) {
                                case MFFloat:
                                    ((float *) data)[itemID] = (float) floatValue;
                                    break;
                                case MFDouble:
                                    ((double *) data)[itemID] = (double) floatValue;
                                    break;
                                default:
                                    CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                    return (DBFault);
                            }
                        }
                    }
                if ((DBInt) fwrite(&dsHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
                if ((DBInt) fwrite(data, (size_t) itemSize, (size_t) (dsHeader.ItemNum), outFile) != dsHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
            }
        }
        else {
            for (layerID = 0; layerID < gridIF->LayerNum(); ++layerID) {
                layerRec = gridIF->Layer(layerID);
                strncpy(dsHeader.Date, layerRec->Name(), MFDateStringLength - 1);
                for (pos.Row = 0; pos.Row < tmplGrdIF->RowNum(); ++pos.Row)
                    for (pos.Col = 0; pos.Col < tmplGrdIF->ColNum(); ++pos.Col) {
                        itemID = pos.Row * tmplGrdIF->ColNum() + pos.Col;
                        if (tmplGrdIF != gridIF) {
                            tmplGrdIF->Pos2Coord(pos, coord);
                            gridRec = gridIF->GridItem(layerRec, coord);
                        }
                        else gridRec = gridIF->GridItem(layerRec, pos);
                        switch (dsHeader.Type) {
                            case MFByte:
                                ((char *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? (char) fieldPTR->Int(gridRec)
                                                                                          : (char) fieldPTR->IntNoData();
                                break;
                            case MFShort:
                                ((short *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? (short) fieldPTR->Int(gridRec)
                                                                                           : (short) fieldPTR->IntNoData();
                                break;
                            case MFInt:
                                ((int *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int(gridRec)
                                                                                         : fieldPTR->IntNoData();
                                break;
                            case MFFloat:
                                ((float *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? (float) fieldPTR->Float(gridRec)
                                                                                           : (float) fieldPTR->FloatNoData();
                                break;
                            case MFDouble:
                                ((double *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? (double) fieldPTR->Float(gridRec)
                                                                                            : (double) fieldPTR->FloatNoData();
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                    }
                if ((DBInt) fwrite(&dsHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
                if ((DBInt) fwrite(data, (size_t) itemSize, (size_t) (dsHeader.ItemNum), outFile) != dsHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
            }
        }
        if (tmplGrdIF != gridIF) delete tmplGrdIF;
    }

/**************************************************************
*                                                             *
* Network Template                                            *
*                                                             *
**************************************************************/
    else if (tmplNetIF != (DBNetworkIF *) NULL) {
        DBObjRecord *cellRec;

        if ((fieldPTR == (DBObjTableField *) NULL) && ((dsHeader.Type == MFFloat) || (dsHeader.Type == MFDouble))) {
            if ((sampler = (DBGridSampler *) calloc(sizeof(DBGridSampler), (size_t) tmplNetIF->CellNum())) ==
                (DBGridSampler *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                return (DBFault);
            }
            for (itemID = 0; itemID < tmplNetIF->CellNum(); ++itemID) {
                cellRec = tmplNetIF->Cell(itemID);
                gridIF->Coord2Sampler(tmplNetIF->Center(cellRec), sampler[itemID]);
            }
        }

        if (fieldPTR == (DBObjTableField *) NULL) {
            for (layerID = 0; layerID < gridIF->LayerNum(); ++layerID) {
                layerRec = gridIF->Layer(layerID);
                strncpy(dsHeader.Date, layerRec->Name(), MFDateStringLength - 1);
                for (itemID = 0; itemID < dsHeader.ItemNum; ++itemID) {
                    cellRec = tmplNetIF->Cell(itemID);
                    if ((dsHeader.Type == MFByte) || (dsHeader.Type == MFShort) ||
                        (dsHeader.Type == MFInt)) {
                        if (gridIF->Value(layerRec, tmplNetIF->Center(cellRec), &intValue) == false)
                            intValue = dsHeader.Missing.Int;
                        switch (dsHeader.Type) {
                            case MFByte:
                                ((char *) data)[itemID] = (char) intValue;
                                break;
                            case MFShort:
                                ((short *) data)[itemID] = (short) intValue;
                                break;
                            case MFInt:
                                ((int *) data)[itemID] = (short) intValue;
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                    }
                    else {
                        if (gridIF->Value(layerRec, sampler[itemID], &floatValue) == false)
                            floatValue = dsHeader.Missing.Float;
                        switch (dsHeader.Type) {
                            case MFFloat:
                                ((float *) data)[itemID] = (float) floatValue;
                                break;
                            case MFDouble:
                                ((double *) data)[itemID] = (double) floatValue;
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                    }
                }
                if ((DBInt) fwrite(&dsHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
                if ((DBInt) fwrite(data, (size_t) itemSize, (size_t) (dsHeader.ItemNum), outFile) != dsHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
            }
        }
        else {
            for (layerID = 0; layerID < gridIF->LayerNum(); ++layerID) {
                layerRec = gridIF->Layer(layerID);
                strncpy(dsHeader.Date, layerRec->Name(), MFDateStringLength - 1);
                for (itemID = 0; itemID < dsHeader.ItemNum; ++itemID) {
                    cellRec = tmplNetIF->Cell(itemID);
                    gridRec = gridIF->GridItem(layerRec, tmplNetIF->Center(cellRec));
                    switch (dsHeader.Type) {
                        case MFByte:
                            ((char *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? (char) fieldPTR->Int(gridRec) : (char) fieldPTR->IntNoData();
                            break;
                        case MFShort:
                            ((short *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? (short) fieldPTR->Int(gridRec) : (short) fieldPTR->IntNoData();
                            break;
                        case MFInt:
                            ((int *) data)[itemID] =
                                    gridRec != (DBObjRecord *) NULL ? fieldPTR->Int(gridRec) : fieldPTR->IntNoData();
                            break;
                        case MFFloat:
                            ((float *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? (float) fieldPTR->Float(gridRec)
                                                                                       : (float) fieldPTR->FloatNoData();
                            break;
                        case MFDouble:
                            ((double *) data)[itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Float(gridRec)
                                                                                        : fieldPTR->FloatNoData();
                            break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                    }
                }
                if ((DBInt) fwrite(&dsHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
                if ((DBInt) fwrite(data, (size_t) itemSize, (size_t) (dsHeader.ItemNum), outFile) != dsHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    ret = DBFault;
                    break;
                }
            }
        }
        delete tmplNetIF;
    }

    free(data);
    if (sampler != (DBGridSampler *) NULL) free (sampler);
    delete gridIF;
    return (ret);
}

DBInt RGlibDataStream2RGIS(DBObjData *outData, DBObjData *tmplData, FILE *inFile) {
    DBInt layerID = 0, itemSize;
    DBPosition pos;
    DBFloat val;
    void *data = (void *) NULL;
    MFdsHeader_t header;
    DBObjRecord *record;


    switch (tmplData->Type()) {
        case DBTypeVectorPoint: {
            DBInt itemID;
            DBDate date;
            DBObjTable *itemTable = outData->Table(DBrNItems);
            DBObjTableField *idField = new DBObjTableField("ItemID", DBTableFieldInt, "%6d", sizeof(DBInt), false);
            DBObjTableField *dateField = new DBObjTableField("Date", DBTableFieldDate, "%s", sizeof(DBDate), false);
            DBObjTableField *valField;
            DBVPointIF *pntIF = new DBVPointIF(tmplData);

            itemTable->AddField(idField);
            itemTable->AddField(dateField);

            while (MFdsHeaderRead (&header, inFile) == CMsucceeded) {
                if (header.ItemNum != pntIF->ItemNum()) {
                    CMmsgPrint(CMmsgUsrError, "Error: Datastream inconsistency %d %d!", header.ItemNum,
                               pntIF->ItemNum());
                    return (DBFault);
                }
                if (data == (void *) NULL) {
                    itemSize = MFVarItemSize(header.Type);
                    if ((data = (void *) realloc(data, (size_t) header.ItemNum * itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    switch (header.Type) {
                        case MFByte:
                            valField = new DBObjTableField("Value", DBTableFieldInt, "%2d", sizeof(char), false); break;
                        case MFShort:
                            valField = new DBObjTableField("Value", DBTableFieldInt, "%4d", sizeof(DBShort), false); break;
                        case MFInt:
                            valField = new DBObjTableField("Value", DBTableFieldInt, "%8d", sizeof(DBInt), false); break;
                        case MFFloat:
                            valField = new DBObjTableField("Value", DBTableFieldFloat, "%8.2f", sizeof(DBFloat4), false); break;
                        case MFDouble:
                            valField = new DBObjTableField("Value", DBTableFieldFloat, "%8.2f", sizeof(DBFloat), false); break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                    }
                    itemTable->AddField(valField);
                }
                if ((int) fread(data, (size_t) itemSize, (size_t) (header.ItemNum), inFile) != header.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Data stream read in: %s %d", __FILE__, __LINE__);
                    return (DBFault);
                }
                for (itemID = 0; itemID < header.ItemNum; ++itemID) {
                    record = itemTable->Add(header.Date);
                    date.Set(header.Date);
                    idField->Int(record, itemID);
                    dateField->Date(record, date);
/*					decDateField->Float (record,date);
*/                    switch (header.Type) {
                        case MFByte:
                            valField->Int(record, ((char *) data)[itemID]);
                            break;
                        case MFShort:
                            valField->Int(record, ((short *) data)[itemID]);
                            break;
                        case MFInt:
                            valField->Int(record, ((int *) data)[itemID]);
                            break;
                        case MFFloat:
                            valField->Float(record, ((float *) data)[itemID]);
                            break;
                        case MFDouble:
                            valField->Float(record, ((double *) data)[itemID]);
                            break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                        }
                    }
                }
                if (ferror (inFile) != 0) CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
                delete pntIF;
            }
            break;
        case DBTypeGridContinuous:
        case DBTypeGridDiscrete: {
            DBGridIF *gridIF = new DBGridIF(outData);

            while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
                if (header.ItemNum != gridIF->RowNum() * gridIF->ColNum()) {
                    CMmsgPrint(CMmsgUsrError, "Error: Datastream inconsistency!");
                    return (DBFault);
                }
                if (layerID == 0) {
                    itemSize = MFVarItemSize(header.Type);
                    if ((data = realloc(data, header.ItemNum * itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    record = gridIF->Layer(layerID);
                    gridIF->RenameLayer(header.Date);
                }
                else record = gridIF->AddLayer(header.Date);

                switch (header.Type) {
                    case MFByte:
                    case MFShort:
                    case MFInt:
                        gridIF->MissingValue(record, header.Missing.Int);
                        break;
                    case MFFloat:
                    case MFDouble:
                        gridIF->MissingValue(record, header.Missing.Float);
                        break;
                    default:
                        CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                }
                if ((int) fread(data, (size_t) itemSize, (size_t) (header.ItemNum), inFile) != header.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Data stream read in: %s %d", __FILE__, __LINE__);
                    return (DBFault);
                }

                for (pos.Row = 0; pos.Row < gridIF->RowNum(); ++pos.Row)
                    for (pos.Col = 0; pos.Col < gridIF->ColNum(); ++pos.Col) {
                        switch (header.Type) {
                            case MFByte:
                                val = (DBFloat) (((char *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFShort:
                                val = (DBFloat) (((short *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFInt:
                                val = (DBFloat) (((int *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFFloat:
                                val = (DBFloat) (((float *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFDouble:
                                val = (DBFloat) (((double *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                        gridIF->Value(record, pos, val);
                    }
                layerID++;
                }
            if (ferror (inFile) != 0) CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
            gridIF->RecalcStats();
            }
            break;
        case DBTypeNetwork: {
            DBInt cellID;
            DBGridIF *gridIF = new DBGridIF(outData);
            DBNetworkIF *netIF = new DBNetworkIF(tmplData);

            while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
                if (header.ItemNum != netIF->CellNum()) {
                    CMmsgPrint(CMmsgUsrError, "Error: Datastream inconsistency!");
                    return (DBFault);
                }
                if (layerID == 0) {
                    itemSize = MFVarItemSize(header.Type);
                    if ((data = (void *) realloc(data, (size_t) header.ItemNum * itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    record = gridIF->Layer(layerID);
                    gridIF->RenameLayer(header.Date);
                } else record = gridIF->AddLayer(header.Date);
                if ((int) fread(data, itemSize, header.ItemNum, inFile) != header.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Data stream read in: %s %d", __FILE__, __LINE__);
                    delete netIF;
                    return (DBFault);
                }

                for (pos.Row = 0; pos.Row < gridIF->RowNum(); ++pos.Row)
                    for (pos.Col = 0; pos.Col < gridIF->ColNum(); ++pos.Col)
                        gridIF->Value(record, pos, gridIF->MissingValue());

                for (cellID = 0; cellID < netIF->CellNum(); ++cellID) {
                    pos = netIF->CellPosition(netIF->Cell(cellID));

                    switch (header.Type) {
                        case MFByte:
                            val = (DBFloat) (((char *) data)[cellID]);
                            break;
                        case MFShort:
                            val = (DBFloat) (((short *) data)[cellID]);
                            break;
                        case MFInt:
                            val = (DBFloat) (((int *) data)[cellID]);
                            break;
                        case MFFloat:
                            val = (DBFloat) (((float *) data)[cellID]);
                            break;
                        case MFDouble:
                            val = (DBFloat) (((double *) data)[cellID]);
                            break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                    }
                    gridIF->Value(record, pos, val);
                }
                layerID++;
            }
            if (ferror(inFile) != 0) CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
            gridIF->RecalcStats();
        } break;
        default:
            CMmsgPrint(CMmsgAppError, "Error: Invalid data in: %s %d", __FILE__, __LINE__);
            break;
    }
    return (DBSuccess);
}
