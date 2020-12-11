#ifndef SE_SOLVER_FACTORY_H
#define SE_SOLVER_FACTORY_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../se_solver.h"

class se_solver_factory
{
   public:
    static std::unique_ptr<se_solver> create(
        std::string const& se_solver_name,
        int max_it);

    static std::vector<std::string> get_solvers();

   private:
    using se_solver_creator = std::unique_ptr<se_solver> (*)(int);           // Define a system_solver_creator to be a pointer to a function that has one int argument and returns a std::unique_ptr<system_solver>
    using se_solver_creator_map = std::map<std::string, se_solver_creator>;  // A map of strings to system_solver_creators
    static se_solver_creator_map se_solver_creators;
};

#endif
