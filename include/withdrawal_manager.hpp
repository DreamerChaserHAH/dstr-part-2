#ifndef WITHDRAWAL_MANAGER_HPP
#define WITHDRAWAL_MANAGER_HPP

#include <iostream>
#include <string>
#include "tournament/tournament_schedule.hpp"
#include "tournament/matchmaking.hpp"
#include "player.hpp"

using namespace std;

// ----------------- Withdrawal Queue -----------------
class WithdrawalQueue {
    Player** queue;
    int front, rear, capacity;
public:
    WithdrawalQueue(int size) {
        queue = new Player*[size];
        front = rear = 0;
        capacity = size;
    }

    void enqueue(Player* p) {
        if (rear < capacity) queue[rear++] = p;
    }

    Player* dequeue() {
        if (front == rear) return nullptr;
        return queue[front++];
    }

    void display() {
        bool anyWithdrawn = false;
        for (int i = front; i < rear; i++) {
            cout << queue[i]->id << ". " << queue[i]->name
                 << " | Country: " << queue[i]->country_of_origin
                 << " | Rating: " << queue[i]->rating << endl;
            anyWithdrawn = true;
        }
        if (!anyWithdrawn) cout << "No withdrawn players." << endl;
    }

    bool isEmpty() { return front == rear; }

    ~WithdrawalQueue() { delete[] queue; }
};

// ----------------- Withdrawal Stack -----------------
class WithdrawalStack {
    Player** stack;
    int top, capacity;
public:
    WithdrawalStack(int size) {
        stack = new Player*[size];
        top = -1;
        capacity = size;
    }

    void push(Player* p) {
        if (top < capacity - 1) stack[++top] = p;
    }

    Player* pop() {
        if (top >= 0) return stack[top--];
        else return nullptr;
    }

    bool isEmpty() { return top == -1; }

    ~WithdrawalStack() { delete[] stack; }
};

// ----------------- Priority Queue -----------------
class PriorityQueue {
    Player** heap;
    int size, capacity;
public:
    PriorityQueue(int cap) {
        heap = new Player*[cap];
        size = 0;
        capacity = cap;
    }

    void insert(Player* p) {
        if (size >= capacity) return;
        heap[size] = p;
        int i = size;
        while (i > 0 && heap[(i - 1) / 2]->rating < heap[i]->rating) {
            swap(heap[i], heap[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
        size++;
    }

    Player* extractMax() {
        if (size <= 0) return nullptr;
        Player* maxPlayer = heap[0];
        heap[0] = heap[--size];
        heapify(0);
        return maxPlayer;
    }

    void heapify(int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < size && heap[left]->rating > heap[largest]->rating) largest = left;
        if (right < size && heap[right]->rating > heap[largest]->rating) largest = right;
        if (largest != i) {
            swap(heap[i], heap[largest]);
            heapify(largest);
        }
    }

    void display() {
        if (size == 0) {
            cout << "No available replacements." << endl;
            return;
        }
        for (int i = 0; i < size; i++) {
            cout << heap[i]->id << ". " << heap[i]->name
                 << " | Country: " << heap[i]->country_of_origin
                 << " | Rating: " << heap[i]->rating << endl;
        }
    }

    bool isEmpty() { return size == 0; }

    ~PriorityQueue() { delete[] heap; }
};

// ----------------- Function Declaration -----------------
bool withdrawal_menu(CompetitionPlayerList* player_list, TournamentSchedulingSystem* schedule_system, MatchmakingSystem* matchmaking_system);

#endif
