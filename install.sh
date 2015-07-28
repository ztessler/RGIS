#!/bin/bash

[ "$1" == "" ]                    || INSTALLDIR="$1"

[    "${INSTALLDIR}" == "" ]      && INSTALLDIR="/usr/local/share/ghaas"

[ -e "${INSTALLDIR}/bin" ]                || mkdir -p "${INSTALLDIR}/bin"
[ -e "${INSTALLDIR}/include" ]            || mkdir -p "${INSTALLDIR}/include"
[ -e "${INSTALLDIR}/lib" ]                || mkdir -p "${INSTALLDIR}/lib"
[ -e "${INSTALLDIR}/XResources" ]         || mkdir -p "${INSTALLDIR}/XResources"
[ -e "${INSTALLDIR}/XResources/bitmpas" ] || mkdir -p "${INSTALLDIR}/XResources/bitmaps"
[ -e "${INSTALLDIR}/html" ]               || mkdir -p "${INSTALLDIR}/html"
[ -e "${INSTALLDIR}/Messages" ]           || mkdir -p "${INSTALLDIR}/Messages"
[ -e "${INSTALLDIR}/Scripts" ]            || mkdir -p "${INSTALLDIR}/Scripts"
[ -e "${INSTALLDIR}/f" ]                  || mkdir -p "${INSTALLDIR}/f"

cp CMlib/include/*.[h,H] "${INSTALLDIR}/include/"
cp DBlib/include/*.[h,H] "${INSTALLDIR}/include/"
cp MFlib/include/*.[h,H] "${INSTALLDIR}/include/"
cp NClib/include/*.[h,H] "${INSTALLDIR}/include/"
cp RGlib/include/*.[h,H] "${INSTALLDIR}/include/"
cp UIlib/include/*.[h,H] "${INSTALLDIR}/include/"

cp CMlib/lib/*.a         "${INSTALLDIR}/lib/"
cp DBlib/lib/*.a         "${INSTALLDIR}/lib/"
cp MFlib/lib/*.a         "${INSTALLDIR}/lib/"
cp NClib/lib/*.a         "${INSTALLDIR}/lib/"
cp RGlib/lib/*.a         "${INSTALLDIR}/lib/"
cp UIlib/lib/*.a         "${INSTALLDIR}/lib/"

cp rGIS/bin/*            "${INSTALLDIR}/bin/"
cp rCommands/bin/*       "${INSTALLDIR}/bin/"
cp rServer/bin/*         "${INSTALLDIR}/bin/"
cp tfCommands/bin/*      "${INSTALLDIR}/bin/"

cp XResources/bitmaps/*  "${INSTALLDIR}/XResources/bitmaps/"
cp XResources/ghaas      "${INSTALLDIR}/XResources/"
cp -rp html/*            "${INSTALLDIR}/html/"
cp Messages/*.*          "${INSTALLDIR}/Messages/"
cp Scripts/*.sh          "${INSTALLDIR}/Scripts/"
cp f/*                   "${INSTALLDIR}/f/"
chmod -R ugo+rX          "${INSTALLDIR}"

[ -e "${INSTALLDIR}/bin/rgis" ] || ln -s "${INSTALLDIR}/Scripts/rgis.sh" "${INSTALLDIR}/bin/rgis"