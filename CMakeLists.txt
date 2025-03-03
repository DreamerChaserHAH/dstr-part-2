cmake_minimum_required(VERSION 3.30)
project(part2_clion)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(enums src/tournament/enums.cpp)
add_library(matchmaking-system src/tournament/matchmaking.cpp)

add_library(tournament-scheduling-system src/tournament/tournament_schedule.cpp)
target_link_libraries(tournament-scheduling-system enums)

add_executable(task1 src/matchmaking-ui.cpp)

target_link_libraries(task1 enums tournament-scheduling-system matchmaking-system)
