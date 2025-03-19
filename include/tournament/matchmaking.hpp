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

/// the base class that all 3 matchmaking system type will be depending on
class BaseMatchmakingSystem {
    // check if this stage is completed or not
    bool is_completed;
    // the type of match
    MATCH_TYPE match_type;

    public:
    virtual ~BaseMatchmakingSystem() = default;

    BaseMatchmakingSystem(MATCH_TYPE match_type) {
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

        /// Matchmake with the information on hand
        virtual MatchesContainer* matchmake() = 0;

        /// display the list of players that is being processed inside the queue
        virtual void display_matchmaking_queue() = 0;

        /// @returns a 8 player list that are qualified from here
        virtual Player** get_remaining_players() = 0;
};

/// @brief single qualifying round that qualify the players to the next round (Circular Queue)
class QualifierRoundMatchmakingSystem : public BaseMatchmakingSystem{

    ///A list of all the players that are being considered
    Player** all_players;
    ///A list of players that will be considered for the current matchmaking round
    Player** matchmaking_free_queue = nullptr;

    /// number of remaining players in the list

    int number_of_remaining_players_in_queue = 0;
    int number_of_remaining_players = 0;
    int number_of_total_qualifying_players = 0;

    int front = 0;
    int last = 0;

    Match* current_running_matches = nullptr;

    inline int move_to_next_index(int current_index) {
        return (current_index + 1) % number_of_total_qualifying_players;
    }

    Player* dequeue() {
        if (front == last) {
            // queue is empty
            return nullptr;
        }
        Player* player = matchmaking_free_queue[front];
        front = move_to_next_index(front);
        this->number_of_remaining_players_in_queue--;
        return player;
    }

    Player* peek() {
        if (front == last) {
            // queue is empty
            return nullptr;
        }
        Player* player = matchmaking_free_queue[front];
        return player;
    }

    public:
        QualifierRoundMatchmakingSystem(Player** player_list, int number_of_qualifying_players) : BaseMatchmakingSystem(QUALIFIER){
            this->all_players = player_list;
            this->matchmaking_free_queue = new Player*[number_of_qualifying_players];
            for(int i = 0; i < number_of_qualifying_players; i++){
                player_list[i]->performance.current_round = MATCH_TYPE::QUALIFIER;
                enqueue(player_list[i]);
            }
            this->number_of_total_qualifying_players = number_of_qualifying_players;
            this->number_of_remaining_players = number_of_qualifying_players;
            this->number_of_remaining_players_in_queue = number_of_qualifying_players;
        }
        /// @returns a list of all the matches that can be made and schedule right now
        MatchesContainer* matchmake() override{
            int potential_remaining_players_after_matchmaking = this->number_of_remaining_players;
            int potential_players_in_queue = this->number_of_remaining_players_in_queue;
            if (potential_remaining_players_after_matchmaking == 8) {
                // the process of getting players is now completed, the other tournament class move to round robin stage
                set_is_completed(true);
                return nullptr;
            }

            int number_of_matches_to_be_made = 0;
            while (potential_remaining_players_after_matchmaking > 8 && potential_players_in_queue > 0) {
                number_of_matches_to_be_made++;
                potential_players_in_queue -= 2;
                potential_remaining_players_after_matchmaking -= 1;
            }

            auto* matches_container = new MatchesContainer;
            matches_container->matches = new Match[number_of_matches_to_be_made];
            for (int i = 0; i < number_of_matches_to_be_made; i++) {
                Player* player1 = dequeue();
                Player* player2 = dequeue();
                Match new_match{};
                new_match.createMatch(QUALIFIER, player1, player2);
                matches_container->matches[i] = new_match;
            }

            std::cout << "Number of Remaining Players : " << this->number_of_remaining_players << std::endl;;
            std::cout << "Matches Created : " << number_of_matches_to_be_made << std::endl;

            this->number_of_remaining_players = potential_remaining_players_after_matchmaking;
            matches_container->number_of_matches = number_of_matches_to_be_made;
            return matches_container;
        }

        void enqueue(Player* player) {
            if (front == move_to_next_index(last)) {
                // queue is full
                return;
            }
            matchmaking_free_queue[last] = player;
            this->number_of_remaining_players_in_queue++;
            last = move_to_next_index(last);
        }

        void display_matchmaking_queue() override{
            int current_index = front;
            for (int i = 0; i < number_of_remaining_players_in_queue; i++) {
                std::cout << matchmaking_free_queue[current_index]->name << std::endl;
                current_index = move_to_next_index(current_index);
            }
        }

        Player** get_remaining_players() override{
            return matchmaking_free_queue;
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
                for (int j = 0; j < 3; j++) {
                    int current_index = back;
                    for (int i = 0; i < 2; i++) {
                        Match new_match{};
                        int player_1_index = move_to_next(current_index);
                        int player_2_index = move_to_next(player_1_index);
                        new_match.createMatch(ROUNDROBIN, players[player_1_index], players[player_2_index]);
                        matches_container->matches[j * 2 + i] = new_match;
                    }
                    rotate();
                }
                return matches_container;
            }
    };

    Player** all_players;

    bool match_made = false;
    bool round_ended = false;

    public:
        explicit RoundRobinRoundMatchmakingSystem(Player** players) : BaseMatchmakingSystem(ROUNDROBIN) {
            this->all_players = players;
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
            return matches_container;
        }

        void display_matchmaking_queue() override {
            if (match_made) {
                std::cout << "Matchmaking has been done already! Please check the scheduling system for reference!" << std::endl;
                return;
            }
        }
        Player** get_remaining_players() override {
            return nullptr;
        }
};

class KnockoutRoundMatchmakingSystem {
    public:
        KnockoutRoundMatchmakingSystem() {

        }
};

class MatchmakingSystem {
    /// <summary>
    /// A list of players that are still within the game (sorted)
    /// </summary>
    Player** player_list;

    /// <summary>
    /// The type of matches that are currently running
    /// </summary>
    MATCH_TYPE current_matching_type = QUALIFIER;

    BaseMatchmakingSystem* base_matchmaking_system = nullptr;

    TournamentSchedulingSystem* tournament_scheduling_system = nullptr;

    int number_of_qualifier_round_players;

    int total_number_of_players;

    public:

        MatchmakingSystem(Player** player_list, int number_of_players, TournamentSchedulingSystem* tss);
        /// @return an array of matches
        void matchmake();

        void add_player_back_to_matchmaking(Player* player);

        bool update_match_status(Match* target_match, MATCH_STATUS status);

        void display_matchmaking_queue();
};
