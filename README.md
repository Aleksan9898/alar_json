# alar_json

`alar_json` is a JSON library for **C++20** focused on type-safe JSON value representation, explicit ownership, structured error handling, parser robustness, and testability.

The project is currently under active development.

## Features

* C++20
* Strict ISO C++ mode
* Type-safe JSON value representation using `std::variant`
* Explicit ownership with `std::unique_ptr`
* Deep-copy semantics for JSON values
* Separate lexer and parser components
* Iterative parser design
* `std::string_view` for parsing input
* Line and column tracking for parser diagnostics
* Structured parser errors using `Result<T, E>`
* Exception-based public parsing API
* JSON object and array access
* Bounds-checked and unchecked element access
* JSON iterators
* GoogleTest unit tests
* CTest integration
* libFuzzer support
* AddressSanitizer and UndefinedBehaviorSanitizer
* Fuzzing corpus
* Deep-nesting test input
* Doxygen documentation

## Supported JSON Types

`alar_json` supports the standard JSON value categories:

* `null`
* `boolean`
* integer
* floating-point number
* string
* array
* object

The main JSON value type is:

```cpp
std::variant<
    std::nullptr_t,
    bool,
    std::int64_t,
    double,
    std::string,
    JsonArray,
    std::unique_ptr<JsonObject>
>
```

`JsonValue` provides deep-copy semantics even though JSON objects use `std::unique_ptr` internally.

## Example

```cpp
#include <alar_json/json.hpp>

#include <iostream>

int main()
{
    try
    {
        auto json = alar::parse(
            R"({"name":"Aleksan","age":28,"active":true})"
        );

        const auto* name = json["name"];

        if (name != nullptr && name->is_string())
        {
            std::cout << "Name: "
                      << name->to_string()
                      << '\n';
        }

        std::cout << "Age: "
                  << json.at("age").to_integer()
                  << '\n';

        std::cout << "Active: "
                  << json.at("active").to_boolean()
                  << '\n';
    }
    catch (const alar::JsonParseError& e)
    {
        std::cerr << "JSON parse error: "
                  << e.what()
                  << '\n';
    }
}
```

The public API currently provides parsing from both strings and input streams:

```cpp
alar::JsonValue alar::parse(std::string_view input);

alar::JsonValue alar::parse(std::istream& input);
```

## Parser Architecture

The JSON processing pipeline is divided into separate stages:

```text
JSON input
    │
    ▼
  Lexer
    │
    ▼
  Tokens
    │
    ▼
  Parser
    │
    ▼
 JsonValue
```

### Lexer

The lexer converts the input JSON text into tokens.

Token information includes the token type and its position in the input, including line and column information.

### Parser

The parser consumes lexer tokens and constructs the corresponding `JsonValue`.

The parser uses explicit parsing state instead of relying on a recursive chain of parser function calls.

This design allows parsing state to be controlled explicitly and is particularly relevant when testing deeply nested JSON input.

## Error Handling

Parser internals use a result-based approach:

```text
Result<T, E>
```

This allows parsing operations to return either a successful result or a structured parser error.

The public `parse()` API exposes parsing failures through `JsonParseError`.

This separates internal parser control flow from the exception-based public API.

## Testing

The project uses GoogleTest and CTest for automated testing.

The test suite covers:

* JSON value construction
* JSON value types
* copying and moving JSON values
* lexer behavior
* parser behavior
* public API behavior
* JSON object and array access
* parser error handling

Additional robustness testing uses:

* libFuzzer
* AddressSanitizer
* UndefinedBehaviorSanitizer
* fuzzing corpus
* deeply nested JSON input

## Fuzzing

Fuzzing support is provided through libFuzzer.

Build the fuzzing configuration with:

```bash
cmake -S . -B build-fuzz -DBUILD_FUZZING=ON
cmake --build build-fuzz -j$(nproc)
```

The resulting fuzzer is:

```text
build-fuzz/alar_json_fuzzer
```

Initial fuzzing inputs are stored in:

```text
fuzz/corpus/
```

The project also contains a dedicated deep-nesting input:

```text
fuzz/deep_array_10000.json
```

This input is used to test parser behavior with a JSON structure containing significant nesting depth.

> Deep-nesting testing is a robustness test, not a claim that arbitrary nesting depth is fully supported.

## Building

### Requirements

* C++20-compatible compiler
* CMake 3.20 or newer
* GoogleTest 1.14.0
* Internet connection for the initial GoogleTest download through CMake `FetchContent`

For fuzzing:

* Clang with libFuzzer support
* AddressSanitizer
* UndefinedBehaviorSanitizer

The project is primarily developed and tested on Linux.

### Configure

```bash
cmake -S . -B build
```

### Build

```bash
cmake --build build -j$(nproc)
```

## Running Tests

Run the complete test suite with CTest:

```bash
ctest --test-dir build --output-on-failure
```

Tests can also be executed directly:

```bash
./build/alar_json_tests
```

## Documentation

API documentation is generated with Doxygen.

```bash
doxygen Doxyfile
```

Generated HTML documentation is placed in:

```text
docs/html/
```

## Project Structure

```text
alar_json/
├── include/
│   └── alar_json/          # Public headers
│
├── src/                    # Library implementation
│   ├── lexer/              # Lexer implementation
│   └── parser/             # Parser implementation
│
├── tests/                  # Unit tests
│
├── fuzz/                   # Fuzzing
│   ├── corpus/             # Fuzzing corpus
│   └── deep_array_10000.json
│
├── CMakeLists.txt
├── Doxyfile
├── LICENSE
├── README.md
└── README_RU.md
```

## Development Status

### Implemented

* JSON value representation
* `null`, boolean, integer, floating-point, string
* arrays
* objects
* lexer
* parser
* structured parser errors
* public parsing API
* object and array access
* bounds-checked access
* iterators
* deep-copy semantics
* unit tests
* CTest integration
* libFuzzer integration
* sanitizer-based testing
* fuzzing corpus
* deep-nesting test input
* Doxygen documentation

### In Development

#### JSON serialization / `dump`

JSON serialization back to a JSON string through a `dump()` API is **not implemented yet**.

This functionality is currently under development and will be added in a future version.

## License

This project is licensed under the **MIT License**.

See the [`LICENSE`](LICENSE) file for details.
