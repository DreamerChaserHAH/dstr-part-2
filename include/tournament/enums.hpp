//
// Created by Victor Mak on 26/02/2025.
//

#pragma once

#include <string>

enum SCHEDULE_TIME_SLOT{
    MONDAY_MORNING,
    MONDAY_AFTERNOON,
    MONDAY_EVENING,
    TUESDAY_MORNING,
    TUESDAY_AFTERNOON,
    TUESDAY_EVENING,
    WEDNESDAY_MORNING,
    WEDNESDAY_AFTERNOON,
    WEDNESDAY_EVENING,
    THURSDAY_MORNING,
    THURSDAY_AFTERNOON,
    THURSDAY_EVENING,
    FRIDAY_MORNING,
    FRIDAY_AFTERNOON,
    FRIDAY_EVENING,
    SATURDAY_MORNING,
    SATURDAY_AFTERNOON,
    SATURDAY_EVENING,
    SUNDAY_MORNING,
};

/// @brief the court at which tournament is played
enum TOURNAMENT_COURT{
    MAIN_COURT,
    SIDE_COURT
};

inline SCHEDULE_TIME_SLOT get_next_time_slot(SCHEDULE_TIME_SLOT current_slot){
    switch(current_slot){
        case MONDAY_MORNING:
            return MONDAY_AFTERNOON;
        case MONDAY_AFTERNOON:
            return MONDAY_EVENING;
        case MONDAY_EVENING:
            return TUESDAY_MORNING;
        case TUESDAY_MORNING:
            return TUESDAY_AFTERNOON;
        case TUESDAY_AFTERNOON:
            return TUESDAY_EVENING;
        case TUESDAY_EVENING:
            return WEDNESDAY_MORNING;
        case WEDNESDAY_MORNING:
            return WEDNESDAY_AFTERNOON;
        case WEDNESDAY_AFTERNOON:
            return WEDNESDAY_EVENING;
        case WEDNESDAY_EVENING:
            return THURSDAY_MORNING;
        case THURSDAY_MORNING:
            return THURSDAY_AFTERNOON;
        case THURSDAY_AFTERNOON:
            return THURSDAY_EVENING;
        case THURSDAY_EVENING:
            return FRIDAY_MORNING;
        case FRIDAY_MORNING:
            return FRIDAY_AFTERNOON;
        case FRIDAY_AFTERNOON:
            return FRIDAY_EVENING;
        case FRIDAY_EVENING:
            return SATURDAY_MORNING;
        case SATURDAY_MORNING:
            return SATURDAY_AFTERNOON;
        case SATURDAY_AFTERNOON:
            return SATURDAY_EVENING;
        case SATURDAY_EVENING:
            return SUNDAY_MORNING;
        case SUNDAY_MORNING:
            return MONDAY_MORNING;
        default:
            return MONDAY_MORNING;
    }
}

inline std::string get_schedule_string(SCHEDULE_TIME_SLOT current_slot) {
    switch(current_slot){
        case MONDAY_MORNING:
            return "MONDAY_AFTERNOON";
        case MONDAY_AFTERNOON:
            return "MONDAY_EVENING";
        case MONDAY_EVENING:
            return "TUESDAY_MORNING";
        case TUESDAY_MORNING:
            return "TUESDAY_AFTERNOON";
        case TUESDAY_AFTERNOON:
            return "TUESDAY_EVENING";
        case TUESDAY_EVENING:
            return "WEDNESDAY_MORNING";
        case WEDNESDAY_MORNING:
            return "WEDNESDAY_AFTERNOON";
        case WEDNESDAY_AFTERNOON:
            return "WEDNESDAY_EVENING";
        case WEDNESDAY_EVENING:
            return "THURSDAY_MORNING";
        case THURSDAY_MORNING:
            return "THURSDAY_AFTERNOON";
        case THURSDAY_AFTERNOON:
            return "THURSDAY_EVENING";
        case THURSDAY_EVENING:
            return "FRIDAY_MORNING";
        case FRIDAY_MORNING:
            return "FRIDAY_AFTERNOON";
        case FRIDAY_AFTERNOON:
            return "FRIDAY_EVENING";
        case FRIDAY_EVENING:
            return "SATURDAY_MORNING";
        case SATURDAY_MORNING:
            return "SATURDAY_AFTERNOON";
        case SATURDAY_AFTERNOON:
            return "SATURDAY_EVENING";
        case SATURDAY_EVENING:
            return "SUNDAY_MORNING";
        case SUNDAY_MORNING:
            return "MONDAY_MORNING";
        default:
            return "MONDAY_MORNING";
    }
}

inline TOURNAMENT_COURT get_next_court(TOURNAMENT_COURT current_court){
    if(current_court == MAIN_COURT){
        return SIDE_COURT;
    }else{
        return MAIN_COURT;
    }
}
