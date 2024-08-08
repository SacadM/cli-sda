


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains the implementation of the Measure class. Measure is a
  very simple class that needs to contain a few member variables for its name,
  codename, and a Standard Library container for data.
*/

#include <stdexcept>
#include <string>
#include <algorithm>

#include "measure.h"

/*
  Constructs a single Measure, that has values across many years.

  @param codename
    The codename for the measure

  @param label
    Human-readable label for the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);
*/
Measure::Measure(std::string codename, const std::string &label) {
  std::transform(codename.begin(), codename.end(), codename.begin(), ::tolower);
  this->codename = codename;
  this->label = label;
}


/*
  Retrieves the code for the Measure. This function should be callable from a 
  constant context and must promise to not modify the state of the instance or 
  throw an exception.

  @return
    The codename for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto codename2 = measure.getCodename();
*/
std::string Measure::getCodename() const {
  return codename;
}


/*
  Retrieve the human-friendly label for the Measure. This function should be 
  callable from a constant context and must promise to not modify the state of 
  the instance and to not throw an exception.

  @return
    The human-friendly label for the Measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto label = measure.getLabel();
*/
std::string Measure::getLabel() const {
  return label;
}


/*
  Changes the label for the Measure.

  @param label
    The new label for the Measure

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    ...
    measure.setLabel("New Population");
*/
void Measure::setLabel(std::string newlabel) {
  this->label = newlabel;
}


/*
  Retrieves a Measure's value for a given year.

  @param key
    The year to find the value for

  @return
    The value stored for the given year

  @throws
    std::out_of_range if year does not exist in Measure with the message
    No value found for year <year>

  @return
    The value

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    ...
    auto value = measure.getValue(1999); // returns 12345678.9
*/
double Measure::getValue(int year) const {
  auto it = values.find(year);
  if (it == values.end()) {
    throw std::out_of_range("No value found for year " + std::to_string(year));
  }
  return it->second;
}




/*
  Adds a particular year's value to the Measure object. If a value already
  exists for the year, replaces it.

  @param key
    The year to insert a value at

  @param value
    The value for the given year

  @return
    void

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
*/
void Measure::setValue(int year, double value) {
  values[year] = value;
}


/*
  Retrieves the number of years data we have for this measure. Should be
  callable from a constant context and must promise to not change the
  state of the instance or throw an exception.

  @return
    The size of the measure

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    auto size = measure.size(); // returns 1
*/
int Measure::size() const {
    return values.size();
}



/*
  Calculates the difference between the first and last year imported. Should be
  callable from a constant context and must promise to not change the state of
  the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year, or 0 if it
    cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(2001, 12345679.9);
    auto diff = measure.getDifference(); // returns 1.0
*/
double Measure::getDifference() const {
    // If there are no values, or only one value, return 0.0
    if (values.empty() || values.size() == 1) {
        return 0;
    }
    
    // Find the first and last year
    auto first = values.begin()->first;
    auto last = values.rbegin()->first;
    
    // Calculate the difference
    return values.at(last) - values.at(first);
}



/*
  Calculates the difference between the first and last year imported as a 
  percentage. Should be callable from a constant context and must promise
  to not change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year as a decminal
    value, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1990, 12345678.9);
    measure.setValue(2010, 12345679.9);
    auto diff = measure.getDifferenceAsPercentage();
*/
double Measure::getDifferenceAsPercentage() const {
  // If there are no values, return 0
  if (values.empty()) {
    return 0;
  }

  // Find the first and last year
  double firstYear = values.begin()->second;
  double lastYear = values.rbegin()->second;

  // Return 0 if the first year's value is 0
  if (firstYear == 0) {
    return 0;
  }

  // Return % in repsect to the first year's value
  return ((lastYear - firstYear) / firstYear) * 100;
}



/*
  Calculates the average/mean value for all the values. This function should be
  callable from a constant context and must promise to not change the state of 
  the instance or throw an exception.

  @return
    The average value for all the years, or 0 if it cannot be calculated

  @example
    Measure measure("pop", "Population");
    measure.setValue(1999, 12345678.9);
    measure.setValue(2001, 12345679.9);
    auto diff = measure.getAverage(); // returns 12345678.4
*/
double Measure::getAverage() const {
  // Check if the measure has any values
  if (values.empty()) {
    return 0;
  }

  // Initialize sum variable to 0
  double sum = 0;

  // Loop through all the years and add the values to sum
  for (const auto& kv : values) {
    sum += kv.second;
  }

  // Calculate the average and return it
  return sum / values.size();
}



/*
  Overloads << operator to print all of the Measure's imported data.

  Aligns the year and value outputs by padding the outputs with spaces,
  i.e. the year and values should be right-aligned to each other so they
  can be read as a table of numerical values.

  Years should be printed in chronological order. Three additional columns
  should be included at the end of the output, correspodning to the average
  value across the years, the difference between the first and last year,
  and the percentage difference between the first and last year.

  If there is no data in this measure, prints the name and code, and 
  on the next line prints: <no data>

  @param os
    The output stream to write to

  @param measure
    The Measure to write to the output stream

  @return
    Reference to the output stream

  @example
    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    measure.setValue(1999, 12345678.9);
    std::cout << measure << std::end;
*/
std::ostream& operator<<(std::ostream& os, const Measure& measure) {
    os << std::right << std::setw(6) << "Year" << "  " << std::setw(15) << "Value" << std::endl;
    os << std::setfill('-') << std::setw(23) << "-" << std::setfill(' ') << std::endl;
    if (measure.values.empty()) {
        os << measure.label << " (" << measure.codename << ")" << std::endl;
        os << "<no data>" << std::endl;
        return os;
    }
    double firstYearValue = measure.values.begin()->second;
    double lastYearValue = measure.values.rbegin()->second;
    double difference = lastYearValue - firstYearValue;
    double percentageDifference = (difference / firstYearValue) * 100;
    for (auto it = measure.values.begin(); it != measure.values.end(); ++it) {
        int year = it->first;
        double value = it->second;
        os << std::right << std::setw(6) << year << "  " << std::setw(15) << value << std::endl;
    }

    os << std::setfill('-') << std::setw(23) << "-" << std::setfill(' ') << std::endl;
    os << std::right << std::setw(6) << "Average" << "  " << std::setw(15) << measure.getAverage() << std::endl;
    os << std::right << std::setw(6) << "Diff" << "  " << std::setw(15) << difference << std::endl;
    os << std::right << std::setw(6) << "%" << "  " << std::setw(15) << percentageDifference << std::endl;
    return os;
}


/*
  Overloads == operator for two Measure objects. Two Measure objects
  are only equal when their codename, label and data are all equal.

  @param lhs
    A Measure object

  @param rhs
    A second Measure object

  @return
    true if both Measure objects have the same codename, label and data; false
    otherwise
*/
bool operator==(const Measure& lhs, const Measure& rhs) {
    return lhs.getCodename() == rhs.getCodename() &&
           lhs.getLabel() == rhs.getLabel() &&
           lhs.values == rhs.values;
}

Measure::Measure() : codename(""), label(""), values({}) {}

void Measure::combine(const Measure& other) {
  for (const auto& value : other.values) {
    this->values[value.first] = value.second;
  }
}