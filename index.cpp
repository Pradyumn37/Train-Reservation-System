#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <list>
#include <queue>
#include <climits>
#include <map>
#include <fstream>
using namespace std;

// Passenger structure for storing passenger details
struct Psg {
    string name;
    int age;
    string id;
    Psg* next;

    Psg(string n, int a, string idNum) : name(n), age(a), id(idNum), next(nullptr) {}
};

// Train structure for storing train details
struct Trn {
    string num;
    string src;
    string dest;
    int seats;
    priority_queue<int, vector<int>, greater<int>> availSeats;
    Psg* psgHead;

    Trn(string tNum, string s, string d, int totalSeats)
        : num(tNum), src(s), dest(d), seats(totalSeats), psgHead(nullptr) {
        for (int i = 1; i <= totalSeats; i++) {
            availSeats.push(i);
        }
    }

    void book(Psg* psg) {
        if (availSeats.empty()) {
            cout << "No seats available!" << endl;
            return;
        }

        int seatNum = availSeats.top();
        availSeats.pop();
        cout << "Seat allocated: " << seatNum << endl;

        psg->next = psgHead;
        psgHead = psg;
    }

    void cancel(string idNum) {
        Psg* temp = psgHead;
        Psg* prev = nullptr;

        while (temp != nullptr && temp->id != idNum) {
            prev = temp;
            temp = temp->next;
        }

        if (temp == nullptr) {
            cout << "Passenger not found!" << endl;
            return;
        }

        // Seat cancellation logic
        int seatNum;
        cout << "Enter seat number to cancel: ";
        cin >> seatNum;

        availSeats.push(seatNum);
        cout << "Seat " << seatNum << " cancelled successfully." << endl;

        if (prev == nullptr) {
            psgHead = temp->next;
        } else {
            prev->next = temp->next;
        }

        delete temp;
    }

    void display() {
        Psg* temp = psgHead;
        while (temp != nullptr) {
            cout << "Name: " << temp->name << ", Age: " << temp->age << ", ID: " << temp->id << endl;
            temp = temp->next;
        }
    }

    Psg* findPassenger(string idNum) {
        Psg* temp = psgHead;
        while (temp != nullptr) {
            if (temp->id == idNum) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }
};

// Graph class for managing train routes
class TrnGraph {
    unordered_map<string, list<pair<string, int>>> adj;

public:
    void add(string s, string d, int dist) {
        adj[s].push_back(make_pair(d, dist));
        adj[d].push_back(make_pair(s, dist)); // Undirected graph for bidirectional routes
    }

    void show() {
        for (auto& route : adj) {
            cout << "From " << route.first << " to:\n";
            for (auto& dest : route.second) {
                cout << "  Destination: " << dest.first << " Distance: " << dest.second << " km" << endl;
            }
        }
    }

    void shortest(string s, string d) {
        unordered_map<string, int> dist;
        unordered_map<string, string> parent;

        for (auto& route : adj) {
            dist[route.first] = INT_MAX;
        }

        dist[s] = 0;

        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
        pq.push(make_pair(0, s));

        while (!pq.empty()) {
            string node = pq.top().second;
            pq.pop();

            for (auto& neighbor : adj[node]) {
                int newDist = dist[node] + neighbor.second;
                if (newDist < dist[neighbor.first]) {
                    dist[neighbor.first] = newDist;
                    pq.push(make_pair(newDist, neighbor.first));
                    parent[neighbor.first] = node;
                }
            }
        }

        if (dist[d] == INT_MAX) {
            cout << "No route found from " << s << " to " << d << endl;
        } else {
            cout << "Shortest path from " << s << " to " << d << " is " << dist[d] << " km." << endl;
            cout << "Path: ";
            string pathNode = d;
            while (pathNode != s) {
                cout << pathNode << " <- ";
                pathNode = parent[pathNode];
            }
            cout << s << endl;
        }
    }
};

// User management class
class UserMgr {
    map<string, string> users; // Username -> Password
    const string filename = "users.txt";

public:
    UserMgr() {
        load();
    }

    void load() {
        ifstream infile(filename);
        string username, password;
        while (infile >> username >> password) {
            users[username] = password;
        }
        infile.close();
    }

    void save() {
        ofstream outfile(filename);
        for (const auto& user : users) {
            outfile << user.first << " " << user.second << endl;
        }
        outfile.close();
    }

    bool signup(string u, string p) {
        if (users.find(u) != users.end()) {
            cout << "Username already exists! Please try a different username." << endl;
            return false;
        }
        users[u] = p;
        save();
        cout << "Signup successful!" << endl;
        return true;
    }

    bool login(string u, string p) {
        if (users.find(u) != users.end() && users[u] == p) {
            cout << "Login successful!" << endl;
            return true;
        }
        cout << "Invalid username or password!" << endl;
        return false;
    }
};

// Train reservation system class
class TrnSys {
    unordered_map<string, Trn*> trains;
    TrnGraph routes;
    UserMgr userMgr;

public:
    TrnSys() {
        load();
    }

    void load() {
        ifstream infile("trains.txt");
        string tNum, src, dest;
        int seats;
        while (infile >> tNum >> src >> dest >> seats) {
            Trn* train = new Trn(tNum, src, dest, seats);
            trains[tNum] = train;
            routes.add(src, dest, rand() % 1000 + 500); // Random distance for example
        }
        infile.close();
    }

    void save() {
        ofstream outfile("trains.txt");
        for (const auto& trainEntry : trains) {
            Trn* train = trainEntry.second;
            outfile << train->num << " " << train->src << " " << train->dest << " " << train->seats << endl;
        }
        outfile.close();
    }

    void add(string tNum, string src, string dest, int totalSeats) {
        Trn* train = new Trn(tNum, src, dest, totalSeats);
        trains[tNum] = train;
        routes.add(src, dest, rand() % 1000 + 500); // Random distance for example
        save();
        cout << "Train " << tNum << " added successfully." << endl;
    }

    void book(string tNum, string name, int age, string id) {
        if (trains.find(tNum) == trains.end()) {
            cout << "Train not found!" << endl;
            return;
        }
        Psg* psg = new Psg(name, age, id);
        trains[tNum]->book(psg);
    }

    void cancel(string tNum, string id) {
        if (trains.find(tNum) == trains.end()) {
            cout << "Train not found!" << endl;
            return;
        }
        trains[tNum]->cancel(id);
    }

    void displayPsg(string tNum) {
        if (trains.find(tNum) == trains.end()) {
            cout << "Train not found!" << endl;
            return;
        }
        trains[tNum]->display();
    }

    void showRoutes() {
        routes.show();
    }

    void findShortest(string s, string d) {
        routes.shortest(s, d);
    }

    bool signup(string u, string p) {
        return userMgr.signup(u, p);
    }

    bool login(string u, string p) {
        return userMgr.login(u, p);
    }

    void printTicket(string tNum, string id) {
        if (trains.find(tNum) == trains.end()) {
            cout << "Train not found!" << endl;
            return;
        }
        Psg* passenger = trains[tNum]->findPassenger(id);
        if (passenger == nullptr) {
            cout << "Passenger not found!" << endl;
            return;
        }

        cout << "\n--- Ticket ---" << endl;
        cout << "Train Number: " << trains[tNum]->num << endl;
        cout << "From: " << trains[tNum]->src << endl;
        cout << "To: " << trains[tNum]->dest << endl;
        cout << "Passenger Name: " << passenger->name << endl;
        cout << "Age: " << passenger->age << endl;
        cout << "ID: " << passenger->id << endl;
        cout << "Allocated Seat: " << (trains[tNum]->seats - trains[tNum]->availSeats.size()) << endl; // Calculate allocated seat
        cout << "------------------" << endl;
    }
};

// Main function with interactive login and signup
int main() {
    TrnSys system;
    int choice;

    // Login/Signup flow
    while (true) {
        cout << "\n--- Welcome to Train Reservation System ---\n";
        cout << "1. Signup\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string u, p;
            cout << "Enter Username: ";
            cin >> u;
            cout << "Enter Password: ";
            cin >> p;
            system.signup(u, p);
        } else if (choice == 2) {
            string u, p;
            cout << "Enter Username: ";
            cin >> u;
            cout << "Enter Password: ";
            cin >> p;

            if (system.login(u, p)) {
                break; // Exit the loop after successful login
            }
        } else if (choice == 3) {
            cout << "Exiting...\n";
            return 0;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    // Main application flow after successful login
    while (true) {
        cout << "\n--- Train Reservation System ---\n";
        cout << "1. Add Train\n";
        cout << "2. Book Train\n";
        cout << "3. Cancel Train Booking\n";
        cout << "4. Display Train Passengers\n";
        cout << "5. Display Train Routes\n";
        cout << "6. Find Shortest Route\n";
        cout << "7. Print Ticket\n"; // New option for printing ticket
        cout << "8. Logout\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string tNum, src, dest;
            int s;
            cout << "Enter Train Number: ";
            cin >> tNum;
            cout << "Enter Source Station: ";
            cin >> src;
            cout << "Enter Destination Station: ";
            cin >> dest;
            cout << "Enter Total Seats: ";
            cin >> s;
            system.add(tNum, src, dest, s);
        } else if (choice == 2) {
            string tNum, name, id;
            int age;
            cout << "Enter Train Number: ";
            cin >> tNum;
            cout << "Enter Passenger Name: ";
            cin >> name;
            cout << "Enter Passenger Age: ";
            cin >> age;
            cout << "Enter ID Number: ";
            cin >> id;
            system.book(tNum, name, age, id);
        } else if (choice == 3) {
            string tNum, id;
            cout << "Enter Train Number: ";
            cin >> tNum;
            cout << "Enter ID Number of Passenger to Cancel: ";
            cin >> id;
            system.cancel(tNum, id);
        } else if (choice == 4) {
            string tNum;
            cout << "Enter Train Number: ";
            cin >> tNum;
            system.displayPsg(tNum);
        } else if (choice == 5) {
            system.showRoutes();
        } else if (choice == 6) {
            string s, d;
            cout << "Enter Source Station: ";
            cin >> s;
            cout << "Enter Destination Station: ";
            cin >> d;
            system.findShortest(s, d);
        } else if (choice == 7) {
            string tNum, id;
            cout << "Enter Train Number: ";
            cin >> tNum;
            cout << "Enter Passenger ID to Print Ticket: ";
            cin >> id;
            system.printTicket(tNum, id); // Call the new printTicket function
        } else if (choice == 8) {
            cout << "Logging out...\n";
            main(); // Restart the login/signup process
            return 0;
        } else if (choice == 9) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
