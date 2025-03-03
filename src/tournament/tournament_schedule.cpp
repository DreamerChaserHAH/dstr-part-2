#include "tournament/tournament_schedule.hpp"

TournamentSchedulingSystem::TournamentSchedulingSystem(){
    this->head = nullptr;;
    this->tail = nullptr;
    this->matchmaking_system = nullptr;
}

/// @brief queue the match to be played
/// @param match
void TournamentSchedulingSystem::add_schedule(MatchesContainer* matches){
    if (matches == nullptr) {
        std::cout << "There is no more matches to add!" << std::endl;
        return;
    }

    int number_of_matches = matches->number_of_matches;
    for (int i = 0; i < number_of_matches; i++) {
        auto* new_slot = new ScheduleSlot(&matches->matches[i], 0, MONDAY_MORNING, MAIN_COURT, nullptr);
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
}

void TournamentSchedulingSystem::add_schedule(Match* match) {
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
ScheduleSlot* TournamentSchedulingSystem::deque_last_schedule(){
    if(head == nullptr){
        return nullptr;
    }
    ScheduleSlot* slot = head;
    head = head->next_slot;
    return slot;
}

/// @brief take a look at the next match to be played without removing it
ScheduleSlot* TournamentSchedulingSystem::view_last_schedule(){
    return head;
}

/// @brief void print the schedule out into a .csv file for backup
void TournamentSchedulingSystem::print_schedule() {
    ScheduleSlot* current_slot = head;

    if (current_slot == nullptr) {
        std::cout << "Scheduling System is currently empty!" << std::endl;
    }

    std::cout << "Here's a upcoming list of schedule" << std::endl;
    std::cout << std::setw(20) << "Match ID" << std::setw(30) << "Player 1" << std::setw(30) << "Player 2" << std::setw(30) << "Schedule Timeslot" << std::setw(20) << "Court" << std::endl;
    while(current_slot != nullptr){
        std::cout << std::setw(20) << current_slot->match->match_id << std::setw(30) << current_slot->match->player1->name << std::setw(30) << current_slot->match->player2->name << std::setw(30) << get_schedule_string(current_slot->time_slot) << std::setw(20) << (current_slot->court == MAIN_COURT ? "MAIN COURT" : "SIDE COURT") << std::endl;
        current_slot = current_slot->next_slot;
    }
}

void TournamentSchedulingSystem::print_last_schedule() {
    ScheduleSlot* current_slot = head;

    if (current_slot == nullptr) {
        std::cout << "Scheduling System is currently empty!" << std::endl;
    }

    std::cout << "Here's the last schedule" << std::endl;
    std::cout << std::setw(20) << "Match ID" << std::setw(30) << "Player 1" << std::setw(30) << "Player 2" << std::setw(30) << "Schedule Timeslot" << std::setw(20) << "Court" << std::endl;
    std::cout << std::setw(20) << current_slot->match->match_id << std::setw(30) << current_slot->match->player1->name << std::setw(30) << current_slot->match->player2->name << std::setw(30) << get_schedule_string(current_slot->time_slot) << std::setw(20) << (current_slot->court == MAIN_COURT ? "MAIN COURT" : "SIDE COURT") << std::endl;
}

void TournamentSchedulingSystem::last_match_completed(MATCH_STATUS status) {
    ScheduleSlot* current_slot = deque_last_schedule();

    if (current_slot == nullptr) {
        std::cout << "Scheduling System is currently empty!" << std::endl;
        return;
    }

    switch (status) {
        case PLAYER_ONE_WIN:
            matchmaking_system->add_player_back_to_matchmaking(current_slot->match->player1);
            break;
        case PLAYER_TWO_WIN:
            matchmaking_system->add_player_back_to_matchmaking(current_slot->match->player2);
            break;
        default:
            // No need to add into the matchmaking system, just requeue
            // Draw
            // Reschedule the match
                auto* rematch = new Match;
                rematch->match_id = 0;
                rematch->match_type = current_slot->match->match_type;
                rematch->player1 = current_slot->match->player1;
                rematch->player2 = current_slot->match->player2;
            break;
    }
}

void TournamentSchedulingSystem::update_matchmaking_system(MatchmakingSystem *matchmaking_system) {
    this->matchmaking_system = matchmaking_system;
}

