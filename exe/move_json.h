#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

void toLowerCase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void capitalizeFirstLetter(std::string& str) {
    if (!str.empty()) {
        str[0] = std::toupper(str[0]);
    }
}

int main() {
    try {
        bool continueProcessing = true;

        while (continueProcessing) {
            std::string jsonKey;
            std::string keyValue;

            std::cout << "Enter the JSON key to search for: ";
            std::getline(std::cin, jsonKey);
            toLowerCase(jsonKey);

            std::cout << "Enter the value of the key to search for: ";
            std::getline(std::cin, keyValue);
            toLowerCase(keyValue);

            fs::path currentPath = fs::current_path();
            std::string lastFolder = currentPath.filename().string();
            std::cout << "Current folder: " << lastFolder << std::endl;

            // Capitalize only the first letter of the folder name
            std::string folderName = keyValue;
            capitalizeFirstLetter(folderName);

            fs::path targetFolder = currentPath / folderName;
            int filesMovedCount = 0;
            bool keyFound = false;

            for (const auto& entry : fs::directory_iterator(currentPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    std::ifstream file(entry.path());
                    std::string line;
                    bool foundKey = false;

                    while (std::getline(file, line)) {
                        std::string lineLower = line;
                        toLowerCase(lineLower);

                        if (lineLower.find("\"" + jsonKey + "\":\"" + keyValue + "\"") != std::string::npos) {
                            foundKey = true;
                            keyFound = true;
                            break;
                        }
                    }
                    file.close();

                    if (foundKey) {
                        if (!fs::exists(targetFolder)) {
                            fs::create_directory(targetFolder);
                            std::cout << "Created folder: " << targetFolder.filename().string() << std::endl;
                        }

                        fs::path newLocation = targetFolder / entry.path().filename();
                        try {
                            fs::rename(entry.path(), newLocation);
                            filesMovedCount++;

                            std::string filename = entry.path().filename().string();
                            std::string firstFourChars = filename.substr(0, 4);

                            std::cout << "Moved file: " << firstFourChars << std::endl;
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error moving file: " << e.what() << std::endl;
                        }
                    }
                }
            }

            if (keyFound) {
                std::cout << "Total files moved: " << filesMovedCount << std::endl;
            }
            else {
                std::cout << "No files found for the given key and value." << std::endl;
            }

            std::cout << "Do you want to process more files? (y/n): ";
            char choice;
            std::cin >> choice;
            std::cin.ignore();
            if (choice != 'y') {
                continueProcessing = false;
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}
