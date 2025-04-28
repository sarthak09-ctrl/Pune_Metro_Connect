#include <bits/stdc++.h>

using namespace std;

// Structure for each metro connection (edge)
struct Edge {
    int to;
    int weight;
};

// Structure for each metro station
struct MetroStation {
    string name_en;
    string name_mr;
    string line;
    vector<string> touristSpots;
    vector<string> foodSpots;
    vector<string> amenities;
    bool isClosed = false;
};

unordered_map<string, int> nameToIndex;

void dijkstra(int src, const vector<vector<Edge>>& graph, const vector<MetroStation>& stations,
              vector<int>& dist, vector<int>& prev) {
    int n = graph.size();
    dist.assign(n, INT_MAX);
    prev.assign(n, -1);
    dist[src] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push(make_pair(0, src));

    while (!pq.empty()) {
        int currDist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (stations[u].isClosed) continue;

        for (int i = 0; i < graph[u].size(); ++i) {
            Edge edge = graph[u][i];
            int v = edge.to;
            int weight = edge.weight;
            if (!stations[v].isClosed && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push(make_pair(dist[v], v));
            }
        }
    }
}

pair<int, int> parseTime(const string& timeStr) {
    int hour, min;
    char colon;
    stringstream ss(timeStr);
    ss >> hour >> colon >> min;
    return make_pair(hour, min);
}

string formatTime(int hour, int min) {
    hour = hour % 24;
    min = min % 60;
    ostringstream oss;
    oss << setfill('0') << setw(2) << hour << ":" << setw(2) << min;
    return oss.str();
}

string getNextMetroTime(int hour, int min, int freq = 15) {
    int totalMins = hour * 60 + min;
    int nextMins = ((totalMins + freq - 1) / freq) * freq;
    int nextHour = nextMins / 60;
    int nextMin = nextMins % 60;
    return formatTime(nextHour, nextMin);
}

void saveTripHistory(const string& source, const string& dest, const string& time) {
    ofstream file("trip_history.txt", ios::app);
    file << "From: " << source << " To: " << dest << " At: " << time << endl;
    file.close();
}

void collectFeedback() {
    string station, feedback;
    cout << "\nEnter station name for feedback: ";
    getline(cin, station);
    cout << "Your feedback: ";
    getline(cin, feedback);

    ofstream file("feedback.txt", ios::app);
    file << station << ": " << feedback << endl;
    file.close();
    cout << "\n✅ Feedback recorded!\n";
}

int calculateFare(int stops) {
    if (stops <= 3) {
        return 5;
    } else if (stops <= 8) {
        return 10;
    } else {
        return 20;
    }
}

void displayRoute(const vector<int>& path, const vector<MetroStation>& stations) {
    cout << "\n🗺 Route Map: ";
    for (int i = 0; i < path.size(); ++i) {
        cout << stations[path[i]].name_en;
        if (i + 1 < path.size()) {
            cout << " -> ";
        }
    }
    cout << endl;
}

void showRouteDetails(const vector<int>& path, const vector<MetroStation>& stations, const vector<vector<Edge>>& graph,
                      int startHour, int startMin) {
    int currHour = startHour;
    int currMin = startMin;
    cout << "\n📍 Route Details:\n";

    for (int i = 0; i < path.size(); ++i) {
        int idx = path[i];
        cout << "\nStation: " << stations[idx].name_en << " (" << stations[idx].name_mr << ") - Line: " << stations[idx].line;
        cout << "\nArrival Time: " << formatTime(currHour, currMin);

        if (!stations[idx].touristSpots.empty()) {
            cout << "\nTourist Places: ";
            for (int j = 0; j < stations[idx].touristSpots.size(); ++j) {
                cout << stations[idx].touristSpots[j] << ", ";
            }
        }
        if (!stations[idx].foodSpots.empty()) {
            cout << "\nFood Places: ";
            for (int j = 0; j < stations[idx].foodSpots.size(); ++j) {
                cout << stations[idx].foodSpots[j] << ", ";
            }
        }
        if (!stations[idx].amenities.empty()) {
            cout << "\nAmenities: ";
            for (int j = 0; j < stations[idx].amenities.size(); ++j) {
                cout << stations[idx].amenities[j] << ", ";
            }
        }

        if (i + 1 < path.size()) {
            for (int j = 0; j < graph[path[i]].size(); ++j) {
                if (graph[path[i]][j].to == path[i + 1]) {
                    currMin += graph[path[i]][j].weight;
                    currHour += currMin / 60;
                    currMin = currMin % 60;
                    break;
                }
            }
        }
        cout << endl;
    }

    cout << "\n🎯 Final Arrival: " << formatTime(currHour, currMin);
    cout << "\n💰 Fare: ₹" << calculateFare(path.size()) << endl;
}

int main() {
    vector<MetroStation> stations = {
        {"PCMC", "पिंपरी चिंचवड", "Purple", {}, {"Jagdish Misal"}, {"ATM", "Parking"}},
        {"Sant Tukaram Nagar", "संत तुकाराम नगर", "Purple", {}, {"Misal Point"}, {}},
        {"Phugewadi", "फुगेवाडी", "Purple", {}, {"Tea Point"}, {}},
        {"Kasarwadi", "कासारवाडी", "Purple", {}, {}, {}},
        {"Bopodi", "बोपोडी", "Purple", {}, {}, {}},
        {"Khadki", "खडकी", "Purple", {}, {}, {}},
        {"Range Hills", "रेंज हिल्स", "Purple", {}, {}, {}},
        {"Shivaji Nagar", "शिवाजी नगर", "Purple", {"Dagadusheth Temple"}, {"Vaishali"}, {"WiFi"}},
        {"Civil Court", "सिविल कोर्ट", "Interchange", {}, {}, {}},
        {"Pune Station", "पुणे स्टेशन", "Purple", {}, {"Mainland China"}, {"Cloak Room"}},
        {"Ruby Hall Clinic", "रुबी हॉल क्लिनिक", "Purple", {}, {}, {}},
        {"Bund Garden", "बंड गार्डन", "Purple", {}, {}, {}},
        {"Yerwada", "येरवडा", "Purple", {}, {}, {}},
        {"Ramwadi", "रामवाडी", "Purple", {}, {}, {}},
        {"Kalyani Nagar", "कळ्याणी नगर", "Purple", {}, {}, {}},
        {"Koregaon Park", "कोरेगाव पार्क", "Purple", {}, {}, {}},
        {"Mangalwar Peth", "मंगळवार पेठ", "Purple", {}, {}, {}},
        {"Nehru Chowk", "नेहरू चौक", "Aqua", {}, {}, {}},
        {"Mandai", "मंडई", "Aqua", {}, {}, {}},
        {"Deccan", "डेक्कन", "Aqua", {"Balgandharva"}, {"Vada Pav"}, {"Parking"}},
        {"Nal Stop", "नळ स्टॉप", "Aqua", {}, {"Wadeshwar"}, {}},
        {"Garware College", "गारवारे कॉलेज", "Aqua", {}, {}, {}},
        {"Karve Nagar", "कार्वे नगर", "Aqua", {}, {}, {}},
        {"Warje", "वारजे", "Aqua", {}, {}, {}},
        {"Swargate", "स्वारगेट", "Aqua", {"Shaniwar Wada"}, {"Sujata Mastani"}, {"Toilets"}}
    };

    for (int i = 0; i < stations.size(); ++i) {
        nameToIndex[stations[i].name_en] = i;
    }

    vector<vector<Edge>> graph(stations.size());

    // Sample connections using full syntax
    graph[nameToIndex["PCMC"]].push_back({nameToIndex["Sant Tukaram Nagar"], 4});
    graph[nameToIndex["Sant Tukaram Nagar"]].push_back({nameToIndex["PCMC"], 4});
    graph[nameToIndex["Sant Tukaram Nagar"]].push_back({nameToIndex["Phugewadi"], 3});
    graph[nameToIndex["Phugewadi"]].push_back({nameToIndex["Sant Tukaram Nagar"], 3});
    graph[nameToIndex["Phugewadi"]].push_back({nameToIndex["Kasarwadi"], 4});
    graph[nameToIndex["Kasarwadi"]].push_back({nameToIndex["Phugewadi"], 4});
    graph[nameToIndex["Kasarwadi"]].push_back({nameToIndex["Bopodi"], 4});
    graph[nameToIndex["Bopodi"]].push_back({nameToIndex["Kasarwadi"], 4});
    graph[nameToIndex["Bopodi"]].push_back({nameToIndex["Khadki"], 4});
    graph[nameToIndex["Khadki"]].push_back({nameToIndex["Bopodi"], 4});
    graph[nameToIndex["Khadki"]].push_back({nameToIndex["Range Hills"], 3});
    graph[nameToIndex["Range Hills"]].push_back({nameToIndex["Khadki"], 3});
    graph[nameToIndex["Range Hills"]].push_back({nameToIndex["Shivaji Nagar"], 4});
    graph[nameToIndex["Shivaji Nagar"]].push_back({nameToIndex["Range Hills"], 4});
    graph[nameToIndex["Shivaji Nagar"]].push_back({nameToIndex["Civil Court"], 2});
    graph[nameToIndex["Civil Court"]].push_back({nameToIndex["Shivaji Nagar"], 2});
    graph[nameToIndex["Civil Court"]].push_back({nameToIndex["Pune Station"], 3});
    graph[nameToIndex["Pune Station"]].push_back({nameToIndex["Civil Court"], 3});
    graph[nameToIndex["Pune Station"]].push_back({nameToIndex["Ruby Hall Clinic"], 2});
    graph[nameToIndex["Ruby Hall Clinic"]].push_back({nameToIndex["Pune Station"], 2});
    graph[nameToIndex["Ruby Hall Clinic"]].push_back({nameToIndex["Bund Garden"], 3});
    graph[nameToIndex["Bund Garden"]].push_back({nameToIndex["Ruby Hall Clinic"], 3});
    graph[nameToIndex["Bund Garden"]].push_back({nameToIndex["Yerwada"], 4});
    graph[nameToIndex["Yerwada"]].push_back({nameToIndex["Bund Garden"], 4});
    graph[nameToIndex["Yerwada"]].push_back({nameToIndex["Ramwadi"], 5});
    graph[nameToIndex["Ramwadi"]].push_back({nameToIndex["Yerwada"], 5});
    graph[nameToIndex["Ramwadi"]].push_back({nameToIndex["Kalyani Nagar"], 3});
    graph[nameToIndex["Kalyani Nagar"]].push_back({nameToIndex["Ramwadi"], 3});
    graph[nameToIndex["Kalyani Nagar"]].push_back({nameToIndex["Koregaon Park"], 4});
    graph[nameToIndex["Koregaon Park"]].push_back({nameToIndex["Kalyani Nagar"], 4});
    graph[nameToIndex["Koregaon Park"]].push_back({nameToIndex["Mangalwar Peth"], 4});
    graph[nameToIndex["Mangalwar Peth"]].push_back({nameToIndex["Koregaon Park"], 4});
    graph[nameToIndex["Mangalwar Peth"]].push_back({nameToIndex["Civil Court"], 4});
    graph[nameToIndex["Civil Court"]].push_back({nameToIndex["Mangalwar Peth"], 4});
    graph[nameToIndex["Civil Court"]].push_back({nameToIndex["Nehru Chowk"], 3});
    graph[nameToIndex["Nehru Chowk"]].push_back({nameToIndex["Civil Court"], 3});
    graph[nameToIndex["Nehru Chowk"]].push_back({nameToIndex["Mandai"], 3});
    graph[nameToIndex["Mandai"]].push_back({nameToIndex["Nehru Chowk"], 3});
    graph[nameToIndex["Mandai"]].push_back({nameToIndex["Deccan"], 4});
    graph[nameToIndex["Deccan"]].push_back({nameToIndex["Mandai"], 4});
    graph[nameToIndex["Deccan"]].push_back({nameToIndex["Nal Stop"], 3});
    graph[nameToIndex["Nal Stop"]].push_back({nameToIndex["Deccan"], 3});
    graph[nameToIndex["Nal Stop"]].push_back({nameToIndex["Garware College"], 3});
    graph[nameToIndex["Garware College"]].push_back({nameToIndex["Nal Stop"], 3});
    graph[nameToIndex["Garware College"]].push_back({nameToIndex["Karve Nagar"], 3});
    graph[nameToIndex["Karve Nagar"]].push_back({nameToIndex["Garware College"], 3});
    graph[nameToIndex["Karve Nagar"]].push_back({nameToIndex["Warje"], 4});
    graph[nameToIndex["Warje"]].push_back({nameToIndex["Karve Nagar"], 4});
    graph[nameToIndex["Warje"]].push_back({nameToIndex["Swargate"], 5});
    graph[nameToIndex["Swargate"]].push_back({nameToIndex["Warje"], 5});

    string source, dest, timeStr;
    cout << "Enter source station: ";
    getline(cin, source);
    cout << "Enter destination station: ";
    getline(cin, dest);
    cout << "Enter travel time (HH:MM): ";
    getline(cin, timeStr);

    if (nameToIndex.find(source) == nameToIndex.end() || nameToIndex.find(dest) == nameToIndex.end()) {
        cout << "Invalid station entered!\n";
        return 1;
    }

    pair<int, int> parsedTime = parseTime(timeStr);
    string nextTime = getNextMetroTime(parsedTime.first, parsedTime.second);
    pair<int, int> travelStart = parseTime(nextTime);

    vector<int> dist, prev;
    dijkstra(nameToIndex[source], graph, stations, dist, prev);

    vector<int> path;
    int at = nameToIndex[dest];
    while (at != -1) {
        path.push_back(at);
        at = prev[at];
    }
    reverse(path.begin(), path.end());

    saveTripHistory(source, dest, timeStr);
    displayRoute(path, stations);
    showRouteDetails(path, stations, graph, travelStart.first, travelStart.second);

    string feedbackChoice;
    cout << "\nWould you like to give feedback? (yes/no): ";
    getline(cin, feedbackChoice);
    if (feedbackChoice == "yes") {
        collectFeedback();
    }

    return 0;
} 
