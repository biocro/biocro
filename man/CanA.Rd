\name{CanA}
\alias{CanA}
\alias{lnParms}
\title{Simulates canopy assimilation}
\usage{
CanA(lai, doy, hr, solar, temp, rh, windspeed, lat = 40, nlayers = 8,
     kd = 0.1, StomataWS = 1, chi.l = 1, leafwidth = 0.04,
     heightFactor = 3, photoControl = list(), lnControl = list())
}
\arguments{
    \item{lai}{Leaf area index.}

    \item{doy}{Day of the year ([1, 365]).}

    \item{hr}{Hour of the day ([0, 23]).}

    \item{solar}{Solar radiation (\ifelse{html}{\out{&mu;mol m<sup>-2</sup> s<sup>-1</sup>}}{\eqn{\mumol m^{-2} s^{-1}}{micromoles m^-2 s^-1}}).}

    \item{temp}{Temperature (Celsius).}

    \item{rh}{Relative humidity ([0, 1]).}

    \item{windspeed}{Wind speed (\ifelse{html}{\out{m s<sup>-1</sup>}}{\eqn{m s^{-1}}{m s^-1}}).}

    \item{lat}{Latitude (degrees north).}

    \item{nlayers}{Number of layers in the simulation of the canopy (maximum allowed is 50).}

    \item{kd}{Light extinction coefficient for diffuse light.}

    \item{StomataWS}{Coefficient controlling the effect of water stress on stomatal conductance and assimilation.}

    \item{chi.l}{Leaf angle as described by projection of horizontal to vertical leaf area.}

    \item{heightFactor}{Height factor. Divide LAI by this number to get the height of a crop.}

    \item{photoControl}{List that sets the photosynthesis parameters. See \code{\link{BioGro}}.}

    \item{lnControl}{List that sets the leaf nitrogen parameters.

        \code{LeafN} Initial value of leaf nitrogen (\ifelse{html}{\out{g m<sup>-2</sup>}}{\eqn{g m^{-2}}{g m^-2}}).

        \code{kpLN} Coefficient of decrease in leaf nitrogen during the growing season.  The equation is LN = iLeafN * exp(-kLN * TTc).

        \code{lnFun} Controls whether there is a decline in leaf nitrogen with the depth of the canopy. `none' means no decline, `linear' means a linear decline
        controlled by the following two parameters.

        \code{lnb0} Intercept of the linear decline of leaf nitrogen in the depth of the canopy.

        \code{lnb1} Slope of the linear decline of leaf nitrogen in the depth of the canopy. The equation is \code{vmax = leafN_lay * lnb1 + lnb0}.}

    \item{Leafwidth}{Leaf width (meters).}

    \item{units}{Whether to return units in
                 \ifelse{html}{\out{kg m<sup>-2</sup> hr<sup>-1</sup>}}{\eqn{kg m^{-2} hr^{-1}}{kg m^-2 hr-1}}
                 or \ifelse{html}{\out{Mg ha<sup>-2</sup> hr<sup>-1</sup>}}{\eqn{Mg ha^{-2} hr^{-1}}{Mg ha^-2 hr-1}}. This is
                 typically run at hourly intervals, that is why the hr is kept, but it could
                 be used with data at finer timesteps and then convert the results.}
}
\value{
    \code{\link{list}}

    Returns a list with the following elements.

    \item{\code{CanopyAssim}}{Canopy assimilation (kg / m^2 / hr or Mg / ha / hr).}

    \item{\code{CanopyTrans}}{Canopy transpiration (kg / m^2 / hr or Mg / ha / hr)}

    \item{\code{CanopyCond}}{Hourly canopy conductance (units ?).}

    \item{\code{TranEpen}}{Canopy transpiration according to Penman (kg / m^2 / hr or Mg / ha / hr).}

    \item{\code{TranEpen}}{Canopy transpiration according to Priestly (kg / m^2 / hr or Mg / ha / hr).} 

    \item{\code{LayMat}}{Hourly by layer matrix containing details of the calculations by
        layer (each layer is a row).  \code{col1}: Direct irradiance. \code{col2}: Diffuse
        irradiance. \code{col3}: Leaf area in the sun. \code{col4}: Leaf area in the shade. \code{col5}:
        Transpiration of leaf area in the sun. \code{col6}: Transpiration of leaf area in
        the shade. \code{col7}: Assimilation of leaf area in the sun. \code{col8}: Assimilation of
        leaf area in the shade. \code{col9}: Difference in temperature between the leaf and
        the air (i.e. TLeaf - TAir) for leaves in sun. \code{col10}: Difference in
        temperature between the leaf and the air (i.e. TLeaf - TAir) for leaves in
        shade.  \code{col11}: Stomatal conductance for leaves in the sun. \code{col12}: Stomatal
        conductance for leaves in the shade. \code{col13}: Nitrogen concentration in the
        leaf (g m^-2). \code{col14}: Vmax value as depending on leaf nitrogen.}
}
\description{
    It represents an integration of the photosynthesis function
    \code{\link{c4photo}}, canopy evapo/transpiration and the multilayer canopy
}

\author{
    Fernando E. Miguez.
}

\examples{

\dontrun{
data(doy124)
tmp <- numeric(24)

for(i in 1:24){
    lai <- doy124[i, 1]
    doy <- doy124[i, 3]
    hr  <- doy124[i, 4]
    solar <- doy124[i, 5]
    temp <- doy124[i, 6]
    rh <- doy124[i, 7]
    ws <- doy124[i, 8]

    tmp[i] <- CanA(lai, doy, hr, solar, temp, rh, ws)$CanopyAssim

}

plot(c(0:23), tmp,
     type='l', lwd=2,
     xlab='Hour',
     ylab=expression(paste('Canopy assimilation (kg  ', m^-2, ' ', h^-1, ')')))

}
}
\keyword{models}
