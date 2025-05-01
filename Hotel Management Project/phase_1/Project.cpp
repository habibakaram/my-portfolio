#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

struct Customer {
    string name;
    int roomNumber;
};

class Stack {
private:
    struct Node {
        Customer data;
        Node* next;
        Node(const Customer& customer) : data(customer), next(nullptr) {}
    };

    Node* topNode;

public:
    Stack() : topNode(nullptr) {}

    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(const Customer& customer) {
        Node* newNode = new Node(customer);
        newNode->next = topNode;
        topNode = newNode;
    }

    Customer pop() {
        if (isEmpty()) {
            throw runtime_error("Stack is empty");
        }
        Node* temp = topNode;
        Customer data = temp->data;
        topNode = topNode->next;
        delete temp;
        return data;
    }

    Customer top() const {
        if (isEmpty()) {
            throw runtime_error("Stack is empty");
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }
};

class Queue {
private:
    struct Node {
        Customer data;
        Node* next;
        Node(const Customer& customer) : data(customer), next(nullptr) {}
    };

    Node* frontNode;
    Node* rearNode;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr) {}

    ~Queue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    void enqueue(const Customer& customer) {
        Node* newNode = new Node(customer);
        if (isEmpty()) {
            frontNode = newNode;
            rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
    }

    Customer dequeue() {
        if (isEmpty()) {
            throw runtime_error("Queue is empty");
        }
        Node* temp = frontNode;
        Customer data = temp->data;
        frontNode = frontNode->next;
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        return data;
    }

    bool isEmpty() const {
        return frontNode == nullptr;
    }
};

class Hotel {
private:
    Stack rooms[3];
    Queue waitlist;

public:
    void bookRoom(const Customer& customer) {
        int availableRoom = -1;
        for (int i = 0; i < 3; ++i) {
            if (rooms[i].isEmpty()) {
                availableRoom = i;
                break;
            }
        }
        if (availableRoom != -1) {
            rooms[availableRoom].push(customer);
            cout << "Room " << availableRoom + 1 << " booked for " << customer.name << endl;
        } else {
            waitlist.enqueue(customer);
            cout << "All rooms are occupied. Added " << customer.name << " to the waitlist." << endl;
        }
    }

    void vacateRoom(int roomNumber) {
        if (roomNumber < 1 || roomNumber > 3) {
            cout << "Invalid room number." << endl;
            return;
        }

        if (!rooms[roomNumber - 1].isEmpty()) {
            rooms[roomNumber - 1].pop();
            cout << "Room " << roomNumber << " vacated." << endl;

            if (!waitlist.isEmpty()) {
                Customer nextCustomer = waitlist.dequeue();
                rooms[roomNumber - 1].push(nextCustomer);
                cout << "Room " << roomNumber << " booked for " << nextCustomer.name << " from the waitlist." << endl;
            }
        } else {
            cout << "Room " << roomNumber << " is already vacant." << endl;
        }
    }

    void printOccupancy() {
        cout << "Current Occupancy:" << endl;
        for (int i = 0; i < 3; ++i) {
            if (!rooms[i].isEmpty()) {
                cout << "Room " << i + 1 << ": Occupied by " << rooms[i].top().name << endl;
            } else {
                cout << "Room " << i + 1 << ": Vacant" << endl;
            }
        }
    }
};

int main() {
    Hotel hotel;
    int choice;
    do {
        cout << "1. Book Room\n2. Vacate Room\n3. Print Occupancy\n4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string name;
                cout << "Enter customer name: ";
                cin >> name;
                hotel.bookRoom({name, 0}); // Room number is not required when booking
                break;
            }
            case 2: {
                int roomNumber;
                cout << "Enter room number to vacate: ";
                cin >> roomNumber;
                hotel.vacateRoom(roomNumber);
                break;
            }
            case 3:
                hotel.printOccupancy();
                break;
            case 4:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 4);

    return 0;
}
