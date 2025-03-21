#ifndef CUSTOM_PRIORITY_QUEUE_H
#define CUSTOM_PRIORITY_QUEUE_H

#include "Customer.h"

class CustomPriorityQueue {
private:
    Customer* heap;       // Dynamic array to store the heap
    int capacity;         // Maximum capacity of the heap
    int size;             // Current number of elements in the heap

    void heapifyUp(int index);    // Restore heap property from bottom to top
    void heapifyDown(int index);  // Restore heap property from top to bottom

public:
    CustomPriorityQueue(int capacity); // Constructor
    ~CustomPriorityQueue();

    void push(Customer customer);      // Insert a new customer into the heap
    Customer top() const;              // Get the customer with the highest priority
    void pop();                        // Remove the customer with the highest priority
    bool empty() const;                // Check if the heap is empty
};

#endif