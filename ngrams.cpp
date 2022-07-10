#include <iostream>
#include <fstream>
#include <string>

int main(void) {
    int n;
    std::string text;
    std::ifstream file("./oanc/1-3_meth_901.txt");

    if(!file) {
        std::cerr << "Could not open the file!" << std::endl;
    } else {
        while(getline(file, text)) {
            std::cout << text << std::endl;
        }
    }

    file.close();
}