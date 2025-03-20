#include "tournament/matchmaking.hpp"

QualifierRoundMatchmakingSystem::QualifierRoundMatchmakingSystem(Player** player_list, int number_of_qualifying_players) : BaseMatchmakingSystem(QUALIFIER, player_list){
    for(int i = 0; i < number_of_qualifying_players; i++){
        int current_index = i % 2 == 0 ? (i == 0? 0: i/2) : (number_of_qualifying_players - 1) - (i/2);
        player_list[current_index]->performance.current_round = MATCH_TYPE::QUALIFIER;
    }
    this->matchmaking_queue = new PlayerDoubleEndedPriorityQueue(this->all_players, number_of_qualifying_players);
    this->number_of_total_qualifying_players = number_of_qualifying_players;
    this->number_of_remaining_players = number_of_qualifying_players;
}

MatchesContainer* QualifierRoundMatchmakingSystem::matchmake() {
    int potential_remaining_players_after_matchmaking = this->number_of_remaining_players;
    int potential_players_in_queue = this->matchmaking_queue->number_of_remaining_players_in_queue;
    if (potential_remaining_players_after_matchmaking == 8 && potential_players_in_queue == 8) {
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
        Match new_match{};
        new_match.createMatch(QUALIFIER, matchmaking_queue->get_min(), matchmaking_queue->get_max());
        matchmaking_queue->dequeue_min();
        matchmaking_queue->dequeue_max();
        matches_container->matches[i] = new_match;
    }

    std::cout << "Number of Remaining Players : " << this->number_of_remaining_players << std::endl;;
    std::cout << "Matches Created : " << number_of_matches_to_be_made << std::endl;

    this->number_of_remaining_players = potential_remaining_players_after_matchmaking;
    matches_container->number_of_matches = number_of_matches_to_be_made;
    return matches_container;
}