#!/bin/bash

install_dir="${1}"

if [ "${install_dir}" == "" ]
then
	echo "Usage: ${0##*/} <install_path>" 
	exit 0
fi

if ! [ -e "${install_dir}" ]
then
	echo "Install path ${install_dir} does not exists!"
	exit 1;
fi

  build_dir=~/.TEMPghaas_build
 source_dir="${0%/*}"
[ "${source_dir}" == "." ] && source_dir=$(pwd)

mkdir ${build_dir}
cd    ${build_dir}
 cmake -DCMAKE_INSTALL_PREFIX="${install_dir}" "${source_dir}"
 make install
[ -e  ${install_dir}/ghaas/bin/rgis ] || ln -s ${install_dir}/ghaas/Scripts/rgis.sh ${install_dir}/ghaas/bin/rgis
rm -rf "${build_dir}"
