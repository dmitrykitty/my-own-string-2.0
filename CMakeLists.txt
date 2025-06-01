cmake_minimum_required(VERSION 3.19)
project(zad12MyString_dlaStudentow)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(UNIT_TEST_DIRECTORY "tests")

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest /Wall /WX")
    endif()
else()
    set(CMAKE_CXX_FLAGS "--std=c++${CMAKE_CXX_STANDARD} -Wall -Wextra -pedantic -Werror -Wno-error=deprecated-declarations")
    set(COMPILER_DEBUG_FLAGS "-D_DEBUG")
    set(COMPILER_RELEASE_FLAGS "-O3 -DNDEBUG")
    set(LINKER_FLAGS "--std=c++${CMAKE_CXX_STANDARD}")
endif()

set(CMAKE_BUILD_TYPE Debug)

# Na Linuxie dla kompilatorow g++ i Clang moga byc dostepne sanitizery:
if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    include(CheckLibraryExists)

    check_library_exists(asan __asan_init "" HAVE_LIBASAN)
    check_library_exists(ubsan __ubsan_handle_add_overflow "" HAVE_LIBUBSAN)

    if (HAVE_LIBASAN AND HAVE_LIBUBSAN)
        message(STATUS "Sanitizers (AddressSanitizer + UndefinedBehaviorSanitizer) są dostępne")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
    else()
        message(WARNING "Brak wymaganych bibliotek (libasan/libubsan). Możesz je zainstalować: na Fedora przez: sudo yum install libasan libubsan")
    endif()
endif()

# include-what-you-use (IWYU)
find_program(IWYU_PATH NAMES include-what-you-use iwyu)

if (IWYU_PATH)
    message(STATUS "Znaleziono include-what-you-use: ${IWYU_PATH}")

    # Przekieruj kompilację przez IWYU
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})

    # Ustaw mapowanie IWYU (jeśli plik istnieje)
    set(IWYU_MAPPING_FILE "${CMAKE_SOURCE_DIR}/${UNIT_TEST_DIRECTORY}/include-what-you-use_exceptions.imp")
    if(EXISTS ${IWYU_MAPPING_FILE})
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU_PATH};-Xiwyu;--mapping_file=${IWYU_MAPPING_FILE}")
        message(STATUS "Używany mapping IWYU: ${IWYU_MAPPING_FILE}")
    else()
        message(WARNING "Brak pliku mappingu IWYU: ${IWYU_MAPPING_FILE}")
    endif()
else()
    message(WARNING "Nie znaleziono include-what-you-use. Możesz zainstalować")
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # for cppcheck To generate compile_commands.json

set(ENABLE_JSON_COMPILATION_OUTPUT_GCC OFF CACHE BOOL "Enable JSON output from GCC compiler")

if (CMAKE_COMPILER_IS_GNUCC AND ENABLE_JSON_COMPILATION_OUTPUT_GCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-format=json")
endif()


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MAIN_PROJECT_NAME "${PROJECT_NAME}")
IF (WIN32)
    set(PROJECT_BINARY_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe")
ELSE()
    set(PROJECT_BINARY_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}")
ENDIF()


file(GLOB SOURCES *.cpp *.cc *.c)
file(GLOB HEADERS *.h *.hpp)
set(SOURCES_WITHOUT_MAIN ${SOURCES})
list(REMOVE_ITEM SOURCES_WITHOUT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
message("Katalog do zbudowanego projektu: ${PROJECT_BINARY_DIR}")
message(" + Znaleziono nagłówki: ${HEADERS}")
message(" + Znaleziono źródła: ${SOURCES}")
message(" + Znaleziono źródła bez main.cpp: ${SOURCES_WITHOUT_MAIN}")


############ dodanie binarki z mainem i danymi studenta
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
   add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
else()
   add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
endif()


############ KOMENDA: make run
add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running the built executable (not tests)"
    USES_TERMINAL
)


############ KOMENDA: make valgrind
find_program(VALGRIND NAMES valgrind)
if (VALGRIND)
    message("\t* valgrind found: ${VALGRIND}")
    add_custom_target(valgrind DEPENDS ${PROJECT_NAME}
        COMMAND ${VALGRIND} --leak-check=full ${PROJECT_BINARY_PATH}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Running valgrind to check for memory leaks"
        USES_TERMINAL
    )
endif()


############ KOMENDA: make docs
find_program(DOXYGEN NAMES doxygen)
if (DOXYGEN)
    # Uwaga - komenda nie jest w pełni dopracowana (nie pobiera nazwy projektu). Poza tym wymaga zależności do pełnego działania (do generacji PDFów)
    message("\t* doxygen found: ${DOXYGEN}")

    add_custom_target(docs
        # 1. Wygeneruj dokumentację (latex/, html/)
        COMMAND ${DOXYGEN} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile

        # 2. Skopiuj obrazki (np. diagramy) do latex/, jeśli są wymagane
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/doxyfiles ${CMAKE_CURRENT_SOURCE_DIR}/latex

        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_CURRENT_SOURCE_DIR}/latex/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/doxyfiles ${CMAKE_CURRENT_SOURCE_DIR}/latex/


        # 3. Buduj PDF z latex/refman.tex – 2x dla poprawnych spisów treści
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/latex pdflatex -interaction=nonstopmode refman.tex
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/latex pdflatex -interaction=nonstopmode refman.tex

        # 4. Przenieś wynikowy PDF do katalogu binarnego jako: nazwaprojektu_coDoZrobienia.pdf
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/latex/refman.pdf
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_coDoZrobienia.pdf

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating documentation using Doxygen + pdflatex"
        USES_TERMINAL
    )
endif()


############ KOMENDA: make show_tree
find_program(TREE NAMES tree)
if (TREE)
    add_custom_target(show_tree
        COMMAND ${CMAKE_COMMAND} -E echo "File structure:"
        COMMAND tree -L2
        USES_TERMINAL
    )
endif()


############ KOMENDA: make zip
get_filename_component(directoryBasename ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(zip
    COMMAND zip -r ${CMAKE_BINARY_DIR}/${directoryBasename}.zip . -x '*.git*' -x 'build/*'
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Zipping the project directory into a .zip file"
    USES_TERMINAL
)


############ KOMENDA: make format
#idealnym standardem dla C++ jest https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines, niestety nie jest on jeszcze wspierany przez narzędzie clang-format, dlatego używam innego popularnego stylu.
add_custom_target(format
    COMMAND clang-format -i -style=Google ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting source files using clang-format (Google style)"
    USES_TERMINAL
)

############ KOMENDA: make ctags
add_custom_target(ctags
    COMMAND ctags -x --c++-types=f --extras --format=1 ${HEADERS} ${SOURCES_WITHOUT_MAIN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating CTags for source and header files"
    USES_TERMINAL
)

############ KOMENDA: make cppcheck_${PROJECT_NAME}
find_program(CPPCHECK NAMES cppcheck)
if (CPPCHECK)
    message("\t* cppcheck found: ${CPPCHECK}")
    add_custom_target(cppcheck_${PROJECT_NAME} DEPENDS ${PROJECT_NAME}
        COMMAND cppcheck --project=${CMAKE_BINARY_DIR}/compile_commands.json --force --enable=warning,performance,unusedFunction,style,information --template=gcc --inline-suppr --library=gnu.cfg --library=std.cfg --library=cppcheck-lib.cfg --library=motif.cfg --library=qt.cfg --library=windows.cfg --library=gnu.cfg --library=posix.cfg --library=avr.cfg --suppress=missingIncludeSystem -i ${HEADERS} ${SOURCES} -i ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

############ KOMENDA: make list_files
if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Przekształcenie listy plików w ciągi oddzielone spacjami
    string(REPLACE ";" " " SOURCES_WITH_SPACES "${SOURCES}")
    string(REPLACE ";" " " HEADERS_WITH_SPACES "${HEADERS}")

    add_custom_target(list_files
        COMMAND echo ${SOURCES_WITH_SPACES} ${HEADERS_WITH_SPACES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Listing all source and header files"
        VERBATIM
    )
endif()

############ KOMENDA: make cpplint
find_program(CPPLINT_TOOL NAMES cpplint)
if (CPPLINT_TOOL)
    message("\t+ adding command found: `cpplint` which would check trailing whitespaces and tabs in files: ${HEADERS} and ${SOURCES}")

    add_custom_target(cpplint
        COMMAND ${CPPLINT_TOOL} --filter=+whitespace/end_of_line,+whitespace/tab,-build,-readability,-runtime,-legal ${SOURCES} ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cpplint to check for tabs and trailing whitespace"
        USES_TERMINAL
    )
endif()

############ KOMENDA: make clang_tidy
find_program(CLANG_TIDY NAMES clang-tidy)
if (CLANG_TIDY)
    message("\t+ adding command found: `clang-tidy` to run static analysis on source files: ${SOURCES} and ${HEADERS}")

    add_custom_target(clang_tidy DEPENDS ${PROJECT_NAME}
        COMMAND ${CLANG_TIDY} -p="${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" ${SOURCES} ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-tidy to perform static analysis on source files"
        USES_TERMINAL
    )
endif()

############ KOMENDA: make flawfinder
find_program(FLAWFINDER_TOOL NAMES flawfinder)
if (FLAWFINDER_TOOL)
    message("\t+ adding command found: `flawfinder` to perform security analysis on files: ${SOURCES} and ${HEADERS}")

    add_custom_target(flawfinder
        COMMAND ${FLAWFINDER_TOOL} --quiet --dataonly --nolink --columns --context --singleline --minlevel=0 ${HEADERS} ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running flawfinder to perform security analysis"
        USES_TERMINAL
    )
endif()


############ dodanie podprojektu z testami:
add_subdirectory(${UNIT_TEST_DIRECTORY})
