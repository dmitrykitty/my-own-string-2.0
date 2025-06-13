# MyString – Optimized Custom String Class

[GitHub Repository](https://github.com/Dmitry_kitty/MyString)

MyString is a C++ string class that implements small-string optimization for efficient text handling. It stores short strings (up to 20 characters) in a fixed-size internal buffer and automatically uses a dynamic `std::string` for longer content. This design leverages C++ STL containers and algorithms to balance performance and usability. MyString provides custom random-access iterators, compatibility with range-based loops and standard algorithms, and utility methods for common string operations.

---

## Description and Motivation

MyString was originally created as an exercise in implementing a high-performance string type using modern C++. The class uses:

- An internal `std::array<char, 20>` (`smallText_`) for short strings  
- A `std::string` (`bigText_`) for overflow  

This small-string optimization avoids heap allocation for most small texts, improving speed for short strings. The implementation heavily utilizes the C++ Standard Library (containers, iterators, algorithms) to simplify development and ensure safety. Key motivations include:

- Learning custom iterator design  
- Practicing STL usage  
- Exploring memory-optimization techniques in string handling  

---

## Features

- **Small-Buffer Optimization**  
  Uses a static 20-character array for short strings, falling back to `std::string` when exceeded.

- **Standard Compatibility**  
  Provides `begin()`/`end()` (const and non-const) and reverse iterators, so you can use range-based loops and STL algorithms (e.g. `std::sort`, `std::all_of`).

- **Element Access**  
  `operator[]` allows indexed access with range checking (throws `std::out_of_range` on invalid index).

- **Mutation**  
  - `operator+=(char)` and `push_back(char)` append a character, expanding `bigText_` as needed.  
  - `operator+=(const MyString&)` concatenates another MyString.

- **Size and Capacity**  
  `size()`, `capacity()`, and `empty()` report the current length and allocated capacity.

- **Clear**  
  `clear()` resets the string to empty.

- **Conversion and I/O**  
  - `toString()` returns an equivalent `std::string`.  
  - Overloaded `operator<<` and `operator>>` support easy stream output/input.

- **Comparison**  
  C++20 three-way comparison (`operator<=>`) and `operator==` allow lexicographical comparison and equality checks.

- **Word Utilities**  
  - `trim()` removes leading/trailing whitespace.  
  - `toLower()` converts the string to lowercase.  
  - `getUniqueWords()` extracts a set of unique alphabetic words (case-insensitive).  
  - `countWordsUsageIgnoringCases()` returns a frequency map of each word (ignoring case).

- **Other Utilities**  
  - `static generateRandomWord(size_t n)` creates a random alphabetic MyString of length `n`.  
  - `startsWith(const MyString&)` / `endsWith(const MyString&)` check prefixes/suffixes.  
  - `join(const std::vector<MyString>&)` concatenates a list of MyStrings, inserting this string as a separator.  
  - `template <typename Pred> all_of(Pred)` applies `std::all_of` to the characters of the string.

---

## Usage Examples

```cpp
#include <iostream>
#include "mystring.h"

int main() {
    // Construct from C-string and repeat-character
    MyString s1("Hello");
    MyString s2(3, 'x');               // "xxx"
    
    // Append characters and other MyStrings
    s1.push_back(' ');
    s1 += MyString("World!");
    std::cout << s1 << std::endl;      // Output: Hello World!

    // Check prefix/suffix
    std::cout << std::boolalpha 
              << s1.startsWith(MyString("Hello")) << '\n'   // true
              << s1.endsWith(MyString("!")) << '\n';      // true

    // Convert to std::string
    std::string native = s1.toString();
    std::cout << "Std string: " << native << std::endl;

    // Iterate over characters
    for (char c : s1) {
        std::cout << c << ' ';
    }
    std::cout << std::endl;
    // Output: H e l l o   W o r l d !

    // Trim and toLower
    MyString s3("  AbC  ");
    s3.trim().toLower();
    std::cout << s3 << std::endl;      // Output: abc

    // Unique words and counting (ignores case)
    MyString sentence("Hello world HELLO");
    auto unique = sentence.getUniqueWords();
    auto counts = sentence.countWordsUsageIgnoringCases();
    // unique contains "hello", "world"
    // counts["hello"] == 2, counts["world"] == 1

    // Join example
    std::vector<MyString> words = { "apple", "banana", "cherry" };
    MyString comma(", ");
    MyString result = comma.join(words);
    std::cout << result << std::endl; // Output: apple, banana, cherry

    // Generate a random word of length 5
    MyString randomWord = MyString::generateRandomWord(5);
    std::cout << "Random word: " << randomWord << std::endl;
}
```
## Build Instructions
**Requirements**
  - C++20 (or later) compiler
  - CMake 3.19+

**Using CMake**
  ```bash
  mkdir build && cd build
  cmake ..
  make
  ```
This produces the executable and runs the unit tests.

## Dependences 
  - Only the C++ Standard Library (`<array>, <string>, <vector>, <map>, <set>, <iterator>`, etc.)
  - For tests: Google Test (libgtest)

## Author
Implemented by Dmitry (GitHub user Dmitry_kitty).
Originally created as a lab exercise under the guidance of AGH University (Prof. Bazior).



  
