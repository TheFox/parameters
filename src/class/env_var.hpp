
#ifndef PARAMETERS_ENV_VAR_HPP_
#define PARAMETERS_ENV_VAR_HPP_

#include <string>

namespace Parameters
{
  struct EnvVar
  {
    // Constructor
    //EnvVar(std::string name, std::string val) noexcept;

    // Variables
    std::string name{};
    std::string val{};
    bool ignore{};
  };
} // Parameters namespace

#endif // PARAMETERS_ENV_VAR_HPP_
