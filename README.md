# cli-std-tool

Beth Yw? is a CLI Statistical Data Analysis tool designed to parse and analyze Welsh Government statistics data files. This project was developed as part of a university coursework assignment by me.

## Project Description

The primary goal of this project was to create a data parser that could process various datasets provided by the Welsh Government. The parser reads data from CSV and JSON files, processes it, and outputs the results in a human-readable table format or JSON format.

## Files Written by me

- `area.cpp`
- `area.h`
- `areas.cpp`
- `areas.h`
- `bethyw.cpp`
- `bethyw.h`
- `input.cpp`
- `input.h`
- `main.cpp`
- `measure.cpp`
- `measure.h`
- `README.md` (this file)

## Architecture

The architecture of the project is organized into several key components:

- **Area and Measure Classes**: These classes represent the fundamental data structures for the project. The `Area` class encapsulates information about specific areas, including names in different languages and measures. The `Measure` class represents various measures with values across multiple years.

- **Areas Class**: This class manages a collection of `Area` objects. It is responsible for parsing input data, creating `Area` and `Measure` objects, and storing them in an organized manner. The `Areas` class also provides methods for filtering and retrieving data based on different criteria.

- **Beth Yw Namespace**: This namespace contains helper functions for initializing and running the program. It includes functions for parsing command-line arguments, loading datasets, and integrating different components of the program.

- **Input Handling**: The `input.cpp` and `input.h` files handle the opening and closing of file streams. The `InputFile` class, derived from `InputSource`, manages file-based input sources.

Overall, while this project had been my first time learning C++, I had an enjoyable journey nonetheless, and I am proud of the work I have accomplished.

## License

See the `LICENSE` file for more details.

---

Thank you for taking the time to read about my project!
