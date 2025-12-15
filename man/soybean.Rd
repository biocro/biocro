\name{soybean}

\docType{data}

\alias{soybean}

\title{Soybean-BioCro model definition}

\description{
  Initial values, parameters, direct modules, differential modules, and
  a differential equation solver that can be used to run soybean growth
  simulations in Champaign, Illinois and other locations. Along with the soybean
  circadian clock specifications (\code{\link{soybean_clock}}), these values
  define the soybean growth model of Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}], which is commonly referred to as
  \emph{Soybean-BioCro}.

  To represent soybean growth in Champaign, IL, these values must be paired with
  the Champaign weather data (\code{\link{cmi_soybean_weather_data}}). This
  weather data includes the output from the soybean circadian clock model
  (\code{\link{soybean_clock}}), so the clock components do not need to be
  included when running a soybean growth simulation using this weather data.
  The parameters already include the \code{clay_loam} values from the
  \code{\link{soil_parameters}} dataset, which is the appropriate soil type for
  Champaign.

  Some specifications, such as the values of photosynthetic parameters, would
  remain the same in any location; others, such as the latitude or longitude,
  would need to change when simulating crop growth in different locations. Care
  must be taken to understand each input quantity before attempting to run
  simulations in other places or for other cultivars.
}

\usage{soybean}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\details{
  As improvements are made to the BioCro modules, their behavior changes, and
  the soybean model parameters must be updated. Following significant module
  updates, reparameterization is performed using a script that is included with
  the BioCro package; its location can be found by typing
  \code{system.file('extdata', 'parameterize_soybean.R', package = 'BioCro')} in
  an R session. The parameterization script generally uses the same method and
  data as used in Matthews \emph{et al.} (2022), with a few differences, such as
  the separation of pod mass into separate seed and shell components.

  The following is a summary of reparameterizations that have occurred since the
  original publication of the Soybean-BioCro model:

  \itemize{
    \item \emph{2023-06-18}: Several modules have been updated, and the value of
    the atmospheric transmittance has been changed from 0.85 to 0.6 based on
    Campbell and Norman, An Introduction to Environmental Biophysics,
    2nd Edition, Pg 173. Due to these changes, reparameterization of the
    following was required: \code{alphaLeaf}, \code{alphaRoot},
    \code{alphaStem}, \code{alphaShell}, \code{betaLeaf}, \code{betaRoot},
    \code{betaStem}, \code{betaShell}, \code{rateSeneLeaf}, \code{rateSeneStem},
    \code{alphaSeneLeaf}, \code{betaSeneLeaf}, \code{alphaSeneStem}, and
    \code{betaSeneStem}.

    \item \emph{2023-03-15}: Several modules have been updated. The most
    significant changes are that (1) the
    \code{BioCro:no_leaf_resp_neg_assim_partitioning_growth_calculator} now
    reduces the leaf growth rate in response to water stress and (2) the
    partitioning modules now include a new tissue type (\code{shell}). The new
    component allows us to distinguish between components of the soybean pod,
    where \code{shell} represents the pericarp and \code{grain} represents the
    seed. This distinction has been found to be important for accurately
    predicting seed biomass, which is more important in agricultural settings
    than the entire pod mass, since the pericarp is not included in typical
    yield measurements. Due to these changes, reparameterization of the
    following was required: \code{alphaLeaf}, \code{alphaRoot},
    \code{alphaStem}, \code{alphaShell}, \code{betaLeaf}, \code{betaRoot},
    \code{betaStem}, \code{betaShell}, \code{rateSeneLeaf}, \code{rateSeneStem},
    \code{alphaSeneLeaf}, \code{betaSeneLeaf}, \code{alphaSeneStem}, and
    \code{betaSeneStem}. It was also necessary to add a new direct module to the
    model definition: \code{BioCro:leaf_water_stress_exponential}. This module
    calculates the fractional reduction in leaf growth rate due to water stress.

    \item \emph{2024-09-12}: Several changes have been made: (1) The
    \code{mrc1} and \code{mrc2} were renamed to \code{grc_stem} and
    \code{grc_root}, respectively. These two parameters are used to scale the
    assimilate rate, which is commonly called growth respiration coefficient
    (grc). (2) A new module, \code{BioCro:maintenance_respiration}, has been
    added to account for maintenance respiration during the biomass
    partitioning. This module removes a fraction from each organ by a constant
    parameter called \code{mrc_*} (e.g., \code{mrc_leaf}) and also by a
    temperature-dependent Q10 scaling factor. Among these \code{mrc_*}
    parameters, \code{mrc_leaf} and \code{mrc_stem} are set equal to represent
    maintenance respiration for the shoot, while \code{mrc_grain} is assigned a
    negligible value to prevent grain biomass reduction at the season end. No
    decreasing trends have been seen in the observed data. (3) Parameter
    optimizations against the 2002-2006 biomass datasets were performed to
    accommodate these changes.

    \item \emph{2025-04-23}: Several changes have been made since the previous
    update. Regarding modules, several new modules have been added:
    \code{BioCro:format_time}, \code{BioCro:sla_linear},
    \code{BioCro:carbon_assimilation_to_biomass}, and
    \code{BioCro:maintenance_respiration_calculator}. These modules do not
    change the model's overall behavior; rather, they make some new quantities
    available in the outputs, such as maintenance respiration rates. The
    \code{BioCro:format_time} module is related to a change in how BioCro
    defines time; previously it was defined in units of days, but now it is
    given in hours. The partitioning growth calculator module was also changed
    to \code{BioCro:partitioning_growth_calculator}, since the previous module
    was removed from the library; this module has identical behavior to the old
    one (\code{BioCro:no_leaf_resp_neg_assim_partitioning_growth_calculator}).
    Regarding parameters, several modules now require new input parameters whose
    values were previously hard-coded: \code{dry_biomass_per_carbon},
    \code{grc_leaf}, \code{grc_rhizome}, \code{grc_grain}, \code{grc_shell},
    \code{mrc_rhizome}, \code{mrc_shell}, \code{alphaRhizome},
    \code{betaRhizome}, \code{kRhizome_emr_DVI}, and several parameters related
    to the temperature dependence of FvCB model parameters (e.g. \code{Gstar_c}
    and \code{Gstar_Ea}). A few parameters have also been renamed to better
    indicate their meaning, such as \code{Rd} becoming \code{RL_at_25}. Finally,
    due to changes in the calculation of growth respiration rates, the model
    needed to be re-parameterized against the 2002-2006 biomass data sets. This
    resulted in changes to the values of \code{alphaLeaf}, \code{alphaStem},
    \code{betaLeaf}, \code{betaStem}, \code{alphaShell}, \code{betaShell},
    \code{grc_root}, \code{grc_stem}, \code{mrc_leaf}, \code{mrc_root},
    \code{mrc_stem}, \code{rateSeneLeaf}, \code{rateSeneStem},
    \code{alphaSeneLeaf}, \code{alphaSeneStem}, \code{betaSeneLeaf}, and
    \code{betaSeneStem}.

    \item \emph{2025-08-25}: The leaf boundary layer conductance model has been
    changed to a model described in Campbell & Norman (1998), causing small
    differences in the simulated soybean biomass and requiring a
    reparameterization. This update alters the values of the same parameters
    that were changed in the previous update on 2025-04-23.
  }

  Whenever a reparameterization is made, this list should be updated, and any
  vignettes using the soybean model should be checked to see if any axis limits,
  etc., need to change.
}

\source{
  This model is described in detail in Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}]. Here we make a few notes about some of
  its components.

  \strong{General Comments}

  \itemize{
    \item For historical reasons, the seed tissue in this model is called
          \code{Grain}. The entire pod biomass can be calculated by adding the
          \code{Grain} and \code{Shell} biomass.

    \item For historical reasons, this model includes a \code{Rhizome} tissue.
          Soybean does not have a rhizome, so the rhizome-related values in the
          model are chosen such that the rhizome begins with zero mass and does
          not grow or senesce. These values include the initial \code{Rhizome}
          and \code{RhizomeLitter}, and the following parameters:
          \code{alphaRhizome}, \code{betaRhizome}, \code{kRhizome_emr},
          \code{kRhizome_emr_DVI}, \code{grc_rhizome}, \code{mrc_rhizome},
          \code{rateSeneRhizome}, \code{alphaSeneRhizome},
          \code{betaSeneRhizome}, and \code{retrans_rhizome}.

    \item Some parameter values are specific to Champaign, Illinois (2002-2006)
          and may need to change for other years and locations: the soil type,
          \code{Catm}, \code{maturity_group}, \code{windspeed_height},
          \code{lat}, and \code{longitude}.
  }

  \strong{ode_solver}

  \itemize{
    \item \code{type}: For this model, the ODE solver type should not be
          \code{boost_rosenbrock} or \code{auto} (which defaults to
          \code{boost_rosenbrock} when a fixed step size Euler ODE solver is not
          required, as in this case) since the integration will fail unless the
          tolerances are stringent (e.g., \code{output_step_size = 0.01},
          \code{adaptive_rel_error_tol = 1e-9},
          \code{adaptive_abs_error_tol = 1e-9}).
  }

  \strong{initial_values}

  \itemize{
    \item \code{Leaf}: The initial leaf biomass is set to 80\% of the mass per
          land area of seeds sown at the start of the season. For the initial
          total seed mass per land area, we use the following equation:
          \code{Number of seeds per meter * weight per seed / row spacing}.
          The number of seeds per meter is 20 and the row spacing is 0.38 m, as
          reported in Morgan \emph{et al.} (2004) [\doi{10.1104/pp.104.043968}].
          The weight per seed is based on the average of .12 to .18 grams, as
          reported by \href{https://www.feedipedia.org/node/42}{Feedipedia}.
          Thus, we have an initial biomass of
          \code{(20 seeds / m) * (0.15 g / seed) / (0.38 m) = 7.89 g / m^2},
          equivalent to \code{0.0789 Mg / ha} in the typical BioCro units.

    \item \code{Stem}: The initial stem biomass is set to 10\% of the initial
          seed biomass; see \code{Leaf} for more info.

    \item \code{Root}: The initial root biomass is set to 10\% of the initial
          seed biomass; see \code{Leaf} for more info.

    \item \code{Grain}: In principle, the initial seed mass should be zero, but
          here it is instead set to a very small value (1e-5) for compatibility
          with some models where key tissue masses cannot be zero.

    \item \code{Seed}: Initialized to a small value for the same reasons as the
          \code{Grain}.
  }

  \strong{parameters}

  \itemize{
    \item Optimized parameters: The following parameter values are determined
          using an optimization procedure: \code{alphaLeaf}, \code{alphaStem},
          \code{betaLeaf}, \code{betaStem}, \code{alphaShell}, \code{betaShell},
          \code{grc_root}, \code{grc_stem}, \code{mrc_leaf}, \code{mrc_root},
          \code{mrc_stem}, \code{rateSeneLeaf}, \code{rateSeneStem},
          \code{alphaSeneLeaf}, \code{alphaSeneStem}, \code{betaSeneLeaf}, and
          \code{betaSeneStem}.

    \item Basic soil info: Soil parameter values corresponding to
          \code{clay_loam} from the \code{\link{soil_parameters}} dataset are
          used here, which is the appropriate soil type for Champaign, Illinois.
          These parameters are: \code{soil_air_entry}, \code{soil_b_coefficient},
          \code{soil_bulk_density}, \code{soil_clay_content},
          \code{soil_field_capacity}, \code{soil_sand_content},
          \code{soil_saturated_conductivity}, \code{soil_saturation_capacity},
          \code{soil_silt_content}, and \code{soil_wilting_point}.

    \item \code{iSp}: Specific leaf area; determined using 2002 values of
          \code{average_LAI / average_leaf_biomass}  as reported in Dermody et
          al. 2006 [\doi{10.1111/j.1469-8137.2005.01565.x}] and
          Morgan et al. 2005 [\doi{10.1111/j.1365-2486.2005.001017.x}].

    \item \code{Sp_thermal_time_decay}: Set to 0 to ensure specific leaf area is
          constant with time.

    \item \code{maturity_group}: Typical maturity group grown in Champaign, IL.

    \item Soybean development (R0 - R1): The "emr - V0" values from Table 2 of
          Setiyono et al., 2007 [\doi{10.1016/j.fcr.2006.07.011}] were
          used for \code{Tmin_R0R1}, \code{Topt_R0R1}, and \code{Tmax_R0R1}.

    \item Soybean development (emr - V0, R1 - R7): Values from Table 2 of
          Setiyono et al., 2007 [\doi{10.1016/j.fcr.2006.07.011}] were
          used for \code{Rmax_emrV0}, \code{Tmin_emrV0}, \code{Topt_emrV0},
          \code{Tmax_emrV0}, \code{Tmin_R1R7}, \code{Topt_R1R7}, and
          \code{Tmax_R1R7}.

    \item \code{sowing_fractional_doy}: Set to 0 because Soybean-BioCro uses the
          weather data to set the sowing time. In other words, the weather data
          is truncated so it begins at the beginning of the simulation.

    \item \code{atmospheric_transmittance}: Campbell and Norman, An Introduction
          to Environmental Biophysics, 2nd Edition, Pg 173.

    \item \code{par_energy_content}: Set to 1 / 4.57, as in Plant Growth Chamber
          Handbook. CHAPTER 1 - RADIATION - John C. Sager and J. Craig McFarlane.
          Table 2, Pg 3
          \href{https://www.controlledenvironments.org/wp-content/uploads/sites/6/2017/06/Ch01.pdf}{online PDF}

    \item \code{heightf}: Estimated using a typical soybean LAI of 6 when canopy
          is 1 m tall.

    \item \code{chil}: Campbell and Norman, An Introduction to Environmental
          Biophysics, 2nd Edition, Table 15.1, pg 253.

    \item \code{k_diffuse}: Estimated from Campbell and Norman, An Introduction
          to Environmental Biophysics, 2nd Edition, Figure 15.4, pg 254

    \item \code{lnfun}: Set to 0 to use the same value of \code{Vcmax_at_25} for
          all canopy layers.

    \item Leaf optical properties: Leaf reflectance and transmittance in the PAR
          band (\code{leaf_reflectance_par} and \code{leaf_transmittance_par})
          are estimated from [\doi{10.2134/agronmonogr31.c7}],
          [\doi{10.2134/agronj1971.00021962006300010038x}], and
          [\doi{10.2134/agronj1991.00021962008300030026x}]. Reflectance and
          transmittance in the NIR band (\code{leaf_reflectance_nir} and
          \code{leaf_transmittance_nir}) are from
          [\doi{10.2134/agronmonogr31.c7}].

    \item FvCB model (temperature response of Rubisco kinetic properties): The
          following values are from Table 1 of Bernacchi et al. 2001
          [\doi{10.1111/j.1365-3040.2001.00668.x}]: \code{Gstar_c},
          \code{Gstar_Ea}, \code{Kc_c}, \code{Kc_Ea}, \code{Ko_c}, \code{Ko_Ea},
          \code{Vcmax_c}, \code{Vcmax_Ea}.

    \item FvCB model (temperature response of non-photorespiratory CO2 release):
          \code{RL_c} and \code{RL_Ea} are from Table 1 of Bernacchi et al. 2001
          [\doi{10.1111/j.1365-3040.2001.00668.x}].

    \item FvCB model (temperature response of electron transport): The following
          values are from Table 1 of Bernacchi et al. 2003
          [\doi{10.1046/j.0016-8025.2003.01050.x}]: \code{Jmax_c} and
          \code{Jmax_Ea}. The following are from Table 2: \code{phi_PSII_0},
          \code{phi_PSII_1}, \code{phi_PSII_2}, \code{theta_0}, \code{theta_1},
          \code{theta_2}.

    \item FvCB model (temperature response of triose phosphate utilization):
          The following values are from the caption of Figure 7 of Yang et al.
          2016 [\doi{10.1007/s00425-015-2436-8}]: \code{Tp_Ha}, \code{Tp_Hd},
          and \code{Tp_S}. The value of \code{Tp_c} was chosen to ensure that
          Tp_norm = 1 at 25 degrees C.

    \item FvCB model (light response): \code{beta_PSII},
          \code{electrons_per_carboxylation}, and
          \code{electrons_per_oxygenation} are from Bernacchi et al. 2003
          [\doi{10.1046/j.0016-8025.2003.01050.x}].

    \item FvCB model (rates at 25 degrees C): \code{Vcmax_at_25} and
          \code{Jmax_at_25} are seasonal averages across 2002 as reported in
          Bernacchi et. al. 2005 [\doi{10.1007/s00425-004-1320-8}].
          \code{RL_at_25} is from Davey et al. 2004
          [\doi{10.1104/pp.103.030569}] (Table 3, cv Pana, co2 368 ppm).
          \code{Tp_at_25} was estimated from unpublished data provided by
          A. Digrado (UIUC, August 2019).

    \item Ball-Berry model: \code{b0} and \code{b1} are from Leakey et al. 2006
          [\doi{10.1111/j.1365-3040.2006.01556.x}].

    \item \code{Catm}: Global average value for 2002.

    \item \code{leafwidth}: Estimate based on large mature leaflets.

    \item \code{rateSeneRoot}: Set to 0 to disable root senescence.

    \item \code{phi2}: from Sugarcane-BioCro, Jaiswal et al. 2017
          [\doi{10.1038/nclimate3410}].
  }

  The following parameters are not used by Soybean-BioCro, but are defined for
  compatability with current or historical modules:

  \itemize{
    \item \code{alpha1}: Needed for the \code{BioCro:parameter_calculator}
          module, but not used for C3 plants.

    \item \code{alphab1}: Needed for the \code{BioCro:parameter_calculator}
          module, but not used for C3 plants.

    \item \code{LeafN}: Needed for the \code{BioCro:parameter_calculator}
          module, but not used for C3 plants.

    \item \code{LeafN_0}: Needed for the \code{BioCro:parameter_calculator}
          module, but not used for C3 plants.

    \item \code{kpLN}: Has no influence on Soybean-BioCro because \code{lnfun}
          is set to to 0.

    \item FvCB model (varying Jmax): For compatability with the
          \code{BioCro:varying_Jmax25} module, \code{Jmax_at_25_mature} is set
          to the same value as \code{Jmax_at_25}. \code{sf_jmax} is also needed
          for this module.

    \item \code{alphaSeneRoot}: Has no influence on Soybean-BioCro because
          \code{rateSeneRoot} is set to 0.

    \item \code{betaSeneRoot}: Has no influence on Soybean-BioCro because
          \code{rateSeneRoot} is set to 0.

    \item \code{km_leaf_litter}: Needed for compatability with the
          \code{BioCro:litter_cover} module. Estimated based on 2021-2022 Energy
          Farm measurements, where the final leaf litter mass was around
          1.5 - 2.5 Mg / Ha and covered around half of the ground area near the
          plants.
  }
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{modules}}
    \item \code{\link{crop_model_definitions}}
    \item \code{\link{soybean_clock}}
  }
}

\keyword{datasets}

\concept{crop_models}
