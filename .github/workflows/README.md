# The _Generate Documentation_ Workflow

The *Generate Documentation* workflow, contained in file
`document.yml`, builds the BioCro User's Manual, the BioCro
Developer's Manual, and the Doxygen BioCro C++ Library documentation
and copies and commits the result to the repository given as the value
of the "PUBLISH_TO" environment variable (henceforth the
_documentation repository_; see the value contained in
`document.yml`).  That repository is in turn set up to publish to a
_GitHub Pages_ Web site at the corresponding canonical location.  (In
general, a repository named `<organization>/<repository>` will be
published to the URL `https://<organization>.github.io/<repository>`.)

The workflow runs

* whenever a new commit is pushed to the `main` branch;
* whenever a new version tag is applied;
* whenever a new pull request is made or altered.

(In the latter case, the documentation is not published but is made
available as an artifact for download.)  It may also be run manually
on the GitHub site.

In order for this to all work correctly, a number of set-up steps were
required.  Here are the steps involved:

1. An SSH public/private key pair is generated on a work station using
the command `ssh-keygen -t rsa -b 4096`.  Use the `-C` option to
include a comment; for example

    ```
    ssh-keygen -t rsa -b 4096 \
        -C "Let biocro/biocro deploy to biocro/BioCro-documentation"
    ```

    You will be prompted for a file name for the key, and then for a
pass-phrase.  The pass-phrase should be empty.

    This will generate a pair of files, one with the public key and
one with the private key.  For example, if you choose `biocro-key` as
the name of the file, this will be the name of the file containing the
private key, and the public key will be in a file called
`biocro-key.pub`.

    This key pair is needed in order to allow a workflow defined in
the BioCro repository to push files to the documentation repository.

2. The private key must be added as an _Actions secret_ in the BioCro
repository (*this* repository) under the name `PRIVATE_SSH_KEY`.  When
doing this, paste the entire contents of the private key file into
text area labeled **Secret**.  (See
https://docs.github.com/en/actions/reference/encrypted-secrets#creating-encrypted-secrets-for-a-repository.
The key name matches the reference `secrets.PRIVATE_SSH_KEY` used in
the `document.yml` workflow file.)

3. Create the documentation repository.  Even though the _Generate
Documentation_ workflow will populate this repository, at least one
file must be added to it to begin with in order for the workflow to
run.  (Suggestion: Add a README.md file when creating the repository;
it can be blank.)

4. Add the *public* SSH key as a deploy key to the documentation
repository under a suitable title; for example, "Access from
biocro/biocro documentation workflow" (assuming "biocro/biocro" is the
name of _this_ repository).  When doing this, paste the entire
contents of the public key file into the text area labeled **Key**.
(See
https://docs.github.com/en/developers/overview/managing-deploy-keys#setup-2.
The title "Access from biocro/biocro documentation workflow" is for
informational purposes only and has no programmatic significance.)

5. Enable _GitHub Pages_ for the documentation repository (see
https://pages.github.com/).  This results in the files in the
documentation repository getting automatically published to the
corresponding GitHub Pages web site.
