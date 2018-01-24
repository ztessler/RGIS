/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2018, UNH - CCNY/CUNY

DBExpARCPnt.C

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

int DBExportARCGenLine(DBObjData *data, char *fileName) {
    FILE *file;
    DBInt vertex, vertexNum;
    DBCoordinate *coords, nodeCoord;
    DBObjTable *items = data->Table(DBrNItems);
    DBObjRecord *lineRec;
    DBVLineIF *lineIF = new DBVLineIF (data);

    if ((file = fopen(fileName, "w")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgSysError, "ARC Generate File Opening Error in: %s %d", __FILE__, __LINE__);
        return (DBFault);
    }
    for (lineRec = items->First(); lineRec != (DBObjRecord *) NULL; lineRec = items->Next()) {
        fprintf (file,"%d\n",lineRec->RowID ());
        nodeCoord = lineIF->FromCoord(lineRec);
        fprintf(file, "%f,%f\n", nodeCoord.X, nodeCoord.Y);
        if (lineIF->VertexNum(lineRec) > 0) {
            coords = lineIF->Vertexes(lineRec);
            for (vertex = 0;vertex < vertexNum; ++vertex)
                fprintf(file, "%f,%f\n", coords [vertex].X, coords [vertex].Y);
        }
        nodeCoord = lineIF->ToCoord(lineRec);
        fprintf(file, "%f,%f\n", nodeCoord.X, nodeCoord.Y);
        fprintf(file, "END\n");
    }
    fprintf(file, "END\n");
    fclose(file);
    return (DBSuccess);
}
