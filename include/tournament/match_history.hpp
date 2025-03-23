#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "match.hpp"
#include "enums.hpp"


// Node structure to store match history
struct MatchHistoryNode {
    Match* match;                    // Pointer to the match
    MATCH_STATUS result;             // Result of the match (Win/Loss/Draw)
    TOURNAMENT_COURT court;          // Court used (Main/Side)
    SCHEDULE_TIME_SLOT time_slot;    // Time slot (Morning/Afternoon/etc.)
    MatchHistoryNode* next;          // Next node in queue
};

// Queue class to manage match history
class MatchHistoryQueue {
private:
    MatchHistoryNode* front;  // Front of the queue
    MatchHistoryNode* rear;   // Rear of the queue

public:
    // Constructor
    MatchHistoryQueue();

    // Destructor
    ~MatchHistoryQueue();

    // Add match to history
    void enqueue(Match* match, MATCH_STATUS result, TOURNAMENT_COURT court, SCHEDULE_TIME_SLOT time_slot);

    // Display all match history
    void displayHistory();

    // Save history to CSV file
    void saveHistoryToFile(const std::string& filename);

    // Filter history by Match Type
    void filterByMatchType(MATCH_TYPE type);

    // Filter history by Player Name
    void filterByPlayer(const std::string& player_name);
};
