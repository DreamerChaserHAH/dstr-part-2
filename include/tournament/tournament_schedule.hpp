#pragma once

#include "enums.hpp"
#include "match.hpp"
#include "matchmaking.hpp"

class MatchmakingSystem;

/// @brief the slot of the schedule containing relevant data
struct ScheduleSlot{

    Match* match = nullptr;
    int week = 0;
    SCHEDULE_TIME_SLOT time_slot = MONDAY_MORNING;
    TOURNAMENT_COURT court = MAIN_COURT;
    ScheduleSlot* next_slot = nullptr;

    ScheduleSlot(Match* match, int week, SCHEDULE_TIME_SLOT time_slot, TOURNAMENT_COURT court, ScheduleSlot* next_slot){
        this->match = match;
        this->week = week;
        this->time_slot = time_slot;
        this->court = court;
        this->next_slot = next_slot;
    }
};

/// @brief the class to manage the tournament schedule (queue implementation)
class TournamentSchedulingSystem {
    /// @brief the first element to be removed
    ScheduleSlot* head;
    /// @brief the last element to be removed
    ScheduleSlot* tail;

    /// @brief the last time slot and court used
    SCHEDULE_TIME_SLOT last_time_slot = MONDAY_MORNING;

    /// @brief the last court used
    TOURNAMENT_COURT last_court = MAIN_COURT;

    MatchmakingSystem* matchmaking_system;

    public:
        explicit TournamentSchedulingSystem();

        /// @brief queue the match to be played
        /// @param match
        void add_schedule(MatchesContainer* matches);

        void add_schedule(Match* match);

        /// @brief dequeue the match to be played
        ScheduleSlot* deque_last_schedule();

        /// @brief take a look at the next match to be played without removing it
        ScheduleSlot* view_last_schedule();

        /// @brief void print the schedule out into a .csv file for backup
        void print_schedule();

        void print_last_schedule();

        void last_match_completed(MATCH_STATUS status);

        void update_matchmaking_system(MatchmakingSystem* matchmaking_system);
};