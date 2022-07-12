#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

namespace fs = std::filesystem;
using index_t = std::vector<int>::size_type;
using std::set;
using std::pair;
using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cin;

/**
 * @brief Checks if a given file exists.
 * 
 * @param name file to be checked
 * @return true if the file exists
 * @return false if the file does not exist
 */
inline bool file_exists(const string& name) {
    struct stat buffer;   
    return (stat(name.c_str(), &buffer) == 0); 
}

class NGramModel {
  public:
    NGramModel() {
        srand(time(NULL));
        context_ngrams = {};
    }

    /**
     * @brief Adds the given words into the language model.
     * 
     * @param words the words to be added to the model
     */
    void update_model(vector<string> words) {
        if(words.size() < 3) {
            return;
        }

        for(index_t i = 0; i < words.size() - 2; i++) {
            // tuple of ({w - 2} + {w - 1}, w) 
            string context = words.at(i) + " " + words.at(i + 1);
            this->context_ngrams[context][words.at(i + 2)]++;
        }
    }

    /**
     * @brief loads all OANC-provided text files into the model.
     */
    void load() {
        cout << "Parsing files..." << '\n';
        if(file_exists("./ngrams.cache")) {

        } else {
            std::ifstream file;
            string path = "./oanc";
            string text;
            for(const auto& entry : fs::directory_iterator(path)) {
                file.open(entry.path());
                while(getline(file, text)) {
                    vector<string> words = NGramModel::tokenize(text + " ");
                    update_model(words);
                }

                file.close();
            }
        }

        cout << "Done!" << "\n";
    }

    /**
     * @brief stores all parsed data from the OANC dataset into a file named ngram_cache.txt
     */
    void cache_ngrams() {
        std::ofstream ngram_cache("ngrams.cache");
        ngram_cache.clear();
        ngram_cache << "gaming" << '\n';
        ngram_cache.close();
    }

    /**
     * @brief Predicts the next word in a sequence given a context of two words
     * 
     * @param context two words to be used as context for prediction
     * @return string the predicted word
     */
    string predict(string context) {
        map<string, int> possible_words = this->context_ngrams[context];
        if(possible_words.size() == 0) {
            return " ";
        }

        int rand_i = range_rand(0, possible_words.size() - 1);
        map<string, int>::iterator it = possible_words.begin();
        std::advance(it, rand_i);
        return std::get<0>(*it);
    }

    /**
     * @brief Given a starting seed, generates a new string of text until no more trigrams are found
     * 
     * @param seed the starting string (must be at least 2 words)
     * @return string the generated string
     */
    string gen_sentence(string seed) {
        vector<string> context = tokenize(seed + " ");
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

    /**
     * @brief Gives the probability that a certain word will appear next given the context
     * 
     * @param context the context for the prediction
     * @param prediction the possible words in the prediction
     * @return double the probability for {prediction} to show up given {context}
     */
    double ngram_prob(string context, string prediction) {
        map<string, int> ngram_freq = this->context_ngrams[context];
        double pred_freq = 0;
        double total_freq = 0;
        for(const auto& [key, freq] : ngram_freq) {
            if(key == prediction) {
                pred_freq = freq;
            }

            total_freq += freq;
        }

        if(total_freq == 0) {
            return 0;
        }
        
        return (pred_freq / total_freq);
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
        /* REWORKING IN PROGRESS */
        for(const auto& [key, gram_map] : this->context_ngrams) {
            for(const auto& [key2, gram_freq] : gram_map) {
                cout << '\'' << key << "', '" << key2 << "' => " << gram_freq << '\n';
            }
        }
    }

  private:
    // Stores trigrams as context -> (possible words -> frequency of those words) 
    map<string, map<string, int>> context_ngrams;

    // Calculates a random value between a given range. max cannot be less than min.
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

    // std::ifstream file("./oanc/1-3_meth_901.txt");
    // string text;
    // while(getline(file, text)) {
    //     vector<string> words = NGramModel::tokenize(text + " ");
    //     model.update_model(words);
    // }

    model.cache_ngrams();
    cout << model.gen_sentence("it is") << '\n';
    cout << model.ngram_prob("it is", "not") << '\n';
    // string input;
    // cout << "Input a seed (at least two words): " << '\n';
    // cin >> input;
    // while(input != "STOP") {
    //     cout << model.gen_sentence(input + " ") << '\n';
    //     cout << "Input a seed (at least two words): " << '\n';
    //     cin >> input;
    // }

    // file.close();
    return EXIT_SUCCESS;
}