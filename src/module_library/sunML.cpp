#include "sunML.h"

/**
 *  @brief Computes absorbed light from incident light for isolated leaves.
 *
 *  In general, the light absorbed by a leaf (`Iabs`) is related to the light
 *  incident on the leaf (`Iinc`) by the leaf absorptance (`abs`), which
 *  represents the fraction of light absorbed by the leaf. In other words,
 *  `Iabs = abs * Iinc`.
 *
 *  For a leaf in isolation, any light that is reflected or transmitted is not
 *  absorbed, so `abs = 1 - leaf_reflectance - leaf_transmittance`. This is the
 *  typical definition of leaf absorptance.
 *
 *  @param [in] leaf_reflectance The fractional amount of light reflected by
 *              the leaves (weighted across the appropriate wavelength band)
 *
 *  @param [in] leaf_transmittance The fractional amount of light transmitted by
 *              the leaves (weighted across the appropriate wavelength band)
 *
 *  @param [in] incident_light The amount of light incident on the leaves; for
 *              quantum fluxes, the units will typically be micromol / m^2 / s;
 *              for energy fluxes, the units will typically be J / m^2 / s.
 *
 *  @return The amount of radiation absorbed by the leaves expressed in the same
 *              units as `incident_light`.
 */
double absorbed_from_incident_leaf(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // Light units such as `micromol / m^2 / s` or
                                //   `J / m^2 / s`
)
{
    // Return value has same units as `incident_light`
    return incident_light * (1 - leaf_reflectance - leaf_transmittance);
}

/**
 *  @brief Computes absorbed light from incident light for a thick layer of
 *  material.
 *
 *  Suppose light of intensity `I_0` (represent a flux density of photons or
 *  energy, expressed in units of photons per area per time or energy per area
 *  per time) is incident on the surface of an infinitely thick piece of a
 *  material that reflects, absorbs, and transmits light with no lateral
 *  variations. As the light passes through the material, its intensity will
 *  gradually diminish until it eventually reaches zero. We can express this
 *  mathematically via a one-dimensional expression: `I(x) = I_0 * f(x)`, where
 *  `x` is a coordinate that represents the amount of material the light has
 *  passed through, and `f(x)` is the fraction of the original light received by
 *  the material at `x`. Note that if the material does not have uniform
 *  density, `x` is not a spatial coordinate, but instead will have a non-linear
 *  dependence on distance. Although we do not know the particular form of
 *  `f(x)`, we can safely assume that `f(0) = 0` (so the incident light at the
 *  material's surface is `I_0`) and that `f(x)` approaches 0 as `x` approaches
 *  infinity (so the light intensity is fully diminished deep within the
 *  material).
 *
 *  Now consider two points within the material separated by an amount of
 *  material `delta_x`; in other words, points specified by `x_0` and
 *  `x_0 + delta_x`. The decrease in light intensity per unit material between
 *  these two points (`delta_I`) can be expressed as
 *  `delta_I = (I(x_0 + delta_x) - I(x_0)) / delta_x`. Rewriting this using
 *  `f(x)`, we have `delta_I = I_0 * (f(x_0 + delta_x) - f(x_0)) / delta_x`. If
 *  we assume that the decrease in intensity is due to the absorption and
 *  reflection of light by `delta_x`, we can also write
 *  `delta_I = - I(x) * (A + R)`, where `A` and `R` are the fractions of
 *  incident light absorbed and reflected by a thin layer of the material. Thus,
 *  we can equate the two expressions for `delta_I` to find that
 *  `I_0 * (f(x_0 + delta_x) - f(x_0)) / delta_x = - I_0 * f(x) * (A + R)` or,
 *  equivalently, `(f(x_0 + delta_x) - f(x_0)) / delta_x = - f(x) * (A + R)`. As
 *  `delta_x` approaches 0, we can recognize the left-hand side of this equation
 *  as the derivative of `f(x)` with respect to `x`: `f'(x) = - f(x) * (A + R)`.
 *  Rearranging, we can express `f(x)` in terms of its derivative and the
 *  material's optical properties: `f(x) = -f'(x) / (A + R)`. Noting that all
 *  light received by a thin layer of the material must be reflected, absorbed,
 *  or transmitted, we can rewrite this equation using the fraction of
 *  transmitted light `T` using `A + R + T = 1` as follows:
 *
 *  `f(x) = -f'(x) / (1 - T)`                    [Equation (1)]
 *
 *  This simple model for calculating light intensities within a material was
 *  first applied in the context of plant biology by Monsi & Saeki (1953), where
 *  it used to calculate light levels within a plant canopy. This paper is
 *  difficult to find and was written in German, but parts of it have been
 *  reproduced in English and are easier to access (Saeki 1960, Saeiki 1963,
 *  Hirose 2004). Saeki (1960) notes the following about this equation: "It must
 *  be noted that in these equations `m` includes not only the fraction
 *  resulting from light transmitted through the leaf blades but also the
 *  fraction reflected downward from inclined leaves. This `m` is not constant
 *  but increases with the depth of foliage, because light of particular
 *  wavelengths is more liable to be reflected and transmitted, and increases in
 *  proportion at deeper positions." (In the original notation, `m` was used in
 *  place of the `T` we use here.) Thus, `T`, `R`, and `A` are not exactly the
 *  same as the corresponding optical properties of an isolated layer of the
 *  material (such as a leaf).
 *
 *  If we consider light from a sufficiently narrow wavelength band, then it may
 *  be reasonable to suppose that `T`, `R`, and `A` are constant throughout the
 *  material. In this case, it is possible to estimate the total amount of light
 *  absorbed by the material. To do this, we first calculate the absorbed light
 *  at depth `x` (`I_abs(x)`) using `I_abs(x) = I(x) * A`. Substituting in
 *  Equation (1), we have `I_abs(x) = - I_0 * f'(x) * (1 - R - T) / (1 - T)`.
 *  Now we can integrate this across the entire range of `x` (0 to infinity) to
 *  find the total amount of light absorbed by the material (`I_abs_tot`):
 *  `I_abs_tot = I_0 * (1 - R - T) / (1 - T) * (f(0) - f(infinity))`.
 *  By assumption, `f(0) = 1` and `f(infinity) = 0`, so this evaluates to
 *
 *  `I_abs_tot = I_0 * (1 - R - T) / (1 - T)`.   [Equation (2)]
 *
 *  Note that Equation (2) agrees with intuition in two extreme situations.
 *  First, if the material does not reflect any light (`R = 0`), then Equation
 *  (2) reduces to `I_abs_tot = I_0`. This makes sense because even if thin
 *  layers of the material transmit light, there is no way for any light to
 *  avoid being absorbed by an infinitely thick layer if there is no reflection.
 *  The other situation is where the material does not transmit any light
 *  (`T = 0`). In this case, Equation (2) reduces to
 *  `I_abs_tot = I_0 * (1 - R)`. This makes sense because the optical properties
 *  of a material with no transmission would be determined only by its surface;
 *  the surface would have the same optical properties as any thin layer,
 *  reflecting a fraction `R` of the light and absorbing the rest.
 *
 *  Although Equation (1) was originally developed for plant canopies, it does
 *  not rely on any specific properties of canopies, and can in principle apply
 *  to any material. (In fact, we have written this derivation in a
 *  material-agnostic way to emphasize this.) Thus, Equation (2) can also apply
 *  to a wide variety of materials. The absorption and reflection of light by
 *  soil is another situation where Equation (2) may be useful, as the
 *  assumption of a thick layer that does not transmit any light through it is
 *  certainly justified in that scenario.
 *
 *  Because it assumes a thick layer of a homogeneous light-absorbing material,
 *  Equation (2) is not appropriate for use in a layered canopy model or one
 *  that makes distinctions between different leaf classes (such as sunlit and
 *  shaded). It is best used for situations like estimating whole-canopy
 *  transpiration or soil evaporation, where it is useful to know the total
 *  solar energy absorbed by a thick layer of leaves or soil. Care must be taken
 *  even in this case, however, since this equation would still not be
 *  appropriate for the small canopies of young plants, which certainly transmit
 *  a significant fraction of the incident light.
 *
 *  Caveat from EL: Although Equation (1) can be found in Monsi & Saeki (1953),
 *  Saeki (1960), and Saeki (1963), Equation (2) is not included in those
 *  references. So, although this derivation makes sense to me, there is still a
 *  chance that it might not be correct. Equation (2) can be found in Steve H's
 *  thesis, the WIMOVAC code, and the BioCro code. In these places, it is
 *  variously attributed to Thornley & Johnson (1990), Monteith (1973), and
 *  Monteith and Unsworth (1990). Unfortunately, these textbooks are not
 *  available in electronic form, and I cannot access them at the moment. Newer
 *  versions of Monteith & Unsworth (1990) are available electronically, but do
 *  not seem to include Equation (2). I have attempted to find an explanation
 *  for Equation (2) elsewhere, but have not been successful so far. Thonrley
 *  (2002) discusses Equation (1), but ultimately just references the
 *  (inaccessible) textbook.
 *
 *  References:
 *
 *  - Monsi, M. & Saeki, T. Über den Lichtfaktor in den Pflanzengesellschaften
 *    und seine Bedeutung für die Stoffproduktion. Japanese Journal of Botany
 *    14, 2252 (1953).
 *
 *  - Saeki, T. Interrelationships between Leaf Amount, Light Distribution and
 *    Total Photosynthesis in a Plant Community. Shokubutsugaku Zasshi 73, 55–63
 *    (1960).
 *
 *  - Saeki, T. Light Relations In Plant Communities. in Environmental Control
 *    of Plant Growth (ed. Evans, L. T.) 79–94 (Academic Press, 1963).
 *
 *  - Hirose, T. Development of the Monsi–Saeki Theory on Canopy Structure and
 *    Function. Annals of Botany 95, 483–494 (2004).
 *
 *  - Thornley, J. H. M. & Johnson, I. R. Plant and crop modelling: A
 *    mathematical approach to plant and crop physiology (2000).
 *
 *  - Monteith, J. L. Principles of Environmental Physics (1973).
 *
 *  - Monteith, J. L. & Unsworth, M. H. Principles of Environmental Physics
 *    (1990).
 *
 *  - Thornley, J. H. M. Instantaneous Canopy Photosynthesis: Analytical
 *    Expressions for Sun and Shade Leaves Based on Exponential Light Decay Down
 *    the Canopy and an Acclimated Non-rectangular Hyperbola for Leaf
 *    Photosynthesis. Ann Bot 89, 451–458 (2002).
 *
 *
 *  @param [in] R The fractional amount of light reflected by a thin layer of
 *              the material in the appropriate wavelength band; note that this
 *              reflectance is not necessary the same as would be measured from
 *              a thin layer in isolation (see full description for more
 *              details).
 *
 *  @param [in] T The fractional amount of light transmitted by a thin layer of
 *              the material in the appropriate wavelength band; note that this
 *              transmittance is not necessary the same as would be measured
 *              from a thin layer in isolation (see full description for more
 *              details).
 *
 *  @param [in] I0 The amount of light incident on the leaves, perhaps
 *              restricted to a particular wavelength band; for quantum fluxes,
 *              the units will typically be micromol / m^2 / s; for energy
 *              fluxes, the units will typically be J / m^2 / s.
 *
 *  @return The amount of radiation absorbed by the leaves expressed in the same
 *              units as `I0`.
 */
double thick_layer_absorption(
    double R,  // dimensionless
    double T,  // dimensionless
    double I0  // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
)
{
    return I0 * (1 - R - T) / (1 - T);  // same units as `I0`
}

/**
 *  @brief Computes total absorbed shortwave radiation from the
 *  photosynthetically active photon flux density (PPFD) incident on a leaf.
 *
 *  The total absorbed shortwave radiation is determined using the following
 *  steps:
 *  - determine the incident photosynthetically active radiation (PAR) using the
 *    energy content of PAR
 *  - determine the incident near-infrared radiation (NIR) from the incident PAR
 *    using the PAR energy fraction
 *  - determine the total incident radiation by adding the incident PAR and NIR
 *  - determine the total absorbed radiation using the leaf's reflection and
 *    transmission coefficients
 *
 *  @param [in] incident_ppfd Photosynthetically active photon flux density
 *              (PPFD) incident on a leaf expressed in micromol / m^2 / s
 *
 *  @param [in] par_energy_content The energy content of PPFD expressed in J /
 *              micromol
 *
 *  @param [in] par_energy_fraction The fraction of total shortwave energy
 *              contained in the PAR band expressed as a real number between 0
 *              and 1
 *
 *  @param [in] leaf_reflectance The fractional amount of shortwave radiation
 *              reflected by the leaf (weighted across all shortwave radiation)
 *
 *  @param [in] leaf_transmittance The fractional amount of shortwave radiation
 *              transmitted through the leaf (weighted across all shortwave
 *              radiation)
 *
 *  @return The total shortwave radiation absorbed by the leaf expressed in
 *          J / m^2 / s
 */
double absorbed_shortwave_from_incident_ppfd(
    double incident_ppfd,        // micromol / m^2 / s
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double leaf_reflectance,     // dimensionless
    double leaf_transmittance    // dimensionless
)
{
    double incident_par = incident_ppfd * par_energy_content;  // J / m^2 / s

    double incident_nir = incident_par *
                          (1 - par_energy_fraction) /
                          par_energy_fraction;  // J / m^2 /s

    double incident_shortwave = incident_par + incident_nir;  // J / m^2 / s

    return thick_layer_absorption(
        leaf_reflectance,
        leaf_transmittance,
        incident_shortwave);  // J / m^2 / s
}

/**
 *  @brief Computes an n-layered light profile from the direct light, diffuse
 *  light, leaf area index, solar zenith angle, and other parameters.
 *
 *  @param [in] ambient_ppfd_beam Photosynthetically active photon flux density
 *              (PPFD) for beam light passing through a surface perpendicular
 *              to the beam direction at the top of the canopy; this represents
 *              direct sunlight for a plant in a field
 *              (micromol / (m^2 beam) / s)
 *
 *  @param [in] ambient_ppfd_diffuse Photosynthetically active photon flux
 *              density (PPFD) for diffuse light at the top of the canopy; this
 *              represents diffuse light scattered out of the solar beam by the
 *              Earth's atmosphere for a plant in a field; as a diffuse flux
 *              density, this represents the flux through any surface
 *              (micromol / m^2 / s)
 *
 *  @param [in] lai Leaf area index (LAI) of the entire canopy, which represents
 *              the leaf area per unit of ground area (dimensionless from m^2
 *              leaf / m^2 ground)
 *
 *  @param [in] nlayers Integer number of layers in the canopy
 *
 *  @param [in] cosine_zenith_angle Cosine of the solar zenith angle
 *              (dimensionless)
 *
 *  @param [in] kd Extinction coefficient for diffuse light (dimensionless)
 *
 *  @param [in] chil Ratio of average projected areas of canopy elements on
 *              horizontal surfaces; for a spherical leaf distribution,
 *              `chil = 0`; for a vertical leaf distribution, `chil = 1`; for a
 *              horizontal leaf distribution, `chil` approaches infinity
 *              (dimensionless from m^2 / m^2)
 *
 *  @param [in] absorptivity The leaf absorptivity on a quantum basis
 *              (dimensionless from mol / mol)
 *
 *  @param [in] heightf Leaf area density, i.e., LAI per height of canopy (m^-1
 *              from m^2 leaf / m^2 ground / m height)
 *
 *  @return An n-layered light profile representing quantities within
 *          the canopy, including several photon flux densities and
 *          the relative fractions of shaded and sunlit leaves
 */
Light_profile sunML(
    double ambient_ppfd_beam,     // micromol / (m^2 beam) / s
    double ambient_ppfd_diffuse,  // micromol / m^2 / s
    double lai,                   // dimensionless from m^2 / m^2
    int nlayers,                  // dimensionless
    double cosine_zenith_angle,   // dimensionless
    double kd,                    // dimensionless
    double chil,                  // dimensionless from m^2 / m^2
    double absorptivity,          // dimensionless from mol / mol
    double heightf,               // m^-1 from m^2 leaf / m^2 ground / m height
    double par_energy_content,    // J / micromol
    double par_energy_fraction,   // dimensionless
    double leaf_transmittance,    // dimensionless
    double leaf_reflectance       // dimensionless
)
{
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }
    if (cosine_zenith_angle > 1 || cosine_zenith_angle < -1) {
        throw std::out_of_range("cosine_zenith_angle must be between -1 and 1.");
    }
    if (kd > 1 || kd < 0) {
        throw std::out_of_range("kd must be between 0 and 1.");
    }
    if (chil < 0) {
        throw std::out_of_range("chil must be non-negative.");
    }
    if (absorptivity > 1 || absorptivity < 0) {
        throw std::out_of_range("absorptivity must be between 0 and 1.");
    }
    if (heightf <= 0) {
        throw std::out_of_range("heightf must greater than zero.");
    }

    // Calculate the leaf shape factor for an ellipsoidal leaf angle
    // distribution using the equation from page 251 of Campbell & Norman
    // (1998). We will use this value as `k`, the canopy extinction coefficient
    // for photosynthetically active radiation throughout the canopy. This
    // quantity represents the ratio of horizontal area to total area for leaves
    // in the canopy and is therefore dimensionless from
    // (m^2 ground) / (m^2 leaf).
    double zenith_angle = acos(cosine_zenith_angle);  // radians
    double k0 = sqrt(pow(chil, 2) + pow(tan(zenith_angle), 2));
    double k1 = chil + 1.744 * pow((chil + 1.182), -0.733);
    double k = k0 / k1;  // dimensionless

    double lai_per_layer = lai / nlayers;

    // Calculate the ambient direct PPFD through a surface parallel to the ground
    const double ambient_ppfd_beam_ground = ambient_ppfd_beam * cosine_zenith_angle;  // micromol / (m^2 ground) / s

    // Calculate the ambient direct PPFD through a unit area of leaf surface
    double ambient_ppfd_beam_leaf = ambient_ppfd_beam_ground * k;  // micromol / (m^2 leaf) / s

    Light_profile light_profile;
    for (int i = 0; i < nlayers; ++i) {
        // Get the cumulative LAI for this layer, which represents the total
        // leaf area above this layer
        const double cumulative_lai = lai_per_layer * (i + 0.5);

        // Calculate the amount of PPFD scattered out of the direct beam using
        // Equations 15.6 and 15.1 from Campbell & Norman (1998), following
        // example 15.2. This is a diffuse flux density representing the flux
        // through any surface.
        const double scattered_ppfd =
            ambient_ppfd_beam_ground * (exp(-k * sqrt(absorptivity) * cumulative_lai) -
                                        exp(-k * cumulative_lai));  // micromol / m^2 / s

        // Calculate the total flux of diffuse photosynthetically active light
        // in this layer by combining the scattered PPFD with the ambient
        // diffuse PPFD. Here we use Equation 15.6 with `alpha` = 1 and
        // `kbe(phi)` = kd.
        double diffuse_ppfd =
            ambient_ppfd_diffuse * exp(-kd * cumulative_lai) + scattered_ppfd;  // micromol / m^2 / s

        // Calculate the fraction of sunlit and shaded leaves in this canopy
        // layer using Equation 15.21.
        const double Ls = (1 - exp(-k * lai_per_layer)) * exp(-k * cumulative_lai) / k;  // dimensionless
        double sunlit_fraction = Ls / lai_per_layer;                                     // dimensionless
        double shaded_fraction = 1 - sunlit_fraction;                                    // dimensionless

        // Calculate an "average" incident PPFD for the sunlit and shaded leaves
        // that doesn't seem to be based on a formula from Campbell & Norman
        // (1998). It's interpreted as a flux density through a unit of leaf
        // area, but that may not be correct.
        double average_ppfd =
            (sunlit_fraction * (ambient_ppfd_beam_leaf + diffuse_ppfd) + shaded_fraction * diffuse_ppfd) *
            (1 - exp(-k * lai_per_layer)) / k;  // micromol / (m^2 leaf) / s

        // For values of cosine_zenith_angle close to or less than 0, in place
        // of the calculations above, we want to use the limits of the above
        // expressions as cosine_zenith_angle approaches 0 from the right:
        if (cosine_zenith_angle <= 1E-10) {
            ambient_ppfd_beam_leaf = ambient_ppfd_beam / k1;
            diffuse_ppfd = ambient_ppfd_diffuse * exp(-kd * cumulative_lai);
            sunlit_fraction = 0;
            shaded_fraction = 1;
            average_ppfd = 0;
        }

        // Store values of incident PPFD
        light_profile.sunlit_incident_ppfd[i] = ambient_ppfd_beam_leaf + diffuse_ppfd;  // micromol / (m^2 leaf) / s
        light_profile.incident_ppfd_scattered[i] = scattered_ppfd;                      // micromol / m^2 / s
        light_profile.shaded_incident_ppfd[i] = diffuse_ppfd;                           // micromol / (m^2 leaf) / s
        light_profile.average_incident_ppfd[i] = average_ppfd;                          // micromol / (m^2 leaf) / s
        light_profile.sunlit_fraction[i] = sunlit_fraction;                             // dimensionless from m^2 / m^2
        light_profile.shaded_fraction[i] = shaded_fraction;                             // dimensionless from m^2 / m^2
        light_profile.height[i] = (lai - cumulative_lai) / heightf;                     // m

        // Store values of absorbed PPFD
        light_profile.sunlit_absorbed_ppfd[i] =
            absorbed_from_incident_leaf(
                leaf_reflectance,
                leaf_transmittance,
                ambient_ppfd_beam_leaf + diffuse_ppfd);  // micromol / m^2 / s

        light_profile.shaded_absorbed_ppfd[i] =
            absorbed_from_incident_leaf(
                leaf_reflectance,
                leaf_transmittance,
                diffuse_ppfd);  // micromol / m^2 / s

        // Store values of absorbed solar energy (including PAR and NIR)
        light_profile.sunlit_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                ambient_ppfd_beam_leaf + diffuse_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s

        light_profile.shaded_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                diffuse_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s

        light_profile.average_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                average_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s
    }
    return light_profile;
}
