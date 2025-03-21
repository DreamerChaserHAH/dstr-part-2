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
    bool is_completed = false;
    /// @brief the type of match
    MATCH_TYPE match_type = UNDEFINED;

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
    PlayerDoubleEndedPriorityQueueNode* head = nullptr;
    PlayerDoubleEndedPriorityQueueNode* tail = nullptr;

    public:
        int number_of_remaining_players_in_queue = 0;

        PlayerDoubleEndedPriorityQueue(Player** all_players, int number_of_players) {
            for (int i = 0; i < number_of_players; i++) {
                enqueue(all_players[i]);
            }
        }

        void enqueue(Player* player) {
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

        void dequeue_min() {
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

        void dequeue_max() {
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
                right_element--;
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
    PlayerDoubleEndedPriorityQueue* matchmaking_queue = nullptr;

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
            matchmaking_queue->enqueue(player);
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

    /// @brief Circular Queue Implementation for Round Robin Matches
    class RoundRobinGrouping {
        Player* players[4];  // Fixed-size circular array
        int size = 4;

    public:
        explicit RoundRobinGrouping(Player** input_players) {
            for (int i = 0; i < size; i++) {
                players[i] = input_players[i];
            }
        }

        void rotate() {
            Player* temp = players[0];
            for (int i = 0; i < size - 1; i++) {
                players[i] = players[i + 1];
            }
            players[size - 1] = temp;
        }

        MatchesContainer* matchmake() {
            auto* matches_container = new MatchesContainer;
            matches_container->matches = new Match[6];
            matches_container->number_of_matches = 6;

            int current_adding_index = 0;
            for (int round = 0; round < 3; round++) {
                for (int i = 0; i < size / 2; i++) {
                    Match new_match{};
                    new_match.createMatch(ROUNDROBIN, players[i], players[size - 1 - i]);
                    matches_container->matches[current_adding_index++] = new_match;
                }
                rotate();
            }
            return matches_container;
        }

        Player* get_winning_player() {

            Player* winning_player = players[0];

            for (int i = 1; i < size; i++) {
                if (players[i]->rating > winning_player->rating) {
                    winning_player->performance.current_status = COMPLETED;
                    winning_player = players[i];
                    continue;
                }
                players[i]->performance.current_status = COMPLETED;
            }
            winning_player->performance.current_status = COMPETING;
            return winning_player;
        }
    };

    bool match_made = false;
    bool round_ended = false;

    public:
        explicit RoundRobinRoundMatchmakingSystem(Player** all_players) : BaseMatchmakingSystem(ROUNDROBIN, all_players) {
            for (int i = 0; i < 128; i++) {
                all_players[i]->performance.current_round = ROUNDROBIN;
            }
        }

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

/// @brief records player ranking using "Stack" data structure
class PlayerRanking {
    /// @brief container that stores the player ranking as Stack
    Player** player_ranking_container = nullptr;

    int maximum_number_of_elements;
    int index;

    void print_player(Player* player) {
        std::cout << std::setw(30) << player->name << std::setw(20) << player->rating << std::setw(20) << player->country_of_origin << std::endl;
    };
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

    void reset_to_top() {
        index = maximum_number_of_elements;
    }

    Player* peek() {
        if (index - 1 < 0) {
            throw std::out_of_range("PlayerRanking::pop");
        }
        return player_ranking_container[index - 1];
    }

    ///First Place -> First Player
    ///Second Place -> Second Player is the one that lost to second player in the final round
    ///Third Place -> The one that lost to First Player and Second Player in the semi-final round
    ///it goes in this pattern (1,1,2,4,8,16)
    void display_ranking() {

        std::cout << "**1st Place**" << std::endl;
        print_player(pop());
        std::cout << std::endl;

        int counter = 2;
        for (int i = 1; i <= 16; i = i *2) {
            std::cout << "**"<<counter++ << " Place**" << std::endl;
            for (int j = 0; j < i; j++) {
                print_player(pop());
            }
        }
        reset_to_top();
    }
};

class KnockoutRoundMatchmakingSystem: public BaseMatchmakingSystem {

    PlayerRanking* player_ranking = nullptr;

    PlayerDoubleEndedPriorityQueue* knockout_round_queue = nullptr;

    Match* current_running_matches = nullptr;

    int number_of_remaining_players;

    public:

        KnockoutRoundMatchmakingSystem(Player** all_players);

        void enqueue(Player* player) {
            this->knockout_round_queue->enqueue(player);
        }

        void push_to_ranking(Player* player) {
            player_ranking->push(player);
        }

        void display_ranking() {
            player_ranking->display_ranking();
        }

        MatchesContainer* matchmake() override;

        void display_matchmaking_queue() override {
            this->knockout_round_queue->display_queue();
        }

        Player** get_remaining_players() override {
            return nullptr;
        }
};

/// @brief the Matchmaking System that is used to matchmake between two players while managing the different stages of the game
class MatchmakingSystem {

    /// @brief A list of players that are still within the game (sorted)
    Player** player_list = nullptr;

    /// <summary>
    /// The type of matches that are currently running
    /// </summary>
    MATCH_TYPE current_matching_type = QUALIFIER;

    BaseMatchmakingSystem* base_matchmaking_system = nullptr;

    TournamentSchedulingSystem* tournament_scheduling_system = nullptr;

    int number_of_qualifier_round_players = -1;

    int total_number_of_players = -1;

    bool completed = false;

    public:

        MatchmakingSystem(Player** player_list, int number_of_players, TournamentSchedulingSystem* tss);
        /// @return an array of matches
        void matchmake();

        void add_player_back_to_matchmaking(Player* winning_player, Player* losing_player);

        void display_ranking();

        bool update_match_status(Match* target_match, MATCH_STATUS status);

        void display_matchmaking_queue();
};
