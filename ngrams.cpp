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

class NGramModel {
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

    // Not yet implemented.
    string predict(string context) {
        set<string> possible_words = this->context_cand[context];
        // int numContext = ngram_counter[std::make_pair(context, "")];
        index_t rand_i = range_rand(0, possible_words.size() - 1);

        set<string>::iterator it = possible_words.begin();
        std::advance(it, rand_i);
        return *(it);
    }

    string gen_sentence(size_t length) {
        vector<string> context{};
        context.push_back("<START>");
        context.push_back("<START>");
        string sentence = "<START> <START>";
        for(index_t i = 0; i < length; i++) {
            context.push_back(predict(context.at(0) + ' ' + context.at(1)));
            context.erase(context.begin());
            sentence += " " + context.at(1);
        }

        return sentence;
    }

    string ngram_prob(string context) {
        // to be implemented
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

        // model.print_ngrams();
        cout << model.predict("it is") << '\n';

        /* FOR ITERATING THROUGH EACH FILE IN OANC DIRECTORY */
        // string path = "./oanc";
        // for(const auto& entry : fs::directory_iterator(path)) {
        //     cout << entry.path() << std::endl;
        // }
    }

    file.close();
    return EXIT_SUCCESS;
}