/// <summary>
///  Author : Htet Aung Hlaing
/// Description : Matchmaking class in the following way
/// 1. Read a list of all the players in the player list
/// 2. Put the top 114 players into the main draw (First sort the players based on their tennis rating descending)
/// 3. Put the rest 72 players into the qualifier round
/// 4. Qualifer round operates in a one-off mode, (put a simple queue), one lose and the player is out 
/// 5. The 128 players (original 114 players + 16 players) is combined together to do the round-robinr rpund
/// 6,. Those 128 players will be splitted into 32 groups of 4 players.
/// 7. And the top 2 players from each group (in ttoal 64 people) will continue into the next round./
/// 8. The 64 players will be splitted into 16 groups of 4 players.
/// 9. And the top 2 players from each group (in total 32 people) will continue into the next round.
/// 10. The 32 players will be splitted into 8 groups of 4 players.
/// 11. And the top 2 players from each group (in total 16 people) will continue into the next round.
/// 12. Now the knock out round start
/// 13. The players will be splitted using their idnividual performance during the round robin round
/// 14. The highest score player will go against the the lowest ranked player
/// 15. The second highest score player will go against the second lowest ranked player
///.      1 - 16, 2 - 15, 3 - 14, 4 - 13, 5 - 12, 6 - 11, 7 - 10, 8 - 9
/// 16. The winner will continue to the next round until the final round is reached where the winner will be announced
/// </summary>

#pragma once

#include <utility>
#include "player.hpp"
#include "match.hpp"
#include "tournament_schedule.hpp"

class TournamentSchedulingSystem;

/// @brief the base class that all 3 matchmaking system type (QUALIFIER, ROUNDROBIN, KNOCKOUT) will be depending on
class BaseMatchmakingSystem {

    /// @brief check if this stage is completed or not
    bool is_completed;
    /// @brief the type of match
    MATCH_TYPE match_type;

    protected:
        /// @brief all the players that will be considered for the knockout round
        Player** all_players;
    public:
        virtual ~BaseMatchmakingSystem() = default;

        BaseMatchmakingSystem(MATCH_TYPE match_type, Player** all_players): all_players(all_players) {
            this->match_type = match_type;
            this->is_completed = false;
        }

        void set_is_completed(bool _is_completed) {
            this->is_completed = _is_completed;
        }

        bool completed() {
            return this->is_completed;
        }

        void set_current_stage(MATCH_TYPE _match_type) {
            this->match_type = _match_type;
        }

        MATCH_TYPE get_current_stage() {
            return this->match_type;
        }

        /// @brief matchmake with the information on hand
        virtual MatchesContainer* matchmake() = 0;

        /// @brief display the players that are waiting to be considered for matchmaking in the queue
        virtual void display_matchmaking_queue() = 0;

        /// @returns a list of all the players that are qualified for this round
        virtual Player** get_remaining_players() = 0;
};

/// @brief The node that will be used in the class below
struct PlayerDoubleEndedPriorityQueueNode{
    Player* player = nullptr;
    PlayerDoubleEndedPriorityQueueNode* next = nullptr;
    PlayerDoubleEndedPriorityQueueNode* prev = nullptr;
};

/// @brief The double ended priority queue that will be used to queue the players for the qualifier round and knock out round.
/// In order to have interesting matches, the players will be queued based on their tennis rating.
/// Strongest Player will be paired with the weakest player
class PlayerDoubleEndedPriorityQueue {
    PlayerDoubleEndedPriorityQueueNode* head;
    PlayerDoubleEndedPriorityQueueNode* tail;

    public:
        int number_of_remaining_players_in_queue = 0;

        PlayerDoubleEndedPriorityQueue(Player** all_players, int number_of_players) {
            for (int i = 0; i < number_of_players; i++) {
                put(all_players[i]);
            }
        }

        void put(Player* player) {
                PlayerDoubleEndedPriorityQueueNode* newNode = new PlayerDoubleEndedPriorityQueueNode{player, nullptr, nullptr};

                if (head == nullptr) {
                    head = tail = newNode;
                } else {
                    PlayerDoubleEndedPriorityQueueNode* current = head;
                    while (current != nullptr && current->player->rating < player->rating) {
                        current = current->next;
                    }

                    if (current == nullptr) {
                        // Insert at the end
                        tail->next = newNode;
                        newNode->prev = tail;
                        tail = newNode;
                    } else if (current == head) {
                        // Insert at the beginning
                        newNode->next = head;
                        head->prev = newNode;
                        head = newNode;
                    } else {
                        // Insert in the middle
                        newNode->next = current;
                        newNode->prev = current->prev;
                        current->prev->next = newNode;
                        current->prev = newNode;
                    }
                }
                this->number_of_remaining_players_in_queue++;
            }

        Player* get_min() {
            if (head == nullptr) return nullptr;
            return head->player;
        }

        Player* get_max() {
            if (tail == nullptr) return nullptr;
            return tail->player;
        }

        void remove_min() {
            if (head == nullptr) return;

            PlayerDoubleEndedPriorityQueueNode* temp = head;
            head = head->next;
            if (head != nullptr) {
                head->prev = nullptr;
            } else {
                tail = nullptr; // The queue is now empty
            }
            delete temp;
            this->number_of_remaining_players_in_queue--;
        }

        void remove_max() {
            if (tail == nullptr) return;

            PlayerDoubleEndedPriorityQueueNode* temp = tail;
            tail = tail->prev;
            if (tail != nullptr) {
                tail->next = nullptr;
            } else {
                head = nullptr; // The queue is now empty
            }
            delete temp;
            this->number_of_remaining_players_in_queue--;
        }

        bool is_empty() {
            return head == nullptr;
        }

        /// display the queue in this fashion, from the left, and then from the right, and then loop
        // it should only do until the middle, because the other half is the same as the first half
        void display_queue() {

            int left_element = 0;
            int right_element = number_of_remaining_players_in_queue - 1;
            PlayerDoubleEndedPriorityQueueNode* left_current = head;
            PlayerDoubleEndedPriorityQueueNode* right_current = tail;

            // Traverse from both ends until the pointers meet or cross
            while (left_current != nullptr && right_current != nullptr && left_element < right_element) {
                std::cout << left_current->player->name << " (" << left_current->player->rating << ") vs "
                          << right_current->player->name << " (" << right_current->player->rating << ")" << std::endl;

                // Move left pointer forward and right pointer backward
                left_current = left_current->next;
                left_element++;
                right_current = right_current->prev;
                right_element++;
            }
        }

        Player** get_remaining_players() {
            Player** remaining_players = new Player*[number_of_remaining_players_in_queue];
            PlayerDoubleEndedPriorityQueueNode* current = head;
            int i = 0;
            while (current != nullptr) {
                remaining_players[i] = current->player;
                current = current->next;
                i++;
            }
            return remaining_players;
        }
};

/// @brief single qualifying round that qualify the players to the next round (Circular Queue)
class QualifierRoundMatchmakingSystem : public BaseMatchmakingSystem{

    ///A list of players that will be considered for the current matchmaking round
    PlayerDoubleEndedPriorityQueue* matchmaking_queue;

    /// number of remaining players in the list
    int number_of_remaining_players = 0;
    int number_of_total_qualifying_players = 0;

    Match* current_running_matches = nullptr;

    public:

        /// @brief setup the Qualifier Round Matchmaking System
        QualifierRoundMatchmakingSystem(Player** player_list, int number_of_qualifying_players);

        /// @returns a list of all the matches that can be made and schedule right now with the players in the queue
        MatchesContainer* matchmake() override;

        void enqueue(Player* player) {
            matchmaking_queue->put(player);
        }

        void display_matchmaking_queue() override{
            matchmaking_queue->display_queue();
        }

        Player** get_remaining_players() override{
            return matchmaking_queue->get_remaining_players();
        }
};

class RoundRobinRoundMatchmakingSystem : public BaseMatchmakingSystem {
    // the list of all players that are being considered

    class RoundRobinGrouping {
        Player** players;

        int front = 0;
        int back = 3;
        public:
            explicit RoundRobinGrouping(Player** players) {
                this->players = players;
            }

            int move_to_next(int original_index) {
                return (original_index + 1) % 4;
            }
            void rotate() {
                front = move_to_next(front);
                back = move_to_next(back);
            }

            MatchesContainer* matchmake() {
                //return two matches
                auto* matches_container = new MatchesContainer;
                matches_container->matches = new Match[6];
                matches_container->number_of_matches = 6;

                rotate();

                int current_adding_index = 0;
                for (int j = 0; j < 3; j++) {
                    int current_index = back;
                    for (int i = move_to_next(current_index); i < 4; i++) {
                        Match new_match{};
                        new_match.createMatch(ROUNDROBIN, players[current_index], players[i]);
                        matches_container->matches[current_adding_index] = new_match;
                        current_adding_index++;
                    }
                    rotate();
                }
                return matches_container;
            }

            Player* get_winning_player() {
                Player* winning_player = players[front];
                for (int i = 1; i < 4; i++) {
                    if (players[front]->rating > winning_player->rating) {
                        winning_player = players[front];
                    }
                }
                return winning_player;
            }

    };

    bool match_made = false;
    bool round_ended = false;

    public:
        explicit RoundRobinRoundMatchmakingSystem(Player** all_players) : BaseMatchmakingSystem(ROUNDROBIN, all_players) {}

        /// @returns a list of all the matches that can be made and schedule right now
        MatchesContainer* matchmake() override{
            if (match_made) {
                return nullptr;
            }
            match_made = true;
            int number_of_matches = 6 * 32;
            auto* matches_container = new MatchesContainer;
            matches_container->matches = new Match[number_of_matches];
            matches_container->number_of_matches = number_of_matches;
            for (int i = 0; i < 32; i++) {
                auto current_group = RoundRobinGrouping(&all_players[i * 4]);
                auto* group_matches = current_group.matchmake();
                for (int j = 0; j < 6; j++) {
                    matches_container->matches[i * 6 + j] = group_matches->matches[j];
                }
            }
            set_is_completed(true);
            return matches_container;
        }

        void display_matchmaking_queue() override {
            if (match_made) {
                std::cout << "Matchmaking has been done already! Please check the scheduling system for reference!" << std::endl;
                return;
            }
        }

        //return 32 players that are qualified to move onto
        Player** get_remaining_players() override {
            Player** winning_players = new Player*[32];
            for (int i = 0; i < 32; i++) {
                winning_players[i] = RoundRobinGrouping(&all_players[i * 4]).get_winning_player();
            }
            return winning_players;
        }
};

class KnockoutRoundMatchmakingSystem: public BaseMatchmakingSystem {

    Player** knockout_round_queue;
    int front = -1;
    int last = 0;

    Match* current_running_matches = nullptr;

    int number_of_remaining_players;
    int number_of_remaining_players_in_queue;

    inline int move_to_next_index(int current_index) {
        return (current_index + 1) % 32;
    }

    Player* dequeue() {
        if (front == -1) {
            // queue is empty
            this->number_of_remaining_players_in_queue = 0;
            return nullptr;
        }
        Player* player = knockout_round_queue[front];
        this->number_of_remaining_players_in_queue--;
        if (number_of_remaining_players_in_queue == 0) {
            front = -1;
        }else {
            front = move_to_next_index(front);
        }
        return player;
    }

    Player* peek() {
        if (front == -1) {
            // queue is empty
            return nullptr;
        }
        Player* player = knockout_round_queue[front];
        return player;
    }

    public:
        explicit KnockoutRoundMatchmakingSystem(Player** all_players) : BaseMatchmakingSystem(MATCH_TYPE::KNOCKOUT, all_players) {
            knockout_round_queue = new Player*[32];
            for (int i = 0; i < 32; i++) {
                Player* player_to_add = all_players[i];
                enqueue(player_to_add);
            }
            this->number_of_remaining_players = 32;
            this->number_of_remaining_players_in_queue = 32;
        }

        void enqueue(Player* player) {
            if (front == last) {
                // queue is full
                return;
            }

            if (front == -1) {
                front = 0;
            }

            knockout_round_queue[last] = player;
            this->number_of_remaining_players_in_queue++;
            last = move_to_next_index(last);
        }

        MatchesContainer* matchmake() override {
            //number of remaining players that will move onto the next stage
            int potential_remaining_players_after_matchmaking = this->number_of_remaining_players;

            //number of players that currently available to be matchmade
            int potential_players_in_queue = this->number_of_remaining_players_in_queue;

            if (potential_remaining_players_after_matchmaking == 0) {
                // the process of getting players is now completed, the other tournament class move to round robin stage
                set_is_completed(true);
                return nullptr;
            }

            int number_of_matches_to_be_made = 0;
            while (potential_players_in_queue > 0 && potential_remaining_players_after_matchmaking > 0) {
                number_of_matches_to_be_made++;
                potential_players_in_queue -= 2;
                potential_remaining_players_after_matchmaking -= 1;
            }

            if (number_of_matches_to_be_made == 0) {
                return nullptr;
            }

            auto* matches_container = new MatchesContainer;
            matches_container->matches = new Match[number_of_matches_to_be_made];
            for (int i = 0; i < number_of_matches_to_be_made; i++) {
                Match new_match{};
                Player* player1_info = dequeue();
                Player* player2_info = dequeue();
                new_match.createMatch(KNOCKOUT, player1_info, player2_info);
                std::cout << player1_info->name << " " << player2_info->name << std::endl;
                matches_container->matches[i] = new_match;
            }

            std::cout << "Number of Remaining Players : " << potential_remaining_players_after_matchmaking << std::endl;;
            std::cout << "Matches Created : " << number_of_matches_to_be_made << std::endl;

            this->number_of_remaining_players = potential_remaining_players_after_matchmaking;
            matches_container->number_of_matches = number_of_matches_to_be_made;
            return matches_container;
        }

        void display_matchmaking_queue() override {
            int current_index = front;
            for (int i = 0; i < number_of_remaining_players_in_queue; i++) {
                std::cout << knockout_round_queue[current_index]->name << " " <<  knockout_round_queue[current_index]->rating << std::endl;
                current_index = move_to_next_index(current_index);
            }
        }

        Player** get_remaining_players() override {
            return nullptr;
        }
};

/// @brief records player ranking using "Stack" data structure
class PlayerRanking {
    /// @brief container that stores the player ranking as Stack
    Player** player_ranking_container;

    int maximum_number_of_elements;
    int index;
    public:
        PlayerRanking(int total_number_of_players) : maximum_number_of_elements(total_number_of_players), player_ranking_container(new Player*[total_number_of_players]), index(0) {}

        void push(Player* player) {
            if (index < 0 || index >= maximum_number_of_elements) {
                throw std::out_of_range("PlayerRanking::push");
            }
            player_ranking_container[index] = player;
            index++;
        }

        Player* pop() {
            if (index - 1 < 0) {
                throw std::out_of_range("PlayerRanking::pop");
            }
            index--;
            return player_ranking_container[index];
        }

        Player* peek() {
            if (index - 1 < 0) {
                throw std::out_of_range("PlayerRanking::pop");
            }
            return player_ranking_container[index];
        }

        void display_ranking() {
            for (int i = index; i >= 0; i--) {
                std::cout << player_ranking_container[i]->name << std::endl;
            }
        }
};

/// @brief the Matchmaking System that is used to matchmake between two players while managing the different stages of the game
class MatchmakingSystem {

    /// @brief A list of players that are still within the game (sorted)
    Player** player_list;

    /// <summary>
    /// The type of matches that are currently running
    /// </summary>
    MATCH_TYPE current_matching_type = QUALIFIER;

    BaseMatchmakingSystem* base_matchmaking_system = nullptr;

    TournamentSchedulingSystem* tournament_scheduling_system = nullptr;

    int number_of_qualifier_round_players;

    int total_number_of_players;

    PlayerRanking* player_ranking;

    public:

        MatchmakingSystem(Player** player_list, int number_of_players, TournamentSchedulingSystem* tss);
        /// @return an array of matches
        void matchmake();

        void add_player_back_to_matchmaking(Player* player);

        bool update_match_status(Match* target_match, MATCH_STATUS status);

        void display_matchmaking_queue();
};
