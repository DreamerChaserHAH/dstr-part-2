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

/// <summary>
/// @brief the class to manage the tournament schedule (Linked List Queue Implementation)
/// Linked List is used for this Queue to ensure the system can handle any arbitary number of matches
/// because the number of matches is not known before hand
/// </summary>
class TournamentSchedulingSystem {

    /// @brief the first element to be removed
    ScheduleSlot* head = nullptr;
    /// @brief the last element to be removed
    ScheduleSlot* tail = nullptr;

    /// @brief the last time slot and court used
    SCHEDULE_TIME_SLOT last_time_slot = MONDAY_MORNING;
    /// @brief the last court used
    TOURNAMENT_COURT last_court = MAIN_COURT;

    /// @brief the matchmaking system that is used together with this system
    MatchmakingSystem* matchmaking_system = nullptr;

    public:
        explicit TournamentSchedulingSystem();
        /// @brief queue the matches to be played
        /// @param matches the list of matches to be played and scheduled
        void push(MatchesContainer* matches);

        /// @brief queue the match to be played
        /// @param match the match to be scheduled for
        void push(Match* match);

        /// @brief dequeue the match to be played
        ScheduleSlot* dequeue();

        /// @brief take a look at the next match to be played without removing it
        ScheduleSlot* peek();

        /// @brief print all the matches that have been queued for matching
        void print_schedule();

        /// @brief print only the last match that has been queued for matching
        void print_last_schedule();

        /// @brief pop the last match and continue the process needed depending on the match type
        void last_match_completed(MATCH_STATUS status);

        void update_matchmaking_system(MatchmakingSystem* matchmaking_system);

        void remove_player(int player_id);

        void read_player(Player* player);
};