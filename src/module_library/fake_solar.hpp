#ifndef FAKE_SOLAR_H
#define FAKE_SOLAR_H

#include "../modules.h"

/** Class created by EBL for testing and experimentation ... not meant for real simulations! */
class fake_solar : public SteadyModule
{
   public:
    fake_solar(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : SteadyModule("fake_solar"),
          // Get pointers to input parameters
          time_ip(get_ip(input_parameters, "time")),
          target_time_ip(get_ip(input_parameters, "target_time")),
          // Get pointers to output parameters
          solar_op(get_op(output_parameters, "solar"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* time_ip;
    const double* target_time_ip;
    // Pointers to output parameters
    double* solar_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> fake_solar::get_inputs()
{
    return {
        "time",
        "target_time"
    };
}

std::vector<std::string> fake_solar::get_outputs()
{
    return {
        "solar"
    };
}

void fake_solar::do_operation() const
{
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    // Get the current time
    double time = *time_ip;

    // Calculate the radiation in a way that's good for testing different day lengths, and the switch to dark conditions

    // Extract out the DOY and hour
    int doy = (int)time;
    double hour = 24.0 * (time - doy);

    // Define the parameters for calculating the solar radiation
    int min_doy = 91;           // The first day to shine light (min in dataset is 91) (mean is 196)
    int max_doy = 301;          // The last day to shine light (max in dataset is 301) (mean is 196)
    double max_solar = 1000.0;  // Maximum intensity from sun
    double bkg = 0.0;           // Background intensity

    double ss_dawn = 4.7;   // Start of civil twilight in Chicago on summer solstice 2019
    double ss_dusk = 21.0;  // End of civil twilight in Chicago on summer solstice 2019 (length = 16.3)

    double ws_dawn = 6.7;   // Start of civil twilight in Chicago on winter solstice 2018
    double ws_dusk = 17.0;  // End of civil twilight in Chicago on winter solstice 2018 (length = 10.3)

    //double se_dawn = 6.5;			// Start of civil twilight in Chicago on spring equinox 2019
    //double se_dusk = 19.5;			// End of civil twilight in Chicago on spring equinox 2019 (length = 13.0)

    double dawn = ss_dawn + (doy - min_doy) * (ws_dawn - ss_dawn) / (max_doy - min_doy);
    double dusk = ss_dusk + (doy - min_doy) * (ws_dusk - ss_dusk) / (max_doy - min_doy);

    //double dawn = se_dawn;
    //double dusk = se_dusk;

    // Calculate the Gaussian parameters
    double mean = 0.5 * (dawn + dusk);
    double sigma = (dusk - dawn) / 4.0;  // Situate dawn and dusk at mean -/+ 2 * sigma, so that 95% of the light is emitted between these times

    // Calculate the radiation intensity
    double solar = bkg;
    if (doy >= min_doy && doy <= max_doy) solar += max_solar * exp(-(hour - mean) * (hour - mean) / (sigma * sigma));

    /*
	// Calculate the radiation in a way that's good for calculating a phase response curve

	// Get the target doy to apply the sun
	double target_time = *target_time_ip;

	// Calculate the radiation
	double max_solar = 1000.0;
	double day_length = 12.0;			// In hours
	double sigma = day_length / 4.0;	// In hours
	sigma /= 24.0;						// In days
	double solar = max_solar * exp(-(time - target_time) * (time - target_time) / (sigma * sigma));
	*/

    //////////////////////////////////////
    // Update the output parameter list //
    //////////////////////////////////////

    update(solar_op, solar);
}

#endif
