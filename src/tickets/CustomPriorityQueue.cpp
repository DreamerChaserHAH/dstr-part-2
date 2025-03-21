#include "tickets/CustomPriorityQueue.h"
#include <iostream>
using namespace std;

// Constructor
CustomPriorityQueue::CustomPriorityQueue(int capacity) : capacity(capacity), size(0) {
    heap = new Customer[capacity];
}

// Deconstructor
CustomPriorityQueue::~CustomPriorityQueue() {
    delete[] heap;
}

// Insert a new customer into the heap
void CustomPriorityQueue::push(Customer customer) {
    if (size >= capacity) {
        cout << "Queue is full. Cannot insert" << endl;
        return;
    }
    heap[size] = customer;
    heapifyUp(size);
    size++;
}

// Get the customer with the highest priority
Customer CustomPriorityQueue::top() const {
    if (size == 0) {
        cout << "Queue is empty." << endl;
        exit(1);
    }
    return heap[0];
}

// Remove the customer with the highest priority
void CustomPriorityQueue::pop() {
    if (size == 0) {
        cout << "Queue is empty. Cannot pop" << endl;
        return;
    }
    heap[0] = heap[size - 1];
    size--;
    heapifyDown(0);
}

// Check if the heap is empty
bool CustomPriorityQueue::empty() const {
    return size == 0;
}

// Restore heap property from bottom to top
void CustomPriorityQueue::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index].getPriority() < heap[parent].getPriority()) {
            std::swap(heap[index], heap[parent]);
            index = parent;
        }
        else {
            break;
        }
    }
}

// Restore heap property from top to bottom
void CustomPriorityQueue::heapifyDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < size && heap[left].getPriority() < heap[smallest].getPriority()) {
            smallest = left;
        }
        if (right < size && heap[right].getPriority() < heap[smallest].getPriority()) {
            smallest = right;
        }

        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            index = smallest;
        }
        else {
            break;
        }
    }
}