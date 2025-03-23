#include "tournament/tournament_schedule.hpp"
using namespace std;

TournamentSchedulingSystem::TournamentSchedulingSystem(){
    this->head = nullptr;;
    this->tail = nullptr;
    this->matchmaking_system = nullptr;
}

/// @brief queue the match to be played
/// @param match
void TournamentSchedulingSystem::push(MatchesContainer* matches){
    if (matches == nullptr) {
        std::cout << "There is no more matches to add!" << std::endl;
        return;
    }

    int number_of_matches = matches->number_of_matches;
    for (int i = 0; i < number_of_matches; i++) {
        push(&matches->matches[i]);
    }
}

void TournamentSchedulingSystem::push(Match* match) {
    if (match == nullptr) {
        std::cout << "You are adding an invalid match" << std::endl;
    }
    auto* new_slot = new ScheduleSlot(match, 0, MONDAY_MORNING, MAIN_COURT, nullptr);
    if(this->head == nullptr){
        // first element
        this->head = new_slot;
        this->tail = new_slot;
    }else{
        last_time_slot = get_next_time_slot(last_time_slot);
        last_court = get_next_court(last_court);
        new_slot->time_slot = last_time_slot;
        new_slot->court = last_court;
        this->tail->next_slot = new_slot;
        this->tail = new_slot;
    }
}

/// @brief dequeue the match to be played
ScheduleSlot* TournamentSchedulingSystem::dequeue(){
    if(head == nullptr){
        return nullptr;
    }
    ScheduleSlot* slot = head;
    head = head->next_slot;
    return slot;
}

/// @brief take a look at the next match to be played without removing it
ScheduleSlot* TournamentSchedulingSystem::peek(){
    return head;
}

/// @brief void print the schedule out into a .csv file for backup
void TournamentSchedulingSystem::print_schedule() {
    ScheduleSlot* current_slot = head;

    int number_of_schedules = 0;
    if (current_slot == nullptr) {
        std::cout << "Scheduling System is currently empty!" << std::endl;
    }
    std::cout << "Here's a upcoming list of schedule" << std::endl;
    std::cout << std::setw(20) << "Match ID" << std::setw(30) << "Player 1" << std::setw(10) << "Rating" << std::setw(30) << "Player 2" << std::setw(10)  << "Rating" <<std::setw(30) << "Schedule Timeslot" << std::setw(20) << "Court" << std::endl;
    while(current_slot != nullptr){
        std::cout << std::setw(20) << current_slot->match->match_id << std::setw(30) << current_slot->match->player1->name << std::setw(10)  << current_slot->match->player1->rating << std::setw(30) << current_slot->match->player2->name << std::setw(10) << current_slot->match->player2->rating << std::setw(30) << get_schedule_string(current_slot->time_slot) << std::setw(20) << (current_slot->court == MAIN_COURT ? "MAIN COURT" : "SIDE COURT") << std::endl;
        current_slot = current_slot->next_slot;
        number_of_schedules++;
    }
    std::cout << "Total Scheduled Matches: " << number_of_schedules << std::endl;
}

void TournamentSchedulingSystem::print_last_schedule() {
    ScheduleSlot* current_slot = head;

    if (current_slot == nullptr) {
        std::cout << "Scheduling System is currently empty!" << std::endl;
    }

    std::cout << "Here's the last schedule" << std::endl;
    std::cout << std::setw(20) << "Match ID" << std::setw(30) << "Player 1" << std::setw(10) << "Rating" << std::setw(30) << "Player 2" << std::setw(10)  << "Rating" <<std::setw(30) << "Schedule Timeslot" << std::setw(20) << "Court" << std::endl;
    std::cout << std::setw(20) << current_slot->match->match_id << std::setw(30) << current_slot->match->player1->name << std::setw(10)  << current_slot->match->player1->rating << std::setw(30) << current_slot->match->player2->name << std::setw(10) << current_slot->match->player2->rating << std::setw(30) << get_schedule_string(current_slot->time_slot) << std::setw(20) << (current_slot->court == MAIN_COURT ? "MAIN COURT" : "SIDE COURT") << std::endl;
}

void TournamentSchedulingSystem::last_match_completed(MATCH_STATUS status) {
    ScheduleSlot* current_slot = dequeue();

    if (current_slot == nullptr) {
        std::cout << "Scheduling System is currently empty!" << std::endl;
        return;
    }

    matchmaking_system->update_match_status(current_slot->match, status);
}

void TournamentSchedulingSystem::update_matchmaking_system(MatchmakingSystem *matchmaking_system) {
    this->matchmaking_system = matchmaking_system;
}

// Remove player from the schedule (void their matches)
void TournamentSchedulingSystem::remove_player(int player_id) {
    ScheduleSlot* current = head;
    while (current != nullptr) {
        if (current->match->player1->id == player_id || current->match->player2->id == player_id) {
            //current->match->status = WITHDRAWN;
            cout << "Match involving withdrawn player ID " << player_id << " has been voided." << endl;
        }
        current = current->next_slot;
    }
}

// Re-add player after undo withdrawal
void TournamentSchedulingSystem::read_player(Player* player) {
    cout << "Player " << player->name << " (ID: " << player->id << ") has been re-added to the tournament schedule." << endl;

    // Re-add player to matchmaking queue
    if (matchmaking_system != nullptr) {
        matchmaking_system->add_player_back_to_matchmaking(player, nullptr);
        cout << "Player " << player->name << " has been re-added to matchmaking queue." << endl;
    } else {
        cout << "Error: Matchmaking system pointer not available." << endl;
    }
}

