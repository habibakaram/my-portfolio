#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>

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
    string reservationDate;

    Customer(const string& n, int a, const string& nat, const string& id, const string& mail,
        const string& phone, int adults, int children, int days, RoomType type, int number, PaymentMethod payment, const string& date)
        : name(n), age(a), nationality(nat), nationalID(id), email(mail), phoneNumber(phone),
        numAdults(adults), numChildren(children), stayDays(days), roomType(type),
        roomNumber(number), paid(false), paymentMethod(payment), reservationDate(date) {}
};

class Node {
public:
    Customer data;
    Node* left;
    Node* right;

    Node(const Customer& customer) : data(customer), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    Node* root;

    Node* insertRecursive(Node* root, const Customer& customer) {
        if (root == nullptr) {
            return new Node(customer);
        }

        if (customer.name < root->data.name) {
            root->left = insertRecursive(root->left, customer);
        }
        else if (customer.name > root->data.name) {
            root->right = insertRecursive(root->right, customer);
        }

        return root;
    }

    Node* searchRecursive(Node* root, const string& name) const {
        if (root == nullptr || root->data.name == name) {
            return root;
        }

        if (name < root->data.name) {
            return searchRecursive(root->left, name);
        }
        else {
            return searchRecursive(root->right, name);
        }
    }

    void inOrderTraversal(Node* root, vector<Customer>& result) const {
        if (root != nullptr) {
            inOrderTraversal(root->left, result);
            result.push_back(root->data);
            inOrderTraversal(root->right, result);
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}

    void insert(const Customer& customer) {
        root = insertRecursive(root, customer);
    }

    Node* search(const string& name) const {
        return searchRecursive(root, name);
    }

    vector<Customer> getInOrderTraversal() const {
        vector<Customer> result;
        inOrderTraversal(root, result);
        return result;
    }
};

class Hotel {
private:
    vector<Customer> rooms[10];
    vector<Customer> waitlist;
    double roomPrices[3] = { 100.0, 150.0, 250.0 }; // Room prices for Single, Double, and Suite respectively
    double childDiscountRate = 0.1; // 10% discount for each child in the room
    int roomCapacities[3] = { 1, 2, 4 }; // Maximum capacity for Single, Double, and Suite rooms respectively

    int currentOccupancy[3] = { 0, 0, 0 }; // Current occupancy for Single, Double, and Suite rooms respectively

    double calculateTotalCost(const Customer& customer) const {
        double basePrice = roomPrices[static_cast<int>(customer.roomType)];
        double adultCost = customer.stayDays * basePrice;
        double childCost = customer.numChildren * (adultCost * childDiscountRate); // Calculate child discount

        double totalCost = adultCost - childCost;
        return totalCost;
    }

    int findEmptyRoom(RoomType roomType) const {
        for (int i = 0; i < 10; ++i) {
            if (rooms[i].empty()) {
                return i;
            }
        }
        return -1;
    }

public:
    void bookRoom(const Customer& customer) {
        int emptyRoomIndex = findEmptyRoom(customer.roomType);
        if (emptyRoomIndex != -1) {
            // Check if the room has reached its maximum capacity
            if (rooms[emptyRoomIndex].size() >= roomCapacities[static_cast<int>(customer.roomType)]) {
                cout << "Room " << emptyRoomIndex + 1 << " is already at full capacity." << endl;
                waitlist.push_back(customer); // Add to waitlist
                return;
            }

            // Empty room available, book it
            double totalCost = calculateTotalCost(customer);
            Customer newCustomer = customer;
            newCustomer.roomNumber = emptyRoomIndex + 1; // Assign room number
            newCustomer.paid = true;

            rooms[emptyRoomIndex].push_back(newCustomer);
            currentOccupancy[static_cast<int>(customer.roomType)]++; // Update current occupancy

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
            cout << ". Total Cost: $" << totalCost << endl;
        }
        else {
            // No empty rooms, add to waitlist
            waitlist.push_back(customer);
            cout << "All rooms are occupied. Added " << customer.name << " to the waitlist." << endl;
        }
    }

    void vacateRoom(int roomNumber) {
        for (int i = 0; i < 10; ++i) {
            auto it = find_if(rooms[i].begin(), rooms[i].end(), [roomNumber](const Customer& c) {
                return c.roomNumber == roomNumber;
                });
            if (it != rooms[i].end()) {
                cout << "Customer " << it->name << " has vacated Room " << roomNumber << endl;
                rooms[i].erase(it);
                currentOccupancy[static_cast<int>(it->roomType)]--; // Update current occupancy
                break;
            }
        }
    }

    void printOccupancy() const {
        cout << "Occupancy Status:\n";
        for (int i = 0; i < 10; ++i) {
            cout << "Room " << i + 1 << ": ";
            if (!rooms[i].empty()) {
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
        for (int i = 0; i < 10; ++i) {
            auto it = find_if(rooms[i].begin(), rooms[i].end(), [name](const Customer& c) {
                return c.name == name;
                });
            if (it != rooms[i].end()) {
                int remainingDays = it->stayDays;
                cout << "Customer: " << it->name << ", Room Number: " << it->roomNumber
                    << ", Remaining Days: " << remainingDays;
                cout << ", Payment Method: ";
                switch (it->paymentMethod) {
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
            auto it = find_if(waitlist.begin(), waitlist.end(), [name](const Customer& c) {
                return c.name == name;
                });
            if (it != waitlist.end()) {
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

    void searchByDate(const string& date) const {
        cout << "Customers who reserved on " << date << ":" << endl;
        for (int i = 0; i < 10; ++i) {
            for (const auto& customer : rooms[i]) {
                if (customer.reservationDate == date) {
                    cout << customer.name << endl;
                }
            }
        }
    }

    void inventoryReport() const {
        cout << "Hotel Inventory Report:\n";
        cout << "Total Rooms: 10\n";
        int bookedRooms = 0;
        for (int i = 0; i < 10; ++i) {
            if (!rooms[i].empty()) {
                bookedRooms++;
            }
        }
        cout << "Booked Rooms: " << bookedRooms << endl;
        cout << "Available Rooms: " << 10 - bookedRooms << endl;
    }

    void switchRooms(const string& customerName, int fromRoom, int toRoom) {
        Customer* customerToSwitch = nullptr;
        for (int i = 0; i < 10; ++i) {
            auto it = find_if(rooms[i].begin(), rooms[i].end(), [customerName, fromRoom](const Customer& c) {
                return c.name == customerName && c.roomNumber == fromRoom;
                });
            if (it != rooms[i].end()) {
                customerToSwitch = &(*it);
                rooms[i].erase(it);
                break;
            }
        }

        if (customerToSwitch != nullptr) {
            customerToSwitch->roomNumber = toRoom;
            rooms[toRoom - 1].push_back(*customerToSwitch);
            cout << "Room switched successfully for customer " << customerName << endl;
        }
        else {
            cout << "Customer " << customerName << " not found in Room " << fromRoom << endl;
        }
    }

    void bookMultipleRooms(const vector<Customer>& customers) {
        for (const auto& customer : customers) {
            bookRoom(customer);
        }
    }

    // Function to calculate and display statistics for the hotel
    void displayHotelStatistics() const {
        cout << "Hotel Statistics:\n";

        // Iterate over all booked rooms and calculate statistics
        double totalProfit = 0.0;
        int totalVisitors = 0;
        int totalStayDuration = 0;
        for (int i = 0; i < 10; ++i) {
            for (const auto& customer : rooms[i]) {
                totalProfit += calculateTotalCost(customer);
                totalVisitors += customer.numAdults + customer.numChildren;
                totalStayDuration += customer.stayDays;
            }
        }

        // Display statistics
        cout << "Total Visitors: " << totalVisitors << endl;
        cout << "Total Stay Duration (days): " << totalStayDuration << endl;
        cout << "Total Profit: $" << totalProfit << endl;
    }

    void manageHousekeeping() {
        // Placeholder message for housekeeping management
        cout << "Housekeeping management feature coming soon..." << endl;
    }

    void upgradeRoom() {
        // Placeholder message for room upgrade feature
        cout << "Room upgrade feature coming soon..." << endl;
    }
};

int main() {
    Hotel hotel;
    int choice;
    do {
        cout << "\nHotel Management System\n";
        cout << "1. Book Room\n";
        cout << "2. Vacate Room\n";
        cout << "3. Print Occupancy\n";
        cout << "4. Search by Name or Reservation Date\n"; // Updated option
        cout << "5. Room Prices\n";
        cout << "6. Inventory Report and Hotel Statistics\n"; // Merged options
        cout << "7. Switch Rooms\n";
        cout << "8. Manage Housekeeping\n";
        cout << "9. Room Upgrade\n";
        cout << "10. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            // Booking a room
            int numRooms;
            cout << "Enter the number of rooms to book: ";
            cin >> numRooms;

            vector<Customer> customers;
            for (int i = 0; i < numRooms; ++i) {
                string name, nationality, nationalID, email, phoneNumber, reservationDate;
                int age, numAdults, numChildren, stayDays, roomTypeInt;

                cout << "Enter customer name for room " << i + 1 << ": ";
                cin >> name;
                cout << "Enter age: ";
                cin >> age;
                cout << "Enter nationality: ";
                cin >> nationality;
                cout << "Enter national ID: ";
                cin >> nationalID;
                cout << "Enter email: ";
                cin >> email;
                cout << "Enter phone number: ";
                cin >> phoneNumber;
                cout << "Enter number of adults: ";
                cin >> numAdults;
                cout << "Enter number of children: ";
                cin >> numChildren;
                cout << "Enter number of stay days: ";
                cin >> stayDays;
                cout << "Enter room type (0 for Single, 1 for Double, 2 for Suite): ";
                cin >> roomTypeInt;
                cout << "Enter reservation date (YYYY-MM-DD): ";
                cin >> reservationDate;

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

                Customer newCustomer(name, age, nationality, nationalID, email, phoneNumber,
                    numAdults, numChildren, stayDays, roomType, -1, PaymentMethod::Cash, reservationDate); // Room number set to -1 initially

                customers.push_back(newCustomer);
            }

            hotel.bookMultipleRooms(customers);
            break;
        }

        case 2: {
            // Vacating a room
            // Code for vacating a room
            // ...
            break;
        }
        case 3: {
            // Printing occupancy status
            hotel.printOccupancy();
            break;
        }

        case 4: {
            // Searching by name or reservation date
            string searchInput;
            cout << "Enter the name or reservation date (YYYY-MM-DD) to search: ";
            cin >> searchInput;
            hotel.printCustomerDetails(searchInput);
            hotel.searchByDate(searchInput);
            break;
        }
        case 5: {
            // Printing room prices
            hotel.printRoomPrices();
            break;
        }
        case 6: {
            // Inventory Report and Hotel Statistics
            hotel.inventoryReport();
            hotel.displayHotelStatistics();
            break;
        }
        case 7: {
            // Switching rooms
            string customerName;
            int fromRoom, toRoom;
            cout << "Enter customer name: ";
            cin >> customerName;
            cout << "Enter current room number: ";
            cin >> fromRoom;
            cout << "Enter new room number: ";
            cin >> toRoom;
            hotel.switchRooms(customerName, fromRoom, toRoom);
            break;
        }
        case 8: {
            // Housekeeping Management
            hotel.manageHousekeeping();
            break;
        }
        case 9: {
            // Room Upgrade
            hotel.upgradeRoom();
            break;
        }
        case 10:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 10);

    return 0;
}