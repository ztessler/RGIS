#!/bin/bash

#!/bin/bash

[ "$1" == "" ]                    || INSTALLDIR="$1"

[    "${INSTALLDIR}" == "" ]      && INSTALLDIR="/usr/local/share/ghaas"

rm $(ls -1 CMlib/include/*.[h,H] | sed "s:CMlib:${INSTALLDIR}:")
rm $(ls -1 DBlib/include/*.[h,H] | sed "s:DBlib:${INSTALLDIR}:")
rm $(ls -1 MFlib/include/*.[h,H] | sed "s:MFlib:${INSTALLDIR}:")
rm $(ls -1 NClib/include/*.[h,H] | sed "s:NClib:${INSTALLDIR}:")
rm $(ls -1 RGlib/include/*.[h,H] | sed "s:RGlib:${INSTALLDIR}:")
rm $(ls -1 UIlib/include/*.[h,H] | sed "s:UIlib:${INSTALLDIR}:")

rm $(ls -1 CMlib/lib/*.a         | sed "s:CMlib:${INSTALLDIR}:")
rm $(ls -1 DBlib/lib/*.a         | sed "s:DBlib:${INSTALLDIR}:")
rm $(ls -1 MFlib/lib/*.a         | sed "s:MFlib:${INSTALLDIR}:")
rm $(ls -1 NClib/lib/*.a         | sed "s:NClib:${INSTALLDIR}:")
rm $(ls -1 RGlib/lib/*.a         | sed "s:RGlib:${INSTALLDIR}:")
rm $(ls -1 UIlib/lib/*.a         | sed "s:UIlib:${INSTALLDIR}:")

rm $(ls -1 rGIS/bin/*            | sed "s:rGIS:${INSTALLDIR}:")
rm $(ls -1 rCommands/bin/*       | sed "s:rCommands:${INSTALLDIR}:")
rm $(ls -1 rServer/bin/*         | sed "s:rServer:${INSTALLDIR}:")
rm $(ls -1 tfCommands/bin/*      | sed "s:tfCommands:${INSTALLDIR}:")

rm $(ls -1 XResources/bitmaps/*  | sed "s:XResources:${INSTALLDIR}/XResources:")
rm "${INSTALLDIR}/XResources/ghaas"
rm -r $(ls -1 html               | sed "s:^:${INSTALLDIR}/html/:")
rm $(ls -1 Messages/*.*          | sed "s:Messages:${INSTALLDIR}/Messages:")
rm $(ls -1 Scripts/*.sh          | sed "s:Scripts:${INSTALLDIR}/Scripts:")
rm $(ls -1 f/*                   | sed "s:f:${INSTALLDIR}/f:")

rm "${INSTALLDIR}/bin/rgis"

[ -e "${INSTALLDIR}/bin" ]                && rmdir    "${INSTALLDIR}/bin"
[ -e "${INSTALLDIR}/include" ]            && rmdir    "${INSTALLDIR}/include"
[ -e "${INSTALLDIR}/lib" ]                && rmdir    "${INSTALLDIR}/lib"
[ -e "${INSTALLDIR}/XResources/bitmaps" ] && rmdir -p "${INSTALLDIR}/XResources/bitmaps" 2> /dev/null
[ -e "${INSTALLDIR}/html" ]               && rmdir    "${INSTALLDIR}/html"
[ -e "${INSTALLDIR}/Messages" ]           && rmdir    "${INSTALLDIR}/Messages"
[ -e "${INSTALLDIR}/Scripts" ]            && rmdir    "${INSTALLDIR}/Scripts"
[ -e "${INSTALLDIR}/f" ]                  && rmdir    "${INSTALLDIR}/f"
[ -e "${INSTALLDIR}" ]                    && rmdir -p "${INSTALLDIR}" 2> /dev/null
