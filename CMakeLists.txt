cmake_minimum_required(VERSION 3.9)
project(minisearch C)

set(CMAKE_C_STANDARD 99)

add_executable(minisearch
        src/main.c
        src/commandParsing.c
        include/commandParsing.h
        src/Trie.c
        include/Trie.h
        src/postingList.c
        include/postingList.h
        src/index.c
        include/index.h
        src/search.c
        include/search.h
        src/printer.c
        include/printer.h
        src/scoring.c
        include/scoring.h)

target_link_libraries(minisearch m)