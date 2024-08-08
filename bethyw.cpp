


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  Contains all the helper functions for initialising and running
  Beth Yw?
*/

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "lib_json.hpp"
#include "lib_cxxopts.hpp"

#include "areas.h"
#include "datasets.h"
#include "bethyw.h"
#include "input.h"

/*
  Run Beth Yw?, parsing the command line arguments, importing the data,
  and outputting the requested data to the standard output/error.

  @param argc
    Number of program arguments

  @param argv
    Program arguments

  @return
    Exit code
*/
int BethYw::run(int argc, char *argv[]) {
  auto cxxopts = BethYw::cxxoptsSetup();
  auto args = cxxopts.parse(argc, argv);

  // Print the help usage if requested
  if (args.count("help")) {
    std::cerr << cxxopts.help() << std::endl;
    return 0;
  }

  // Parse data directory argument
  std::string dir = args["dir"].as<std::string>() + DIR_SEP;
  try {
    std::string exampleFile = "areas.csv";
    std::string inputFilePath = dir + exampleFile;
    InputFile inputFile(inputFilePath);
    inputFile.open();
  } catch (const std::exception &e) {
    std::cerr << "Error importing dataset: " << std::endl;
    std::cerr << e.what();
    std::cerr.flush();
    abort();
  }

  // Parse other arguments and import data
  auto datasetsToImport = BethYw::parseDatasetsArg(args);
  auto areasFilter      = BethYw::parseAreasArg(args);
  auto measuresFilter   = BethYw::parseMeasuresArg(args);
  auto yearsFilter      = BethYw::parseYearsArg(args);

  Areas data = Areas();

  BethYw::loadAreas(data, dir, areasFilter);
  
  BethYw::loadDatasets(data,
                       dir,
                       datasetsToImport,
                       areasFilter,
                       measuresFilter,
                       yearsFilter);

  if (args.count("json")) {
    // The output as JSON
    std::cout << data.toJSON() << std::endl;
  } else {
    // The output as tables
    std::cout << data << std::endl;
  }

  return 0;
}

/*
  Sets up and returns a valid cxxopts object.

  @return
     A constructed cxxopts object

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);
*/
cxxopts::Options BethYw::cxxoptsSetup() {
  cxxopts::Options cxxopts(
        "bethyw",
        "Student ID: " + STUDENT_NUMBER + "\n\n"
        "This program is designed to parse official Welsh Government"
        " statistics data files.\n");
    
  cxxopts.add_options()(
      "dir",
      "Directory for input data passed in as files",
      cxxopts::value<std::string>()->default_value("datasets"))(

      "d,datasets",
      "The dataset(s) to import and analyse as a comma-separated list of codes "
      "(omit or set to 'all' to import and analyse all datasets)",
      cxxopts::value<std::vector<std::string>>())(

      "a,areas",
      "The areas(s) to import and analyse as a comma-separated list of "
      "authority codes (omit or set to 'all' to import and analyse all areas)",
      cxxopts::value<std::vector<std::string>>())(

      "m,measures",
      "Select a subset of measures from the dataset(s) "
      "(omit or set to 'all' to import and analyse all measures)",
      cxxopts::value<std::vector<std::string>>())(

      "y,years",
      "Focus on a particular year (YYYY) or "
      "inclusive range of years (YYYY-ZZZZ)",
      cxxopts::value<std::string>()->default_value("0"))(

      "j,json",
      "Print the output as JSON instead of tables.")(

      "h,help",
      "Print usage.");

  return cxxopts;
}

/*
  Parses the datasets argument passed into the command line. 

  The datasets argument is optional, and if it is not included, all datasets 
  should be imported. If it is included, it should be a comma-separated list of 
  datasets to import. If the argument contains the value "all"
  (case-insensitive), all datasets should be imported.

  Also validates the passed in dataset names against the codes in
  DATASETS array in the InputFiles namespace in datasets.h. If an invalid code
  is entered, std::invalid_argument is thrown with the message:
  No dataset matches key: <input code>
  where <input name> is the name supplied by the user through the argument.

  @param args
    Parsed program arguments

  @return
    A std::vector of BethYw::InputFileSource instances to import

  @throws
    std::invalid_argument if the argument contains an invalid dataset with
    message: No dataset matches key <input code>

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);

    auto datasetsToImport = BethYw::parseDatasetsArg(args);
 */
std::vector<BethYw::InputFileSource> BethYw::parseDatasetsArg(
    cxxopts::ParseResult& args) {

  // Retrieve all valid datasets, see datasets.h
  size_t numDatasets = InputFiles::NUM_DATASETS;
  auto &allDatasets = InputFiles::DATASETS;


  // Create the container for the return type
  std::vector<InputFileSource> datasetsToImport;

  // If the datasets tag is passed as a command-line argument...:
  if (args.count("datasets")) {
    std::vector<std::string> datasetList;
    for (unsigned int i = 0; i < numDatasets; i++) {
      datasetList.push_back(allDatasets[i].CODE);
    }

    // Obtain the command-line arguments pertaining to the mentioned tag
    auto inputDatasets = args["datasets"].as<std::vector<std::string>>();

    // Check if tag contains the value 'all'
    std::string firstVal = inputDatasets[0];
    std::transform(firstVal.begin(), firstVal.end(), firstVal.begin(), [](unsigned char c) {return std::tolower(c);});
    if (firstVal == "all") {
      // Include all datasets in the return statement
      for (unsigned int i = 0; i < numDatasets; i++) {
        datasetsToImport.push_back(allDatasets[i]);
      }
    } else {
      // Compare each element obtained from the command-line arguments against the list of possible datasets
      for (unsigned int i = 0; i < inputDatasets.size(); i++) {
        auto element = find(datasetList.begin() ,datasetList.end(), inputDatasets[i]);
        // If element exists
        if (element != datasetList.end()) {
          int index = element - datasetList.begin();
          datasetsToImport.push_back(allDatasets[index]);
        } else { // If element does not exist
          try {
            throw std::logic_error("No dataset matches key: " + inputDatasets[i]);
          } catch (const std::logic_error &e) {
            std::cerr << e.what();
            std::cerr.flush();
            abort();
          }
        }
      }
    }
  } else { // If the datasets tag is not passed as a command-line argument...:
    // Include all datasets in the return statement to follow default clause
    for (unsigned int i = 0; i < numDatasets; i++) {
      datasetsToImport.push_back(allDatasets[i]);
    }
  }

  return datasetsToImport;
}

/*
  Parses the areas command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all areas should be
  imported, i.e., the filter should be an empty set.

  Unlike datasets validity of the values can't be checked as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, simply fetch the list of areas and later pass it to the
  Areas::populate() function.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific areas
    to import, or an empty set if all areas should be imported.

  @throws
    std::invalid_argument if the argument contains an invalid areas value with
    message: Invalid input for area argument
*/
std::unordered_set<std::string> BethYw::parseAreasArg(
    cxxopts::ParseResult& args) {
  // The unordered set you will return
  std::unordered_set<std::string> areasToReturn;

  // Read CSV file to obtain area codes
  std::vector<std::vector<std::string>> content;
  std::vector<std::string> row;
  std::string line, word;

  std::fstream file ("datasets/areas.csv", std::ios::in);
  if (file.is_open()) {
    while (getline(file, line)) {
      row.clear();
      std::stringstream sr(line);
      while (getline(sr, word, ',')) {
        row.push_back(word);
      }
      content.push_back(row);
    }
  } else {
    try {
      throw std::logic_error("File: datasets/areas.csv could not be opened.");
    } catch (const std::logic_error &e) {
      std::cerr << e.what();
      std::cerr.flush();
      abort();
    }
  }

  // Create a list of area codes
  std::vector<std::string> areaCodes;
  for (unsigned int i = 1; i < content.size(); i++) {
    areaCodes.push_back(content[i][0]);
  }


  // If the below tag is passed as a command-line argument...:
  if (args.count("areas")) {
    // Obtain the values passed with the tag
    auto inputAreas = args["areas"].as<std::vector<std::string>>();
    std::string firstVal = inputAreas[0];
    std::transform(firstVal.begin(), firstVal.end(), firstVal.begin(), [](unsigned char c) {return std::tolower(c);});
    // If the first value is all then import all lists
    if (firstVal == "all") {
      for (unsigned int i = 0; i < areaCodes.size(); i++) {
        areasToReturn.insert(areaCodes[i]);
      }
      return areasToReturn;
    } else { // If not, parse through the parameters and check if they are a real area code
      for (unsigned int i = 0; i < inputAreas.size(); i++) {
        auto element = find(areaCodes.begin() ,areaCodes.end(), inputAreas[i]);
        if (element != areaCodes.end()) { // If element exists...:
          int index = element - areaCodes.begin();
          areasToReturn.insert(areaCodes[index]);
        } else { // Error
          // throw std::logic_error("Area with name: " + inputAreas[i] + " not found");
          throw std::invalid_argument("Invalid input for area argument");
        }
      }
    }


  }
  return areasToReturn;
}

/*
  Parses the measures command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all measures should
  be imported.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific measures
    to import, or an empty set if all measures should be imported.

  @throws
    std::invalid_argument if the argument contains an invalid measures value
    with the message: Invalid input for measures argument
*/
std::unordered_set<std::string> BethYw::parseMeasuresArg(
  cxxopts::ParseResult& args) {

    // Construct variable to be returned at the head of this scope
    std::unordered_set<std::string> measuresToReturn;

    // Construct variable that stores all measures
    std::vector<std::string> allMeasures;

    // Obtain raw data of all measures importable
    std::ifstream popu1009JSON("datasets/popu1009.json");
    std::string popuRawData((std::istreambuf_iterator<char>(popu1009JSON)), std::istreambuf_iterator<char>());
    popu1009JSON.close();

    std::ifstream econ0080JSON("datasets/econ0080.json");
    std::string econRawData((std::istreambuf_iterator<char>(econ0080JSON)), std::istreambuf_iterator<char>());
    econ0080JSON.close();

    std::ifstream envi0201JSON("datasets/envi0201.json");
    std::string enviRawData((std::istreambuf_iterator<char>(envi0201JSON)), std::istreambuf_iterator<char>());
    envi0201JSON.close();

    std::ifstream tran0152JSON("datasets/tran0152.json");
    std::string tranRawData((std::istreambuf_iterator<char>(tran0152JSON)), std::istreambuf_iterator<char>());
    tran0152JSON.close();

    // Convert raw data strings to JSON objects
    nlohmann::json popuJSON = nlohmann::json::parse(popuRawData);
    nlohmann::json econJSON = nlohmann::json::parse(econRawData);
    nlohmann::json enviJSON = nlohmann::json::parse(enviRawData);
    nlohmann::json tranJSON = nlohmann::json::parse(tranRawData);

    // Store unique measures into a list

    // popson unique measures
    for (auto determinent : popuJSON["value"]) {
      auto element = find(allMeasures.begin() ,allMeasures.end(), determinent["Measure_Code"]);
        // If element isn't in list
        if (element == allMeasures.end()) {
          std::string measureCode = determinent["Measure_Code"];
          allMeasures.push_back(measureCode);
        }
    }

    // econ unique measures
    for (auto determinent : econJSON["value"]) {
      auto element = find(allMeasures.begin() ,allMeasures.end(), determinent["Variable_Code"]);
        // If element isn't in list
        if (element == allMeasures.end()) {
          std::string measureCode = determinent["Variable_Code"];
          allMeasures.push_back(measureCode);
        }
    }

    // envi unique measures
    for (auto determinent : enviJSON["value"]) {
      auto element = find(allMeasures.begin() ,allMeasures.end(), determinent["Pollutant_ItemName_ENG"]);
        // If element isn't in list
        if (element == allMeasures.end()) {
          std::string measureCode = determinent["Pollutant_ItemName_ENG"];
          allMeasures.push_back(measureCode);
        }
    }

    // Add hard coded unique measure for tran
    std::string tranMeasureCode = "rail";
    allMeasures.push_back(tranMeasureCode);

    // Convert allMeasures vector to lower case
    std::transform(allMeasures.begin(), allMeasures.end(), allMeasures.begin(),
                   [](std::string s) {
                       std::transform(s.begin(), s.end(), s.begin(),
                                      [](unsigned char c) {
                                          return std::tolower(c);
                                      });
                       return s;
                   });

    // Observe value passed this tag
    if (args.count("measures")) {

      // Obtain the command-line arguments pertaining to this tag
      auto inputMeasures = args["measures"].as<std::vector<std::string>>();

      // Convert command-line arguments to lower case
      std::transform(inputMeasures.begin(), inputMeasures.end(), inputMeasures.begin(),
                   [](std::string s) {
                       std::transform(s.begin(), s.end(), s.begin(),
                                      [](unsigned char c) {
                                          return std::tolower(c);
                                      });
                       return s;
                   });

      // Iterate through each command-line argument
      for (unsigned int i = 0; i < inputMeasures.size(); i++) {
        if (inputMeasures[i] == "all") {
          measuresToReturn.clear();
          for (unsigned int i = 0; i < allMeasures.size(); i++) {
            measuresToReturn.insert(allMeasures[i]);
          }
          return measuresToReturn;
        } else {
          auto element = find(allMeasures.begin() ,allMeasures.end(), inputMeasures[i]);
          // If element exists
          if (element != allMeasures.end()) {
            int index = element - allMeasures.begin();
            measuresToReturn.insert(allMeasures[index]);
          } else { // If element does not exist
            // throw std::invalid_argument("Invalid input for measures argument");
            throw std::invalid_argument("Invalid input for measures argument");
          }
        }
      }

    } else { // If not passed, return all measures
      for (unsigned int i = 0; i < allMeasures.size(); i++) {
        measuresToReturn.insert(allMeasures[i]);
      }
      return measuresToReturn;
    }


    return measuresToReturn;
}


/*
  Parses the years command line argument. Years is either a four digit year 
  value, or two four digit year values separated by a hyphen (i.e. either 
  YYYY or YYYY-ZZZZ).

  This should be parsed as two integers and inserted into a std::tuple,
  representing the start and end year (inclusive). If one or both values are 0,
  then there is no filter to be applied. If no year argument is given return
  <0,0> (i.e. to import all years).

  @param args
    Parsed program arguments

  @return
    A std::tuple containing two unsigned ints

  @throws
    std::invalid_argument if the argument contains an invalid years value with
    the message: Invalid input for years argument
*/
std::tuple<unsigned int, unsigned int> BethYw::parseYearsArg(cxxopts::ParseResult& args) {
  // Initialize the start and end years to 0
  unsigned int startYear = 0;
  unsigned int endYear = 0;

  // Check if the "years" argument is provided
  if (args.count("years")) {
    std::string yearsInput = args["years"].as<std::string>();
    std::regex yearPattern("(\\d{4})(?:-(\\d{4}))?");

    std::smatch match;
    if (std::regex_match(yearsInput, match, yearPattern)) {
      startYear = std::stoi(match[1]);
      if (match[2].length() > 0) {
        endYear = std::stoi(match[2]);
      } else {
        endYear = startYear;
      }
    } else {
      // throw std::invalid_argument("Invalid input for years argument");
      try {
        throw std::invalid_argument("Invalid input for years argument");
      } catch (const std::invalid_argument &e) {
        std::cerr << e.what();
        std::cerr.flush();
        abort();
      }
    }
  }

  return std::make_tuple(startYear, endYear);
}

/*
  Loads the areas.csv file from the directory `dir`.

  @param areas
    An Areas instance that should be modified (i.e. the populate() function
    in the instance should be called)

  @param dir
    Directory where the areas.csv file is

  @param areasFilter
    An unordered set of areas to filter, or empty to import all areas

  @return
    void

  @example
    Areas areas();

    BethYw::loadAreas(areas, "data", BethYw::parseAreasArg(args));
*/
void BethYw::loadAreas(
    Areas &areas,
    const std::string &dir,
    const std::unordered_set<std::string> &areasFilter) {

  // Access the SourceColumnMapping instance from the InputFiles::AREAS.COLS
  const SourceColumnMapping &cols = InputFiles::AREAS.COLS;

  // Load the file and call the populateFromAuthorityCodeCSV function
  InputFile inputFile("datasets/areas.csv");
  std::istream &fileStream = inputFile.open();
  
  // Convert the unordered_set to a StringFilterSet (if required)
  StringFilterSet areasFilterSet(areasFilter.begin(), areasFilter.end());

  areas.populateFromAuthorityCodeCSV(fileStream, cols, &areasFilterSet);
}


/*
  Imports datasets from `datasetsToImport` as files in `dir` into areas, and
  filtering them with the `areasFilter`, `measuresFilter`, and `yearsFilter`.

  The actual filtering will be done by the Areas::populate() function, thus 
  this function serves to merely pass pointers on to these flters.

  @param areas
    An Areas instance that should be modified (i.e. datasets loaded into it)

  @param dir
    The directory where the datasets are

  @param datasetsToImport
    A vector of InputFileSource objects

  @param areasFilter
    An unordered set of areas (as authority codes encoded in std::strings)
    to filter, or empty to import all areas

  @param measuresFilter
    An unordered set of measures (as measure codes encoded in std::strings)
    to filter, or empty to import all measures

  @param yearsFilter
    An two-pair tuple of unsigned ints corresponding to the range of years 
    to import, which should both be 0 to import all years.

  @return
    void

  @example
    Areas areas();

    BethYw::loadDatasets(
      areas,
      "data",
      BethYw::parseDatasetsArgument(args),
      BethYw::parseAreasArg(args),
      BethYw::parseMeasuresArg(args),
      BethYw::parseYearsArg(args));
*/
void BethYw::loadDatasets(
    Areas &areas,
    const std::string &dir,
    const std::vector<BethYw::InputFileSource> &datasetsToImport,
    const std::unordered_set<std::string> &areasFilter,
    const std::unordered_set<std::string> &measuresFilter,
    const std::tuple<unsigned int, unsigned int> &yearsFilter) {

  StringFilterSet areasFilterSet(areasFilter.begin(), areasFilter.end());
  StringFilterSet measuresFilterSet(measuresFilter.begin(), measuresFilter.end());

  for (const auto &dataset : datasetsToImport) {
    std::string inputFilePath = dir + dataset.FILE;
    InputFile inputFile(inputFilePath);
    try {
      std::istream &fileStream = inputFile.open();
      const SourceColumnMapping &cols = dataset.COLS;
      const SourceDataType &type = dataset.PARSER;

      areas.populate(
        fileStream,
        type,
        cols,
        &areasFilterSet,
        &measuresFilterSet,
        &yearsFilter);
    } catch (const std::out_of_range &e) {
      std::cerr << "Key not found in map: " << std::endl;
      std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Error importing dataset: " << std::endl;
      std::cerr << e.what() << std::endl;
    }
  }
}
