#pragma once
#include "tournament/enums.hpp"

/// forward declarations
struct Player;

struct Match {
    /// @brief the unique id of the match
    int match_id;
    /// @brief  the type of the match
    MATCH_TYPE match_type;
    /// @brief the pointer to the player 1
    Player* player1;
    /// @brief the pointer to the player 2
    Player* player2;
};

struct MatchesContainer {
    Match* matches;
    int number_of_matches;
};