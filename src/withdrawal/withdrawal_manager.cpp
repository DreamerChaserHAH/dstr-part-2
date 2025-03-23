#include "withdrawal_manager.hpp"
#include <limits>
#include <fstream>
#include <map>
using namespace std;

// Static log file path
const string LOG_FILE_PATH = "src/withdrawal/withdrawal_log.txt";

// Clears the log file at start
void clearLogFile() {
    ofstream clearFile(LOG_FILE_PATH, ios::trunc);
    clearFile.close();
}

bool withdrawal_menu(CompetitionPlayerList* player_list, TournamentSchedulingSystem* schedule_system, MatchmakingSystem* matchmaking_system) {
    // Clear log file at start
    clearLogFile();

    int totalPlayers = player_list->get_total_players();
    WithdrawalQueue withdrawalQueue(totalPlayers);
    WithdrawalStack withdrawalStack(totalPlayers);
    PriorityQueue priorityQueue(totalPlayers);

    // Summary Counters
    int totalWithdrawals = 0, totalReplacements = 0, totalUndos = 0;

    // Reason storage
    map<int, string> withdrawalReasons;

    // Insert players into Priority Queue initially
    for (int i = 0; i < totalPlayers; i++) {
        priorityQueue.insert(player_list->get_player_by_index(i));
    }

    ofstream logFile(LOG_FILE_PATH, ios::app);
    logFile << "----- New Withdrawal Management Session -----\n";

    int choice;
    do {
        cout << "\n===== Withdrawal Management =====" << endl;
        cout << "1. Withdraw Player" << endl;
        cout << "2. Replace Withdrawn Player" << endl;
        cout << "3. Undo Last Withdrawal" << endl;
        cout << "4. Display Withdrawn Players" << endl;
        cout << "5. Display Replacement Queue" << endl;
        cout << "6. Exit Withdrawal Management" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input!" << endl;
            continue;
        }

        switch (choice) {
            case 1: { // Withdraw Player
                int id;
                cout << "Enter Player ID to withdraw: ";
                cin >> id;
                bool found = false;
                for (int i = 0; i < totalPlayers; i++) {
                    Player* currentPlayer = player_list->get_player_by_index(i);
                    if (currentPlayer->id == id) {
                        found = true;
                        if (currentPlayer->performance.current_status == PLAYER_STATUS::WITHDRAWN) {
                            cout << "Player " << currentPlayer->name << " is already withdrawn!" << endl;
                            break;
                        }
                        cout << "Confirm withdrawal of " << currentPlayer->name << "? (Y/N): ";
                        char confirm;
                        cin >> confirm;
                        if (confirm == 'Y' || confirm == 'y') {
                            cout << "Enter reason for withdrawal: ";
                            cin.ignore();
                            string reason;
                            getline(cin, reason);
                            currentPlayer->performance.current_status = PLAYER_STATUS::WITHDRAWN;
                            withdrawalQueue.enqueue(currentPlayer);
                            withdrawalStack.push(currentPlayer);
                            withdrawalReasons[currentPlayer->id] = reason;
                            cout << currentPlayer->name << " has been withdrawn." << endl;
                            logFile << "Withdrawn Player: " << currentPlayer->name << " | Reason: " << reason << "\n";
                            schedule_system->remove_player(currentPlayer->id);
                            totalWithdrawals++;
                        } else {
                            cout << "Withdrawal cancelled." << endl;
                        }
                        break;
                    }
                }
                if (!found) cout << "Player not found!" << endl;
                break;
            }
            case 2: { // Replace Player
                if (!withdrawalQueue.isEmpty()) {
                    Player* replacement = nullptr;
                    while (!priorityQueue.isEmpty()) {
                        replacement = priorityQueue.extractMax();
                        if (replacement->performance.current_status != PLAYER_STATUS::WITHDRAWN) break;
                        replacement = nullptr;
                    }
                    if (replacement != nullptr) {
                        Player* withdrawn = withdrawalQueue.dequeue();
                        cout << "Replacing " << withdrawn->name << " with " << replacement->name << endl;
                        matchmaking_system->add_player_back_to_matchmaking(replacement, nullptr);
                        replacement->performance.current_status = PLAYER_STATUS::COMPETING;
                        logFile << "Replaced Withdrawn Player: " << withdrawn->name
                                << " with Replacement: " << replacement->name << "\n";
                        totalReplacements++;
                    } else {
                        cout << "No suitable replacements." << endl;
                    }
                } else {
                    cout << "No withdrawn players to replace!" << endl;
                }
                break;
            }
            case 3: { // Undo Last Withdrawal
                if (!withdrawalStack.isEmpty()) {
                    Player* restored = withdrawalStack.pop();
                    cout << "Restored " << restored->name << endl;
                    restored->performance.current_status = PLAYER_STATUS::COMPETING;
                    schedule_system->read_player(restored);
                    logFile << "Undo Withdrawal: " << restored->name << "\n";
                    totalUndos++;
                    // Remove from queue & reasons
                    withdrawalReasons.erase(restored->id);
                } else {
                    cout << "No withdrawals to undo!" << endl;
                }
                break;
            }
            case 4: { // Display Withdrawn Players
                cout << "\nWithdrawn Players:" << endl;
                if (withdrawalQueue.isEmpty()) {
                    cout << "No withdrawn players." << endl;
                } else {
                    withdrawalQueue.display();
                    // Show reasons
                    cout << "\nReasons:" << endl;
                    for (auto& pair : withdrawalReasons) {
                        cout << "Player ID " << pair.first << ": " << pair.second << endl;
                    }
                }
                break;
            }
            case 5:
                cout << "\nReplacement Queue:" << endl;
                priorityQueue.display();
                break;
            case 6: { // Exit, print summary
                cout << "\nWithdrawal Summary:" << endl;
                cout << "Total Withdrawals: " << totalWithdrawals << endl;
                cout << "Total Replacements: " << totalReplacements << endl;
                cout << "Total Undo Actions: " << totalUndos << endl;
                logFile << "Summary - Withdrawals: " << totalWithdrawals
                        << ", Replacements: " << totalReplacements
                        << ", Undo Actions: " << totalUndos << "\n";
                cout << "Exiting Withdrawal Management..." << endl;
                break;
            }
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 6);

    logFile << "----- End of Session -----\n\n";
    logFile.close();

    return true;
}
