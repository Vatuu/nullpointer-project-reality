#!/bin/bash

optstring="i:g:f:s:m:"

while getopts ${optstring} arg; do
    case ${arg} in
        g)
            inputfile="${OPTARG}"
            ;;
        i)
            outputfile="${OPTARG}"
            ;;
        f)
            format="${OPTARG}"
            ;;
        s)
            size="${OPTARG}"
            ;;
        m)
            mapfile="${OPTARG}"
            ;;
    esac
done

format_full="${format}${size}"
dimensions=$(identify -format '%G' ${inputfile})

echo "${outputfile} ${format}/${size} ${dimensions}\n" >> ${mapfile}

n64graphics -i ${outputfile} -g ${inputfile} -f ${format_full}

exit 0