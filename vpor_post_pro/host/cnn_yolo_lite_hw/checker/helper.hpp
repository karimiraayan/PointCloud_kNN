//
// Created by gesper on 20.11.20.
//

#include <unistd.h>
#include <cctype>
#include <iostream>

void getUserHexChoice(bool &hex) {
    // get user choice
//    char c;
    std::cout << "On ERROR on verification, print data as Hex? [Y|n]\n";
//    std::cin >> c;
    std::string name;
    getline(std::cin, name);
    hex = true;
    if (name == "n" || name == "N")
        hex = false;
    printf("Selected Print as %s\n", (hex) ? "Hex" : "Dec");
}

