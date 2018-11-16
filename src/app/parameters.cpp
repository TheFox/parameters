
#include <cstdio>
#include <string>
#include <algorithm>
#include <iostream>
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
#include "../class/env_var.hpp"

int main(int argc, char* const argv[], char** const envp)
{
  using std::string;
  using std::cout;
  using std::cin;
  using std::cerr;
  using std::endl;
  using bpo::options_description;
  using bpo::value;
  using Parameters::EnvVar;

#ifdef DEBUG
  cerr << "--- DEBUG ---" << endl << endl;
#endif

  // Generic options
  options_description genericOpts("Options");
  genericOpts.add_options()
               ("input,i", value<string>()->value_name("path"), "Input path to template file. (required)")
               ("output,o", value<string>()->value_name("path"), "Path to output file. Defaut: STDOUT")
               ("regexp,r", value<string>()->value_name("string"),
                 "Search regular expression for environment variable names. (required)")
               ("env,e", value<string>()->value_name("string"), "Name of the environment. For example: production")
               ("instance,n", value<string>()->value_name("string"), "Name of the Instance. For example: SHOPA, or SHOPB.")
               ("quiet,q", bpo::bool_switch()->default_value(false),
                 "Do not throw an error if there are variables missing being replaced.")
#ifdef TERMCOLOR_HPP_
               ("no-color", bpo::bool_switch()->default_value(false), "Disable colors.")
#endif
               ("help,h", "This message");

  options_description opts;
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
    cerr << termcolor::on_red << termcolor::white << "ERROR: " << e.what() << termcolor::reset << endl;
#else
    cerr << "ERROR: " << e.what() << endl;
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
  cerr << "stdin: " << (hasStdIn ? "YES" : "NO") << endl;
  cerr << "help: " << (vm.count("help")) << endl;
  cerr << "input: " << (vm.count("input") == 0) << endl;
  cerr << "regex: " << (vm.count("regex") == 0) << endl;
#endif

  // Help
  if (vm.count("help") || (vm.count("input") == 0 && !hasStdIn) || vm.count("regexp") == 0) {
    cerr << PROJECT_NAME << ' '
         << PROJECT_VERSION_MAJOR << '.' << PROJECT_VERSION_MINOR << '.' << PROJECT_VERSION_PATCH
         << PROJECT_VERSION_APPENDIX /*<< " (" << __DATE__ << ' ' << __TIME__ << ')'*/ << endl;
    cerr << "Build: " << __DATE__ << ' ' << __TIME__ << endl;
    cerr << PROJECT_COPYRIGHT << endl << endl;

    cerr << "Usage: " << argv[0] << " [options]" << endl << endl;
    cerr << genericOpts << endl;

    return 3;
  }

  // Debug
#ifdef DEBUG
  cerr << "get vm params" << endl;
#endif

  const auto inputFilePath = vm.count("input") == 0 ? string{} : vm["input"].as<std::string>();
  const auto outputFilePath = vm.count("output") == 0 ? string{} : vm["output"].as<std::string>();
  const auto searchRegexpStr = vm["regexp"].as<std::string>();
  const auto envStr = vm.count("env") == 0 ? string{} : boost::to_upper_copy<std::string>(vm["env"].as<std::string>());
  const auto instanceStr = vm.count("instance") == 0 ? string{} : boost::to_upper_copy<std::string>(vm["instance"].as<std::string>());
  const auto isQuiet = vm.count("quiet") == 0 ? false : vm["quiet"].as<bool>();
#ifdef TERMCOLOR_HPP_
  const auto isNoColor = vm.count("no-color") == 0 ? false : vm["no-color"].as<bool>();
#endif

  // Debug
#ifdef DEBUG
  cerr << "stdin: " << (hasStdIn ? "YES" : "NO") << endl;
  cerr << "input file: '" << inputFilePath << "'" << endl;
  cerr << "output file: '" << outputFilePath << "'" << endl;
  cerr << "search: '" << searchRegexpStr << "'" << endl;
  cerr << "env: '" << envStr << "'" << endl;
  cerr << "instance: '" << instanceStr << "'" << endl;
  cerr << "quiet: '" << isQuiet << "'" << endl;
#ifdef TERMCOLOR_HPP_
  cerr << "no_color: '" << isNoColor << "'" << endl;
#endif
#endif

  // Content
  string content{};

  if (hasStdIn) {
    std::string line;
    while (std::getline(std::cin, line)) {
      content += line + '\n';
    }
  } else {
    // Open input file.
    std::ifstream ifile(inputFilePath, std::ios::binary | std::ios::ate);
    if (ifile.bad()) {
      string errMsg{"Could not open file: " + inputFilePath};
#ifdef TERMCOLOR_HPP_
      if (isNoColor) {
        cerr << "ERROR: " << errMsg << endl;
      } else {
        cerr << termcolor::on_red << termcolor::white << "ERROR: "
             << errMsg << termcolor::reset << endl;
      }
#else
      cerr << "ERROR: " << errMsg << endl;
#endif
      return 1;
    }
    auto fsize = ifile.tellg();
    if (fsize == -1) {
      string errMsg{"Could not open file: " + inputFilePath};
#ifdef TERMCOLOR_HPP_
      if (isNoColor) {
        cerr << "ERROR: " << errMsg << endl;
      } else {
        cerr << termcolor::on_red << termcolor::white << "ERROR: "
             << errMsg << termcolor::reset << endl;
      }
#else
      cerr << "ERROR: " << errMsg << endl;
#endif
      return 1;
    }
    ifile.seekg(0, std::ios::beg);

    // Input Buffer
    std::vector<char> buffer(fsize);

    // Read input file.
    if (!ifile.read(buffer.data(), fsize)) {
      string errMsg{"Could not read input file."};
#ifdef TERMCOLOR_HPP_
      if (isNoColor) {
        cerr << "ERROR: " << errMsg << endl;
      } else {
        cerr << termcolor::on_red << termcolor::white << "ERROR: "
             << errMsg << termcolor::reset << endl;
      }
#else
      cerr << "ERROR: " << errMsg << endl;
#endif
    }
    ifile.close();

    // Create string from buffer.
    content.assign(buffer.begin(), buffer.end());
  }

  // Regexp
  const std::regex search(searchRegexpStr);

  // Collect env vars.
  std::unordered_map<string, EnvVar> envMap;

  // https://stackoverflow.com/a/2085385/823644
  for (char** env = envp; *env != nullptr; ++env) {
    const string envs{*env};
    auto pos = envs.find_first_of('=');
    const auto name = envs.substr(0, pos);
    const auto val = envs.substr(pos + 1);

    if (std::regex_search(name, search)) {
      envMap.emplace(std::make_pair(name, EnvVar{name, val}));

#ifdef DEBUG
      cerr << "collect val='" << name << "' val='" << val << "'" << endl;
#endif
    }
  }

  for (const auto& tmpEnv : envMap) {
    auto env = tmpEnv.second;
    auto val = env.val;
#ifdef DEBUG
    cerr << "val='" << env.name << "' val='" << val << "' (" << env.ignore << ")" << endl;
#endif

    if (env.ignore) {
      continue;
    }

    // Instance
    if (!instanceStr.empty()) {
      const auto pos = env.name.length() - instanceStr.length();
      const auto instanceSubstr = env.name.substr(pos);

#ifdef DEBUG
      cerr << " -> test instance: '" << instanceSubstr << "' (" << pos << ")" << endl;
#endif

      if (instanceSubstr == instanceStr) {
        env.name = env.name.substr(0, pos - 1);
#ifdef DEBUG
        cerr << "   -> OK" << endl;
#endif
      }
    }

    // Env
    if (!envStr.empty()) {
      const auto pos = env.name.length() - envStr.length();
      const auto envSubstr = env.name.substr(pos);
#ifdef DEBUG
      cerr << " -> test env: '" << envSubstr << "' (" << pos << ")" << endl;
#endif

      if (envSubstr == envStr) {
        env.name = env.name.substr(0, pos - 1);
#ifdef DEBUG
        cerr << "   -> OK" << endl;
#endif
      }
    }

    const auto envKey{env.name + '_' + envStr};
    const auto instanceKey{envKey + '_' + instanceStr};

    if (envMap.count(instanceKey)) {
#ifdef DEBUG
      cerr << " -> has instance key" << endl;
#endif
      envMap[instanceKey].ignore = true;
      val = envMap[instanceKey].val;
    } else if (envMap.count(envKey)) {
#ifdef DEBUG
      cerr << " -> has env key" << endl;
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
    cerr << " -> '" << env.name << "' '" << envKey << "' '" << instanceKey << "' => '" << val << "'" << endl;
#endif

    const string searchName{"@" + env.name + "@"};
    boost::replace_all(content, searchName, val);
  }

  if (outputFilePath.empty()) {
    cout << content;
  } else {
    // Save Month file.
    std::ofstream fout(outputFilePath);
    fout << content;
    fout.close();
  }

  // Remove 'begin of line' character.
  auto searchEndRegexpStr = searchRegexpStr;
  searchEndRegexpStr.erase(std::remove(searchEndRegexpStr.begin(), searchEndRegexpStr.end(), '^'),
    searchEndRegexpStr.end());

  // Regexp
  const std::regex endSearch('@' + searchEndRegexpStr);

  if (!isQuiet && std::regex_search(content, endSearch)) {
#ifdef TERMCOLOR_HPP_
    if (isNoColor) {
      cerr << "ERROR: " << "Cannot build configuration file; Missing ENV variables." << endl;
    } else {
      cerr << termcolor::on_red << termcolor::white << "ERROR: "
           << "Cannot build configuration file; Missing ENV variables." << termcolor::reset << endl;
    }
#else
    cerr << "ERROR: " << "Cannot build configuration file; Missing ENV variables." << endl;
#endif
    return 1;
  }

  return 0;
}
