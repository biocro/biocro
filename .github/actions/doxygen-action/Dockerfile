# Container image that runs your code

# Alpine Linux version 3.13 contains Doxygen version 1.9.1 instead of the buggy version 1.8.19.
# See https://github.com/doxygen/doxygen/issues/7975.
FROM alpine:3.13

# Copies your code file from your action repository to the filesystem path `/` of the container
COPY entrypoint.sh /entrypoint.sh

# Install required packages
RUN apk add doxygen graphviz ttf-freefont make git

# Code file to execute when the docker container starts up (`entrypoint.sh`)
ENTRYPOINT ["/entrypoint.sh"]
