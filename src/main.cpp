#include <iostream>
#include "tournament/tournament_schedule.hpp"
#include "player.hpp"
#include "tournament/matchmaking.hpp"

int main() {
    CompetitionPlayerList player_list = CompetitionPlayerList("../data/tennis-tournament-players.csv");
    std::cout<< player_list.number_of_players << std::endl;
    player_list.sort();
    player_list.display_player_list();
    TournamentSchedule schedule = TournamentSchedule();
    return 0;
}