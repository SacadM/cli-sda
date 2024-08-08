#ifndef AREA_H_
#define AREA_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains the Area class declaration. Area objects contain all the
  Measure objects for a given local area, along with names for that area and a
  unique authority code.
 */

#include <string>
#include <map>
#include <algorithm>

#include "measure.h"

/*
  An Area object consists of a unique authority code, a container for names
  for the area in any number of different languages, and a container for the
  Measures objects.
*/
class Area {
  private:
    std::string areaAuthCode;
    std::map<std::string, std::string> names;
    std::map<std::string, Measure> measures;
  public:
    Area() {};
    Area(const std::string& localAuthorityCode);
    std::string getLocalAuthorityCode() const;
    void setName(const std::string& lang, const std::string& name);
    std::string getName(const std::string& lang) const;
    void setMeasure(const std::string& code, const Measure& measure);
    Measure getMeasure(const std::string& code) const;
    std::size_t size() const;
    friend std::ostream& operator<<(std::ostream& os, const Area& area);
    friend bool operator==(const Area& lhs, const Area& rhs);
    const std::map<std::string, Measure>& getMeasures() const {
      return measures;
    };
};

#endif // AREA_H_