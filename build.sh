#!/bin/bash
#-------------------------------------------------------------------------------
# Copyright (c) 2012 University of Illinois, NCSA.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the
# University of Illinois/NCSA Open Source License
# which accompanies this distribution, and is available at
# http://opensource.ncsa.illinois.edu/license.html
#-------------------------------------------------------------------------------

EMAIL="dlebauer@gmail.com"

# location where to install package
if [ ! -z $R_LIBS_USER ]; then
    if [ ! -e ${R_LIBS_USER} ]; then mkdir -p ${R_LIBS_USER}; fi
    rm -rf ${R_LIBS_USER}/PEcAn.*
    R_LIB_INC="--library=${R_LIBS_USER}"
else
    echo "R_LIBS_USER not set"
    echo "add the following line to your ~/.bashrc:"
    echo "export R_LIBS_USER=${HOME}/R/library"
fi

# pull any changes
git pull > changes.log
if [ $? != 0 ]; then
    echo Error pulling > changes.log
    exit
fi

START=`date +'%s'`
STATUS="OK"

# get changes
echo "----------------------------------------------------------------------" >> changes.log
echo "CHANGES" >> changes.log
echo "----------------------------------------------------------------------" >> changes.log
git log > newlog
diff git.log newlog | grep '^> ' | sed 's/^> //' > changes.log
mv newlog git.log
REVNO=$( git show -s --pretty=format:%T master )

## check/install package

R CMD check --install-args="--resave-data" ${R_LIB_INC} ../biocro &> out.log

if [ $? -ne 0 ]; then
    STATUS="BROKEN"
    echo "----------------------------------------------------------------------" >> changes.log
    echo "CHECK BioCro BROKEN" >> changes.log
    echo "----------------------------------------------------------------------" >> changes.log
    cat out.log >> changes.log
fi

## all done
TIME=$(echo "`date +'%s'` - $START" |bc -l)
echo "build took ${TIME} seconds." >> changes.log

echo "-------------------------------------------------------" >> changes.log
echo "results of 00install.out">> changes.log
echo "-------------------------------------------------------" >> changes.log

cat biocro.Rcheck/00install.out>> changes.log
echo "-------------------------------------------------------" >> changes.log

echo "contents of 00check.log" >> changes.log
echo "-------------------------------------------------------" >> changes.log
cat  biocro.Rcheck/00check.log >> changes.log
echo "-------------------------------------------------------" >> changes.log

cat changes.log
if [ "$STATUS" == "BROKEN" ]; then
    cat changes.log | mail -s "BioCro BUILD ${REVNO} is BROKEN" ${EMAIL}
fi


