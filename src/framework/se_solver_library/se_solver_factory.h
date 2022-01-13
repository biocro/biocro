#ifndef SE_SOLVER_FACTORY_H
#define SE_SOLVER_FACTORY_H

#include <string>
#include <map>
#include "../se_solver.h"
#include "../state_map.h"  // for string_vector

class se_solver_factory
{
   public:
    static se_solver* create(std::string const& se_solver_name, int max_it);

    static string_vector get_solvers();

   private:
    // Define a ode_solver_creator to be a pointer to a function that has one
    // int argument and returns a pointer to an se_solver
    using se_solver_creator = se_solver* (*)(int);

    // A map of strings to se_solver_creators
    using se_solver_creator_map = std::map<std::string, se_solver_creator>;

    static se_solver_creator_map se_solver_creators;
};

#endif
