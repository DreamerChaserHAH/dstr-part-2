#include <tournament/matchmaking.hpp>

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

void QualifierRoundMatchmakingSystem::enqueue(Player* player) {
    matchmaking_queue->enqueue(player);
}

void QualifierRoundMatchmakingSystem::display_matchmaking_queue() {
    matchmaking_queue->display_queue();
}

Player **QualifierRoundMatchmakingSystem::get_remaining_players() {
    return matchmaking_queue->get_remaining_players();
}

KnockoutRoundMatchmakingSystem::KnockoutRoundMatchmakingSystem(Player** all_players) : BaseMatchmakingSystem(MATCH_TYPE::KNOCKOUT, all_players) {
    knockout_round_queue = new PlayerDoubleEndedPriorityQueue(all_players, 32);
    this->number_of_remaining_players = 32;
    player_ranking = new PlayerRanking(32);
    for (int i = 0; i < 32; i++) {
        all_players[i]->performance.current_round = MATCH_TYPE::KNOCKOUT;
    }
}

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

void KnockoutRoundMatchmakingSystem::enqueue(Player *player) {
    this->knockout_round_queue->enqueue(player);
}

void KnockoutRoundMatchmakingSystem::push_to_ranking(Player* player) {
    player_ranking->push(player);

}

void KnockoutRoundMatchmakingSystem::display_ranking() {
    player_ranking->display_ranking();
}

void KnockoutRoundMatchmakingSystem::display_matchmaking_queue() {
    this->knockout_round_queue->display_queue();
}

Player **KnockoutRoundMatchmakingSystem::get_remaining_players() {
    return nullptr;
}

MatchmakingSystem::MatchmakingSystem(Player** player_list, int number_of_players, TournamentSchedulingSystem* tss){
    this->player_list = new Player*[number_of_players];

    for (int i = 0; i < number_of_players; i++) {
        this->player_list[i] = player_list[i];
    }

    this->total_number_of_players = number_of_players;
    number_of_qualifier_round_players = number_of_players - 120;
    //this->qualifier_round_matchmaking_system = new QualifierRoundMatchmakingSystem();
    tournament_scheduling_system = tss;

    /// V.I.P players from index 112 to the end, will be considered with priority for up to 8 players

    this->base_matchmaking_system = new QualifierRoundMatchmakingSystem(&player_list[120], number_of_qualifier_round_players);
}

/// @return an array of matches
void MatchmakingSystem::matchmake() {

    if (base_matchmaking_system == nullptr || completed) {
        // The tournament has completed because there is no more stages to go through!
        std::cout << "No more matchmaking can be done as a winner has already been defined! Please view the ranking for details" << std::endl;
        return;
    }

    MatchesContainer* matches_container = this->base_matchmaking_system->matchmake();

    if (this->base_matchmaking_system->get_current_stage() == KNOCKOUT) {
        std::cout << "Current Matchmaking Stage is Knockout!" << std::endl;
    }

    if (this->base_matchmaking_system->completed()) {
        switch (this->base_matchmaking_system->get_current_stage()) {
            case QUALIFIER:
                current_matching_type = ROUNDROBIN;
                this->total_number_of_players = 128;
                for (int i = 0; i < 8; i++) {
                    player_list[120 + i] = base_matchmaking_system->get_remaining_players()[i];
                }
                this->base_matchmaking_system = new RoundRobinRoundMatchmakingSystem(player_list);
                this->current_matching_type = ROUNDROBIN;
                matches_container = this->base_matchmaking_system->matchmake();
                break;
            case ROUNDROBIN:
                // check if the scheduling system is empty or not
                if (tournament_scheduling_system->peek() != nullptr) {
                    std::cout << "All matches inside Round Robin Stage must be completed before the matchmaking system can proceed to the next round" << std::endl;
                    break;
                }
                this->current_matching_type = KNOCKOUT;
                std::cout << "32 players that will proceed to knockout round has been generated!" << std::endl;
                this->base_matchmaking_system = new KnockoutRoundMatchmakingSystem(base_matchmaking_system->get_remaining_players());
                matches_container = this->base_matchmaking_system->matchmake();
                break;
            case KNOCKOUT:
                std::cout << "The tournament has been completed!" << std::endl;
                completed = true;
                break;
            default:
                break;
        }
    }

    tournament_scheduling_system->push(matches_container);
}

void MatchmakingSystem::display_ranking() {
    if (current_matching_type == KNOCKOUT) {
        if (base_matchmaking_system->completed() && tournament_scheduling_system->peek() == nullptr) {
            dynamic_cast<KnockoutRoundMatchmakingSystem*>(this->base_matchmaking_system)->display_ranking();
            return;
        }
        std::cout << "Ranking is only available after all the matches are completed!" << std::endl;
        return;
    }
    std::cout << "Ranking is only available in Knockout Round!" << std::endl;
}

void MatchmakingSystem::add_player_back_to_matchmaking(Player* winning_player, Player* losing_player) {
    if (current_matching_type == QUALIFIER) {
        dynamic_cast<QualifierRoundMatchmakingSystem*>(this->base_matchmaking_system)->enqueue(winning_player);
        losing_player->performance.current_status = COMPLETED;
    }

    if (current_matching_type == KNOCKOUT) {
        dynamic_cast<KnockoutRoundMatchmakingSystem*>(this->base_matchmaking_system)->enqueue(winning_player);
        dynamic_cast<KnockoutRoundMatchmakingSystem*>(this->base_matchmaking_system)->push_to_ranking(losing_player);
        losing_player->performance.current_status = COMPLETED;
    }
}

bool MatchmakingSystem::update_match_status(Match* target_match, MATCH_STATUS status) {
    switch (base_matchmaking_system->get_current_stage()) {
        case QUALIFIER:
            case KNOCKOUT:
            switch (status) {
                case PLAYER_ONE_WIN:
                    add_player_back_to_matchmaking(target_match->player1, target_match->player2);
                    target_match->player1->performance.total_matches_won++;
                    target_match->player2->performance.total_matches_lost++;
                    break;
                case PLAYER_TWO_WIN:
                    add_player_back_to_matchmaking(target_match->player2, target_match->player1);
                    target_match->player2->performance.total_matches_won++;
                    target_match->player1->performance.total_matches_lost++;
                    break;
                case DRAW:
                    default:
                    auto* rematch = new Match;
                    rematch->createMatch(target_match->match_type, target_match->player1, target_match->player2);
                    tournament_scheduling_system->push(rematch);
                break;
            }
            break;
        case ROUNDROBIN:
            switch (status) {
                case PLAYER_ONE_WIN:
                    target_match->player1->performance.total_matches_won++;
                    target_match->player2->performance.total_matches_lost++;
                    target_match->player1->performance.roundrobin_score += 2;
                    break;
                case PLAYER_TWO_WIN:
                    target_match->player2->performance.total_matches_won++;
                    target_match->player1->performance.total_matches_lost++;
                    target_match->player2->performance.roundrobin_score += 2;
                    break;
                case DRAW:
                    default:
                    target_match->player1->performance.roundrobin_score += 1;
                    target_match->player2->performance.roundrobin_score += 1;
                    break;
            }
            break;
        default:
            break;
    }
    target_match->player2->performance.total_matches_played++;
    target_match->player1->performance.total_matches_played++;
    return true;
}

void MatchmakingSystem::display_matchmaking_queue() {
    this->base_matchmaking_system->display_matchmaking_queue();
}
