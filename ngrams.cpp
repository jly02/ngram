#include <sys/stat.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <map>

using index_t = std::vector<int>::size_type;
namespace fs = std::filesystem;
using std::string;
using std::vector;
using std::cout;
using std::map;
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
            string context = words.at(i) + ' ' + words.at(i + 1);
            this->context_ngrams[context][words.at(i + 2)]++;
        }
    }

    /**
     * @brief loads all OANC-provided text files into the model.
     */
    void load() {
        cout << "Parsing files...\n";
        
        if(file_exists("./ngrams.cache")) {
            // Cache file will be parsed if it exists
            cout << "Reading from ngrams.cache...\n";

            std::ifstream file("./ngrams.cache");
            string text;
            vector<string> words{};
            string context;
            while(getline(file, text)) {
                words = tokenize(text + " ");
                context = words.at(0) + ' ' + words.at(1);
                for(index_t i = 2; i < words.size() - 1; i += 2) {
                    this->context_ngrams[context][words.at(i)] = stoi(words.at(i + 1));
                }
            }
        } else {
            // If not, manually parse every file in the OANC directory and gather the data
            std::ifstream file;
            string path = "./oanc";
            string text;
            for(const auto& entry : fs::directory_iterator(path)) {
                file.open(entry.path());
                while(getline(file, text)) {
                    vector<string> words = NGramModel::tokenize(text + ' ');
                    update_model(words);
                }

                file.close();
            }
        }

        cout << "Done!\n";
    }

    /**
     * @brief stores all parsed data from the OANC dataset into a file named ngram_cache.txt
     */
    void cache_ngrams() {
        std::ofstream ngram_cache("ngrams.cache");
        ngram_cache.clear();
        for(const auto& [key, gram_map] : this->context_ngrams) {
            ngram_cache << key;
            for(const auto& [key2, gram_freq] : gram_map) {
                ngram_cache << ' ' << key2 << ' ' << gram_freq;
            }

            ngram_cache << '\n';
        }

        ngram_cache.close();
        cout << "Parsed data has been cached into ngrams.cache\n";
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

        vector<string> word_freq{};
        for(const auto& [key, freq] : possible_words) {
            for(index_t i = 0; i < freq; i++) {
                word_freq.push_back(key);
            }
        }

        int rand_i = range_rand(0, word_freq.size() - 1);
        return word_freq.at(rand_i);
    }

    /**
     * @brief Given a starting seed, generates a new string of text until no more trigrams are found
     * 
     * @param seed the starting string (must be at least 2 words)
     * @return string the generated string
     */
    string gen_sentence(string seed) {
        vector<string> context = tokenize(seed + ' ');

        if(context.size() < 2) {
            throw std::invalid_argument("Input cannot be less than two words!");
        }

        string sentence = context.at(0) + ' ' + context.at(1);
        string prediction;
        size_t sentence_length = 2;
        while((prediction = predict(context.at(0) + ' ' + context.at(1))) != " ") {
            context.push_back(prediction);
            context.erase(context.begin());
            sentence += ' ' + context.at(1);

            if(++sentence_length == 150) {
                break;
            }
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

  private:
    // Stores trigrams as context -> (possible words -> frequency of those words) 
    map<string, map<string, int>> context_ngrams;
    std::random_device rd;

    // Calculates a random value between a given range. max cannot be less than min.
    int range_rand(int min, int max){
        std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased

        auto random_integer = uni(rng);
        return random_integer;
    }
};

int main(void) {
    NGramModel model = NGramModel();
    model.load();
    // Uncomment this if you have not yet created a cache file.
    // model.cache_ngrams();

    string input;
    cout << "Input a seed (at least two words): \n";
    std::getline(cin, input);
    while(input != "STOP") {
        cout << "Generated sentence:\n" << model.gen_sentence(input) << '\n';
        cout << "Input a seed (at least two words): \n";
        std::getline(cin, input);
    }

    return EXIT_SUCCESS;
}