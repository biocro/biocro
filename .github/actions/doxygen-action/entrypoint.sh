#!/bin/sh

if [ $# -lt 1 ]; then
    echo "At least one argument is required"
    exit
fi

if [ ! -d "$1" ]; then
    echo "Path $1 could not be found!"
    exit
fi
cd "$1"


make color=$2 extract_private=$3 generate_treeview=$4 $5 $6

