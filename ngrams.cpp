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

class NGramModel {
  public:
    NGramModel() {
        context = {};
        ngram_counter = {};
    }

    void update_model(vector<string> words) {
        if(words.size() < 3) {
            return;
        }

        for(index_t i = 0; i < words.size() - 2; i++) {
            // tuple of ({w - 2} + {w - 1}, w) 
            string key = words.at(i) + " " + words.at(i + 1);
            this->context[key].insert(words.at(i + 2));
            this->ngram_counter[std::make_pair(key, words.at(i + 2))]++;
        }
    }

    // Not yet implemented.
    string predict(string context) {
        return NULL;
    }

    /**
     * @brief Splits a given string into a list by spaces.
     * 
     * @param text the string to be split
     * @return vector<string> the list of 'words' after splitting
     */
    static vector<string> tokenize(string text) {
        string delimiter = " ";
        vector<string> words{};

        size_t pos = 0;
        while ((pos = text.find(delimiter)) != string::npos) {
            words.push_back(text.substr(0, pos));
            text.erase(0, pos + delimiter.length());
        }

        return words;
    }

    /**
     * @brief Splits a given string into a list by a given delimiter.
     * 
     * @param text the string to be split
     * @param delimiter the text that the string will be split by
     * @return vector<string> the list of 'words' after splitting
     */
    static vector<string> tokenize(string text, string delimiter) {
        vector<string> words{};

        size_t pos = 0;
        while ((pos = text.find(delimiter)) != string::npos) {
            words.push_back(text.substr(0, pos));
            text.erase(0, pos + delimiter.length());
        }

        return words;
    }

    // for testing purposes
    void print_ngrams() {
        for(const auto& [key, val] : this->ngram_counter) {
            cout << '\'' << std::get<0>(key) << "', '" << std::get<1>(key) << "' => " << val << '\n';
        }
    }

  private:
    map<string, set<string>> context;
    map<pair<string, string>, int> ngram_counter;
};

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

        NGramModel model = NGramModel();
        string text;
        while(getline(file, text)) {
            vector<string> words = NGramModel::tokenize(text + " ");
            model.update_model(words);
        }

        model.print_ngrams();

        /* FOR ITERATING THROUGH EACH FILE IN OANC DIRECTORY */
        // string path = "./oanc";
        // for(const auto& entry : fs::directory_iterator(path)) {
        //     cout << entry.path() << std::endl;
        // }
    }

    file.close();
    return EXIT_SUCCESS;
}