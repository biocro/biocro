#!

# Go to the package root directory:
pushd ../..



# Make an about page that shows information about the Git version used
# to generated the pkgdown documentation:
echo "# About this documentation" > about_pkgdown_documentation.md
git show -s --format="This document was generated from the version of BioCro specified as follows:%n%n- Commit Hash: %h%n- Commit Date: %aD" >> about_pkgdown_documentation.md
echo -n "- Branch: " >> about_pkgdown_documentation.md
git branch --show-current >> about_pkgdown_documentation.md
echo -n "- Generation Date and Time: " >> about_pkgdown_documentation.md
date >> about_pkgdown_documentation.md
if [[ `git diff | wc -c` -ne 0 ]]; then echo; printf "\n%s" "_Some files have been altered from the given commit version._" >> about_pkgdown_documentation.md; fi


# Make a new index.md file for pkgdown's home page that strips the
# badge image from README.md.
sed 's/ \[!\[Build Status.*//' README.md > index.md


# Now run pkgdown:
Rscript -e "pkgdown::build_site()"


