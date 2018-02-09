#!/bin/bash

if [ "${4}" == "" ]
then
	echo "Usage ${0##*/} [rgisfile] [dbname] [schema] [tblname]"
	exit 1
fi
       RGISFILE="${1}"; shift
         DBNAME="${1}"; shift
         SCHEMA="${1}"; shift
        TBLNAME="${1}"; shift

TEMPFILE="TEMP${RGISFILE##*/}"
TEMPFILE="${TEMPFILE%.gdb*}.txt"

rgis2sql -a "DBItems" "${RGISFILE}" -s "${SCHEMA}" -q "${TBLNAME}" | psql "${DBNAME}"
rgis2ascii "${RGISFILE}" "${TEMPFILE}"


ogr2ogr -f PostgreSQL "PG: dbname=${DBNAME}" -a_srs EPSG:4326 -nln ${SCHEMA}.${TBLNAME}_geom "${TEMPFILE}"
echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;
UPDATE \"${SCHEMA}\".\"${TBLNAME}\"
      SET \"geom\" = \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\".\"wkb_geometry\"
      FROM   \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\"
      WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"ID\" =  \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\".\"ogc_fid\";
      DROP TABLE \"${SCHEMA}\".\"$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]")_geom\";" | psql "${DBNAME}"
rm "${TEMPFILE}"
