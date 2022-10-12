# Container image that runs your code

# Alpine Linux version 3.16 contains Doxygen version 1.9.4-r0.  See
# https://pkgs.alpinelinux.org/packages for details about Alpine
# packages.

FROM alpine:3.16

# Copies your code file from your action repository to the filesystem path `/` of the container
COPY entrypoint.sh /entrypoint.sh

# Install required packages
RUN apk add doxygen graphviz ttf-freefont make git

# Code file to execute when the docker container starts up (`entrypoint.sh`)
ENTRYPOINT ["/entrypoint.sh"]
