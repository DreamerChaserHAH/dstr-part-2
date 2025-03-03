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

/// @brief Qualifier Round with Circular Queue implementation
/*class QualifierRoundMatchmakingSystem{
    /// @brief queue the players
    Player* matchmaking_queue;

    // @brief depending on the number of players in the qualifier round, a certain number of players will be chosen for wildcard
    Player* wildcard_players;

    int front = 0;
    int last = 0;

    int number_of_qualifier_round_players = 0;
    int number_of_players_in_queue = 0;

    inline int move_to_next_index(int current_index){
        return (current_index + 1) % number_of_qualifier_round_players;
    }

    public:
        QualifierRoundMatchmakingSystem(Player* player_list, int number_of_qualifier_round_players){
            this->matchmaking_queue = new Player[number_of_qualifier_round_players];
            this->number_of_qualifier_round_players = number_of_qualifier_round_players;
            for(int i = 0; i < number_of_qualifier_round_players; i++){
                this->add_registered_player_to_matchmaking_system(&player_list[i]);
            }
        }

        /// this is essentially the enqueue operation
        void add_registered_player_to_matchmaking_system(Player* new_player){
            // add the player to the queue (last index)
            if (move_to_next_index(last) == front){
                // queue is full
                std::cout << "queue is full" << std::endl;
                return;
            }

            matchmaking_queue[move_to_next_index(last)] = *new_player;
            last = move_to_next_index(last);
        }

        std::pair<Player*, Player*> get_next_two_players(){
            if (number_of_players_in_queue == 8) {
                // the process of getting players is now completed, the other tournament class move to round robin stage
                return std::pair(nullptr, nullptr);
            }
            std::pair<Player*, Player*> players = std::make_pair(nullptr, nullptr);
            players.first = &matchmaking_queue[front];
            players.second= &matchmaking_queue[];
            front = (front + 1) % number_of_qualifier_round_players;
            number_of_players_in_queue--;
            if(front == last){
                front = -1;
                last = -1;
            }
            number_of_qualifier_round_players -= 2;
            return players;
        }

        std::pair<Player*, Player*> view_next_two_players(){
            return std::make_pair(&matchmaking_queue[front], &matchmaking_queue[(front + 1) % number_of_qualifier_round_players]);
        }

        Player* get_remaining_player(){
            return &matchmaking_queue[front];
        }
};*/

/// @brief single qualifying round that qualify the players to the next round (Circular Queue)
class QualifierRoundMatchmakingSystem {

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
    bool round_ended = false;

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
        QualifierRoundMatchmakingSystem(Player** player_list, int number_of_qualifying_players){
            this->all_players = player_list;
            this->matchmaking_free_queue = new Player*[number_of_qualifying_players];
            for(int i = 0; i < number_of_qualifying_players; i++){
                enqueue(player_list[i]);
            }
            this->number_of_total_qualifying_players = number_of_qualifying_players;
            this->number_of_remaining_players = number_of_qualifying_players;
            this->number_of_remaining_players_in_queue = number_of_qualifying_players;
        }
        /// @returns a list of all the matches that can be made and schedule right now
        MatchesContainer* matchmake(){
            int potential_remaining_players_after_matchmaking = this->number_of_remaining_players;
            int potential_players_in_queue = this->number_of_remaining_players_in_queue;
            if (potential_remaining_players_after_matchmaking == 8) {
                // the process of getting players is now completed, the other tournament class move to round robin stage
                round_ended = true;
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
                new_match.match_id = 0;
                new_match.match_type = QUALIFIER;
                new_match.player1 = player1;
                new_match.player2 = player2;
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

        void display_matchmaking_queue() {
            int current_index = front;
            for (int i = 0; i < number_of_remaining_players_in_queue; i++) {
                std::cout << matchmaking_free_queue[current_index]->name << std::endl;
                current_index = move_to_next_index(current_index);
            }
        }

        bool is_ended() {
            return round_ended;
        }
        /// @returns a 8 player list that are qualified from here
        Player** get_remaining_players() {
            return matchmaking_free_queue;
        }
};

class RoundRobinRoundMatchmakingSystem {
    public:
        RoundRobinRoundMatchmakingSystem() {

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

    /// <summary>
    ///
    /// </summary>
    QualifierRoundMatchmakingSystem* qualifier_round_matchmaking_system = nullptr;;

    RoundRobinRoundMatchmakingSystem* round_robin_round_matchmaking_system = nullptr;

    KnockoutRoundMatchmakingSystem* knockout_round_matchmaking_system = nullptr;

    TournamentSchedulingSystem* tournament_scheduling_system = nullptr;

    int number_of_qualifier_round_players;

    public:
        MatchmakingSystem(Player** player_list, int number_of_players, TournamentSchedulingSystem* tss){
            this->player_list = new Player*[number_of_players];
            for (int i = 0; i < number_of_players; i++) {
                this->player_list[i] = player_list[i];
            }

            number_of_qualifier_round_players = number_of_players - 112;
            //this->qualifier_round_matchmaking_system = new QualifierRoundMatchmakingSystem();
            tournament_scheduling_system = tss;

            if (this->qualifier_round_matchmaking_system == nullptr) {
                this->qualifier_round_matchmaking_system = new QualifierRoundMatchmakingSystem(&player_list[105], number_of_qualifier_round_players);
            }
        }

        /// @return an array of matches
        Match* matchmake() {
            if (current_matching_type == QUALIFIER) {
                tournament_scheduling_system->add_schedule(this->qualifier_round_matchmaking_system->matchmake());
            }
            return nullptr;
        }


    /// <summary>
    /// Get a list of all the matches that can be scheduled at this time with the currently available information
    /// </summary>
    /*std::pair<Player*, Player*>* matchmake(){
        switch(current_matching_type){
            case QUALIFIER:
                return qualifier_round_matchmaking_system->dequeue_next_two_players();
            case ROUNDROBIN:
                return round_robin_round_matchmaking_system->dequeue_next_two_players();
            case KNOCKOUT:
                return knockout_round_matchmaking_system->dequeue_next_two_players();
        }
    }*/

        void add_player_back_to_matchmaking(Player* player) {
            qualifier_round_matchmaking_system->enqueue(player);
        }

        bool update_match_status(Match* target_match, MATCH_STATUS status) {
            switch (status) {
                case PLAYER_ONE_WIN:
                    add_player_back_to_matchmaking(target_match->player1);
                    break;
                case PLAYER_TWO_WIN:
                    add_player_back_to_matchmaking(target_match->player2);
                    break;
                case DRAW:
                default:
                    add_player_back_to_matchmaking(target_match->player1);
                    add_player_back_to_matchmaking(target_match->player2);
            }
            return true;
        }

        void display_matchmaking_queue() {
            switch(current_matching_type){
                case QUALIFIER:
                    return qualifier_round_matchmaking_system->display_matchmaking_queue();
                case ROUNDROBIN:
                case KNOCKOUT:
                    break;
            }
        }

};