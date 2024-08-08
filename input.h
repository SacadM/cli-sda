#ifndef INPUT_H_
#define INPUT_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains declarations for the input source handlers. There are
  two classes: InputSource and InputFile. InputSource is abstract (i.e. it
  contains a pure virtual function). InputFile is a concrete derivation of
  InputSource, for input from files.
 */

#include <string>
#include <fstream>

/*
  InputSource is an abstract/purely virtual base class for all input source 
  types. In future versions of our application, we may support multiple input 
  data sources such as files and web pages. Therefore, this virtual class 
  will allow us to mix/match sources as needed.
*/
class InputSource {
public:
  const std::string& getSource() const;

protected:
  InputSource(const std::string& source);
  std::string source;
};

/*
  Source data that is contained within a file. For now, our application will
  only work with files.
*/
class InputFile : public InputSource {
public:
  InputFile(const std::string& filePath);

  std::istream& open();

private:
  std::ifstream fileStream;
};


#endif // INPUT_H_