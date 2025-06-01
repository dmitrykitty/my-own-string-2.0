project(tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
# Disabling include-what-you-use for GTest
set_target_properties(gtest PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "")
set_target_properties(gtest_main PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "")

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


file(GLOB TEST_SOURCES *.cpp *.cc *.c)
message(" + Znaleziono pliki testów: ${TEST_SOURCES}")
message(" + katalog nagłówków: ${INCLUDE_DIRECTORY}")

add_executable(${PROJECT_NAME} ${TEST_SOURCES} ${SOURCES_WITHOUT_MAIN})
target_link_libraries(${PROJECT_NAME} gtest gtest_main)


############ KOMENDA: make run_tests
add_custom_target(run_${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

############ KOMENDA: make valgrind_tests
find_program(VALGRIND NAMES valgrind)
if (VALGRIND)
    add_custom_target(valgrind_${PROJECT_NAME} DEPENDS ${PROJECT_NAME}
        COMMAND valgrind --leak-check=full ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

############ KOMENDA: make cppcheck_tests
find_program(CPPCHECK NAMES cppcheck)
if (CPPCHECK)
    add_custom_target(cppcheck_${PROJECT_NAME} DEPENDS ${PROJECT_NAME}
        COMMAND cppcheck --project=${CMAKE_BINARY_DIR}/compile_commands.json --force --enable=warning,performance,unusedFunction,style,information --template=gcc --inline-suppr --library=gnu.cfg --library=std.cfg --library=cppcheck-lib.cfg --library=motif.cfg --library=qt.cfg --library=windows.cfg --library=gnu.cfg --library=posix.cfg --library=avr.cfg --suppress=missingIncludeSystem -i   ${TEST_SOURCES} -i ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

############ KOMENDA: make perf_stats_tests
find_program(PERF_TOOL NAMES perf)
if (PERF_TOOL)
    message("\t* perf found: ${PERF_TOOL}, so adding command: 'perf_stats_${PROJECT_NAME}'")
    add_custom_target(perf_stats_${PROJECT_NAME} DEPENDS ${PROJECT_NAME}
        COMMAND ${PERF_TOOL} stat
            -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,faults,task-clock,context-switches,cpu-migrations,page-faults
            -- ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} 2>&1 > /dev/null
        COMMAND echo "======================"
        COMMAND echo "Program 'perf' to narzędzie do analizy wydajności działania programów."
        COMMAND echo "Podkomenda 'perf stat' mierzy podstawowe statystyki wydajności: ile operacji wykonał CPU, jak dobrze działała pamięć cache itd."
        COMMAND echo ""
        COMMAND echo "Tłumaczenie wyników perf stat:"
        COMMAND echo ""
        COMMAND echo "1. cycles         - liczba cykli CPU zużyta przez program. Jeden cykl to podstawowa jednostka pracy procesora: np. wykonanie małego fragmentu instrukcji."
        COMMAND echo "2. instructions   - liczba wykonanych instrukcji procesora. Instrukcja to pojedyncza operacja typu dodawanie, przesunięcie, skok itd."
        COMMAND echo "3. cache-references - liczba prób odczytu/zapisu danych z szybkiej pamięci cache procesora."
        COMMAND echo "4. cache-misses   - ile razy procesor próbował dostać dane z cache, ale ich tam nie było, przez co musiał pobrać z RAM-u."
        COMMAND echo "5. branches       - liczba instrukcji skoku, np. w if-ach, pętlach for/while lub switch."
        COMMAND echo "6. branch-misses  - liczba sytuacji, kiedy procesor źle przewidział wynik skoku i musiał cofnąć się i poprawić, co spowalnia program."
        COMMAND echo "7. faults         - liczba błędów dostępu do pamięci lub pułapek systemowych. Np. gdy program odwołuje się do pamięci, której jeszcze nie załadowano do RAM-u."
        COMMAND echo "8. task-clock     - łączny czas w milisekundach, jaki procesor aktywnie spędził wykonując program."
        COMMAND echo "9. context-switches - ile razy system operacyjny przełączył wykonanie z jednego procesu lub wątku na inny: przełączenie uwagi CPU."
        COMMAND echo "10. cpu-migrations - ile razy system przeniósł proces między różnymi rdzeniami CPU, co może spowalniać działanie."
        COMMAND echo "11. page-faults    - liczba sytuacji, kiedy program chciał użyć fragmentu pamięci, który nie był jeszcze załadowany do RAM i system musiał go dopiero wczytać."
        COMMAND echo ""
        COMMAND echo "Dodatkowo:"
        COMMAND echo "- time elapsed - całkowity rzeczywisty czas trwania programu: z perspektywy zegarka na ścianie."
        COMMAND echo "- user/sys     - czas spędzony na wykonywaniu kodu użytkownika [user] oraz kodu systemowego [sys]."
        COMMAND echo "======================"
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Running perf stat and explaining results"
        USES_TERMINAL
    )
else()
    message(WARNING "perf not found - perf_stats_${PROJECT_NAME} target will not be available")
endif()
