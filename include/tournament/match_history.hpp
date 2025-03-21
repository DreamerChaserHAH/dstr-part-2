#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "match.hpp"

/// Node to store each match result
struct MatchHistoryNode {
    Match* match;
    MATCH_STATUS result;
    MatchHistoryNode* next;
};

/// Queue class declaration
class MatchHistoryQueue {
private:
    MatchHistoryNode* front;
    MatchHistoryNode* rear;

public:
    MatchHistoryQueue();
    ~MatchHistoryQueue();

    void enqueue(Match* match, MATCH_STATUS result);
    void displayHistory();
    void saveHistoryToFile(const std::string& filename);
};
