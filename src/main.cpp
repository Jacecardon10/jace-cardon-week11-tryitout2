#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct Pokemon {
	string name;
	int level;
};

string getBasePath() {
	fs::path p = fs::current_path();

	while (p.has_parent_path()) {
		if (fs::exists(p / "src")) {
			return(p / "src").string() + "/";
		}
		p = p.parent_path();
	}

	return "src/";
}

vector<string> loadVersion(const string& filename) {
	vector<string> list;
	ifstream file(filename);

	if (!file.is_open()) {
		cout << "Error opening file: " << filename << endl;
		return list;
	}

	string name;
	while (file >> name) {
		list.push_back(name);
	}

	return list;
}

vector<Pokemon> loadCaptured(const string& filename) {
	vector<Pokemon> list;
	ifstream file(filename);

	if (!file.is_open()) return list;

	string name;
	int level;

	while (file >> name >> level) {
		list.push_back({name, level});
	}

	return list;
}

void saveCaptured(const string& filename, const vector<Pokemon>& list) {
	ofstream file(filename);
	
	for (const auto& p : list) {
		file << p.name << " " << p.level << endl;
	}
}

void viewCaptured(const vector<Pokemon>& list) {
	if (list.empty()) {
		cout << "\nYou have no Pokemon yet.\n";
		return;
	}

	cout << "\nYour Pokemon:\n";
	for (const auto& p : list) {
		cout << p.name << " - Level " << p.level << endl;
	}
}

void attemptCapture(vector<string>& versionList, vector<Pokemon>& capturedList) {
	if (versionList.empty()) {
		cout << "No Pokemon loaded.\n";
		return;
	}

	string encountered = versionList[rand() % versionList.size()];
	cout << "\nYou encountered: " << encountered << endl;

	int guess;
	cout << "Guess a number (1-15): ";
	cin >> guess;
	
	int target = rand() % 15 + 1;

	cout << "Target was: " << target << endl;

	if (abs(guess - target) <= 3) {
		cout << "Capture successful!\n";

		bool found = false;

		for (auto& p : capturedList) {
			if (p.name == encountered) {
				if (p.level < 5) p.level++;
				cout << p.name << "leveled up to " << p.level << "!\n";
				found = true;
				break;
			}
		}

		if (!found) {
			capturedList.push_back({ encountered, 1 });
			cout << "New Pokemon added!\n";
		}
	}
	else {
		cout << "The Pokemon escaped!\n";
	}
}

void tradePokemon(vector<Pokemon>& capturedList, const string& basePath) {
	string tradePath = basePath + "trade";

	if (!fs::exists(tradePath)) {
		cout << "No Pokemon to trade with.\n";
		return;
	}

	vector<string> files;

	for (const auto& entry : fs::directory_iterator(tradePath)) {
		files.push_back(entry.path().string());
	}

	if (files.empty()) {
		cout << "No Pokemon to trade with.\n";
		return;
	}

	cout << "\nTrade Files:\n";
	for (int i = 0; i < files.size(); i++) {
		cout << i << ": " << files[i] << endl;
	}

	int choice;
	cout << "Select file: ";
	cin >> choice;

	if (choice < 0 || choice >= files.size()) return;

	vector<Pokemon> friendList = loadCaptured(files[choice]);

	if (capturedList.empty() || friendList.empty()) {
		cout << "Battle cannot happen.\n";
		return;
	}

	cout << "\nYour Pokemon:\n";
	for(int i = 0; i < capturedList.size(); i++) {
		cout << i << ": " << capturedList[i].name
			<< " (LV " << capturedList[i].level << ")\n";
	}

	int myPick;
	cout << "Choose your Pokemon: ";
	cin >> myPick;

	cout << "\nOpponent Pokemon:\n";
	for (int i = 0; i < friendList.size(); i++) {
		cout << i << ": " << friendList[i].name
			<< " (LV " << friendList[i].level << ")\n";
	}

	int theirPick;
	cout << "Choose opponent Pokemon: ";
	cin >> theirPick;

	int myPower = capturedList[myPick].level + (rand() % 6);
	int theirPower = friendList[theirPick].level + (rand() % 6);

	cout << "\nBattle Result:\n";
	cout << "You: " << myPower << "VS. Them: " << theirPower << endl;

	if (myPower >= theirPower) {
		cout << "You win and obtain " << friendList[theirPick].name << "!\n";

		bool found = false;

		for (auto& p : capturedList) {
			if (p.name == friendList[theirPick].name) {
				if (p.level < 5) p.level++;
				cout << p.name << " " << " leveled up to " << p.level << "!\n";
				found = true;
				break;
			}
		}

		if (!found) {
			capturedList.push_back({friendList[theirPick].name, 1});
		}
	}
	else {
		cout << "You lost the battle.\n";
	}
}

int main() {
	srand(time(0));

	string base = getBasePath();

	cout << "Choose version (red/blue): ";
	string version;
	cin >> version;

	string versionFile =
		(version == "red") ? base + "red.txt" : base + "blue.txt";

	vector<string> versionList = loadVersion(versionFile);
	vector<Pokemon> capturedList = loadCaptured(base + "captured.txt");

	int choice;

	do {
		cout << "\n\n1. Capture Pokemon";
		cout << "\n2. View Pokemon";
		cout << "\n3. Trade Pokemon";
		cout << "\n4. Exit\n";
		cin >> choice;

		if (choice == 1) {
			attemptCapture(versionList, capturedList);
			saveCaptured(base + "captured.txt", capturedList);
		}else if(choice == 2) {
			viewCaptured(capturedList);
		}else if (choice == 3) {
			tradePokemon(capturedList, base);
			saveCaptured(base + "captured.txt", capturedList);
		}
	} while (choice != 4);

	return 0;
}