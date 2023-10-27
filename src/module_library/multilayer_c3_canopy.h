#ifndef MULTILAYER_C3_CANOPY_H
#define MULTILAYER_C3_CANOPY_H

#include "../framework/state_map.h"
#include "multilayer_canopy_photosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "c3_leaf_photosynthesis.h"

namespace standardBML
{
using ten_layer_c3_canopy_parent =
    multilayer_canopy_photosynthesis<
        ten_layer_canopy_properties,
        c3_leaf_photosynthesis>;

/**
 * @class ten_layer_c3_canopy
 *
 * @brief Represents a ten layer canopy where leaf-level photosynthesis is
 * calculated using the Farquhar-von-Cammerer-Berry model for C3 photosynthesis;
 * see the `c3_leaf_photosynthesis` class for more information about this model.
 *
 * More specifically, this is a child class of
 * `multilayer_canopy_photosynthesis` where:

 *  - The canopy module is set to the `ten_layer_canopy_properties` module
 *
 *  - The leaf module is set to the `c3_leaf_photosynthesis` module
 *
 *  - The number of layers is set to 10
 *
 * Instances of this class can be created using the module factory, unlike the
 * parent class `multilayer_canopy_photosynthesis`.
 */
class ten_layer_c3_canopy : public ten_layer_c3_canopy_parent
{
   public:
    ten_layer_c3_canopy(
        state_map const& input_quantities,
        state_map* output_quantities)
        : ten_layer_c3_canopy_parent(
              ten_layer_c3_canopy::nlayers,
              input_quantities,
              output_quantities)
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ten_layer_c3_canopy"; }

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
