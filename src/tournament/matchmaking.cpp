#include <tournament/matchmaking.hpp>

MatchmakingSystem::MatchmakingSystem(Player** player_list, int number_of_players, TournamentSchedulingSystem* tss){
    this->player_list = new Player*[number_of_players];
    for (int i = 0; i < number_of_players; i++) {
        this->player_list[i] = player_list[i];
    }

    this->total_number_of_players = number_of_players;
    number_of_qualifier_round_players = number_of_players - 112;
    //this->qualifier_round_matchmaking_system = new QualifierRoundMatchmakingSystem();
    tournament_scheduling_system = tss;

    if (this->qualifier_round_matchmaking_system == nullptr) {
        this->qualifier_round_matchmaking_system = new QualifierRoundMatchmakingSystem(&player_list[105], number_of_qualifier_round_players);
    }
}

/// @return an array of matches
Match* MatchmakingSystem::matchmake() {
    if (current_matching_type == QUALIFIER) {
        tournament_scheduling_system->add_schedule(this->qualifier_round_matchmaking_system->matchmake());
        if (this->qualifier_round_matchmaking_system->is_ended()) {
            current_matching_type = ROUNDROBIN;
            this->total_number_of_players = 128;
            for (int i = 0; i < 8; i++) {
                player_list[112 + i] = qualifier_round_matchmaking_system->get_remaining_players()[i];
            }
        }
    }

    if (current_matching_type == MATCH_TYPE::ROUNDROBIN) {
        if (round_robin_round_matchmaking_system == nullptr) {
            round_robin_round_matchmaking_system = new RoundRobinRoundMatchmakingSystem(player_list);
        }
        tournament_scheduling_system->add_schedule(this->round_robin_round_matchmaking_system->matchmake());
        if (this->round_robin_round_matchmaking_system->is_ended()) {
            current_matching_type = MATCH_TYPE::KNOCKOUT;
            this->total_number_of_players = 128;
            for (int i = 0; i < 8; i++) {
                player_list[112 + i] = round_robin_round_matchmaking_system->get_remaining_players()[i];
            }
        }
    }
    return nullptr;
}


void MatchmakingSystem::add_player_back_to_matchmaking(Player* player) {
    qualifier_round_matchmaking_system->enqueue(player);
}

bool MatchmakingSystem::update_match_status(Match* target_match, MATCH_STATUS status) {
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

void MatchmakingSystem::display_matchmaking_queue() {
    switch(current_matching_type){
        case QUALIFIER:
            return qualifier_round_matchmaking_system->display_matchmaking_queue();
        case ROUNDROBIN:
        case KNOCKOUT:
            break;
    }
}
