struct Light_model {
    double direct_transmittance;   //!< Atmospheric transmittance to direct radiation (dimensionless)
    double diffuse_transmittance;  //!< Atmospheric transmittance to diffuse radiation (dimensionless)
    double direct_fraction;        //!< Fraction of direct irradiance at the Earth's surface (dimensionless)
    double diffuse_fraction;       //!< Fraction of diffuse irradiance at the Earth's surface (dimensionless)
};

Light_model lightME(
    double cosine_zenith_angle,
    double atmospheric_pressure,
    double atmospheric_transmittance,
    double atmospheric_scattering);
