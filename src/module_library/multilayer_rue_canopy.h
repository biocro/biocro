#ifndef MULTILAYER_RUE_CANOPY_H
#define MULTILAYER_RUE_CANOPY_H

#include "../framework/state_map.h"
#include "multilayer_canopy_photosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "rue_leaf_photosynthesis.h"

namespace standardBML
{
using ten_layer_rue_canopy_parent =
    multilayer_canopy_photosynthesis<
        ten_layer_canopy_properties,
        rue_leaf_photosynthesis>;

/**
 * @class ten_layer_rue_canopy
 *
 * @brief Represents a ten layer canopy where leaf-level photosynthesis is
 * calculated using a simple radiation use efficiency (RUE) model; see the
 * `rue_leaf_photosynthesis` class for more information about this model.
 *
 * More specifically, this is a a child class of
 * `multilayer_canopy_photosynthesis` where:
 *
 *  - The canopy module is set to the `ten_layer_canopy_properties` module
 *
 *  - The leaf module is set to the `rue_leaf_photosynthesis` module
 *
 *  - The number of layers is set to 10
 *
 * Instances of this class can be created using the module factory, unlike
 * the parent class `multilayer_canopy_photosynthesis`.
 */
class ten_layer_rue_canopy : public ten_layer_rue_canopy_parent
{
   public:
    ten_layer_rue_canopy(
        state_map const& input_quantities,
        state_map* output_quantities)
        : ten_layer_rue_canopy_parent(
              ten_layer_rue_canopy::nlayers,
              input_quantities,
              output_quantities)
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ten_layer_rue_canopy"; }

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
