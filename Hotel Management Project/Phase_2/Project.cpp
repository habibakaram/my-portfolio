#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

enum class RoomType { Single, Double, Suite };
enum class PaymentMethod { Cash, Visa };

struct Customer {
    string name;
    int age;
    string nationality;
    string nationalID;
    string email;
    string phoneNumber;
    int numAdults;
    int numChildren;
    int stayDays;
    RoomType roomType;
    int roomNumber;
    bool paid;
    PaymentMethod paymentMethod;

    Customer(const string& n, int a, const string& nat, const string& id, const string& mail,
        const string& phone, int adults, int children, int days, RoomType type, int number, PaymentMethod payment)
        : name(n), age(a), nationality(nat), nationalID(id), email(mail), phoneNumber(phone),
        numAdults(adults), numChildren(children), stayDays(days), roomType(type),
        roomNumber(number),
        paid(false), paymentMethod(payment) {}
};

class Node {
public:
    Customer data;
    Node* next;

    Node(const Customer& customer) : data(customer), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;

public:
    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void insert(const Customer& customer) {
        Node* newNode = new Node(customer);
        newNode->next = head;
        head = newNode;
    }

    Customer remove() {
        if (isEmpty()) {
            throw runtime_error("List is empty");
        }
        Node* temp = head;
        Customer data = temp->data;
        head = head->next;
        delete temp;
        return data;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    bool search(const Customer& customer) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.name == customer.name && current->data.roomNumber == customer.roomNumber) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    Customer* searchByName(const string& name) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.name == name) {
                return &current->data;
            }
            current = current->next;
        }
        return nullptr;
    }

    Customer* searchByRoomNumber(int roomNumber) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.roomNumber == roomNumber) {
                return &current->data;
            }
            current = current->next;
        }
        return nullptr;
    }

    Node* getHead() const {
        return head;
    }
};

class Hotel {
private:
    LinkedList rooms[5];
    LinkedList waitlist;
    double roomPrices[3] = { 100.0, 150.0, 250.0 }; // Room prices for Single, Double, and Suite respectively
    double childDiscountRate = 0.1; // 10% discount for each child in the room

double calculateTotalCost(const Customer& customer) const {
    double basePrice = roomPrices[static_cast<int>(customer.roomType)];
    double adultCost = customer.stayDays * basePrice;
    double childCost = customer.numChildren * (adultCost*childDiscountRate); // Calculate child discount

    double totalCost = adultCost - childCost;
    return totalCost;
}


    int findEmptyRoom() const {
        for (int i = 0; i < 3; ++i) {
            if (rooms[i].isEmpty()) {
                return i;
            }
        }
        return -1;
    }

public:
    void bookRoom(const Customer& customer) {
        int emptyRoomIndex = findEmptyRoom();
        if (emptyRoomIndex != -1) {
            // Empty room available, book it
            double totalCost = calculateTotalCost(customer);
            Customer newCustomer = customer;
            newCustomer.roomNumber = emptyRoomIndex + 1; // Assign room number

            // Prompt for payment method
            cout << "Choose payment method (0 for Cash, 1 for Visa): ";
            int paymentChoice;
            cin >> paymentChoice;

            switch (paymentChoice) {
            case 0:
                newCustomer.paymentMethod = PaymentMethod::Cash;
                break;
            case 1:
                newCustomer.paymentMethod = PaymentMethod::Visa;
                break;
            default:
                cout << "Invalid payment method choice. Defaulting to Cash." << endl;
                newCustomer.paymentMethod = PaymentMethod::Cash;
                break;
            }

            rooms[emptyRoomIndex].insert(newCustomer);

            cout << "Room " << newCustomer.roomNumber << " booked for " << newCustomer.name << " - Type: ";
            switch (newCustomer.roomType) {
            case RoomType::Single:
                cout << "Single";
                break;
            case RoomType::Double:
                cout << "Double";
                break;
            case RoomType::Suite:
                cout << "Suite";
                break;
            }
            cout << ". Payment method: ";
            switch (newCustomer.paymentMethod) {
            case PaymentMethod::Cash:
                cout << "Cash";
                break;
            case PaymentMethod::Visa:
                cout << "Visa";
                break;
            }
            cout << ". Total Cost: $" << totalCost << endl;
        }
        else {
            // No empty rooms, add to waitlist
            waitlist.insert(customer);
            cout << "All rooms are occupied. Added " << customer.name << " to the waitlist." << endl;
        }
    }

    void vacateRoom(int roomNumber) {
        for (int i = 0; i < 3; ++i) {
            Customer* customer = rooms[i].searchByRoomNumber(roomNumber);
            if (customer != nullptr) {
                cout << "Customer " << customer->name << " has vacated Room " << roomNumber << endl;
                rooms[i].remove();
                break;
            }
        }
    }

    void printOccupancy() const {
        cout << "Occupancy Status:\n";
        for (int i = 0; i < 3; ++i) {
            cout << "Room " << i + 1 << ": ";
            if (!rooms[i].isEmpty()) {
                cout << "Booked\n";
            }
            else {
                cout << "Available\n";
            }
        }
    }

    void printCustomerDetails(const string& name) const {
        bool found = false;

        // Search in the rooms
        for (int i = 0; i < 3; ++i) {
            Customer* customer = rooms[i].searchByName(name);
            if (customer != nullptr) {
                int remainingDays = customer->stayDays;
                cout << "Customer: " << customer->name << ", Room Number: " << customer->roomNumber
                    << ", Remaining Days: " << remainingDays ;
                cout << ", Payment Method: ";
                switch (customer->paymentMethod) {
                case PaymentMethod::Cash:
                    cout << "Cash";
                    break;
                case PaymentMethod::Visa:
                    cout << "Visa";
                    break;
                }
                cout << endl;
                found = true;
                break;
            }
        }

        // If not found in rooms, check the waitlist
        if (!found) {
            Customer* customerInWaitlist = waitlist.searchByName(name);
            if (customerInWaitlist != nullptr) {
                cout << "Customer " << name << " is in the waitlist." << endl;
                found = true;
            }
        }

        // If still not found, display "No"
        if (!found) {
            cout << "Customer '" << name << "' not found. Paid: No" << endl;
        }
    }


    void printRoomPrices() const {
        cout << "Room Prices:\n";
        cout << "Single Room: $" << roomPrices[static_cast<int>(RoomType::Single)] << endl;
        cout << "Double Room: $" << roomPrices[static_cast<int>(RoomType::Double)] << endl;
        cout << "Suite: $" << roomPrices[static_cast<int>(RoomType::Suite)] << endl;
    }
};

int main() {
    Hotel hotel;
    int choice;
    do {
        cout << "1. Book Room\n2. Vacate Room\n3. Print Occupancy\n4. Search by Name\n5. Room Prices\n6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            // Prompt user for customer details
            string name;
            int age, roomTypeInt, numAdults, numChildren, stayDays;

            cout << "Enter customer name: ";
            cin >> name;

            cout << "Enter age: ";
            cin >> age;
            cout << "Enter number of adults: ";
            cin >> numAdults;
            cout << "Enter number of children: ";
            cin >> numChildren;
            cout << "Enter number of stay days: ";
            cin >> stayDays;
            cout << "Enter room type (0 for Single, 1 for Double, 2 for Suite): ";
            cin >> roomTypeInt;

            RoomType roomType;
            switch (roomTypeInt) {
            case 0:
                roomType = RoomType::Single;
                break;
            case 1:
                roomType = RoomType::Double;
                break;
            case 2:
                roomType = RoomType::Suite;
                break;
            default:
                cout << "Invalid room type." << endl;
                continue; // Restart loop
            }

            // Create the customer object
            Customer newCustomer(name, age, "", "", "", "",
                numAdults, numChildren, stayDays, roomType, -1, PaymentMethod::Cash); // Room number set to -1 initially

            hotel.bookRoom(newCustomer);
            break;
        }
        case 2: {
            // Prompt for room number to vacate
            int roomNumber;
            cout << "Enter room number to vacate (1 to 3): ";
            cin >> roomNumber;
            hotel.vacateRoom(roomNumber);
            break;
        }
        case 3: {
            hotel.printOccupancy();
            break;
        }
        case 4: {
            // Prompt for customer name to search
            string name;
            cout << "Enter customer name: ";
            cin >> name;
            hotel.printCustomerDetails(name);
            break;
        }
        case 5: {
            hotel.printRoomPrices();
            break;
        }
        case 6:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}
