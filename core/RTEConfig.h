#pragma once

#include "Component.h"
#include "ECS.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

typedef enum { NEWLINE, IDENT, NUMBER, SEMICOLON } RTETokenType;

struct RTEToken {
    RTETokenType type;
    std::string value;
};

struct RTELexer {
    void fromFile(std::string_view path) {
        currentIndex = 0;

        constexpr auto read_size = std::size_t(4096);
        auto stream = std::ifstream(path.data());
        stream.exceptions(std::ios_base::badbit);

        if (not stream) {
            throw std::ios_base::failure("file does not exist");
        }

        auto out = std::string();
        auto buf = std::string(read_size, '\0');
        while (stream.read(&buf[0], read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        out.append(buf, 0, stream.gcount());
        content = out;
    }

    void fromString(const char *str) {
        currentIndex = 0;
        content = std::string(str);
    }

    std::string nextToken() {
        while (content[currentIndex] == ' ' || content[currentIndex] == '\t') {
            currentIndex++;
        }

        if (currentIndex >= content.size())
            return "EOF";

        if (content[currentIndex] == '\n') {
            currentIndex += 1;
            return "\n";
        }

        if (content[currentIndex] == ';') {
            currentIndex += 1;
            return ";";
        }

        if (isalpha(content[currentIndex])) {
            int endOfIndentifier = currentIndex;

            while (isalpha(content[endOfIndentifier])) {
                endOfIndentifier++;
            }

            int sizeOfIdentifier = endOfIndentifier - currentIndex;
            int startOfIdentifier = currentIndex;
            currentIndex = endOfIndentifier;
            return content.substr(startOfIdentifier, sizeOfIdentifier);
        }

        if (isdigit(content[currentIndex])) {
            int endOfNum = currentIndex;

            while (isalpha(content[endOfNum])) {
                endOfNum++;
            }

            int sizeOfNum = endOfNum - currentIndex;
            int startOfNum = currentIndex;
            currentIndex = endOfNum;
            return content.substr(startOfNum, sizeOfNum);
        }

        return "";
    }

    int currentIndex = 0;
    std::string content;
};
