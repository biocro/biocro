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
CHANGES_FILE="changes.log"

# Remove the previous installation to avoid problems.
rm -rf ${R_LIBS_USER}/BioCro
R_LIB_INC="--library=${R_LIBS_USER}"

START=`date +'%s'`

R CMD check ${R_LIB_INC} . &> out.log
R CMD INSTALL ${R_LIB_INC} . >> out.log 2>&1
echo "devtools::test()" | R --vanilla >> out.log 2>&1

exec 6>&1  # Store the location of stdout so it can be restored later.
exec &> $CHANGES_FILE  # Change stdout to point to the file where changes will be logged.

git log > newlog
diff git.log newlog | grep '^> ' | sed 's/^> //'
mv newlog git.log

TIME=$(echo "`date +'%s'` - $START" |bc -l)

printf "\n%s\n" \
"-------------------------------------------------------
CHANGES
-------------------------------------------------------
The build took ${TIME} seconds.

*******************************************************
The contents of 00install.out
*******************************************************
""$(cat BioCro.Rcheck/00install.out)" \
"-------------------------------------------------------

*******************************************************
The contents of 00check.log
*******************************************************
""$(cat BioCro.Rcheck/00check.log)" \
"-------------------------------------------------------
"

exec 1>&6  # Make stdout point to whatever it had originally pointed to.

rm BioCro.Rcheck/00check.log BioCro.Rcheck/00install.out

cat changes.log

if [ `grep failed changes.log` ]; then
	REVNO=$( git show -s --pretty=format:%T master )
    # cat changes.log | mail -s "BioCro BUILD ${REVNO} is BROKEN" ${EMAIL}
    echo "ERROR: THE BioCro IS BUILD BROKEN" >&2
    exit 1
fi

