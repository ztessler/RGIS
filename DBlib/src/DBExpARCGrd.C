/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - ASRC/CUNY

DBExpARCGrd.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

int DBExportARCGridLayer(DBObjData *data, DBObjRecord *layerRec, const char *fileName) {
    FILE *file;
    DBInt ret;

    if ((file = fopen(fileName, "w")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgSysError, "File Opening Error in: %s %d", __FILE__, __LINE__);
        return (DBFault);
    }
    ret = DBExportARCGridLayer(data, layerRec, file);
    fclose(file);
    return (ret);
}

int DBExportARCGridLayer(DBObjData *data, DBObjRecord *layerRec, FILE *file) {
    DBInt row, col;
    DBPosition pos;
    DBGridIF *gridIF = new DBGridIF(data);

    fprintf(file, "ncols         %d\n", gridIF->ColNum());
    fprintf(file, "nrows         %d\n", gridIF->RowNum());
    fprintf(file, "xllcorner     %f\n", (data->Extent()).LowerLeft.X);
    fprintf(file, "yllcorner     %f\n", (data->Extent()).LowerLeft.Y);
    fprintf(file, "cellsize      %.8f\n", gridIF->CellWidth());
    switch (data->Type()) {
        case DBTypeGridContinuous:
        DBFloat value;
        fprintf(file, "NODATA_value  %f\n", gridIF->MissingValue());
        for (row = gridIF->RowNum() - 1; row >= 0; row--) {
            for (col = 0; col < gridIF->ColNum(); col++) {
                pos.Row = row;
                pos.Col = col;
                if (gridIF->Value(layerRec, pos, &value)) fprintf(file, " %.15g", value);
                else fprintf(file, " %f", gridIF->MissingValue());
            }
            fprintf(file, "\n");
        }
        case DBTypeGridDiscrete:
        fprintf(file, "NODATA_value  %d\n", DBDefaultMissingIntVal);
        for (row = gridIF->RowNum() - 1; row >= 0; row--) {
            for (col = 0; col < gridIF->ColNum(); col++) {
                pos.Row = row;
                pos.Col = col;
                fprintf(file, " %d", gridIF->GridValue(layerRec, pos));
            }
            fprintf(file, "\n");
        }
    }
    delete gridIF;
    return (DBSuccess);
}
