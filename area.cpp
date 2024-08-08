


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains the implementation for the Area class.
*/

#include <stdexcept>

#include "area.h"

/*
  Construct an Area with a given local authority code.

  @param localAuthorityCode
    The local authority code of the Area
*/
Area::Area(const std::string& localAuthorityCode)
        : areaAuthCode(localAuthorityCode) {}

/*
  Retrieve the local authority code for this Area. Should be 
  callable from a constant context and not modify the state of the instance.
  
  @return
    The Area's local authority code
*/
std::string Area::getLocalAuthorityCode() const {
  return areaAuthCode;
}


/*
  Get a name for the Area in a specific language.  Should be 
  callable from a constant context and not modify the state of the instance.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    The name for the area in the given language

  @throws
    std::out_of_range if lang does not correspond to a language of a name stored
    inside the Area instance

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);
    ...
    auto name = area.getName(langCode);
*/
std::string Area::getName(const std::string& lang) const {
    std::string langLower = lang;
    std::transform(langLower.begin(), langLower.end(), langLower.begin(), ::tolower);

    auto it = names.find(langLower);
    if (it == names.end()) {
        throw std::out_of_range("Language not found");
    }

    return it->second;
}


/*
  Set a name for the Area in a specific language.

  @param lang
    A three-letter (alphabetical) language code in ISO 639-3 format,
    e.g. cym or eng, which should be converted to lowercase

  @param name
    The name of the Area in `lang`

  @throws
    std::invalid_argument if lang is not a three letter alphabetic code

  @example
    Area area("W06000023");
    std::string langCodeEnglish  = "eng";
    std::string langValueEnglish = "Powys";
    area.setName(langCodeEnglish, langValueEnglish);

    std::string langCodeWelsh  = "cym";
    std::string langValueWelsh = "Powys";
    area.setName(langCodeWelsh, langValueWelsh);
*/
void Area::setName(const std::string& lang, const std::string& name) {
  std::string langLower = lang;
  std::transform(langLower.begin(), langLower.end(), langLower.begin(), ::tolower);
  names[langLower] = name;
}

/*
  Retrieve a Measure object, given its codename.

  @param key
    The codename for the measure you want to retrieve

  @return
    A Measure object

  @throws
    std::out_of_range if there is no measure with the given code, throwing
    the message:
    No measure found matching <codename>

  @example
    Area area("W06000023");
    Measure measure("Pop", "Population");
    area.setMeasure("Pop", measure);
    ...
    auto measure2 = area.getMeasure("pop");
*/
Measure Area::getMeasure(const std::string &key) const {
  // Convert key to lowercase for case-insensitivity
  std::string keyLower = key;
  std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);


  // Look up measure in the map
  auto it = measures.find(key);

  // If the measure is not found, throw an exception
  if (it == measures.end()) {
    throw std::out_of_range("No measure found matching " + key);
  }

  // Return the measure
  return it->second;
}


/*
  Add a particular Measure to this Area object. Measure's codename should be
  converted to lowercase. If a Measure already exists with the same codename
  in this Area, it should overwrite any values contained within the existing
  Measure with those in the new Measure passed into this function. The
  resulting Measure stored inside the Area instance should be a combination
  of the two Measures instances.

  @param codename
    The codename for the Measure

  @param measure
    The Measure object

  @return
    void

  @example
    Area area("W06000023");

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    double value = 12345678.9;
    measure.setValue(1999, value);

    area.setMeasure(codename, measure);
*/
void Area::setMeasure(const std::string& code, const Measure& measure) {
  std::string codeLower = code;
  std::transform(codeLower.begin(), codeLower.end(), codeLower.begin(), ::tolower);

  auto it = measures.find(codeLower);
  if (it != measures.end()) {
    it->second.combine(measure);
  } else {
    measures[codeLower] = measure;
  }
}




/*
  Retrieve the number of Measures we have for this Area. Should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The size of the Area (i.e., the number of Measures)

  @example
    Area area("W06000023");
    std::string langCode  = "eng";
    std::string langValue = "Powys";
    area.setName(langCode, langValue);

    std::string codename = "Pop";
    std::string label = "Population";
    Measure measure(codename, label);

    area.setMeasure(code, measure);
    auto size = area.size();
*/
std::size_t Area::size() const {
  return measures.size();
}


/*
  Overload the stream output operator as a free/global function.
  Output the name of the Area in English and Welsh, followed by the local
  authority code. If the Area only has only one name, output it. If the area
  has no names, output should be "Unnamed". Measures should be ordered by
  their Measure codename. If there are no measures, the line "<no measures>"
  should be outputted after outputting area names.

  @param os
    The output stream to write to

  @param area
    Area to write to the output stream

  @return
    Reference to the output stream

  @example
    Area area("W06000023");
    area.setName("eng", "Powys");
    std::cout << area << std::endl;
*/
std::ostream& operator<<(std::ostream& os, const Area& area) {
    // Output the English and Welsh names of the area, or "Unnamed" if no names exist
    if (area.names.empty()) {
        os << "Unnamed" << std::endl;
    } else if (area.names.size() == 1) {
        os << area.names.begin()->second << std::endl;
    } else {
        os << area.names.find("eng")->second << " / " << area.names.find("cym")->second << std::endl;
    }

    // Output the local authority code
    os << "Local authority code: " << area.areaAuthCode << std::endl;

    // Output the measures, ordered by their codenames
    if (area.measures.empty()) {
        os << "<no measures>" << std::endl;
    } else {
        for (std::pair<std::string, Measure> const& p : area.measures) {
            os << p.second << std::endl;
        }
    }

    return os;
}


/*
  Overloaded == operator for two Area objects as a global/free function. Two
  Area objects are only equal when their local authority code, all names, and
  all data are equal.

  @param lhs
    An Area object

  @param rhs
    A second Area object

  @return
    true if both Area instanes have the same local authority code, names
    and data; false otherwise.

  @example
    Area area1("MYCODE1");
    Area area2("MYCODE1");

    bool eq = area1 == area2;
*/
bool operator==(const Area& lhs, const Area& rhs) {
    // Check if local authority codes match
    if (lhs.getLocalAuthorityCode() != rhs.getLocalAuthorityCode()) {
        return false;
    }

    // Check if names match
    if (lhs.names != rhs.names) {
        return false;
    }

    // Check if measures match
    if (lhs.measures != rhs.measures) {
        return false;
    }

    // If all checks passed, return true
    return true;
}
