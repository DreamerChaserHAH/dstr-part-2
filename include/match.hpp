#pragma once
#include "tournament/enums.hpp"

/// forward declarations
struct Player;

/// Store the match id of the current match
static int CurrentMatchID = 1;

struct Match {
    /// @brief the unique id of the match
    int match_id;
    /// @brief  the type of the match
    MATCH_TYPE match_type;
    /// @brief the pointer to the player 1
    Player* player1;
    /// @brief the pointer to the player 2
    Player* player2;

    void createMatch(MATCH_TYPE match_type, Player* player1, Player* player2) {
        this->match_id = CurrentMatchID;
        this->match_type = match_type;
        this->player1 = player1;
        this->player2 = player2;
        CurrentMatchID++;
    }
};

struct MatchesContainer {
    Match* matches;
    int number_of_matches;
};