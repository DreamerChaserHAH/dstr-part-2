#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include "tickets/Customer.h"
#include "tickets/CustomPriorityQueue.h"

using namespace std;

int ticketID = 1001;
int customerID = 1001;
enum Zone { V, A, B, C, D };
enum Session { Morning, Afternoon, Evening };
enum Priority { VIP, Early_Bird, Normal };
const int maximumCapacity = 100;
int zoneCapacity[3][5];

Customer customerList[300]; //store all customers

//declare the functions 
void updateZoneCapacity(int session, char zone, int numTickets);
char findAvailableZone(int session, int numTickets, int priority);
void checkRequest(int checkCustomerID);
bool checkEntranceExit(int tNumber, int session, char zone);
void writeBookingToFile(const Customer& customer);
void loadLastTicketID();

//to retrieve last ticket ID from txt file
void loadLastTicketID() {
    ifstream inFile("BookingRecords.txt");
    if (!inFile.is_open()) {
        cout << "BookingRecords.txt not found. Starting fresh." << endl;
        ticketID = 1001;
        customerID = 1001;
        return;
    }

    string line;
    int highestID = 1000;
    int highestCustomerID = 1000;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string field;
        int fieldCount = 0;
        string ticketIDsField;
        int currentCustomerID = 0;

        //splits line by the comma
        while (getline(ss, field, ',')) {
            fieldCount++;
            if (fieldCount == 1)
            { // Customer IDs field
                currentCustomerID = stoi(field);
                if (currentCustomerID > highestCustomerID)
                {
                    highestCustomerID = currentCustomerID;
                }
            }
            if (fieldCount == 6)
            { //Ticket IDs field
                ticketIDsField = field;
                break;
            }
        }

        //Split Ticket IDs by '|'
        stringstream ticketStream(ticketIDsField);
        string ticketStr;
        while (getline(ticketStream, ticketStr, '|'))
        {
            if (!ticketStr.empty())
            {
                int id = stoi(ticketStr);
                if (id > highestID)
                {
                    highestID = id;
                }
            }
        }
    }

    ticketID = highestID + 1;
    customerID = highestCustomerID + 1;
    inFile.close();
}

bool ticket_sales_menu() {
    loadLastTicketID();

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 5; j++)
            zoneCapacity[i][j] = maximumCapacity / 5;

    CustomPriorityQueue customerQueue(300); // Initialize custom priority queue

    int choice;
    while (true) {
        cout << "----------------------------------\n";
        cout << "       TICKET BOOKING MENU        \n";
        cout << "----------------------------------\n";
        cout << "1. Register Customer\n";
        cout << "2. Process Payment Requests\n";
        cout << "3. Retrieve Booking Info\n";
        cout << "4. Entrance/Exit Check-In\n";
        cout << "0. Exit System\n";
        cout << "----------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            //register customer
            string customerName;
            int priorityChoice, sessionChoice, numTickets;

            cout << "----------------------------------\n";
            cout << "             COUNTER              \n";
            cout << "----------------------------------\n";

            //ask customer name
            cout << "Enter your name: ";
            getline(cin, customerName);


            //ask which ticket customer wants
            cout << "\nSelect Priority:\n";
            cout << "1. VIP\n2. Early Bird\n3. Normal\n";
            cout << "Enter your choice (1 - 3): ";
            cin >> priorityChoice;
            while (priorityChoice < 1 || priorityChoice > 3) {
                cout << "We do not offer that choice. Please try again: ";
                cin >> priorityChoice;
            }

            //ask the number of tickets the customer wants to buy
            cout << "\nEnter number of tickets: ";
            cin >> numTickets;
            while (numTickets <= 0 || numTickets > maximumCapacity) {
                cout << "Invalid number of tickets! Please enter a number within " << maximumCapacity << ": ";
                cin >> numTickets;
            }

            //ask customer which slot they want
            cout << "\nSelect Session:\n";
            cout << "1. Morning\n2. Afternoon\n3. Evening\n";
            cout << "Enter your choice (1 - 3): ";
            cin >> sessionChoice;
            while (sessionChoice < 1 || sessionChoice > 3) {
                cout << "Invalid session number chosen! Please try again: ";
                cin >> sessionChoice;
            }

            //Map to enums
            Priority priority;
            switch (priorityChoice)
            {
            case 1:
                priority = VIP;
                break;
            case 2:
                priority = Early_Bird;
                break;
            case 3:
                priority = Normal;
                break;
            }

            Session session;
            switch (sessionChoice)
            {
            case 1:
                session = Morning;
                break;
            case 2:
                session = Afternoon;
                break;
            case 3:
                session = Evening;
                break;
            }

            //create customer object
            Customer temp(customerName, priority, numTickets, session, customerID);
            customerID++;

            //add to queue
            customerQueue.push(temp);

            cout << "\nCustomer registered successfully! Customer ID: " << temp.getCustomerID() << endl;

            cout << "\nReturning to Main Menu...\n";
        }
        else if (choice == 2) {
            // ---Process Requests / Payment---

            cout << "----------------------------------\n";
            cout << "             CASHIER              \n";
            cout << "----------------------------------\n";


            if (customerQueue.empty()) {
                cout << "No pending customers in queue!\n";
            }
            else {
                cout << "\nProcessing all customers...\n";

                while (!customerQueue.empty()) {
                    Customer buff = customerQueue.top();
                    customerQueue.pop();

                    char availableZone = findAvailableZone(buff.getSession(), buff.getNumTickets(), buff.getPriority());

                    if (availableZone != 'F') {
                        // If payment made
                        buff.setPaymentMade(true);
                        updateZoneCapacity(buff.getSession(), availableZone, buff.getNumTickets());

                        buff.setZone(availableZone);
                        int tempTickets[buff.maxTicket] = { 0 };
                        for (int i = 0; i < buff.getNumTickets(); i++) {
                            tempTickets[i] = ticketID;
                            ticketID++;
                        }
                        buff.setTicketIDs(tempTickets, buff.getNumTickets());
                        customerList[buff.getCustomerID() - 1001] = buff;

                        cout << "\nBooking Successful for: " << buff.getName() << endl;
                        cout << "Payment Done!" << endl;
                        buff.print();

                        writeBookingToFile(buff);
                    }
                    else {
                        cout << "Unable to allocate zone for: " << buff.getName() << endl;
                    }
                }
            }
            cout << "\nReturning to Main Menu...\n";
        }
        else if (choice == 3) {
            // --- Check Requests ---
            int checkCustomerID;

            cout << "----------------------------------\n";
            cout << "       CUSTOMER SERVICE           \n";
            cout << "----------------------------------\n";

            cout << "\nPlease enter your customer ID to check your booking (e.g. 1001): ";
            cin >> checkCustomerID;
            checkRequest(checkCustomerID);

            cout << "\nReturning to Main Menu...\n";
        }
        else if (choice == 4) {
            // --- Entrance/Exit Check-In ---
            int selectedSession;
            char selectedZone;
            int enterTicketNumber;

            cout << "----------------------------------\n";
            cout << "       ENTRANCE CHECK GATE        \n";
            cout << "----------------------------------\n";

            //ask for session
            cout << "Select your session:\n";
            cout << "0. Morning\n1. Afternoon\n2. Evening\n";
            cout << "Enter session (0-2): ";
            cin >> selectedSession;

            //ask zone
            cout << "Enter zone (V/A/B/C/D): ";
            cin >> selectedZone;
            selectedZone = toupper(selectedZone);

            //ask ticket number
            cout << "Enter your ticket number: ";
            cin >> enterTicketNumber;

            cout << "\nChecking your ticket...\n";

            if (checkEntranceExit(enterTicketNumber, selectedSession, selectedZone)) {
                cout << "\nTicket Verified! Welcome, enjoy the game!" << endl;
            }
            else {
                cout << "Invalid ticket number! Please proceed to the correct entrance/exit" << endl;
            }

            cout << "\nReturning to Main Menu...\n";
        }
        else if (choice == 0) {
            cout << "Thank you for using the Ticket Booking System!\n";
            break;
        }
        else {
            cout << "Invalid choice! Please try again.\n";
        }
    }
    return true;
}

// --- Function Definitions ---

bool checkEntranceExit(int tNumber, int session, char zone) {
    for (int i = 0; i < customerID - 1001; i++) {
        for (int j = 0; j < customerList[i].getNumTickets(); j++) {
            int ticketID = *(customerList[i].getTicketIDs() + j);
            if (ticketID == tNumber) {
                if (customerList[i].getZone() == zone && customerList[i].getSession() == session) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
    }
    return false;
}

void checkRequest(int checkCustomerID) {
    for (int i = 0; i < customerID - 1001; i++) {
        if (customerList[i].getCustomerID() == checkCustomerID) {
            cout << "\n----------------------------------\n";
            cout << "       CUSTOMER BOOKING DETAILS     \n";
            cout << "\n----------------------------------\n";
            cout << "Customer Name     : " << customerList[i].getName() << endl;
            cout << "Customer ID       : " << customerList[i].getCustomerID() << endl;
            
            cout << "Priority          : ";
            if (customerList[i].getPriority() == 0) 
                cout << "VIP\n";
            else if (customerList[i].getPriority() == 1) 
                cout << "Early Bird\n";
            else cout << "Normal\n";

            cout << "Session           : ";
            if (customerList[i].getSession() == 0) 
                cout << "Morning\n";
            else if (customerList[i].getSession() == 1) 
                cout << "Afternoon\n";
            else cout << "Evening\n";
            
            cout << "Number of Tickets : " << customerList[i].getNumTickets() << endl;
            cout << "Zone              : " << customerList[i].getZone() << endl;

            cout << "Ticket IDs        : ";
            for (int j = 0; j < customerList[i].getNumTickets(); j++) {
                cout << customerList[i].getTicketIDs()[j];
                if (j != customerList[i].getNumTickets() - 1) cout << ", ";
            }
            cout << endl;
            cout << "------------------------------------\n";

            return;
        }
    }
    cout << "No booking found for Customer ID: " << checkCustomerID << endl;
}

void updateZoneCapacity(int session, char zone, int numTickets) {
    switch (zone) {
    case 'V':
        zoneCapacity[session][0] -= numTickets;
        break;
    case 'A':
        zoneCapacity[session][1] -= numTickets;
        break;
    case 'B':
        zoneCapacity[session][2] -= numTickets;
        break;
    case 'C':
        zoneCapacity[session][3] -= numTickets;
        break;
    case 'D':
        zoneCapacity[session][4] -= numTickets;
        break;
    }
}

char findAvailableZone(int session, int numTickets, int priority) {
    if (priority == VIP) {
        if (zoneCapacity[session][0] >= numTickets) {
            return 'V';
        }
    }
    int maxi = zoneCapacity[session][1];
    int tempZone = 1;
    for (int j = 1; j < 5; j++) {
        if (zoneCapacity[session][j] > maxi) {
            maxi = zoneCapacity[session][j];
            tempZone = j;
        }
    }
    if (numTickets > maxi) {
        return 'F'; // Means fail
    }

    switch (tempZone) {
    case 1:
        return 'A';
    case 2:
        return 'B';
    case 3:
        return 'C';
    case 4:
        return 'D';
    }
}

//append booking to file
void writeBookingToFile(const Customer& customer) {
    ofstream outFile("BookingRecords.txt", ios::app); // open in append mode
    if (outFile.is_open()) {
        outFile << customer.getCustomerID() << ",";
        outFile << customer.getName() << ",";
        
        //priority
        if (customer.getPriority() == 0)
            outFile << "VIP,";
        else if (customer.getPriority() == 1)
            outFile << "EarlyBird,";
        else 
            outFile << "Normal,";

        //session
        if (customer.getSession() == 0)
            outFile << "Morning,";
        else if (customer.getSession() == 1)
            outFile << "Afternoon,";
        else 
            outFile << "Evening,";

        outFile << customer.getZone() << ",";

        for (int i = 0; i < customer.getNumTickets(); i++) {
            outFile << customer.getTicketIDs()[i];
            if (i != customer.getNumTickets() - 1)
                outFile << "|";
        }
        outFile << "\n";
        outFile.close();
    }
    else {
        cout << "Error opening file to save booking!\n";
    }
}
