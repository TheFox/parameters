
#include <cstdio>
#include <string>
// #include <unistd.h>
#include <iostream>

#ifdef __has_include
#  if __has_include(<boost/program_options.hpp>)
#    include <boost/program_options.hpp>
#  else
#    error "Missing <program_options>"
#  endif
#endif // __has_include

#include "../config.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* const argv[])
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
  options_description genericOpts("Generic options");
  genericOpts.add_options()
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
  if (vm.count("help")) {
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

  puts("OK");
}
