#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // For system()
#include <unordered_map>

// Function to trim whitespace from a string
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to check if a line is a comment
bool isComment(const std::string& line) {
    return line.substr(0, 3) == "// ";
}

// Function to handle escape sequences like \n and \t
std::string processEscapes(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                case '\'': result += '\''; break;
                default: result += str[i + 1]; break;
            }
            ++i; // Skip the next character as it is part of the escape sequence
        } else {
            result += str[i];
        }
    }
    return result;
}

// Function to handle printing content
void executePrint(const std::string& content, const std::unordered_map<std::string, std::string>& variables) {
    if (content.size() >= 4 && content.substr(0, 2) == "${" && content.back() == '}') {
        std::string varName = content.substr(2, content.size() - 3);
        if (variables.find(varName) != variables.end()) {
            std::cout << variables.at(varName) << std::endl;
        } else {
            std::cerr << "Error: Variable '" << varName << "' not defined." << std::endl;
        }
    } else {
        std::cout << content << std::endl;
    }
}

// Function to run a shell command
void executeRun(const std::string& command) {
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Error: Failed to execute command '" << command << "'." << std::endl;
    }
}

// Function to open a URL
void executeOpen(const std::string& url) {
    std::string command;
    #ifdef _WIN32
    // For Windows, use start command to open URLs in default browser
    command = "start " + url;
    #elif __linux__
    // For Linux, use xdg-open to open URLs in default browser
    command = "xdg-open " + url;
    #else
    std::cerr << "Error: Platform not supported for URL opening." << std::endl;
    return;
    #endif

    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Error: Failed to open URL '" << url << "'." << std::endl;
    }
}

// Interpreter for BATASM
void interpretBATASM(const std::string& scriptPath) {
    std::ifstream scriptFile(scriptPath);
    if (!scriptFile.is_open()) {
        std::cerr << "Error: Could not open file '" << scriptPath << "'." << std::endl;
        return;
    }

    std::unordered_map<std::string, std::string> globalVariables;
    bool inStartFunction = false;

    std::string line;
    while (std::getline(scriptFile, line)) {
        line = trim(line);

        if (line.empty() || isComment(line)) {
            // Skip empty lines and comments
            continue;
        }

        if (!inStartFunction) {
            if (line.substr(0, 5) == "&{$S=") {
                if (line.substr(5, 6) != "BATASM") {
                    std::cerr << "Error: Invalid source directive. Expected BATASM format." << std::endl;
                    return;
                }
            } else if (line == "START ({") {
                inStartFunction = true;
            } else {
                std::cerr << "Error: Code outside of START block is not allowed." << std::endl;
            }
            continue;
        }

        if (inStartFunction) {
            if (line.substr(0, 4) == "EXIT") {
                // Check if "EXIT" command includes the "silent" parameter
                size_t paramPos = line.find("silent");
                if (paramPos != std::string::npos) {
                    // Silent exit - do not print anything
                    break;
                } else {
                    // Default exit with message
                    std::cout << "Program terminated by EXIT command." << std::endl;
                    break; // Stops the interpreter
                }
            } else if (line == "})") {
                inStartFunction = false;
            } else if (line.substr(0, 5) == "GVAR ") {
                size_t eqPos = line.find("==");
                if (eqPos != std::string::npos) {
                    std::string varName = trim(line.substr(5, eqPos - 5));
                    std::string varValue = trim(line.substr(eqPos + 2));
                    if (varValue.size() > 1 && 
                        (varValue.front() == '\'' || varValue.front() == '"') && 
                        varValue.front() == varValue.back()) {
                        varValue = varValue.substr(1, varValue.size() - 2);
                    }
                    globalVariables[varName] = varValue;
                } else {
                    std::cerr << "Error: Invalid GVAR declaration." << std::endl;
                }
            } else if (line.substr(0, 5) == "PRINT") {
                std::string content = trim(line.substr(5));
                if (!content.empty() && (content.front() == '\'' || content.front() == '"' || content.front() == '`')) {
                    content = content.substr(1, content.size() - 2);
                }
                executePrint(content, globalVariables);
            } else if (line.substr(0, 3) == "RUN") {
                std::string content = trim(line.substr(4));
                executeRun(content);
            } else {
                std::cerr << "Error: Unknown or unsupported command in START block: " << line << std::endl;
            }
        }
    }

    scriptFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: batasm_interpreter <script.batasm>" << std::endl;
        return 1;
    }

    std::string scriptPath = argv[1];
    interpretBATASM(scriptPath);

    return 0;
}
