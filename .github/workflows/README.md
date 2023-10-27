# The _Update Documentation_ Workflow

This workflow, *Update Documentation*, runs Doxygen in various
configurations on the BioCro C++ Library source code and copies and
commits the result to the repository given as the value of the
"PUBLISH_TO" environment variable, currently
"ebimodeling/biocro-documentation".  That repository is, in turn, set
up to publish to a _GitHub Pages_ Web site at the corresponding
canonical location (currently,
https://ebimodeling.github.io/biocro-documentation/).

The workflow runs whenever changes to the C++ source files or the
files implementing this workflow are checked into the master branch.
It may also be run manually on the GitHub site.

In order for this to all work correctly, a number of set-up steps were
required:

1. An SSH public/private key pair was generated on a work station
using the command `ssh-keygen -t rsa -b 4096`.  (Use the `-C` option
to include a comment.  Use an empty pass-phrase) This key pair is used
in order to allow a workflow defined in _this_ repository to push
files to a _different_ repository (namely,
"ebimodeling/biocro-documentation"); see steps 2 and 6 below.

2. The private key was added as a _secret_ in the
`ebimodeling/biocro` repository (*this* repository) under the key
`PRIVATE_SSH_KEY`.  (See
https://docs.github.com/en/actions/reference/encrypted-secrets#creating-encrypted-secrets-for-a-repository.
The key name matches the reference `secrets.PRIVATE_SSH_KEY` used in
the `document.yml` workflow file.)

3. The "ebimodeling/biocro-documentation" repository was created.

4. A README.md file was added to the top-level directory of this new
repository with (relative) links pointing to the (prospective)
locations of various versions of the documentation.

5. _GitHub Pages_ was enabled for the
"ebimodeling/biocro-documentation" repository (see
https://pages.github.com/).  This results in the files in this
repository getting automatically published as a Web site to the URL
https://ebimodeling.github.io/biocro-documentation/.

6. The *public* SSH key was added as a deploy key to the
"ebimodeling/biocro-documentation" repository under the name "Access
from ebimodeling/biocro actions".  (See
https://docs.github.com/en/developers/overview/managing-deploy-keys#setup-2.
The name "Access from biocro action" is for informational purposes
only and has no programmatic significance.)

