# Container image that runs your code

# alpine 3.12 Contains Doxygen version 1.8.18 instead of buggy 1.8.19.
# See https://github.com/doxygen/doxygen/issues/7975.
FROM alpine:3.12

# Copies your code file from your action repository to the filesystem path `/` of the container
COPY entrypoint.sh /entrypoint.sh

# Install required packages
RUN apk add doxygen graphviz ttf-freefont make

# Code file to execute when the docker container starts up (`entrypoint.sh`)
ENTRYPOINT ["/entrypoint.sh"]