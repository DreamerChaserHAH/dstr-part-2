#include "tournament/match_history.hpp"
#include <player.hpp>
#include <fstream>
#include <iostream>

// Helper function: Convert string to lowercase
std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    for (char& c : lowerStr) {
        c = tolower(c);
    }
    return lowerStr;
}

// Constructor
MatchHistoryQueue::MatchHistoryQueue() {
    front = nullptr;
    rear = nullptr;
}

// Destructor: Clear queue
MatchHistoryQueue::~MatchHistoryQueue() {
    while (front != nullptr) {
        MatchHistoryNode* temp = front;
        front = front->next;
        delete temp;
    }
}

// Add match result to history
void MatchHistoryQueue::enqueue(Match* match, MATCH_STATUS result, TOURNAMENT_COURT court, SCHEDULE_TIME_SLOT time_slot) {
    MatchHistoryNode* newNode = new MatchHistoryNode;
    newNode->match = match;
    newNode->result = result;
    newNode->court = court;
    newNode->time_slot = time_slot;
    newNode->next = nullptr;

    if (rear == nullptr) { // Queue is empty
        front = rear = newNode;
    } else { // Add to end
        rear->next = newNode;
        rear = newNode;
    }
}

// Display all match history
void MatchHistoryQueue::displayHistory() {
    if (front == nullptr) {
        std::cout << "No match history yet.\n";
        return;
    }

    MatchHistoryNode* temp = front;
    std::cout << "\n--- Match History ---\n";

    while (temp != nullptr) {
        std::cout << "Match ID: " << temp->match->match_id
                  << " | Type: " << get_match_type_string(temp->match->match_type)
                  << " | " << temp->match->player1->name << " vs " << temp->match->player2->name
                  << " | Court: " << (temp->court == MAIN_COURT ? "Main Court" : "Side Court")
                  << " | Time: " << get_schedule_string(temp->time_slot)
                  << " | Result: ";

        if (temp->result == PLAYER_ONE_WIN) {
            std::cout << temp->match->player1->name << " won";
        } else if (temp->result == PLAYER_TWO_WIN) {
            std::cout << temp->match->player2->name << " won";
        } else {
            std::cout << "Draw";
        }

        std::cout << std::endl;
        temp = temp->next;
    }

    std::cout << "----------------------\n";
}

// Save match history to CSV file
void MatchHistoryQueue::saveHistoryToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error opening file!\n";
        return;
    }

    // CSV header
    file << "MatchID,MatchType,Player1,Player2,Court,TimeSlot,Result\n";

    MatchHistoryNode* temp = front;
    while (temp != nullptr) {
        file << temp->match->match_id << ","
             << get_match_type_string(temp->match->match_type) << ","
             << temp->match->player1->name << ","
             << temp->match->player2->name << ","
             << (temp->court == MAIN_COURT ? "Main Court" : "Side Court") << ","
             << get_schedule_string(temp->time_slot) << ",";

        if (temp->result == PLAYER_ONE_WIN) {
            file << temp->match->player1->name << " won";
        } else if (temp->result == PLAYER_TWO_WIN) {
            file << temp->match->player2->name << " won";
        } else {
            file << "Draw";
        }
        file << "\n";

        temp = temp->next;
    }

    file.close();
    std::cout << "History saved to file: " << filename << "\n";
}

// Filter by match type
void MatchHistoryQueue::filterByMatchType(MATCH_TYPE type) {
    if (front == nullptr) {
        std::cout << "No match history yet.\n";
        return;
    }

    std::cout << "\n--- Matches of Type: " << get_match_type_string(type) << " ---\n";
    MatchHistoryNode* temp = front;
    bool found = false;

    while (temp != nullptr) {
        if (temp->match->match_type == type) {
            found = true;
            std::cout << "Match ID: " << temp->match->match_id
                      << " | " << temp->match->player1->name << " vs " << temp->match->player2->name
                      << " | Court: " << (temp->court == MAIN_COURT ? "Main Court" : "Side Court")
                      << " | Time: " << get_schedule_string(temp->time_slot)
                      << " | Result: ";

            if (temp->result == PLAYER_ONE_WIN) {
                std::cout << temp->match->player1->name << " won";
            } else if (temp->result == PLAYER_TWO_WIN) {
                std::cout << temp->match->player2->name << " won";
            } else {
                std::cout << "Draw";
            }
            std::cout << std::endl;
        }
        temp = temp->next;
    }

    if (!found) {
        std::cout << "No matches found of this type.\n";
    }
}

// Filter by player name (case-insensitive)
void MatchHistoryQueue::filterByPlayer(const std::string& player_name) {
    if (front == nullptr) {
        std::cout << "No match history yet.\n";
        return;
    }

    std::string input_lower = toLower(player_name);
    std::cout << "\n--- Matches Involving Player: " << player_name << " ---\n";

    MatchHistoryNode* temp = front;
    bool found = false;

    while (temp != nullptr) {
        std::string player1_lower = toLower(temp->match->player1->name);
        std::string player2_lower = toLower(temp->match->player2->name);

        if (player1_lower == input_lower || player2_lower == input_lower) {
            found = true;
            std::cout << "Match ID: " << temp->match->match_id
                      << " | Type: " << get_match_type_string(temp->match->match_type)
                      << " | " << temp->match->player1->name << " vs " << temp->match->player2->name
                      << " | Court: " << (temp->court == MAIN_COURT ? "Main Court" : "Side Court")
                      << " | Time: " << get_schedule_string(temp->time_slot)
                      << " | Result: ";

            if (temp->result == PLAYER_ONE_WIN) {
                std::cout << temp->match->player1->name << " won";
            } else if (temp->result == PLAYER_TWO_WIN) {
                std::cout << temp->match->player2->name << " won";
            } else {
                std::cout << "Draw";
            }
            std::cout << std::endl;
        }
        temp = temp->next;
    }

    if (!found) {
        std::cout << "No matches found involving this player.\n";
    }
}
