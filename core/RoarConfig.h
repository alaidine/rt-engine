#pragma once

#include "Application.h"
#include "Common.h"
#include "Component.h"
#include "ECS.h"
#include "Scripting.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

typedef enum { IDENT, NUMBER, SEMICOLON } RoarTokenType;

struct RoarToken {
    RoarTokenType type;
    std::string value;
};

struct RoarLexer {
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

    void fromString(std::string str) {
        currentIndex = 0;
        content = std::string(str);
    }

    std::string nextToken() {
        while (content[currentIndex] == ' ' || content[currentIndex] == '\t' || content[currentIndex] == '\r' ||
               content[currentIndex] == '\n') {
            currentIndex++;
        }

        if (currentIndex >= content.size())
            return "EOF";

        if (content[currentIndex] == '=') {
            currentIndex += 1;
            return "=";
        }

        if (content[currentIndex] == ';') {
            currentIndex += 1;
            return ";";
        }

        if (content[currentIndex] == '{') {
            currentIndex += 1;
            return "{";
        }

        if (content[currentIndex] == '}') {
            currentIndex += 1;
            return "}";
        }

        if (isalnum(content[currentIndex])) {
            int endOfIndentifier = currentIndex;

            while (isalnum(content[endOfIndentifier])) {
                endOfIndentifier++;
            }

            int sizeOfIdentifier = endOfIndentifier - currentIndex;
            int startOfIdentifier = currentIndex;
            currentIndex = endOfIndentifier;
            return content.substr(startOfIdentifier, sizeOfIdentifier);
        }

        return "";
    }

    int currentIndex = 0;
    std::string content;
};

struct RoarConfig {
    RoarLexer lexer;
    std::string currentToken;
    Ref<Scene> scene;

    void advanceToken() { lexer.nextToken(); }

    void expectToken(std::string expected) {
        if (currentToken != expected) {
            RO_LOG_ERR("Expected {} but got {}", expected, currentToken);
            Roar::Application::Get().Stop();
        }
        currentToken = lexer.nextToken();
    }

    bool isNumeric(std::string str) {
        for (const char &ch : str) {
            if (!isdigit(ch))
                return false;
        }
        return true;
    }

    int expectNum(std::string tok) {
        if (!isNumeric(tok)) {
            RO_LOG_ERR("Expected {} to be a number", tok);
            Roar::Application::Get().Stop();
        }
        currentToken = lexer.nextToken();
        return atoi(tok.c_str());
    }

    void parseComponent(Entity entity) {
        if (currentToken == "transform2D") {
            Roar::TransformComponent transform;

            currentToken = lexer.nextToken();
            expectToken("start");
            while (true) {
                if (currentToken == "end" || currentToken == "EOF") {
                    break;
                }
                if (currentToken == "pos") {
                    currentToken = lexer.nextToken();
                    expectToken("{");
                    expectToken("x");
                    expectToken("=");
                    transform.pos.x = expectNum(currentToken);
                    expectToken("y");
                    expectToken("=");
                    transform.pos.y = expectNum(currentToken);
                    expectToken("}");
                } else if (currentToken == "size") {
                    currentToken = lexer.nextToken();
                    expectToken("{");
                    expectToken("x");
                    expectToken("=");
                    transform.size.x = expectNum(currentToken);
                    expectToken("y");
                    expectToken("=");
                    transform.size.y = expectNum(currentToken);
                    expectToken("}");
                } else {
                    currentToken = lexer.nextToken();
                }
            }
            expectToken("end");
            scene->AddComponent(entity, transform);
        } else if (currentToken == "script") {
            Roar::ScriptComponent script;

            currentToken = lexer.nextToken();
            expectToken("start");
            while (true) {
                if (currentToken == "end" || currentToken == "EOF") {
                    break;
                }
                if (currentToken == "name") {
                    currentToken = lexer.nextToken();
                    expectToken("=");
                    script.name = "Sandbox." + currentToken;
                } else if (currentToken == "ref") {
                    // parse references
                } else {
                    currentToken = lexer.nextToken();
                }
            }
            expectToken("end");
            scene->AddComponent(entity, script);
            Roar::Scripting::OnCreateEntity(entity);

            Ref<Roar::ScriptInstance> scriptInstance = Roar::Scripting::GetEntityScriptInstance(entity);
            RO_LOG_INFO("Speed value: {}", scriptInstance->GetFieldValue<float>("Speed"));
            scriptInstance->SetFieldValue("Speed", 2.3f);
            RO_LOG_INFO("Speed value after change: {}", scriptInstance->GetFieldValue<float>("Speed"));
            RO_LOG_INFO("Current ID: {}", scriptInstance->GetFieldValue<uint32_t>("ID"));
        } else if (currentToken == "rectangle") {
            Roar::RectangleComponent rect;

            currentToken = lexer.nextToken();
            expectToken("start");
            while (true) {
                if (currentToken == "end" || currentToken == "EOF") {
                    break;
                }

                if (currentToken == "color") {
                    currentToken = lexer.nextToken();
                    expectToken("{");
                    expectToken("r");
                    expectToken("=");
                    rect.color[0] = expectNum(currentToken);
                    expectToken("g");
                    expectToken("=");
                    rect.color[1] = expectNum(currentToken);
                    expectToken("b");
                    expectToken("=");
                    rect.color[2] = expectNum(currentToken);
                    expectToken("a");
                    expectToken("=");
                    rect.color[3] = expectNum(currentToken);
                    expectToken("}");
                } else {
                    currentToken = lexer.nextToken();
                }
            }
            expectToken("end");
            scene->AddComponent(entity, rect);
        }
    }

    void parseEntity() {
        currentToken = lexer.nextToken();

        Entity entity = scene->CreateEntity(currentToken);
        RO_LOG_INFO("Entity {} created", currentToken);

        currentToken = lexer.nextToken();
        expectToken("start");

        while (true) {
            if (currentToken == "end" || currentToken == "EOF") {
                break;
            }

            if (currentToken != "component") {
                RO_LOG_ERR("Expected a component but got {}", currentToken);
                Roar::Application::Get().Stop();
            }

            currentToken = lexer.nextToken();
            parseComponent(entity);
        }

        expectToken("end");
    }

    bool fromFile(std::shared_ptr<Scene> sc, std::string filename) {
        scene = sc;
        lexer.fromFile(filename);

        while (true) {
            if (currentToken == "EOF") {
                break;
            }

            if (currentToken == "entity") {
                parseEntity();
            } else {
                currentToken = lexer.nextToken();
            }
        }

        return true;
    }
};
