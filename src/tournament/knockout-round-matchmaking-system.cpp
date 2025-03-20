#include "tournament/matchmaking.hpp"

MatchesContainer* KnockoutRoundMatchmakingSystem::matchmake() {
    //number of remaining players that will move onto the next stage
    int potential_remaining_players_after_matchmaking = this->number_of_remaining_players;

    //number of players that currently available to be matchmade
    int potential_players_in_queue = this->knockout_round_queue->number_of_remaining_players_in_queue;

    if (potential_remaining_players_after_matchmaking == 1) {
        // the process of getting players is now completed, the other tournament class move to round robin stage
        set_is_completed(true);
        push_to_ranking(knockout_round_queue->get_max());
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
        Player* player1_info = this->knockout_round_queue->get_min();
        this->knockout_round_queue->dequeue_min();
        Player* player2_info = this->knockout_round_queue->get_max();
        this->knockout_round_queue->dequeue_max();
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