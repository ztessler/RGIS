#!/bin/bash

function PrintUsage ()
{
	echo "Usage ${0##*/} [options] <rgisfile>"
	echo "      -d, --dbname  <dbnam>"
	echo "      -s, --schema  <schema>"
	echo "      -t, --tblname <tblname>"
	exit 1
}

RGISFILE=""
  DBNAME=""
  SCHEMA=""
 TBLNAME=""

while [ "${1}" != "" ]
do
	case "${1}" in
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

TEMPFILE="TEMP${FILENAME}"
TEMPFILE="${TEMPFILE}.asc"

case "${EXTENSION}" in
	(gdbp|gdbp.gz|gdbl|gdbl.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.asc"
		
		rgis2sql -a "DBItems" "${RGISFILE}" -s "${SCHEMA}" -q "${TBLNAME}" | psql "${DBNAME}"
		ogr2ogr -f PostgreSQL "PG: dbname=${DBNAME}" -a_srs EPSG:4326 -nln ${SCHEMA}.${TBLNAME}_geom "${TEMPFILE}.asc"
		echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;
      		UPDATE \"${SCHEMA}\".\"${TBLNAME}\"
        	SET \"geom\" = \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\".\"wkb_geometry\"
         	FROM   \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\"
        	WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"ID\" =  \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\".\"ogc_fid\";
         	DROP TABLE \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\";" | psql "${DBNAME}"
         rm "${TEMPFILE}.asc"
	;;
	(gdbd|gdbd.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		rgis2sql -a "DBItems" "${RGISFILE}" -s "${SCHEMA}" -q "${TBLNAME}" | psql "${DBNAME}"
		gdal_translate -a_srs EPSG:4326 "${TEMPFILE}.grd" "${TEMPFILE}.tif"
		gdal_polygonize.py -8  "${TEMPFILE}.tif" -f "ESRI Shapefile" "${TEMPFILE}.shp"
		shp2pgsql -k -s 4326 "${TEMPFILE}.shp" "${SCHEMA}"."${TBLNAME}_poly" | psql PostGIS_COMPASS
		echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;
      		UPDATE \"${SCHEMA}\".\"${TBLNAME}\"
        	SET \"geom\" = \"${SCHEMA}\".\"${TBLNAME}_poly\".\"geom\"
         	FROM  \"${SCHEMA}\".\"${TBLNAME}_poly\"
        	WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"ID\" = \"${SCHEMA}\".\"${TBLNAME}_poly\".\"DN\";
         	DROP TABLE\"${SCHEMA}\".\"${TBLNAME}_poly\";" | psql "${DBNAME}"
         rm "${TEMPFILE}".*
	;;
	(gdbc|gdbc.gz|nc)
#		raster2pgsql "${TEMPFILE}" "${SCHEMA}"."${TBLNAME}_Raster" |\
#		sed "s:$(echo "${SCHEMA}"  | tr "[A-Z]" "[a-z]"):${SCHEMA}:g"  |\
#		sed "s:$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]"):${TBLNAME}:g" |\
#		psql "${DBNAME}"
	;;
esac