#pragma once

/// forward declarations
struct Player;

/// the enum to categorize between the 3 match types
enum MATCH_TYPE{
    QUALIFIER,
    ROUNDROBIN,
    KNOCKOUT
};

enum MATCH_STATUS {
    PLAYER_ONE_WIN,
    PLAYER_TWO_WIN,
    DRAW
};

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