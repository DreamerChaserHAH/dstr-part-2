cmake_minimum_required(VERSION 3.30)
project(apu_tennis_tournament)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(matchmaking-system src/tournament/matchmaking.cpp)
add_library(match-history-system src/tournament/match_history.cpp include/tournament/match_history.hpp)
add_library(tournament-scheduling-system src/tournament/tournament_schedule.cpp)
add_library(ticketing-sales-system src/tickets/Customer.cpp src/tickets/CustomPriorityQueue.cpp "src/tickets/Ticketing Sales.cpp")

add_executable(apu_tennis_tournament_application src/main.cpp)


target_link_libraries(apu_tennis_tournament_application tournament-scheduling-system matchmaking-system match-history-system ticketing-sales-system)
