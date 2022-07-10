#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main(void) {
    int n;
    std::string text;
    std::ifstream file("./oanc/1-3_meth_901.txt");

    if(!file) {
        std::cerr << "Could not open the file!" << std::endl;
    } else {
        // testing area
        while(getline(file, text)) {
            std::cout << text << std::endl;
        }

        /* FOR ITERATING THROUGH EACH FILE IN OANC DIRECTORY */
        // std::string path = "./oanc";
        // for (const auto & entry : fs::directory_iterator(path)) {
        //     std::cout << entry.path() << std::endl;
        // }
    }

    file.close();
}