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

R CMD check ${R_LIB_INC} . &> check.log
R CMD INSTALL ${R_LIB_INC} . &> install.log
echo "devtools::test()" | R --vanilla &> devtools.log

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
The output of R CMD INSTALL
*******************************************************
""$(cat install.log)" \
"-------------------------------------------------------

*******************************************************
The output of R CMD check
*******************************************************
""$(cat check.log)" \
"-------------------------------------------------------

*******************************************************
The output of devtools
*******************************************************
""$(cat devtools.log)" \
"-------------------------------------------------------
"

exec 1>&6 6>&-  # Make stdout point to whatever it had originally pointed to and close file descriptor 6.

rm check.log install.log devtools.log

if [ `grep failed changes.log` ]; then
	REVNO=$( git show -s --pretty=format:%T master )
    # cat changes.log | mail -s "BioCro BUILD ${REVNO} is BROKEN" ${EMAIL}
    echo "ERROR: THE BioCro BUILD IS BROKEN" >&2
    exit 1
fi

