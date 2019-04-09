FROM rocker/geospatial

RUN apt-get update &&\
    apt-get install -y --no-install-recommends wget \
    icu-devtools \
    libicu-dev \
    libxml2-dev &&\
    mkdir /work &&\
    mkdir /data &&\
    cd /work &&\
    wget https://github.com/ebimodeling/biocro/archive/master.zip &&\
    unzip master.zip &&\
    R CMD INSTALL biocro-master &&\
    install2.r --error XML \
    lubridate \
    dplyr \
    data.table

COPY biocro.Rscript /work

COPY biocro_helpers.R /work

ENTRYPOINT ["Rscript", "/work/biocro.Rscript"]
