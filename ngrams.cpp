#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
using std::string;
using std::vector;

/**
 * @brief splits a given string into words by space
 * 
 * @param text the text to be split
 * @return vector<string> a vector of the words after being split
 */
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

int main(void) {
    int n;
    string text;
    std::ifstream file("./oanc/1-3_meth_901.txt");

    // Check for error opening file
    if(!file) {
        std::cerr << "Could not open the file!" << std::endl;
    } else {
        // testing area
        // while(getline(file, text)) {
        //     std::cout << text << '\n';
        // }

        while(getline(file, text)) {
            vector<string> splitWords = split(text);

            for(const auto& word : splitWords) {
                std::cout << word << '\n';
            }
        }

        /* FOR ITERATING THROUGH EACH FILE IN OANC DIRECTORY */
        // std::string path = "./oanc";
        // for(const auto& entry : fs::directory_iterator(path)) {
        //     std::cout << entry.path() << std::endl;
        // }
    }

    file.close();
    return EXIT_SUCCESS;
}