# Doxygen Docker Action with Makefile

This action is a customization of the code found at
https://github.com/mattnotmitt/doxygen-action.  Instead of calling
Doxygen directly, it assumes that there is a Make file in the working
directory that invokes Doxygen to build the documentation.

## Inputs

### 'working-directory'

**Required** Path of the working directory to change to before running
  Make.  This should be the location of the Make file used with
  Doxygen.

### 'color'

**Optional** The color setting (given as an angle from 0 to 360 in the
  [HSL/HSV color space](https://en.wikipedia.org/wiki/HSL_and_HSV)).
  Default: 143 (greenish)

### 'document-private'

YES to document private class members, NO to document only the public
and protected ones.  Default: YES

### 'extra-settings'

A space-separated set of settings of the form *key=value*.  Only keys
recognized by the make file will have any effect.  Example:
`module_docs_directory=doxygen_docs_modules_public_members_only` will
change the output directory for module-docs-* targets from
`doxygen_docs_modules` to
`doxygen_docs_modules_public_members_only`. Default: '' (blank)

### 'makefile-target'

**Optional** The name of the Make target to use.  Default: '' (none)
