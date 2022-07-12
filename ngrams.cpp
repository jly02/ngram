#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <set>

namespace fs = std::filesystem;
using index_t = std::vector<int>::size_type;
using std::set;
using std::pair;
using std::string;
using std::vector;
using std::map;
using std::cout;

vector<string> tokenize(string text) {
    string space_delimiter = " ";
    vector<string> words{};

    size_t pos = 0;
    while ((pos = text.find(space_delimiter)) != string::npos) {
        words.push_back(text.substr(0, pos));
        text.erase(0, pos + space_delimiter.length());
    }

    return words;
}

void gen_trigrams(vector<string> words, map<string, set<string>> &context, map<pair<string, string>, int> &ngram_counter) {
    if(words.size() < 3) {
        return;
    }

    for(index_t i = 0; i < words.size() - 2; i++) {
        // tuple of ({w - 2} + {w - 1}, w) 
        string key = words.at(i) + " " + words.at(i + 1);
        context[key].insert(words.at(i + 2));
        ngram_counter[std::make_pair(key, words.at(i + 2))]++;
    }
}

int main(void) {
    std::ifstream file("./oanc/1-3_meth_901.txt"); // testing file

    map<string, set<string>> context{};
    map<pair<string, string>, int> ngram_counter{};

    // Check for errors opening files
    if(!file) {
        std::cerr << "Could not open the file!" << std::endl;
    } else {
        // testing area
        // while(getline(file, text)) {
        //     cout << text << '\n';
        // }

        string text;
        while(getline(file, text)) {
            vector<string> words = tokenize(text + " ");
            gen_trigrams(words, context, ngram_counter);
        }

        for(const auto& [key, val] : ngram_counter) {
            cout << '\'' << std::get<0>(key) << "', '" << std::get<1>(key) << "' => " << val << '\n';
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