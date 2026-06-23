#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <chrono>
#include <cctype>
#include <windows.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: naive_code.exe <filepath>\n";
        return 1;
    }

    string filepath = argv[1];

    auto start_time = chrono::high_resolution_clock::now();

    ifstream file(filepath, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filepath << "\n";
        return 1;
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    string content(size, '\0');
    if (!file.read(&content[0], size)) {
        cerr << "Failed to read file.\n";
        return 1;
    }
    file.close();

    map<string, int> word_index;
    string word;
    for (size_t i = 0; i < content.size(); i++) {
        char c = content[i];
        if (!isalnum((unsigned char)c)) {
            if (!word.empty()) {
                word_index[word]++;
                word.clear();
            }
        } else {
            word += tolower((unsigned char)c);
        }
    }
    if (!word.empty()) {
        word_index[word]++;
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;

    cout << "Total execution time: " << fixed << duration.count() << " seconds\n";

    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        cout << "Peak RAM Usage: " << pmc.PeakWorkingSetSize / (1024.0 * 1024.0) << " MB\n";
    }

    return 0;
}
