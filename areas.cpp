


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now. Areas is also
  responsible for importing data from a stream (using the various
  populate() functions) and creating the Area and Measure objects.
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <sstream>

#include "lib_json.hpp"

#include "datasets.h"
#include "areas.h"
#include "measure.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  Constructor for an Areas object.
*/
Areas::Areas() {

}

/*
  Add a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param localAuthorityCode
    The local authority code of the Area

  @param area
    The Area object that will contain the Measure objects

  @return
    void

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
*/
void Areas::setArea(const std::string &localAuthorityCode, Area area) {
  // check if the area already exists
  auto it = areas.find(localAuthorityCode);
  if (it != areas.end()) {
    // if it exists, update the existing area with the new area's data
    Area& existingArea = it->second;
    existingArea = area;
  } else {
    // if it doesn't exist, insert the new area into the map
    areas.emplace(localAuthorityCode, std::move(area));
  }
}



/*
  Retrieve an Area instance with a given local authority code.

  @param localAuthorityCode
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    ...
    Area area2 = areas.getArea("W06000023");
*/
Area& Areas::getArea(const std::string& localAuthorityCode) {
    auto it = areas.find(localAuthorityCode);
    if (it == areas.end()) {
        throw std::out_of_range("Area not found");
    }
    return it->second;
}



/*
  Retrieve the number of Areas within the container. Should be callable
  from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The number of Area instances

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    
    auto size = areas.size(); // returns 1
*/
std::size_t Areas::size() const {
    return areas.size();
}



/*
  This function specifically parses the compiled areas.csv file of local 
  authority codes, and their names in English and Welsh. Once parsed,
  create the appropriate Area objects and insert them in to a Standard
  Library container within Areas.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @example
    InputFile input("data/areas.csv");
    auto is = input.open();

    auto cols = InputFiles::AREAS.COLS;

    auto areasFilter = BethYw::parseAreasArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(
    std::istream &is,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter) {

  std::string line;

  // Skip the header line
  std::getline(is, line);

  // Read each line from the input stream
  while (std::getline(is, line)) {
    std::istringstream ss(line);
    std::string token;

    std::vector<std::string> tokens;
    while (std::getline(ss, token, ',')) {
      tokens.push_back(token);
    }

    // Check if there are enough columns in the CSV
    if (tokens.size() < 3) {
      throw std::out_of_range("Not enough columns in the CSV file.");
    }

    // Extract data from tokens
    std::string authorityCode = tokens.at(0);
    std::string authorityNameEng = tokens.at(1);
    std::string authorityNameCym = tokens.at(2);

    // Check if the area should be imported based on areasFilter
    if (areasFilter != nullptr && !areasFilter->empty()) {
      if (areasFilter->find(authorityCode) == areasFilter->end()) {
        continue;
      }
    }

    // Create Area object and insert it into the Areas container
    Area area(authorityCode);
    area.setName("eng", authorityNameEng);
    area.setName("cym", authorityNameCym);

    this->insertArea(area);
  }
}

void Areas::insertArea(const Area& area) {
    areas.insert({area.getLocalAuthorityCode(), area});
}

/*
  Parse the JSON datasets, extracting the local authority code, English name,
  and each measure by year.

  If an Area that does not exist in the Areas container is found, create the
  Area object.

  If areasFilter is a non-empty set, only include areas matching the filter. If
  measuresFilter is a non-empty set, only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populateFromWelshStatsJSON(
      is,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populateFromWelshStatsJSON(std::istream &is,
                                       const BethYw::SourceColumnMapping &cols,
                                       const StringFilterSet * const areasFilter,
                                       const StringFilterSet * const measuresFilter,
                                       const YearFilterTuple * const yearsFilter) {
  // Parse JSON data from the input stream
  json j;
  is >> j;

  for (auto& el : j["value"].items()) {
    auto &data = el.value();

    std::map<BethYw::SourceColumn, std::string> mappedData;
    for (const auto &col : cols) {
      if (data[col.second].is_null()) {
        continue;
      } else if (data[col.second].is_number()) {
        mappedData[col.first] = std::to_string(data[col.second].get<double>());
      } else {
        mappedData[col.first] = data[col.second];
      }
    }

    bool isSingleMeasure = true;
    for (const auto& element : mappedData) {
      if (element.first == 3) {
        isSingleMeasure = false;
        break;
      }
    }

    std::string localAuthorityCode = mappedData[BethYw::AUTH_CODE];
    std::string localAuthorityNameEng = mappedData[BethYw::AUTH_NAME_ENG];

    // Skip areas NOT in filter
    if (!areasFilter->empty() && areasFilter->find(localAuthorityCode) == areasFilter->end()) {
      continue;
    }

    std::string measureCode;
    std::string measureLabel;

    if (isSingleMeasure) {
      measureCode = BethYw::InputFiles::TRAINS.COLS.at(BethYw::SINGLE_MEASURE_CODE);
      measureLabel = BethYw::InputFiles::TRAINS.COLS.at(BethYw::SINGLE_MEASURE_NAME);
    } else {
      measureCode = mappedData[BethYw::MEASURE_CODE];
      measureLabel = mappedData[BethYw::MEASURE_NAME];
    }

    std::transform(measureCode.begin(), measureCode.end(), measureCode.begin(), [](unsigned char c) { return std::tolower(c); });
    std::transform(measureLabel.begin(), measureLabel.end(), measureLabel.begin(), [](unsigned char c) { return std::tolower(c); });

    // Skip measures NOT in filter
    if (!measuresFilter->empty() && measuresFilter->find(measureCode) == measuresFilter->end()) {
      continue;
    }

    std::string yearString = mappedData[BethYw::YEAR];
    unsigned int year = std::stoi(yearString);

    // Skip years NOT in filter
    if ((std::get<0>(*yearsFilter) == 0 && std::get<1>(*yearsFilter) == 0) ||
          (year >= std::get<0>(*yearsFilter) && year <= std::get<1>(*yearsFilter))) {
      std::string valueString = mappedData[BethYw::VALUE];
      Measure measure = Measure(measureCode, measureLabel);
      measure.setValue(year, std::stod(valueString));

      Area area(localAuthorityCode);
      area.setName("eng", localAuthorityNameEng);
      insertArea(area);

      areas[localAuthorityCode].setMeasure(measureCode, measure);
    }
  }
}



/*
  Import CSV files that contain a single measure.

  Datasets to be parsed:
   - complete-popu1009-area.csv
   - complete-popu1009-pop.csv
   - complete-popu1009-opden.csv

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of strings for measures to import, or an empty 
    set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @example
    InputFile input("data/complete-popu1009-pop.csv");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["complete-pop"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto yearsFilter = BethYw::parseYearsArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter, &yearsFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityByYearCSV(
    std::istream &is,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter,
    const StringFilterSet * const measuresFilter,
    const YearFilterTuple * const yearsFilter) {

  std::string line;

  // Skip the header line
  std::getline(is, line);

  std::istringstream header_ss(line);
  std::vector<unsigned int> years;
  std::string year_token;
  std::getline(header_ss, year_token, ','); // Skip first col a.k.a authority code
  while (std::getline(header_ss, year_token, ',')) {
      years.push_back(std::stoi(year_token));
  }

  const auto& firstElement = *cols.begin();

  std::string measureCode;
  std::string measureLabel;

  for (const auto& dataset : BethYw::InputFiles::DATASETS) {
    if (dataset.PARSER == BethYw::SourceDataType::AuthorityByYearCSV &&
        dataset.COLS.at(BethYw::SINGLE_MEASURE_NAME) == firstElement.second) {
      measureCode = dataset.COLS.at(BethYw::SINGLE_MEASURE_CODE);
      measureLabel = dataset.COLS.at(BethYw::SINGLE_MEASURE_NAME);
      break;
    }
  }

  // Read each line from the input stream
  while (std::getline(is, line)) {
    std::istringstream ss(line);
    std::string token;

    std::vector<std::string> tokens;
    while (std::getline(ss, token, ',')) {
      tokens.push_back(token);
    }

    // Extract data from tokens
    std::string localAuthorityCode = tokens.at(0);

    // Check if the area should be imported based on areasFilter
    if (areasFilter != nullptr && !areasFilter->empty()) {
      if (areasFilter->find(localAuthorityCode) == areasFilter->end()) {
        continue;
      }
    }

    // Create the Measure object
    Measure measure(measureCode, measureLabel);

    // Iterate through the rest of the tokens (year-value pairs)
    for (size_t i = 1, year_idx = 0; i < tokens.size(); i += 1, year_idx += 1) {
      unsigned int year = years.at(year_idx);
      double value = std::stod(tokens.at(i));

      // Check if the year should be imported based on yearsFilter
      if (yearsFilter != nullptr &&
          std::get<0>(*yearsFilter) != 0 &&
          std::get<1>(*yearsFilter) != 0) {
        if (year < std::get<0>(*yearsFilter) || year > std::get<1>(*yearsFilter)) {
          continue;
        }
      }

      // Add the year-value pair to the Measure object
      measure.setValue(year, value);
    }

    // Insert the Measure object into the corresponding Area object
    this->areas.at(localAuthorityCode).setMeasure(measureCode, measure);
  }
}




/*
  Parse data from an standard input stream `is`, that has data of a particular
  `type`, and with a given column mapping in `cols`. By looking at the `type`,
  hand off to one of the three functions populate………() functions.

  Must also check if the stream is in working order and has content.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols);
*/
void Areas::populate(std::istream &is,
                     const BethYw::SourceDataType &type,
                     const BethYw::SourceColumnMapping &cols) {
  if (!is) {
    throw std::runtime_error("Input stream is not open or not in a valid state");
  }

  if (is.peek() == std::istream::traits_type::eof()) {
    throw std::runtime_error("Input stream is empty");
  }

  const StringFilterSet emptyFilter;
  const YearFilterTuple emptyYearFilter;

  if (type == BethYw::AuthorityCodeCSV) {
    populateFromAuthorityCodeCSV(is, cols, &emptyFilter);
  } else if (type == BethYw::WelshStatsJSON) {
    populateFromWelshStatsJSON(is, cols, &emptyFilter, &emptyFilter, &emptyYearFilter);
  } else if (type == BethYw::AuthorityByYearCSV) {
    populateFromAuthorityByYearCSV(is, cols, &emptyFilter, &emptyFilter, &emptyYearFilter);
  } else {
    throw std::runtime_error("Areas::populate: Unexpected data type");
  }
}



/*
  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  Given a `type`, hand off to one of the three functions populate() methods.
  Must also check if the stream is in working order and has content.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populate(
    std::istream &is,
    const BethYw::SourceDataType &type,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter,
    const StringFilterSet * const measuresFilter,
    const YearFilterTuple * const yearsFilter) {
  if (!is) {
    throw std::runtime_error("Input stream is not open or not in a valid state");
  }

  if (is.peek() == std::istream::traits_type::eof()) {
    throw std::runtime_error("Input stream is empty");
  }

  if (type == BethYw::AuthorityCodeCSV) {
    populateFromAuthorityCodeCSV(is, cols, areasFilter);
  } else if (type == BethYw::WelshStatsJSON) {
    populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter, yearsFilter);
  } else if (type == BethYw::AuthorityByYearCSV) {
    populateFromAuthorityByYearCSV(is, cols, areasFilter, measuresFilter, yearsFilter);
  } else {
    throw std::runtime_error("Areas::populate: Unexpected data type");
  }
}

/*
  Convert an Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.
  
  @return
    std::string of JSON // `{}` is Areas is empty

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    std::cout << data.toJSON();
*/
std::string Areas::toJSON() const {
  json j;

  for (const auto &areaPair : areas) {
    const auto &localAuthorityCode = areaPair.first;
    const auto &area = areaPair.second;

    json areaJson;
    bool hasCym = true;
    try {
      area.getName("cym");
    } catch (const std::out_of_range &e) {
      hasCym = false;
    }
    areaJson["names"]["eng"] = area.getName("eng");
    if (hasCym) {
      areaJson["names"]["cym"] = area.getName("cym");
    }

    json measuresJson;
    for (const auto &measurePair : area.getMeasures()) {
      const auto &measure = measurePair.second;
      const auto &measureCode = measure.getCodename();

      json yearsJson;
      for (const auto &yearPair : measure.getYears()) {
        const auto &year = yearPair.first;
        const auto &value = yearPair.second;
        yearsJson[std::to_string(year)] = value;
      }

      measuresJson[measureCode] = yearsJson;

    }

    areaJson["measures"] = measuresJson;
    j[localAuthorityCode] = areaJson;
  }

  return j.dump();
}

/*
  Overloaded << operator to print all of the imported data.

  Areas should be printed and ordered alphabetically by their local authority code.
  Measures within each Area should be ordered alphabetically by their codename.

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream

  @example
    Areas areas();
    std::cout << areas << std::end;
*/
std::ostream& operator<<(std::ostream& os, const Areas& areas) {
  for (const auto& areaPair : areas.areas) {
    const auto& areaCode = areaPair.first;
    const auto& area = areaPair.second;

    bool hasCym = true;

    try {
      area.getName("cym");
    } catch (const std::out_of_range &e) {
      hasCym = false;
    }

    if (hasCym) {
      os << area.getName("eng") << " / " << area.getName("cym") << " (" << areaCode << ")\n";
    } else {
      os << area.getName("eng") << " (" << areaCode << ")\n";
    }

    if (area.getMeasures().empty()) {
      os << "<no measures>\n";
    } else {
      for (const auto& measurePair : area.getMeasures()) {
        const auto& measure = measurePair.second;

        os << measure.getLabel() << " (" << measure.getCodename() << ")\n";
        
        // Print years header
        for (const auto& yearPair : measure.getYears()) {
          os << std::setw(11) << yearPair.first;
        }
        os << std::setw(11) << "Average" << std::setw(11) << "Diff." << std::setw(11) << "% Diff.\n";

        // Print values for each year
        for (const auto& yearPair : measure.getYears()) {
          os << std::fixed << std::setprecision(6) << std::setw(11) << yearPair.second << " ";
        }

        // Print average, difference, and percentage difference
        os << std::fixed << std::setprecision(6) << std::setw(11) << measure.getAverage()
          << std::setw(11) << measure.getDifference()
          << std::setw(11) << measure.getDifferenceAsPercentage() << "\n\n";
      }
    }
  }
  return os;
}
