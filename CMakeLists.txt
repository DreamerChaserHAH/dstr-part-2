cmake_minimum_required(VERSION 3.30)
project(part2_clion)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(matchmaking-system src/tournament/matchmaking.cpp)
add_library(qualifier-round-matchmaking-system src/tournament/qualifier-round-matchmaking-system.cpp)
add_library(knockout-round-matchmaking-system src/tournament/knockout-round-matchmaking-system.cpp
        src/tournament/match_history.cpp
        include/tournament/match_history.hpp
)
add_library(tournament-scheduling-system src/tournament/tournament_schedule.cpp)

add_executable(task1 src/main.cpp)

add_library(ticketing-sales src/tickets/Customer.cpp src/tickets/CustomPriorityQueue.cpp "src/tickets/Ticketing Sales.cpp")

target_link_libraries(task1 tournament-scheduling-system matchmaking-system qualifier-round-matchmaking-system knockout-round-matchmaking-system ticketing-sales)
