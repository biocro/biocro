#!

# Go to the package root directory:
pushd ../..



# Make a new index.md file for pkgdown's home page.  This will include
# information about the Git version used to generated the
# documentation followed by the top-level README page:

git show -s --format="This document was generated from the version of BioCro specified as follows:%n%n- Commit Hash: %h%n- Date: %aD" > index.md
echo -n "- Branch: " >> index.md
git branch --show-current >> index.md

if [[ `git diff | wc -c` -ne 0 ]]; then echo; printf "\n%s" "_Some files have been altered from the given commit version._" >> index.md; fi


# Add a blank line to end the list:
echo >> index.md


# Copy the top-level README file without the badge image:
sed 's/ \[!\[Build Status.*//' README.md >> index.md


# Now run pkgdown:
Rscript -e "pkgdown::build_site()"


