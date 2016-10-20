#!/bin/bash

usage() { echo "Flasher: $0 [-d <coldchain>] [-p <PORT>]" 1>&2; exit 1; }

while getopts ":d:p:" o; do
    case "${o}" in
        d)
            d=${OPTARG}
            ;;
        p)
            p=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${d}" ] || [ -z "${p}" ]; then
    usage
fi

if [ "${d}" != "coldchain" ]; then
    usage
fi

if [ "${d}" == "coldchain" ]; then
  myfile="coldchain-thingsio-llsec-cmdtopic-ch26-nullrdc.bin"
fi

echo "Flashing ${p} port with ${myfile} binary"

python cc2538-bsl.py -e -w -v -p ${p} -a 0x202000 "${myfile}"
