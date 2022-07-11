#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <map>

namespace fs = std::filesystem;
using index_t = std::vector<int>::size_type;
using std::string;
using std::vector;
using std::map;
using std::cout;

vector<string> split(string text) {
    string space_delimiter = " ";
    vector<string> words{};

    size_t pos = 0;
    while ((pos = text.find(space_delimiter)) != string::npos) {
        words.push_back(text.substr(0, pos));
        text.erase(0, pos + space_delimiter.length());
    }

    return words;
}

void trigrams(vector<string> words) {
    vector<string> triwords{};
    if(words.size() < 3) {
        return;
    }

    for(index_t i = 0; i < words.size() - 2; i++) {
        // put three words together
        triwords.push_back(words.at(i) + " " + words.at(i + 1) + " " + words.at(i + 2));
    }

    for(const auto& word : triwords) {
        cout << word << '\n';
    }
}

int main(void) {
    int n;
    string text;
    std::ifstream file("./oanc/1-3_meth_901.txt");
    map<string, int> freq{};

    // Check for error opening file
    if(!file) {
        std::cerr << "Could not open the file!" << std::endl;
    } else {
        // testing area
        // while(getline(file, text)) {
        //     cout << text << '\n';
        // }

        while(getline(file, text)) {
            vector<string> splitWords = split(text + " ");
            trigrams(splitWords);
        }

        /* FOR ITERATING THROUGH EACH FILE IN OANC DIRECTORY */
        // string path = "./oanc";
        // for(const auto& entry : fs::directory_iterator(path)) {
        //     cout << entry.path() << std::endl;
        // }
    }

    file.close();
    return EXIT_SUCCESS;
}