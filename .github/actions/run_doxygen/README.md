# The _Create All Doxygen Versions_ action

This action builds four different version of the Doxygen documentation
for the BioCro package:

* The complete documentation of all of the BioCro C++ code, including
  documentation of both public and private class members.

* The documentation of only code for the BioCro modules

* The documentation of the BioCro framework (basically, everything but
  the modules)

* Documentation of only public members of the BioCro module classes

## Preconditions

* The BioCro repository must be checked out to the GitHub workspace

This action will take care of installing everything Doxygen needs to
run (including Doxygen itself).

## Inputs

### 'package_root'

This should be the relative path from the GitHub workspace directory
to the root directory of the BioCro source tree.  Default: '.'

Note that this value really can not be anything other than the default
'.'.  This is because the
"uses: ./.github/actions/doxygen-action" settings assume that the
package root is identical to the GitHub workspace directory.

(The "uses" value can not, as of this writing, be set using a context
expression such as

    "${{ inputs.package_root }}/.github/actions/doxygen-action".

We use inputs.package_root in this workflow rather than a hard-coded
value to serve as a form of documentation.  And if the GitHub actions
syntax ever *does* change so as to allow variables in "uses" values,
it will then be easier to modify this action to one that really does
allow a range of "pacakge_root" input values.)
