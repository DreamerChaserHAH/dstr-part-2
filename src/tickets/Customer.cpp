#include "tickets/Customer.h"
#include <iostream>
Customer::Customer(string name, int priority, int numTickets, int session, int customerID)
{
	this->name = name;
	this->priority = priority;
	this->numTickets = numTickets;
	this->session = session;
	this->customerID = customerID;
	this->paymentMade = false;

	for (int i = 0; i < maxTicket; i++) {
		ticketIDs[i] = 0;
	}
}

Customer::Customer()
{

}

//getters

string Customer::getName() const
{
	return this->name;
}

int Customer::getPriority() const
{
	return this->priority;
}
int  Customer::getNumTickets() const {
	return this->numTickets;
}
int  Customer::getSession() const {
	return this->session;
}
int  Customer::getCustomerID() const
{
	return this->customerID;
}
char  Customer::getZone() const
{
	return this->zone;
}
const int* Customer::getTicketIDs() const {
	return this->ticketIDs;
}
bool  Customer::getPaymentMade() const
{
	return paymentMade;
}

// setters 

void Customer::setPriority(int priority)
{
	this->priority = priority;
}
void Customer::setNumTickets(int numTickets)
{
	this->numTickets = numTickets;
}
void Customer::setSession(int numSession)
{
	this->session = numSession;
}
void Customer::setCustomerID(int customerID)
{
	this->customerID = customerID;
}
void Customer::setZone(char zone)
{
	this->zone = zone;
}
void Customer::setTicketIDs(int* ptr, int size)
{
	for (int i = 0; i < size; i++)
	{
		this->ticketIDs[i] = *(ptr + i);
	}
}
void Customer::setPaymentMade(bool paid)
{
	this->paymentMade = paid;
}
void Customer::print()
{
	cout << this->name << "," << this->priority << "," << this->numTickets << "," << this->session << "," << customerID << endl;
	cout << "Ticket IDs:";
	for (int i = 0; i < this->numTickets; i++)
	{
		cout << ticketIDs[i];
		if (i != this->numTickets - 1)
			cout << ", ";
	}
	cout << endl << "Zone: " << this->zone << endl;
}

