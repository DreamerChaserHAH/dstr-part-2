#include "tournament/match_history.hpp"

#include <player.hpp>

MatchHistoryQueue::MatchHistoryQueue() {
    front = nullptr;
    rear = nullptr;
}

MatchHistoryQueue::~MatchHistoryQueue() {
    while (front != nullptr) {
        MatchHistoryNode* temp = front;
        front = front->next;
        delete temp;
    }
}

void MatchHistoryQueue::enqueue(Match* match, MATCH_STATUS result) {
    MatchHistoryNode* newNode = new MatchHistoryNode;
    newNode->match = match;
    newNode->result = result;
    newNode->next = nullptr;

    if (rear == nullptr) { // Empty queue
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

void MatchHistoryQueue::displayHistory() {
    if (front == nullptr) {
        std::cout << "No match history yet." << std::endl;
        return;
    }
    MatchHistoryNode* temp = front;
    while (temp != nullptr) {
        std::cout << "Match ID: " << temp->match->match_id
                  << " | " << temp->match->player1->name
                  << " vs " << temp->match->player2->name
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
}

void MatchHistoryQueue::saveHistoryToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error opening file!" << std::endl;
        return;
    }
    file << "MatchID,Player1,Player2,Result\n";

    MatchHistoryNode* temp = front;
    while (temp != nullptr) {
        file << temp->match->match_id << ","
             << temp->match->player1->name << ","
             << temp->match->player2->name << ",";
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
    std::cout << "Match history saved to " << filename << std::endl;
}
