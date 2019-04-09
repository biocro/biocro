# BioCro Docker

## Build image

Step 1: Clone my forked biocro repository `git clone https://github.com/kimberlyh66/biocro.git`

Step 2: cd to folder containing Dockerfile `cd biocro/Docker`

Step 3: Build docker image `docker build -t biocro-docker .`

## Setup input / output folders for BioCro run

Step 1: Create a folder named `biocro_run`

Step 2: cd to folder `cd biocro_run`

Step 3: Create a rundir folder (contains input data for BioCro) `mkdir rundir`

Step 4: Create a outdir folder (location where BioCro output is saved) `mkdir outdir`

## Create input files for BioCro run

_The following files should be saved in `biocro_run/rundir` as described above_

Step 1: Create a met data file. An example of how this data file should be formated can be found in the `Docker/example_data` folder of this repository. 

* This file should be named in the format `met.yyyy.csv` where `yyyy` is a year. Example: `met.2013.csv`

Step 2: Create a config xml file. An example of how this data file should be formated can be found in the `Docker/example_data` folder of this repository. 

* This file should be named `config.xml`

* The value set within the `<met.path>` tag should be `/data/rundir/met`. 

## Run container using example data
 
Step 1: Copy `config.xml` and `met.2013.csv` from the `Docker/example_data` folder of this repository into the `biocro_run/rundir` folder that you set up for the BioCro run (shown in above section `Setup input / output folders for BioCro run`)

Step 2: cd into `biocro_run`

Step 3: Run docker container using `docker run -v /$(PWD):/data biocro-docker /data/rundir /data/outdir`

Step 4: Once the container finishes running, cd into the output directory `cd outdir`

Step 5: If successful, you should see the following outputs: `biocro_output.RData` and `daily.result.RData` 
