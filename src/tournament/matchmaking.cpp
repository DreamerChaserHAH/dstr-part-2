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
    this->base_matchmaking_system = new QualifierRoundMatchmakingSystem(&player_list[105], number_of_qualifier_round_players);
}

/// @return an array of matches
void MatchmakingSystem::matchmake() {

    if (base_matchmaking_system == nullptr) {
        // The tournament has completed because there is no more stages to go through!
        std::cout << "No more matchmaking can be done as a winner has already been defined!" << std::endl;
        return;
    }

    MatchesContainer* matches_container = this->base_matchmaking_system->matchmake();

    if (this->base_matchmaking_system->completed()) {
        switch (this->base_matchmaking_system->get_current_stage()) {
            case QUALIFIER:
                current_matching_type = ROUNDROBIN;
                this->total_number_of_players = 128;
                for (int i = 0; i < 8; i++) {
                    player_list[112 + i] = base_matchmaking_system->get_remaining_players()[i];
                }
                this->base_matchmaking_system = new RoundRobinRoundMatchmakingSystem(player_list);
                matches_container = this->base_matchmaking_system->matchmake();
                break;
            case ROUNDROBIN:
                break;
            case KNOCKOUT:
                default:
                break;
        }
    }

    tournament_scheduling_system->add_schedule(matches_container);
}

void MatchmakingSystem::add_player_back_to_matchmaking(Player* player) {
    if (current_matching_type == QUALIFIER) {
        dynamic_cast<QualifierRoundMatchmakingSystem*>(this->base_matchmaking_system)->enqueue(player);
    }
}

bool MatchmakingSystem::update_match_status(Match* target_match, MATCH_STATUS status) {
    switch (base_matchmaking_system->get_current_stage()) {
        case QUALIFIER:
            switch (status) {
                case PLAYER_ONE_WIN:
                    add_player_back_to_matchmaking(target_match->player1);
                    target_match->player1->performance.total_matches_played++;
                    target_match->player1->performance.total_matches_won++;

                    target_match->player2->performance.total_matches_played++;
                    target_match->player2->performance.total_matches_lost++;
                    break;
                case PLAYER_TWO_WIN:
                    add_player_back_to_matchmaking(target_match->player2);
                    target_match->player2->performance.total_matches_played++;
                    target_match->player2->performance.total_matches_won++;

                    target_match->player1->performance.total_matches_played++;
                    target_match->player1->performance.total_matches_lost++;
                    break;
                case DRAW:
                    default:
                    auto* rematch = new Match;
                    rematch->createMatch(target_match->match_type, target_match->player1, target_match->player2);
                    tournament_scheduling_system->add_schedule(rematch);
                    target_match->player2->performance.total_matches_played++;
                    target_match->player1->performance.total_matches_played++;
                break;
            }
            break;
        case ROUNDROBIN:
            break;
        case KNOCKOUT:
            default:
            break;
    }
    return true;
}

void MatchmakingSystem::display_matchmaking_queue() {
    this->base_matchmaking_system->display_matchmaking_queue();
}
