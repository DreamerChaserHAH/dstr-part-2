cmake_minimum_required(VERSION 3.30)
project(part2_clion)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(matchmaking-system src/tournament/matchmaking.cpp)
add_library(qualifier-round-matchmaking-system src/tournament/qualifier-round-matchmaking-system.cpp)
add_library(knockout-round-matchmaking-system src/tournament/knockout-round-matchmaking-system.cpp)

add_library(tournament-scheduling-system src/tournament/tournament_schedule.cpp)

add_executable(task1 src/matchmaking-ui.cpp)

target_link_libraries(task1 tournament-scheduling-system matchmaking-system qualifier-round-matchmaking-system knockout-round-matchmaking-system)
