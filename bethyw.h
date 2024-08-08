#ifndef BETHYW_H_
#define BETHYW_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains declarations for the helper functions for initialising and
  running Beth Yw?
 */

#include <string>
#include <unordered_set>
#include <vector>

#include "lib_cxxopts.hpp"

#include "areas.h"
#include "datasets.h"

const char DIR_SEP =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

namespace BethYw {

const std::string STUDENT_NUMBER = "2009594";

/*
  Run Beth Yw?, parsing the command line arguments and acting upon them.
*/
int run(int argc, char *argv[]);

/*
  Create a cxxopts instance.
*/
cxxopts::Options cxxoptsSetup();

/*
  Parses the datasets argument and return a std::vector of all the datasets
  to import. InputFileSource is declared in datasets.h.
*/
std::vector<BethYw::InputFileSource> parseDatasetsArg(
  cxxopts::ParseResult& args);

/*
  Parses the areas argument and return a std::unordered_set of all the
  areas to import, or an empty set if all areas should be imported.
*/
std::unordered_set<std::string> parseAreasArg(cxxopts::ParseResult& args);

/*
  Parses the measures argument and return an std::unordered_set of all the
  measures to import, or an empty set if all measures should be imported.
*/
std::unordered_set<std::string> parseMeasuresArg(cxxopts::ParseResult& args);

/*
  Parses the years argument and return an std::unordered_set of all of the
  years to import, or an empty set if all years should be imported.
*/
std::tuple<unsigned int, unsigned int> parseYearsArg(cxxopts::ParseResult& args);

void loadAreas(
    Areas &areas,
    const std::string &dir,
    const std::unordered_set<std::string> &areasFilter);

void loadDatasets(Areas &areas,
                          const std::string &dir,
                          const std::vector<BethYw::InputFileSource> &datasetsToImport,
                          const std::unordered_set<std::string> &areasFilter,
                          const std::unordered_set<std::string> &measuresFilter,
                          const std::tuple<unsigned int, unsigned int> &yearsFilter);

} // namespace BethYw

#endif // BETHYW_H_