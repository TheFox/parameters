
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

#ifdef __has_include
#  if __has_include(<boost/program_options.hpp>)
#    include <boost/program_options.hpp>
#include <regex>
namespace bpo = boost::program_options;
#  else
#    error "Missing <program_options>"
#  endif
#endif // __has_include

#include "../config.hpp"

int main(int argc, char* const argv[], char** const envp)
{
  using std::string;
  using std::cout;
  using std::cerr;
  using std::endl;
  using bpo::options_description;
  using bpo::value;

#ifdef DEBUG
  puts("--- DEBUG ---");
  puts("");
#endif

  // Generic options
  options_description genericOpts("Options");
  genericOpts.add_options()
               ("input,i", value<string>()->value_name("path"), "Input path to template file. (required)")
               ("regexp,r", value<string>()->value_name("string"),
                 "Search regular expression for environment variable names. (required)")
               ("env,e", value<string>()->value_name("name"), "Name of the environment. For example: production")
               //("quiet,q", "Do not throw an error if there are variables missing being replaced.")
               ("help,h", "This message");

  // Common options
  // options_description commonOpts("Common options");
  // commonOpts.add_options()
  //             ("date,d", value<string>()->value_name("string"), "Set a Date. (Format: YYYY-MM-DD)");

  options_description opts;
  opts
    .add(genericOpts)
    // .add(commonOpts)
    ;

  auto parsedOptions = bpo::command_line_parser(argc, argv)
    .options(opts)
    .allow_unregistered()
    .run();

  bpo::variables_map vm{};
  bpo::store(parsedOptions, vm);
  bpo::notify(vm);

  // Help
  if (vm.count("help") || vm.count("input") == 0 || vm.count("regexp") == 0) {
    cout << PROJECT_NAME << ' '
         << PROJECT_VERSION_MAJOR << '.' << PROJECT_VERSION_MINOR << '.' << PROJECT_VERSION_PATCH
         << PROJECT_VERSION_APPENDIX << endl;
    cout << PROJECT_COPYRIGHT << endl << endl;

    cout << "Usage: " << argv[0] << " [options]" << endl << endl;
    // cout << endl;

    cout << genericOpts << endl;
    // cout << commonOpts << endl;

    return 3;
  }

  const auto inputFilePath = vm["input"].as<std::string>();
  const auto searchRegexpStr = vm["regexp"].as<std::string>();
  //const auto isQuiet = vm["quiet"].as<bool>();

  // Open input file.
  std::ifstream ifile(inputFilePath, std::ios::binary | std::ios::ate);
  auto fsize = ifile.tellg();
  ifile.seekg(0, std::ios::beg);

  // Read input file.
  std::vector<char> buffer(fsize);
  if (!ifile.read(buffer.data(), fsize)) {
    throw string{"Could not read input file."};
  }
  ifile.close();

  // Create string from buffer.
  string content(buffer.begin(), buffer.end());

  // Debug
  cout << content << endl;

  // Regexp
  // http://www.cplusplus.com/reference/regex/regex_replace/
  const std::regex search(searchRegexpStr);
  //const std::regex search("SSH_", std::regex_constants::syntax_option_type::egrep);
  //const std::regex search("SSH_", std::regex_constants::syntax_option_type::nosubs);
  //const std::regex search("^SSH_");
  //cout << std::regex_replace(content, search, "_OK") << endl;

  // https://stackoverflow.com/a/2085385/823644
  for (char** env = envp; *env != nullptr; ++env) {
    const string envs{*env};
    //char* thisEnv = *env;
    //printf("%s\n", thisEnv);
    //cout << envs << endl;
    auto pos = envs.find_first_of('=');
    const auto name = envs.substr(0, pos);
    //cout << name << endl;

    //std::regex_constants::match_continuous
    //std::regex_constants::match_not_bol
    if (std::regex_search(name, search)) {
      cout << name << endl;
    }
  }

  return 0;
}
