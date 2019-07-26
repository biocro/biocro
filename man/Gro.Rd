\name{Gro}
\alias{Gro}
\title{Model crop growth.}
\usage{
    Gro(initial_values, parameters, varying_parameters, modules)
}
\arguments{
    \item{initial_values}{Parameter values that are known only at the start of the simulation period. These are recorded in the result. (a list of named numeric values)}

    \item{parameters}{Parameter values that are constant for the entire simulation period. These are not recorded in the result. (a list of named numeric values)}

    \item{varying_parameters}{Times at which a solution is desired and parameter values that are known \emph{a priori} for the entire simulation period. These are not recorded in the result. (a data frame with at least the columns \code{year}, \code{doy}, and \code{hour} and optionally columns for parameters that are known \emph{a priori}.)}

    \item{modules}{The model modules that should be run. Required modules are given in the details. (a list of named character values)}
}
\value{
    A data frame with the columns \code{year}, \code{doy}, and \code{hour}; and a column for each parameter in \code{initial_values}; and a row for each row in \code{varying_parameters}:
}
\description{
    \code{Gro()} models crop growth over time given species-specific parameters and climate data. It can be used to predict yield, determine regions that are suitable for growing a crop, and identify traits that have the most influence on yield.
}
\details{
    \subsection{An overview of model definitions}{
        A model can be fully defined using two types of arguments: (1) the set of equations and (2) the parameters required by those equations. The set of all parameters in the model is commonly called the state.

        State values can be known, \emph{a priori}, for the entire simulation (\emph{a priori} state), or known only at the start of the simulation period (calculated state).
    }

    \subsection{Model implementation using Gro()}{
        In Gro(), the sets of equations are passed via the modules argument, and the state is passed via the \code{initial_values}, \code{parameters}, and \code{varying_parameters} arguments.

        The set of modules determines the parameters that must be defined in the state arguments. Each module has parameters it requires, which must be defined in exactly one of the state arguments

        The state argument in which a parameter is defined determines whether a parameter is considered calculated state or \emph{a priori} state. Parameters defined in \code{initial_values} are considered calculated state, whereas parameters defined in either \code{parameters} or \code{varying_parameters} are considered \emph{a priori} state. The difference between \code{parameters} and \code{varying_parameters} is that values in \code{parameters} are considered constant for the simulation period, whereas values in \code{varying_parameters} may be different for each time point. Only calculated state variables are recorded in the output. A priori state variable values will overwrite any calculations of those values performed internally by the model.
    }


    \subsection{State argument details}{
        \code{initial_values}: Parameter values that are known only at the start of the simulation period. Parameter values are recorded in the output. If the model calculates the parameter value, it will change with time. If the model does not calculate the value, it will be constant with time, but it will still be recorded.

        \code{parameters}: Parameter values that are known and constant for the entire simulation period. Values are not recorded in the output. If the model calculates the parameter value, the calculated value will be overwritten by the value in \code{parameters}.

        \code{varying_parameters}: Parameter values that are known \emph{a priori} at every time step in the entire simulation period. Values are not recorded in the output. If the model calculates the parameter value, the calculated value will be overwritten by the value in \code{varying_parameters}.

        The data frame \code{varying_parameters} must contain at least the columns \code{year}, \code{doy}, and \code{hour}, specifying the times at which a solution is required. Times must be evenly spaced, and the difference in times must be given in the \code{timestep} parameter (units are hours).
    }

    \subsection{Required module names and available modules}{
        \itemize{
            \item \code{canopy_module_name}
            \itemize{
                \item c3canopy, c4canopy
            }
            \item \code{soil_module_name}
            \itemize{
                \item one_layer_soil_profile, two_layer_soil_profile
            }
            \item \code{growth_module_name}
            \itemize{
                \item partitioning_growth
            }
            \item \code{senescence_module_name}
            \itemize{
                \item thermal_time_senescence, thermal_time_and_frost_senescence
            }
        }
    }

    \subsection{Precompiled module and parameter sets}{
        Sets of parameters and modules are provided for sorghum, miscanthus, and willow, and are named [crop]_initial_state, [crop]_parameters, and [crop]_modules, e.g., sorghum_initial_state.

        Weather data are provided. These are typically for one year (January 1 to December 31) and should be subsetted to include only the period of growth. The function \code{get_growing_season_climate()} is provided as one means of subsetting climate data.
    }
}
\examples{
## Simulate sorghum growth
result = Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_modules)
lattice::xyplot(Stem + Leaf + Root ~ TTc, data=result, type='l', auto=list(points=FALSE, lines=TRUE), ylab=expression('Biomass'~(Mg/ha)), xlab=expression('Thermal time'~(degree*C~day)))

### The results can be combined with the data that were used to drive the model.
result = cbind(result, get_growing_season_climate(weather05))

## Change a subset of the parameters.
canopy_architecture = within(sorghum_parameters, {chil = 1.2; leafwidth = 0.05})
architechture_result = Gro(sorghum_initial_state, canopy_architecture, get_growing_season_climate(weather05), sorghum_modules)
lattice::xyplot(Stem + Leaf + Root ~ TTc, data=architechture_result, type='l', auto=list(points=FALSE, lines=TRUE), ylab=expression('Biomass'~(Mg/ha)), xlab=expression('Thermal time'~(degree*C~day)))

}
