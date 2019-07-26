#ifndef GRO_H
#define GRO_H

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

state_map Gro(
        state_map state,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

#endif
