This workflow, *Update Documentation*, runs Doxygen in various
configurations on the BioCro C++ Library source code and copies and
commits the result to the repository given as the value of the
"publish-to" environment variable, currently
"ebimodeling/biocro-documentation".  That repository is, in turn, set
up to publish to GitHub pages at the corresponding canonical location
(currently, https://ebimodeling.github.io/biocro-documentation/).

In order for this to all work correctly, a number of set-up steps were
required.

1. An SSH public/private key pair was generated on a work station
using the command `ssh-keygen -t rsa -b 4096`.  (Use the `-C` option
to include a comment.  Use an empty pass-phrase)

2. The private key was added as a secret in the
`ebimodeling/biocro-dev` repository (*this* repository) under the key
`PRIVATE_SSH_KEY`.  (See
https://docs.github.com/en/actions/reference/encrypted-secrets#creating-encrypted-secrets-for-a-repository.
The key name matches the reference `secrets.PRIVATE_SSH_KEY` used in
the `document.yml` workflow file.)

3. The "ebimodeling/biocro-documentation" repository was created.

4. A README.md file was created in this new repository with (relative)
links pointing to the (prospective) locations of various versions of
the Doxygen documentation.

5. GitHub pages was enabled for the "ebimodeling/biocro-documentation"
repository.

6. The *public* SSH key was added as a deploy key to the
"ebimodeling/biocro-documentation" repository under the name "Access
from biocro-dev action".  (See
https://docs.github.com/en/developers/overview/managing-deploy-keys#setup-2.
The name "Access from biocro-dev action" is for informational purposes
only and has no programmatic significance.)

