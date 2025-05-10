#ifndef FUNCS_H
#define FUNCS_H

#include "structs.h"
#include <cstddef>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <sstream>

std::string to_hex(char c) {
    std::stringstream ss;
    ss << "\\x" << std::hex << std::uppercase;
    if ((unsigned char)c < 16) ss << "0";
    ss << (int)(unsigned char)c;
    return ss.str();
}

int randint(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

std::vector<std::string> split(std::string line, char ch) {
    std::string temp;
    std::vector<std::string> tokens;
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] != ch) {
            temp += line[i];
        } else {
            tokens.push_back(temp);
            temp = "";
        }
    }
    if (temp != "") {
        tokens.push_back(temp);
    }
    return tokens;
}

std::string replaceLine(std::string line, const std::vector<Replacement>& reps) {
    for (const auto& rep : reps) {
        size_t pos = 0;
        while ((pos = line.find(rep.name, pos)) != std::string::npos) {
            line.replace(pos, rep.name.size(), rep.replace);
            pos += std::max<size_t>(1, rep.replace.size());  // move forward safely
        }
    }
    return line;
}



std::string generate_random_var_name() {
    srand(static_cast<unsigned int>(time(NULL)));
    std::string temp;
    std::vector<std::string> chars = {"I", "l", "O", "0"};
    for (int i = 0; i < (randint(5,200)); ++i) {
        temp += chars[rand() % chars.size()];
    }
    return temp;
}

std::vector<std::string> find_varnames(std::string line) {
    std::vector<std::string> temp;
    bool insidefunc = false;
    if (line.find("def") != std::string::npos) {
        insidefunc = true;
        std::string str = line;
        std::string def = "def";

        std::string name = str.substr(str.find(def) + 4, str.find('(') - str.find(def) - 4);

        temp.push_back(name);
    }

    if (insidefunc) {
        size_t open = line.find('(');
        size_t close = line.find(')');

        std::string z = line.substr(open + 1, close - open - 1);

        Replacement rep1;
        rep1.name = ",";
        rep1.replace = "";

        Replacement rep2;
        rep1.name = " ";
        rep1.replace = "";

        z = replaceLine(z, {rep2});

        for (const auto& tokens : split(z, ',')) {
            temp.push_back(tokens);
        }
    } else {
        if (line.find('=') != std::string::npos) {
            temp.push_back(line.substr(0, line.find('=')));
        }
    }
    return temp;
}


std::string obfuscate_string(const std::string& s) {
    std::string result;
    for (char c : s) result += to_hex(c);
    return "\"" + result + "\"";
}

std::string obfuscateStrings(std::string str) {
    bool quote = false;
    std::string temp;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '"') {
            quote = !quote;
            temp += "\"";

        } else {
            if (quote) {
                temp += to_hex(str[i]);
            } else {
                temp += str[i];
            }
        }
    }
    return temp;
}


Replacement makerep(std::string varname) {
    Replacement rep;
    rep.name = varname;
    rep.replace = generate_random_var_name();
    return rep;
}

void obfuscate(std::string str) {
    std::vector<std::string> lines = split(str, '\n');

    std::vector<Replacement> reps;

    for (const auto& line : lines) {
        std::vector<std::string> vars = find_varnames(line);
        for (const auto& var : vars) {
            reps.push_back(makerep(var));
        }
    }
    std::vector<std::string> temp;

    Replacement rep;
    rep.name = "'";
    rep.replace = "\"";

    reps.push_back(rep);

    for (auto &line : lines) {
       temp.push_back(replaceLine(line, reps));
    }
    std::vector<std::string> phase2;
    for (auto &line : temp) {
       phase2.push_back(obfuscateStrings(line));
    }

    for (const auto& line : phase2) {
        std::cout << line << std::endl;
    }
}


#endif
