#include <iostream>
#include <string>
#include <limits>
#include "player.hpp"
#include "tournament/matchmaking.hpp"
#include "tournament/tournament_schedule.hpp"
#include "tournament/match_history.hpp"
#include "tickets/TicketSalesMenu.hpp"
#include "withdrawal_manager.hpp"


typedef bool (*FunctionPointer)();

CompetitionPlayerList* player_list = nullptr;
TournamentSchedulingSystem* tournament_scheduling_system = nullptr;
MatchmakingSystem* matchmaking_system = nullptr;
MATCH_STATUS result_status_buffer;
MatchHistoryQueue* match_history_queue = new MatchHistoryQueue();

inline bool initialize_matchmaking_system() {
    if (player_list == nullptr) {
        std::cout << "Player List has yet to be loaded" << std::endl;
        return true;
    }

    if (tournament_scheduling_system == nullptr) {
        std::cout << "Tournament Scheduling System has yet to be initialized" << std::endl;
        return true;
    }

    matchmaking_system = new MatchmakingSystem(player_list->sort(), player_list->number_of_players, tournament_scheduling_system);
    tournament_scheduling_system->update_matchmaking_system(matchmaking_system);
    return true;
}

inline bool start_matchmaking() {
    if (matchmaking_system == nullptr) {
        std::cout << "Matchmaking System has yet to be initialized" << std::endl;
        return true;
    }

    matchmaking_system->matchmake();
    return true;
}

inline bool view_matchmaking_queue() {
    if (matchmaking_system == nullptr) {
        std::cout << "Matchmaking System has yet to be initialized" << std::endl;
        return true;
    }

    matchmaking_system->display_matchmaking_queue();
    return true;
}

inline bool view_ranking() {
    if (matchmaking_system == nullptr) {
        std::cout << "Matchmaking System has yet to be initialized" << std::endl;
        return true;
    }
    matchmaking_system->display_ranking();
    return true;
}

inline bool initialize_scheduling_system() {
    tournament_scheduling_system = new TournamentSchedulingSystem();
    std::cout << "Tournament Scheduling System has been initialized" << std::endl;
    return true;
}

inline bool view_all_upcoming_schedule() {
    if (tournament_scheduling_system == nullptr) {
        std::cout << "" << std::endl;
        return true;
    }
    tournament_scheduling_system->print_schedule();
    return true;
}

inline bool view_upcoming_match_details() {
    if (tournament_scheduling_system == nullptr) {
        std::cout << "Tournament Scheduling System is Empty!" << std::endl;
        return true;
    }
    tournament_scheduling_system->print_last_schedule();
    return true;
}

bool load_players() {
    player_list = new CompetitionPlayerList("../data/tennis-tournament-players.csv");
    std::cout << "Player List Loaded Successfully" << std::endl;
    return true;
}

bool display_players() {
    if (player_list == nullptr) {
        std::cout << "Player List is not loaded" << std::endl;
        return true;
    }

    player_list->display_player_list();
    return true;
}

bool display_players_sorted() {
    if (player_list == nullptr) {
        std::cout << "Player List is not loaded" << std::endl;
        return true;
    }

    player_list->display_sorted_player_list();
    return true;
}

bool reset_players() {
    player_list = nullptr;
    return true;
}

bool display_player_status() {
    if (player_list == nullptr) {
        std::cout << "Player List is not loaded" << std::endl;
        return true;
    }
    player_list->display_player_status();
    return true;
}

bool run_menu(std::string context, std::string menu_name[], FunctionPointer functions[], int number_of_functions){
    std::cout << "___________________________" << std::endl;
    std::cout << context << ":" << std::endl;
    for(int i = 0; i < number_of_functions; i++){
        std::cout << i + 1 << ". " << menu_name[i] << std::endl;
    }
    std::cout << "0. Exit" << std::endl;
    std::cout << ">";
    int input;
    std::cin >> input;
    if(not std::cin.fail()){

        if (input == 0) {
            return false;
        }

        if(input < 0 || input > number_of_functions){
            std::cout << "Invalid input" << std::endl;
            return true;
        }

        return functions[input - 1]();
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Invalid input! Input must be numerical" << std::endl;
    return true;
}


bool matchmaking_menu() {
    std::string matchmaking_menu[] = {"Initialize Matchmaking System", "Start Matchmaking", "View Matchmaking Queue", "View Ranking"};
    FunctionPointer matchmaking_functions[] = {initialize_matchmaking_system, start_matchmaking, view_matchmaking_queue, view_ranking};
    while (run_menu("Scheduling and Matchmaking Management", matchmaking_menu, matchmaking_functions, 4)) {}
    return true;
}

inline bool result_player1_won() {
    result_status_buffer = PLAYER_ONE_WIN;
    return false;
}

inline bool result_player2_won() {
    result_status_buffer = PLAYER_TWO_WIN;
    return false;
}

inline bool result_draw() {
    result_status_buffer = DRAW;
    return false;
}

inline bool update_current_match_status() {
    if (tournament_scheduling_system == nullptr) {
        std::cout << "Tournament Scheduling System is Empty!" << std::endl;
        return true;
    }

    ScheduleSlot* last_schedule_slot = tournament_scheduling_system->peek();
    if (last_schedule_slot == nullptr) {
        std::cout << "There is no schedules anymore, run matchmaking system again!" << std::endl;
        return true;
    }
    std::string match_result_menu[] = {last_schedule_slot->match->player1->name + " won! (Player 1)", last_schedule_slot->match->player2->name + " won! (Player 2)", "Draw Match"};
    FunctionPointer match_result_functions[] = {result_player1_won, result_player2_won, result_draw};
    result_status_buffer = MATCH_STATUS::UNDECIDED;
    while (run_menu("Scheduling and Matchmaking Management", match_result_menu, match_result_functions, 3)) {}
    if (result_status_buffer == MATCH_STATUS::UNDECIDED) {
        return true;
    }
    tournament_scheduling_system->print_last_schedule();
    tournament_scheduling_system->last_match_completed(result_status_buffer);
    match_history_queue->enqueue(
    last_schedule_slot->match,
    result_status_buffer,
    last_schedule_slot->court,
    last_schedule_slot->time_slot
    );


    return true;
}

bool scheduling_menu() {
    std::string scheduling_menu[] = {"Initialize Scheduling Management System", "View All Upcoming Schedule", "View Upcoming Match Details", "Update Current Match Status"};
    FunctionPointer scheduling_functions[] = {initialize_scheduling_system, view_all_upcoming_schedule, view_upcoming_match_details, update_current_match_status};
    while (run_menu("Match Scheduling Management System", scheduling_menu, scheduling_functions, 4)){}
    return true;
}

bool withdrawal_management() {
    if (tournament_scheduling_system == nullptr) {
        std::cout << "Initializing Tournament Scheduling System for Withdrawal Management..." << std::endl;
        tournament_scheduling_system = new TournamentSchedulingSystem();
    }
    if (matchmaking_system == nullptr) {
        std::cout << "Initializing Matchmaking System for Withdrawal Management..." << std::endl;
        matchmaking_system = new MatchmakingSystem(player_list->sort(), player_list->number_of_players, tournament_scheduling_system);
        tournament_scheduling_system->update_matchmaking_system(matchmaking_system);
    }
    return withdrawal_menu(player_list, tournament_scheduling_system, matchmaking_system);
}


bool player_menu() {
    std::string player_menu[] = {
        "Load Players",
        "Display Players",
        "Display Players (Sorted)",
        "Reset Players",
        "Display Player Status",
        "Withdrawal Management"
    };

    FunctionPointer player_functions[] = {
        load_players,
        display_players,
        display_players_sorted,
        reset_players,
        display_player_status,
        withdrawal_management
    };

    while (run_menu("Player Management", player_menu, player_functions, 6)) {}  // Change count to 6
    return true;
}

bool view_match_history() {
    match_history_queue->displayHistory();
    return true;
}

bool save_match_history() {
    match_history_queue->saveHistoryToFile("match_history.csv");
    return true;
}
bool filter_by_match_type() {
    std::cout << "\nSelect Match Type:\n";
    std::cout << "1. Qualifier\n2. Round Robin\n3. Knockout\n";
    int choice;
    std::cin >> choice;
    MATCH_TYPE selected_type;

    switch (choice) {
        case 1:
            selected_type = QUALIFIER;
        break;
        case 2:
            selected_type = ROUNDROBIN;
        break;
        case 3:
            selected_type = KNOCKOUT;
        break;
        default:
            std::cout << "Invalid choice!\n";
        return true;
    }

    match_history_queue->filterByMatchType(selected_type);
    return true;
}
// Filter by player name function
bool filter_by_player() {
    std::cout << "\nEnter Player Name to Filter: ";
    std::string name;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get full line input (supports spaces in player name)
    std::getline(std::cin, name);

    // Call filter function from match history
    match_history_queue->filterByPlayer(name);

    return true;
}


bool match_history_menu() {
    std::string match_history_options[] = {
        "View Match History",
        "Save Match History to File",
        "Filter By Match Type",
        "Filter By Player Name"
    };

    FunctionPointer match_history_functions[] = {
        view_match_history,
        save_match_history,
        filter_by_match_type,
        filter_by_player
    };

    while (run_menu("Match History Management", match_history_options, match_history_functions, 4)) {}
    return true;
}


int main(){
     std::cout << "Welcome to APU Tennis Tournament Management System" << std::endl;
     std::string main_menu[] = {
         "Matchmaking Management", "Schedule Management System", "Player Management", "Match History Management", "Ticket Sales Management"
     };
    FunctionPointer main_functions[] = {matchmaking_menu, scheduling_menu, player_menu, match_history_menu, ticket_sales_menu};
     while (run_menu("Main Menu", main_menu, main_functions, 5)) {}
    return 0;
}
