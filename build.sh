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
rm -rf ${R_LIBS_USER}/BioCro
R_LIB_INC="--library=${R_LIBS_USER}"

START=`date +'%s'`
STATUS="OK"

# get changes
echo "----------------------------------------------------------------------" > changes.log
echo "CHANGES" >> changes.log
echo "----------------------------------------------------------------------" >> changes.log

git log > newlog
diff git.log newlog | grep '^> ' | sed 's/^> //' > changes.log
mv newlog git.log
REVNO=$( git show -s --pretty=format:%T master )

## check/install package

R CMD check ${R_LIB_INC} ../biocro &> out.log

echo "devtools::test()" | R --vanilla &>out.log

## all done
TIME=$(echo "`date +'%s'` - $START" |bc -l)
echo "build took ${TIME} seconds." >> changes.log

echo "-------------------------------------------------------" >> changes.log
echo "results of 00install.out">> changes.log
echo "-------------------------------------------------------" >> changes.log

cat biocro.Rcheck/00install.out>> changes.log
echo "-------------------------------------------------------" >> changes.log
rm biocro.Rcheck/00install.out

echo "contents of 00check.log" >> changes.log
echo "-------------------------------------------------------" >> changes.log
cat  biocro.Rcheck/00check.log >> changes.log
echo "-------------------------------------------------------" >> changes.log
rm biocro.Rcheck/00check.log

cat changes.log
if [ `grep failed changes.log` ]; then
    # cat changes.log | mail -s "BioCro BUILD ${REVNO} is BROKEN" ${EMAIL}
    echo "ERROR BioCro BUILD BROKEN" >&2
    exit 1

fi


