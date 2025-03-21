//customer class to store customer info

#include <string>
using namespace std;
#pragma once
class Customer
{
public:
	static const int maxTicket = 10; //max number of tickets per custoemr
	
	//constructors
	Customer();
	Customer(string name, int priority, int numTickets, int session, int customerID);
	void print();
	string getName() const;
	
	//getters
	int getPriority() const;
	int getNumTickets() const;
	int getSession() const;
	int getCustomerID() const;
	char getZone() const;
	const int* getTicketIDs() const;
	bool getPaymentMade() const;

	//setters
	void setPriority(int priority);
	void setNumTickets(int numTickets);
	void setSession(int numSession);
	void setCustomerID(int customerID);
	void setZone(char zone);
	void setTicketIDs(int* ptr, int size);
	void setPaymentMade(bool paid);


private:
	string name;
	int priority, numTickets, session, customerID;
	char zone;
	int ticketIDs[maxTicket] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	bool paymentMade = false;
};

