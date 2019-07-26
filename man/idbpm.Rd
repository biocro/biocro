\name{idbpm}
\alias{idbpm}
\title{idbpm}
\usage{
    idbpm(data, MaizePhenoControl = list())
}
\arguments{
    \item{data}{as data frame, e.g.:
        \preformatted{
            ThermalT     Stem    Leaf    Root Grain     LAI
            1       0.211  0.00733 0.00104 0.00704     0 0.00119
            611   280.000  1.08019 0.95531 0.11618     0 1.62350
            1221  560.000  5.91862 2.08684 1.42061     0 3.54748
            1831  747.000 10.16707 2.36378 2.53192     0 4.01843
            2442  969.000 15.08485 2.42849 3.57410     0 4.12843
            3052 1080.000 18.56392 2.46765 4.32115     0 4.19501
            3662 1136.000 20.87121 2.04021 4.82178     0 3.46836
            4273 1452.000 22.05770 0.89954 5.20210     0 1.52921
            }}

    \item{MaizePhenoControl}{}
}
\value{
    Vector of biomass pools.
}
\description{
    This estimates initial dry biomass partitioning coefficients based on data for an annual grass
}
\author{
    Fernando E. Miguez
}
