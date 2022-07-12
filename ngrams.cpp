#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <stdlib.h>

namespace fs = std::filesystem;
using index_t = std::vector<int>::size_type;
using std::set;
using std::pair;
using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cin;

class NGramModel {
  public:
    NGramModel() {
        srand(time(NULL));
        context_cand = {};
        ngram_counter = {};
    }

    void update_model(vector<string> words) {
        if(words.size() < 3) {
            return;
        }

        for(index_t i = 0; i < words.size() - 2; i++) {
            // tuple of ({w - 2} + {w - 1}, w) 
            string key = words.at(i) + " " + words.at(i + 1);
            this->context_cand[key].insert(words.at(i + 2));
            this->ngram_counter[std::make_pair(key, words.at(i + 2))]++;
        }
    }

    void load() {
        std::ifstream file;
        string path = "./oanc";
        string text;
        cout << "Parsing files..." << '\n';
        for(const auto& entry : fs::directory_iterator(path)) {
            file.open(entry.path());
            while(getline(file, text)) {
                vector<string> words = NGramModel::tokenize(text + " ");
                update_model(words);
            }
            file.close();
        }

        cout << "Done!" << "\n";
    }

    // Not yet fully implemented.
    string predict(string context) {
        set<string> possible_words = this->context_cand[context];
        if(possible_words.size() == 0) {
            return " ";
        }

        index_t rand_i = range_rand(0, possible_words.size() - 1);
        set<string>::iterator it = possible_words.begin();
        std::advance(it, rand_i);
        return *(it);
    }

    string gen_sentence(string seed) {
        vector<string> context = tokenize(seed);
        string sentence = context.at(0) + ' ' + context.at(1);
        string prediction;
        while((prediction = predict(context.at(0) + ' ' + context.at(1))) != " ") {
            context.push_back(prediction);
            context.erase(context.begin());
            sentence += " " + context.at(1);
            prediction = predict(context.at(0) + ' ' + context.at(1));
        }

        return sentence;
    }

    string ngram_prob(string context, string prediction) {
        // to be implemented
        return "Not yet implemented.";
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
        while((pos = text.find(delimiter)) != string::npos) {
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
        while((pos = text.find(delimiter)) != string::npos) {
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
    map<string, set<string>> context_cand;
    map<pair<string, string>, int> ngram_counter;

    int range_rand(int min, int max){
        int n = max - min + 1;
        int remainder = RAND_MAX % n;
        int x;

        do {
            x = rand();
        } while(x >= RAND_MAX - remainder);

        return min + x % n;
    }
};

int main(void) {
    NGramModel model = NGramModel();
    model.load();

    // test with just the single file again tomorrow
    string input;
    cout << "Input a seed (at least two words): " << '\n';
    cin >> input;
    while(input != "STOP") {
        cout << model.gen_sentence(input + " ") << '\n';
        cout << "Input a seed (at least two words): " << '\n';
        cin >> input;
    }

    return EXIT_SUCCESS;
}