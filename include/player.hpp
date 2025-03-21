#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "tournament/enums.hpp"
#include <algorithm>
#include <iomanip>

struct TournamentPerformance {
    int total_matches_played = 0;
    int total_matches_won = 0;
    int total_matches_lost = 0;
    int roundrobin_score = 0;
    MATCH_TYPE current_round = MATCH_TYPE::UNDEFINED;
    PLAYER_STATUS current_status = PLAYER_STATUS::COMPETING;
};

struct Player{
    /// @brief player id
    int id = -1;
    /// @brief Player's name
    std::string name = "";
    /// @brief Player's age
    int age = -1;
    /// @brief  the country where the player is from
    std::string country_of_origin = "";
    /// @brief the type of the player
    PLAYER_TYPE player_type = PLAYER_TYPE::NORMAL;
    /// @brief the initial tennis rating of the player
    int rating = -1;
    /// @brief the performance of the player
    TournamentPerformance performance;
};

/// Priority Queue Implementation
/// Store a list of players who should be scheduled to play in the tournament
/// Also record the remaining player in the match after the 3 checkpoints
class CompetitionPlayerList{

    Player* player_list = nullptr;

    static void remove_double_quotes(std::string& text) {
        text.erase(std::remove_if(
            text.begin(),
            text.end(),
            [](unsigned char c) {
                return c == '\"';
            }
            ), text.end());
    }

    public:
        int number_of_players = 0;
        /// @brief the constructor to initialize the player list from a designated file path
        /// @param file_path the path to the file containing the player list
        CompetitionPlayerList(std::string file_path){
            /// maximum 128 players only
            player_list = new Player[172];

            std::ifstream player_list_file(file_path);
            if (!player_list_file.is_open()) {
                throw std::invalid_argument("Player list file does not exist. Please check your path!");
            }
            std::string full_line, current_line;;
            std::getline(player_list_file, current_line);

            int index = 0;

            while (std::getline(player_list_file, current_line)) {
                full_line = current_line;
                std::istringstream line_string_stream(full_line);
                std::string id_string, name, age_string, country_of_origin, type_string, tennis_rating_string;

                std::getline(line_string_stream, id_string, ',');
                std::getline(line_string_stream, name, ',');
                std::getline(line_string_stream, age_string, ',');
                std::getline(line_string_stream, country_of_origin, ',');
                std::getline(line_string_stream, type_string, ',');
                std::getline(line_string_stream, tennis_rating_string, '\n');

                remove_double_quotes(id_string);
                remove_double_quotes(name);
                remove_double_quotes(age_string);
                remove_double_quotes(country_of_origin);
                remove_double_quotes(type_string);
                remove_double_quotes(tennis_rating_string);

                player_list[number_of_players].id = std::stoi(id_string);
                player_list[number_of_players].name = name;
                player_list[number_of_players].age = std::stoi(age_string);
                player_list[number_of_players].country_of_origin = country_of_origin;

                player_list[number_of_players].player_type = PLAYER_TYPE::NORMAL;
                if (type_string == "VIP") {
                    player_list[number_of_players].player_type = PLAYER_TYPE::VIP;
                }
                player_list[number_of_players].rating = std::stoi(tennis_rating_string);

                number_of_players++;
            }

            sort();
        }

        void display_player_list() {
            if (player_list == nullptr) {
                std::cout << "Player List is not loaded" << std::endl;
                return;
            }

            std::cout << "ID" << std::setw(30) << "Name" << std::setw(10) << "Age" << std::setw(20) << "Country" << std::setw(8) << "Type" << " Rating" << std::endl;
            for (int i = 0; i < number_of_players; i++) {
                std::cout <<std::setw(2) << player_list[i].id << std::setw(30) << player_list[i].name << std::setw(10) << player_list[i].age << std::setw(20) << player_list[i].country_of_origin << std::setw(8) << player_list[i].player_type << " " << player_list[i].rating << std::endl;
            }
        }

        void display_sorted_player_list() {
            if (player_list == nullptr) {
                std::cout << "Player List is not loaded" << std::endl;
                return;
            }

            Player** sorted_player_list = sort();
            std::cout << "ID" << std::setw(30) << "Name" << std::setw(10) << "Age" << std::setw(20) << "Country" << std::setw(8) << "Type" << " Rating" << std::endl;
            for (int i = 0; i < number_of_players; i++) {
                std::cout <<std::setw(2) << sorted_player_list[i]->id << std::setw(30) << sorted_player_list[i]->name << std::setw(10) << sorted_player_list[i]->age << std::setw(20) << sorted_player_list[i]->country_of_origin << std::setw(8) << sorted_player_list[i]->player_type << " " << sorted_player_list[i]->rating << std::endl;
            }
        }

        Player* get_player(int player_id){
            for(int i = 0; i < number_of_players; i++){
                if(player_list[i].id == player_id){
                    return &player_list[i];
                }
            }
            return nullptr;
        }
        
        /// @brief bubble sort implementation to sort the player list array by their rating
        /// @returns a sorted array of players
        Player** sort() {

            /// Bubble Sort Implementation Steps
            /// 1. Start from the first element and compare it with the next element
            /// 2. If the first element is less than the next element, swap them
            /// 3. Move to the next element and repeat the process
            /// 4. Continue until the last element
            /// 5. Repeat the process until no more swaps are needed

            auto** sorted_player_list = new Player*[number_of_players];

            //inject value into sorted_player_list first
            for (int i = 0; i < number_of_players; i++) {
                sorted_player_list[i] = &player_list[i];
            }

            bool swapped = true;
            while (swapped) {
                swapped = false;
                for (int i = 0; i < number_of_players - 1; i++) {
                    if (sorted_player_list[i]->rating < sorted_player_list[i + 1]->rating) {
                        Player* temp = sorted_player_list[i];
                        sorted_player_list[i] = sorted_player_list[i + 1];
                        sorted_player_list[i + 1] = temp;
                        swapped = true;
                    }
                }
            }

            return sorted_player_list;
        }

        /// print out the status of all players
        bool display_player_status() {
            std::cout << std::setw(30) << "Player Name" << std::setw(20) << "Matches Won" << std::setw(20) << "Matches Lost" << std::setw(20) << "Total Matches Played" << std::setw(20) << "Round Robin Score" << std::setw(20) << "Current Round" << std::setw(20) << "Current Status" << std::endl;
            for (int i = 0; i < number_of_players; i++) {
                std::cout << std::setw(30) << player_list[i].name << std::setw(20) << player_list[i].performance.total_matches_won << std::setw(20) << player_list[i].performance.total_matches_lost << std::setw(20) << player_list[i].performance.total_matches_played << std::setw(20) << player_list[i].performance.roundrobin_score << std::setw(20) << get_match_type_string(player_list[i].performance.current_round) << std::setw(20) << get_player_status_string(player_list[i].performance.current_status) << std::endl;
            }
        }
};