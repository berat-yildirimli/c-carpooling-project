#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
using namespace std;

// ---------------- Vehicle Class ----------------
class Vehicle {
private:
    string model;
    string plate;
    int seatCount;

    friend void printVehicleDetails(const Vehicle&);// sadece bu fonksiyon erişebilir


public:
    Vehicle(string m, string p, int s) : model(m), plate(p), seatCount(s) {}
    Vehicle(string m, string p) : model(m), plate(p), seatCount(4) {}

    void showInfo() const {
        cout << "Model: " << model << ", Plate: " << plate << ", Seat Count: " << seatCount << "\n";
    }

    int getSeatCount() const { return seatCount; }
    void setSeatCount(int count) { seatCount = count; }
};

// ---------------- Base User Class ----------------
class User {
protected:
    static int idCounter;
    int id;
    string username;
    string password;
    string role;
    Vehicle* vehicle;

public:
    User(string uname, string pwd, string rl) {
        id = ++idCounter;
        username = uname;
        password = pwd;
        role = rl;
        vehicle = nullptr;
    }

    virtual ~User() { delete vehicle; }

    static int getUserCount() { return idCounter; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    virtual string getRole() const { return role; }
    Vehicle* getVehicle() const { return vehicle; }

    void addVehicle(string model, string plate, int seats, bool saveToFile = true) {
        if (vehicle != nullptr) delete vehicle;
        vehicle = new Vehicle(model, plate, seats);

        if (saveToFile) {
            cout << "Vehicle added successfully.\n";
            ofstream file("vehicle.txt", ios::app);
            if (file.is_open()) {
                file << username << "," << plate << "," << model << "," << seats << "\n";
                file.close();
            } else {
                cout << "\033[31mFailed to open vehicle.txt for writing.\n\033[0m";
            }
        }
    }


    void showInfo(bool showPassword = false) {
        cout << "ID: " << id << "\nName: " << username << "\nRole: " << getRole() << "\n";
        if (showPassword) cout << "Password: " << password << "\n";
        if (vehicle != nullptr) {
            printVehicleDetails(*vehicle);

        } else {
            cout << "--- No Vehicle Added ---\n";
        }
    }

    void saveToFile(const string& filename = "user.txt") {
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << id << "," << username << "," << password << "," << getRole() << "\n";
            file.close();
        }
    }

    static void initMaxIdFromFile(const string& filename = "user.txt") {
        ifstream file(filename);
        string line;
        int maxId = 0;

        while (getline(file, line)) {
            stringstream ss(line);
            string idStr;
            getline(ss, idStr, ',');
            int id = stoi(idStr);
            if (id > maxId) maxId = id;
        }

        idCounter = maxId;
        file.close();
    }

    static bool isUsernameTaken(const string& uname, const string& filename = "user.txt") {
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string idStr, u, p, r;
            getline(ss, idStr, ',');
            getline(ss, u, ',');
            if (u == uname) return true;
        }
        return false;
    }

    static User* loginFromFile(const string& uname, const string& pwd, const string& filename = "user.txt");
};
int User::idCounter = 0;

void printVehicleDetails(const Vehicle& v) {
    cout << "--- Vehicle Info (Friend Function) ---\n";
    cout << "Plate: " << v.plate << "\n";
    cout << "Model: " << v.model << "\n";
    cout << "Capacity: " << v.seatCount << "\n";
}

// driver for inheritance
class Driver : public User {
public:
    Driver(string uname, string pwd) : User(uname, pwd, "driver") {}
    string getRole() const override { return "driver"; }
};

//Passenger class for inheritance
class Passenger : public User {
public:
    Passenger(string uname, string pwd) : User(uname, pwd, "passenger") {}
    string getRole() const override { return "passenger"; }
};


// Struct class bi fark yok
struct Trip {
    string from;
    string to;
    string date;
    int availableSeats;
    double price;

    Vehicle vehicle;  // COMPOSITION: Trip, bir araca SAHİPTİR

    Trip(string f, string t, string d, int seats, double pr, const Vehicle& v)
        : from(f), to(t), date(d), availableSeats(seats), price(pr),vehicle(v) {}

    void showTripInfo() const {
        cout << "Route: " << from << " to " << to
             << "\nDate: " << date
             << "\nAvailable Seats: " << availableSeats
             << "\nPrice per seat: $" << price << "\n";
    }

    bool reserveSeat() {
        if (availableSeats > 0) {
            availableSeats--;
            cout << "Reservation successful. Remaining seats: " << availableSeats << "\n";
            return true;
        }
        cout << "\033[33mSorry, no available seats.\n\033[0m";
        return false;
    }

    void saveToFile(const string& filename = "trip.txt") const {
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << from << "," << to << "," << date << "," << availableSeats << "," << price << "\n";
            file.close();
            cout << "Route successfully saved.\n";
        }
    }

    string getFrom() const { return from; }
    string getTo() const { return to; }
    string getDate() const { return date; }
    int getAvailableSeats() const { return availableSeats; }
    double getPrice() const { return price; }
};


// loginFromFile() Uygun Alt Sınıfla
User* User::loginFromFile(const string& uname, const string& pwd, const string& filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, u, p, r;
        getline(ss, idStr, ',');
        getline(ss, u, ',');
        getline(ss, p, ',');
        getline(ss, r);

        if (u == uname && p == pwd) {
            User* user;
            if (r == "driver") user = new Driver(u, p);
            else user = new Passenger(u, p);
            user->id = stoi(idStr);
            if (user->id > idCounter) idCounter = user->id;

            ifstream vfile("vehicle.txt");
            string vline;
            while (getline(vfile, vline)) {
                stringstream vss(vline);
                string unameV, plate, model;
                int seatCount;

                getline(vss, unameV, ',');
                getline(vss, plate, ',');
                getline(vss, model, ',');
                vss >> seatCount;

                if (unameV == uname) {
                    user->addVehicle(model, plate, seatCount,false);
                    break;
                }
            }
            vfile.close();

            return user;
        }
    }
    return nullptr;
}

// ---------------- Main Function ----------------
int main() {
    User::initMaxIdFromFile();
    vector<User*> users;
    User* currentUser = nullptr;
    int secim;

    while (true) {
        cout << "\033[32m\n=== C++ CARPOOLING SYSTEM ===\n\033[0m";
        cout << "1. Log In\n2. Sign Up\n\033[31m0. Exit\033[0m\n\033[36mYour Choice: \033[0m";
        cin >> secim;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\033[31mInvalid input.\n\033[0m";
            continue;
        }

        if (secim == 0) break;
        string uname, pwd;

        if (secim == 1) {
            cout << "Username: "; cin >> uname;
            cout << "Password: "; cin >> pwd;
            currentUser = User::loginFromFile(uname, pwd);
            if (!currentUser) {
                cout << "\033[31mLogin failed.\n\033[0m";
                continue;
            }
            users.push_back(currentUser);
        } else if (secim == 2) {
            string role;
            cout << "Username: "; cin >> uname;

            if (User::isUsernameTaken(uname)) {
                cout << "\033[33mThis username is already taken. Please choose another.\n\033[0m";
                continue;
            }

            cout << "Password (at least 8 characters): "; cin >> pwd;
            if (pwd.length() < 8) {
                cout << "\033[31mPassword is too short!\n\033[0m";
                continue;
            }

            cout << "Select Role (driver/passenger): "; cin >> role;
            if (role == "driver") currentUser = new Driver(uname, pwd);
            else currentUser = new Passenger(uname, pwd);

            currentUser->saveToFile();
            users.push_back(currentUser);
        }

        bool exitToMain = false;
        while (!exitToMain) {
            cout << "\033[32m\n=== MAIN MENU (" << currentUser->getRole() << ") ===\n\033[0m";
            cout << "1. User Info\n2. Create Route\n3. Make Reservation\n4. Add Vehicle\n\033[31m0. Exit\033[0m\n\033[36mYour Choice: \033[0m";
            cin >> secim;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\033[31mInvalid input.\n\033[0m";
                continue;
            }

            switch (secim) {
                case 0:
                    exitToMain = true;
                    break;
                case 1:
                    currentUser->showInfo();
                    break;
                case 2: {
                    if (currentUser->getRole() == "passenger") {
                        cout << "\033[33mPassengers cannot create routes.\n\033[0m";
                        break;
                    }
                    if (currentUser->getVehicle() == nullptr) {
                        cout << "\033[33mYou must add a vehicle before creating a route.\n\033[0m";
                        break;
                    }
                    cin.ignore();
                    string from, to, date;
                    int seats;
                    double price;
                    cout << "Departure location: "; getline(cin, from);
                    cout << "Destination location: "; getline(cin, to);
                    string day, month, year;
                    cout << "Day (dd): "; getline(cin, day);
                    cout << "Month (mm): "; getline(cin, month);
                    cout << "Year (yyyy): "; getline(cin, year);

                    // --- Tarih Kontrolleri ---
                    bool validDate = true;

                    if (day.length() != 2 || stoi(day) < 1 || stoi(day) > 31) validDate = false;
                    if (month.length() != 2 || stoi(month) < 1 || stoi(month) > 12) validDate = false;
                    if (year.length() != 4 || stoi(year) < 2025) validDate = false;

                    if (!validDate) {
                        cout << "\033[31mInvalid date. Day: 01-31, Month: 01-12, Year: 2025 or later.\n\033[0m";
                        break;
                    }

                    date = day + "-" + month + "-" + year;

                    cout << "Available Seats: "; cin >> seats;

                    if (cin.fail() || seats <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "\033[31mInvalid seat number.\n\033[0m";
                        break;
                    }

                    int maxSeats = currentUser->getVehicle()->getSeatCount();
                    if (seats > maxSeats) {
                        cout << "\033[31mError: More than vehicle capacity (" << maxSeats << ").\n\033[0m";
                        break;
                    }

                    cout << "Price per seat ($): "; cin >> price;
                    if (cin.fail() || price < 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "\033[31mInvalid price.\n\033[0m";
                        break;
                    }

                    Trip trip(from, to, date, seats, price, *currentUser->getVehicle());
                    trip.saveToFile();
                    break;
                }
                case 3: {
                    ifstream file("trip.txt");
                    vector<Trip*> tripList;
                    string line;

                    while (getline(file, line)) {
                        if (line.empty()) continue;
                        stringstream ss(line);
                        string from, to, date, seatStr, priceStr;
                        int seats;
                        double price;

                        if (getline(ss, from, ',') && getline(ss, to, ',') && getline(ss, date, ',') &&
                            getline(ss, seatStr, ',') && getline(ss, priceStr)) {
                            seats = stoi(seatStr);
                            price = stod(priceStr);
                            Vehicle dummy("N/A", "00 XX 0000", 4);
                            tripList.push_back(new Trip(from, to, date, seats, price, dummy));

                        }
                    }
                    file.close();

                    if (tripList.empty()) {
                        cout << "No trips found.\n";
                    } else {
                        for (size_t i = 0; i < tripList.size(); i++) {
                            if (tripList[i]->getAvailableSeats() > 0) {
                                cout << i + 1 << ") ";
                                tripList[i]->showTripInfo();
                                cout << "---------------------\n";
                            }
                        }

                        int choice;
                        cout << "Select trip number to reserve: ";
                        cin >> choice;

                        if (cin.fail() || choice < 1 || choice > (int)tripList.size()) {
                            cout << "\033[31mInvalid selection.\n\033[0m";
                        } else {
                            bool success = tripList[choice - 1]->reserveSeat();
                            if (success) {
                                ofstream outFile("trip.txt", ios::trunc);
                                for (Trip* trip : tripList) {
                                    outFile << trip->getFrom() << "," << trip->getTo() << "," << trip->getDate()
                                            << "," << trip->getAvailableSeats() << "," << trip->getPrice() << "\n";
                                }
                                outFile.close();
                            }
                        }

                        for (Trip* t : tripList) delete t;
                    }
                    break;
                }
                case 4: {
                    if (currentUser->getRole() == "passenger") {
                        cout << "\033[33mPassengers cannot add vehicles.\n\033[0m";
                        break;
                    }

                    cin.ignore();
                    string model, code, group, number;
                    cout << "Model: "; getline(cin, model);
                    cout << "Plate Code (2 digits): "; cin >> code;
                    cout << "Letters (1-3): "; cin >> group;
                    cout << "Number (1-4 digits): "; cin >> number;

                    bool valid = true;
                    if (code.length() != 2 || !isdigit(code[0]) || !isdigit(code[1])) valid = false;
                    for (char c : group) if (!isalpha(c)) valid = false;
                    for (char c : number) if (!isdigit(c)) valid = false;
                    if (number.length() == 0 || number.length() > 4) valid = false;

                    if (!valid) {
                        cout << "\033[31mInvalid plate format.\n\033[0m";
                        break;
                    }

                    string plate = code + " " + group + " " + number;
                    int seats;
                    cout << "Seat Count: "; cin >> seats;
                    currentUser->addVehicle(model, plate, seats);
                    break;
                }

                default:
                    cout << "\033[31mInvalid selection.\n\033[0m";
                    break;
            }
        }
    }

    for (User* u : users) delete u;
    cout << "Exiting...\n";
    return 0;
}
