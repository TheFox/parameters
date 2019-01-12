
#include <cstdio>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <regex>
#include <unordered_map>

#ifdef __has_include
#  if __has_include(<boost/program_options.hpp>)
#    include <boost/program_options.hpp>
namespace bpo = boost::program_options;
#  else
#    error "Missing <boost/program_options.hpp>"
#  endif
#  if __has_include(<boost/algorithm/string.hpp>)
#    include <boost/algorithm/string.hpp>
#  else
#    error "Missing <boost/algorithm/string.hpp>"
#  endif
#  if __has_include(<termcolor/termcolor.hpp>)
#    include <termcolor/termcolor.hpp>
#  endif
#endif // __has_include

#include "config.hpp"
#include "class/env_var.hpp"

int main(int argc, char* const argv[], char** const envp)
{
#ifdef DEBUG
  std::cerr << "--- DEBUG ---" << std::endl << std::endl;
#endif

  // Generic options
  bpo::options_description genericOpts{"Options"};
  genericOpts.add_options()
               ("input,i", bpo::value<std::string>()->value_name("path"), "Input path to template file. (required)")
               ("output,o", bpo::value<std::string>()->value_name("path"), "Path to output file. Defaut: STDOUT")
               ("regexp,r", bpo::value<std::string>()->value_name("string"),
                 "Search regular expression for environment variable names. (required)")
               ("env,e", bpo::value<std::string>()->value_name("string"), "Name of the environment. For example: production")
               ("instance,n", bpo::value<std::string>()->value_name("string"),
                 "Name of the Instance. For example: SHOPA, or SHOPB.")
               ("char,c", bpo::value<u_char>()->value_name("char"), "Search char for template variables. Default: @")
               ("quiet,q", bpo::bool_switch()->default_value(false),
                 "Do not throw an error if there are variables missing being replaced.")
#ifdef TERMCOLOR_HPP_
               ("no-color", bpo::bool_switch()->default_value(false), "Disable colors.")
#endif
               ("help,h", "This message");

  bpo::options_description opts{};
  opts.add(genericOpts);

  bpo::variables_map vm{};

  try {
    auto parsedOptions = bpo::command_line_parser(argc, argv)
      .options(opts)
      .allow_unregistered()
      .run();
    bpo::store(parsedOptions, vm);
    bpo::notify(vm);
  }
  catch (std::exception& e) {
#ifdef TERMCOLOR_HPP_
    std::cerr << termcolor::on_red << termcolor::white << "ERROR: " << e.what() << termcolor::reset << std::endl;
#else
    std::cerr << "ERROR: " << e.what() << std::endl;
#endif
    return 2;
  }

#ifdef DEBUG
  constexpr bool hasStdIn = false;
#else
  const auto hasStdIn = !static_cast<bool>(isatty(fileno(stdin)));
#endif

  // Debug
#ifdef DEBUG
  std::cerr << "stdin: " << (hasStdIn ? "YES" : "NO") << std::endl;
  std::cerr << "help: " << (vm.count("help")) << std::endl;
  std::cerr << "input: " << (vm.count("input") == 0) << std::endl;
  std::cerr << "regex: " << (vm.count("regex") == 0) << std::endl;
#endif

  // Help
  if (vm.count("help") || (vm.count("input") == 0 && !hasStdIn) || vm.count("regexp") == 0) {
    std::cerr << PROJECT_NAME << ' ' << PROJECT_VERSION << std::endl;
    std::cerr << "Built at: " << __DATE__ << ' ' << __TIME__ << std::endl;
    std::cerr << "Build Tag: " << PROJECT_VERSION_TAG << std::endl;
    std::cerr << "Build Commit: " << PROJECT_VERSION_COMMIT << std::endl;
    std::cerr << PROJECT_COPYRIGHT << std::endl << std::endl;

    std::cerr << "Usage: " << argv[0] << " [options]" << std::endl << std::endl;
    std::cerr << genericOpts << std::endl;

    return 3;
  }

  // Debug
#ifdef DEBUG
  std::cerr << "get vm params" << std::endl;
#endif

  const auto inputFilePath = vm.count("input") == 0 ? std::string{} : vm["input"].as<std::string>();
  const auto outputFilePath = vm.count("output") == 0 ? std::string{} : vm["output"].as<std::string>();
  const auto searchRegexpStr = vm["regexp"].as<std::string>();
  const auto envStr = vm.count("env") == 0 ? std::string{} : boost::to_upper_copy<std::string>(vm["env"].as<std::string>());
  const auto instanceStr =
    vm.count("instance") == 0 ? std::string{} : boost::to_upper_copy<std::string>(vm["instance"].as<std::string>());
  const u_char searchChar = vm.count("char") == 0 ? '@' : vm["char"].as<u_char>();
  const auto isQuiet = vm.count("quiet") == 0 ? false : vm["quiet"].as<bool>();
#ifdef TERMCOLOR_HPP_
  const auto isNoColor = vm.count("no-color") == 0 ? false : vm["no-color"].as<bool>();
#endif

  // Debug
#ifdef DEBUG
  std::cerr << "stdin: " << (hasStdIn ? "YES" : "NO") << std::endl;
  std::cerr << "input file: '" << inputFilePath << "'" << std::endl;
  std::cerr << "output file: '" << outputFilePath << "'" << std::endl;
  std::cerr << "search: '" << searchRegexpStr << "'" << std::endl;
  std::cerr << "env: '" << envStr << "'" << std::endl;
  std::cerr << "instance: '" << instanceStr << "'" << std::endl;
  std::cerr << "search char: '" << static_cast<u_char>(searchChar) << "'" << std::endl;
  std::cerr << "quiet: '" << isQuiet << "'" << std::endl;
#ifdef TERMCOLOR_HPP_
  std::cerr << "no_color: '" << isNoColor << "'" << std::endl;
#endif
#endif

  // Content
  std::string content{};

  if (hasStdIn) {
    std::string line;
    while (std::getline(std::cin, line)) {
      content += line + '\n';
    }
  } else {
    // Open input file.
    std::ifstream ifile(inputFilePath, std::ios::binary | std::ios::ate);
    if (ifile.bad()) {
      const std::string errMsg{"Could not open file: " + inputFilePath};
#ifdef TERMCOLOR_HPP_
      if (isNoColor) {
        std::cerr << "ERROR: " << errMsg << std::endl;
      } else {
        std::cerr << termcolor::on_red << termcolor::white << "ERROR: "
             << errMsg << termcolor::reset << std::endl;
      }
#else
      std::cerr << "ERROR: " << errMsg << std::endl;
#endif
      return 1;
    }
    auto fsize = ifile.tellg();
    if (fsize == -1) {
      const std::string errMsg{"Could not open file: " + inputFilePath};
#ifdef TERMCOLOR_HPP_
      if (isNoColor) {
        std::cerr << "ERROR: " << errMsg << std::endl;
      } else {
        std::cerr << termcolor::on_red << termcolor::white << "ERROR: "
             << errMsg << termcolor::reset << std::endl;
      }
#else
      std::cerr << "ERROR: " << errMsg << std::endl;
#endif
      return 1;
    }
    ifile.seekg(0, std::ios::beg);

    // Input Buffer
    std::vector<char> buffer(fsize);

    // Read input file.
    if (!ifile.read(buffer.data(), fsize)) {
      const std::string errMsg{"Could not read input file."};
#ifdef TERMCOLOR_HPP_
      if (isNoColor) {
        std::cerr << "ERROR: " << errMsg << std::endl;
      } else {
        std::cerr << termcolor::on_red << termcolor::white << "ERROR: "
             << errMsg << termcolor::reset << std::endl;
      }
#else
      std::cerr << "ERROR: " << errMsg << std::endl;
#endif
    }
    ifile.close();

    // Create string from buffer.
    content.assign(buffer.begin(), buffer.end());
  }

  // Regexp
  const std::regex search(searchRegexpStr);

  // Collect env vars.
  std::unordered_map<std::string, Parameters::EnvVar> envMap{};

  // https://stackoverflow.com/a/2085385/823644
  for (char** env = envp; *env != nullptr; ++env) {
    const std::string envs{*env};
    const auto pos = envs.find_first_of('=');
    const auto name = envs.substr(0, pos);
    const auto val = envs.substr(pos + 1);

    if (std::regex_search(name, search)) {
      envMap.emplace(std::make_pair(name, Parameters::EnvVar{name, val}));

#ifdef DEBUG
      std::cerr << "collect val='" << name << "' val='" << val << "'" << std::endl;
#endif
    }
  }

  for (const auto& tmpEnv : envMap) {
    auto env = tmpEnv.second;
    auto val = env.val;
#ifdef DEBUG
    std::cerr << "val='" << env.name << "' val='" << val << "' (" << env.ignore << ")" << std::endl;
#endif

    if (env.ignore) {
      continue;
    }

    // Instance
    if (!instanceStr.empty()) {
      const auto pos = env.name.length() - instanceStr.length();
      const auto instanceSubstr = env.name.substr(pos);

#ifdef DEBUG
      std::cerr << " -> test instance: '" << instanceSubstr << "' (" << pos << ")" << std::endl;
#endif

      if (instanceSubstr == instanceStr) {
        env.name = env.name.substr(0, pos - 1);
#ifdef DEBUG
        std::cerr << "   -> OK" << std::endl;
#endif
      }
    }

    // Env
    if (!envStr.empty()) {
      const auto pos = env.name.length() - envStr.length();
      const auto envSubstr = env.name.substr(pos);
#ifdef DEBUG
      std::cerr << " -> test env: '" << envSubstr << "' (" << pos << ")" << std::endl;
#endif

      if (envSubstr == envStr) {
        env.name = env.name.substr(0, pos - 1);
#ifdef DEBUG
        std::cerr << "   -> OK" << std::endl;
#endif
      }
    }

    const auto envKey{env.name + '_' + envStr};
    const auto instanceKey{envKey + '_' + instanceStr};

    if (envMap.count(instanceKey)) {
#ifdef DEBUG
      std::cerr << " -> has instance key" << std::endl;
#endif
      envMap[instanceKey].ignore = true;
      val = envMap[instanceKey].val;
    } else if (envMap.count(envKey)) {
#ifdef DEBUG
      std::cerr << " -> has env key" << std::endl;
#endif
      envMap[envKey].ignore = true;
      val = envMap[envKey].val;
    }

    if (envMap.count(instanceKey)) {
      envMap[instanceKey].ignore = true;
    }
    if (envMap.count(envKey)) {
      envMap[envKey].ignore = true;
    }
    if (envMap.count(env.name)) {
      envMap[env.name].ignore = true;
    }

#ifdef DEBUG
    std::cerr << " -> '" << env.name << "' '" << envKey << "' '" << instanceKey << "' => '" << val << "'" << std::endl;
#endif

    std::stringstream searchNameStream{};
    searchNameStream << searchChar << env.name << searchChar;
    const auto searchName = searchNameStream.str();
    boost::replace_all(content, searchName, val);
  }

  if (outputFilePath.empty()) {
    std::cout << content;
  } else {
    // Save Month file.
    std::ofstream fout{outputFilePath};
    fout << content;
    fout.close();
  }

  // Remove 'begin of line' character.
  auto searchEndRegexpStr = searchRegexpStr;
  searchEndRegexpStr.erase(std::remove(searchEndRegexpStr.begin(), searchEndRegexpStr.end(), '^'),
    searchEndRegexpStr.end());

  // Regexp
  std::stringstream endSearchStream{};
  endSearchStream << searchChar << searchEndRegexpStr;
  const std::regex endSearch(endSearchStream.str());

  if (!isQuiet && std::regex_search(content, endSearch)) {
#ifdef TERMCOLOR_HPP_
    if (isNoColor) {
      std::cerr << "ERROR: " << "Cannot build configuration file; Missing ENV variables." << std::endl;
    } else {
      std::cerr << termcolor::on_red << termcolor::white << "ERROR: "
           << "Cannot build configuration file; Missing ENV variables." << termcolor::reset << std::endl;
    }
#else
    std::cerr << "ERROR: " << "Cannot build configuration file; Missing ENV variables." << std::endl;
#endif
    return 1;
  }

  return 0;
}
