/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <Sacad Muhumed>

  This file contains the code responsible for opening and closing file
  streams. The actual handling of the data from that stream is handled
  by the functions in areas.cpp.
 */

#include "input.h"
#include <stdexcept>
#include <istream>
#include <fstream>

/*
  Constructor for an InputSource.

  @param source
    A unique identifier for a source (i.e. the location).
*/
InputSource::InputSource(const std::string& source) : source(source) {}

/*
  Should be callable from a constant context.

  @return
    A non-modifable value for the source passed into the construtor.
*/
const std::string& InputSource::getSource() const {
  return source;
}


/*
  Constructor for a file-based source.

  @param path
    The complete path for a file to import.
*/
InputFile::InputFile(const std::string& filePath) : InputSource(filePath) {}

/*
  Opens a file stream to the file path retrievable from getSource()
  and returns a reference to the stream.

  @return
    A standard input stream reference

  @throws
    std::runtime_error if there is an issue opening the file, with the message:
    InputFile::open: Failed to open file <file name>

  @example
    InputFile input("data/areas.csv");
    input.open();
*/
std::istream& InputFile::open() {
  fileStream.open(getSource());
  if (!fileStream.is_open()) {
    throw std::runtime_error("InputFile::open: Failed to open file " + getSource());
  }
  return fileStream;
}