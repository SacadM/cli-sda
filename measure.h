#ifndef MEASURE_H_
#define MEASURE_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains the decalaration of the Measure class.
 */

#include <string>
#include <unordered_set>
#include <map>
#include <iostream>
#include <iomanip>

/*
  The Measure class contains a measure code, label, and a container for readings
  from across a number of years.
*/
class Measure {
  private:
    std::string codename;
    std::string label;
    std::map<int, double> values;
  public:
    Measure();
    Measure(std::string code, const std::string &label);
    std::string getCodename() const;
    std::string getLabel() const;
    void setLabel(std::string newlabel);
    void setValue(int year, double value);
    double getValue(int year) const;
    int size() const;
    double getDifference() const;
    double getDifferenceAsPercentage() const;
    double getAverage() const;
    friend std::ostream& operator<<(std::ostream& os, const Measure& measure);
    friend bool operator==(const Measure& lhs, const Measure& rhs);
    const std::map<int, double>& getYears() const {
      return values;
    };
    void combine(const Measure& other);
};

#endif // MEASURE_H_