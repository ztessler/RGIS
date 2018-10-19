#!/bin/bash

function PrintUsage ()
{
	echo "Usage ${0##*/} [options] <rgisfile>"
	echo "      -c, --case [sensitive|lower|upper]"
	echo "      -d, --dbname  <dbname>"
	echo "      -s, --schema  <schema>"
	echo "      -t, --tblname <tblname>"
	exit 1
}

RGISFILE=""
  DBNAME=""
  SCHEMA=""
 TBLNAME=""
    CASE="sensitive"

while [ "${1}" != "" ]
do
	case "${1}" in
	(-c|--case)
	    shift; if [ "${1}" == "" ]; then PrintUsage; fi
	    case "${1}" in
	    (sensitive|lower|upper)
	        CASE="${1}"
	        ;;
	    (*)
	        CASE="sensitive"
	        PrintUsage
	        ;;
	    esac
	    shift;
	    ;;
	(-d|--dbname)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		DBNAME="${1}"
		shift
	;;
	(-s|--schema)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		SCHEMA="${1}"; shift
	;;
	(-t|--tblname)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		TBLNAME="${1}"; shift
	;;
	(-*)
		PrintUsage
	;;
	(*)
		RGISFILE="${1}"; shift
	esac
done

if [ "${RGISFILE}" == "" ]; then PrintUsage; fi
FILENAME="${RGISFILE##*/}"
FILENAME="${FILENAME%%.*}"
EXTENSION="${RGISFILE#*.}"

if [ "${SCHEMA}"  == "" ]; then  SCHEMA="public"; fi
if [ "${TBLNAME}" == "" ]; then TBLNAME="${FILENAME}"; fi

case "${CASE}" in
(lower)
	 SCHEMA=$(echo "${SCHEMA}"  | tr "[A-Z]" "[a-z]")
	TBLNAME=$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")
;;
(upper)
	 SCHEMA=$(echo "${SCHEMA}"  | tr "[a-z]" "[A-Z]")
	TBLNAME=$(echo "${TBLNAME}" | tr "[a-z]" "[A-Z]")
;;
esac

TEMPFILE="TEMP${FILENAME}"

case "${EXTENSION}" in
	(gdbp|gdbp.gz|gdbl|gdbl.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.asc"

        rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" "${RGISFILE}" | tee DEBUG.sql | psql "${DBNAME}"
        exit 1
		ogr2ogr -f "ESRI Shapefile" "${TEMPFILE}.shp" "${TEMPFILE}.asc"
		shp2pgsql -k -s 4326 "${TEMPFILE}.shp" "${SCHEMA}"."${TBLNAME}_geom" | psql "${DBNAME}"
		echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;
      		UPDATE \"${SCHEMA}\".\"${TBLNAME}\"
        	SET \"geom\" = \"${SCHEMA}\".\"${TBLNAME}_geom\".\"geom\"
         	FROM   \"${SCHEMA}\".\"${TBLNAME}_geom\"
        	WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"ID\" =  \"${SCHEMA}\".\"${TBLNAME}_geom\".\"ID\";
         	DROP TABLE \"${SCHEMA}\".\"${TBLNAME}_geom\";" | psql "${DBNAME}"
      rm "${TEMPFILE}".*
	;;
	(gdbd|gdbd.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" "${RGISFILE}" | psql "${DBNAME}"
		gdal_translate -a_srs EPSG:4326 "${TEMPFILE}.grd" "${TEMPFILE}.tif"
		gdal_polygonize.py -8  "${TEMPFILE}.tif" -f "ESRI Shapefile" "${TEMPFILE}.shp"
		shp2pgsql -k -s 4326 "${TEMPFILE}.shp" "${SCHEMA}"."${TBLNAME}_geom" | tee "${TEMPFILE}".sql | psql "${DBNAME}"
		echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;
      		UPDATE \"${SCHEMA}\".\"${TBLNAME}\"
        	SET \"geom\" = \"${TBLNAME}_SELECTION\".\"geom\"
         	FROM  (SELECT \"DN\", ST_BUFFER (ST_UNION (\"geom\"),0.0) AS \"geom\" FROM \"${SCHEMA}\".\"${TBLNAME}_geom\"
         	       GROUP BY \"${SCHEMA}\".\"${TBLNAME}_geom\".\"DN\") AS \"${TBLNAME}_SELECTION\"
        	WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"GridValue\" = \"${TBLNAME}_SELECTION\".\"DN\";
	       	DROP TABLE\"${SCHEMA}\".\"${TBLNAME}_geom\";" | psql "${DBNAME}"
         rm "${TEMPFILE}".*
	;;
	(gdbc|gdbc.gz|nc)
	    rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		raster2pgsql "${TEMPFILE}.grd" "${SCHEMA}"."${TBLNAME}_Raster" |\
		sed "s:$(echo "${SCHEMA}"  | tr "[A-Z]" "[a-z]"):${SCHEMA}:g"  |\
		sed "s:$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]"):${TBLNAME}:g" |\
		psql "${DBNAME}"
		rm "${TEMPFILE}.grd"
	;;
	(*)
		echo ${EXTENSION}
		PrintUsage
	;;
esac