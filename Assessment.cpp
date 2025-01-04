#include "Assessment.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>

/* thread stuff, after submission */
std::atomic<bool> returnPressed {false};
std::atomic<bool> errorRow{false};
std::atomic<bool> breakOut{false};

void checkInput() {
    while (true) {
        if (breakOut) {
            break;
        }

        if (std::cin.get() == '\n' && !errorRow) {
            returnPressed = true;
            if (returnPressed) {
                std::cout << "Return pressed, printing layer to filesystem" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // block the thread from hogging CPU usage
    }
}

/* thread stuff, after submission */

FakePrinter::FakePrinter(const std::string& printName,
                         const std::string& outputPath,
                         const PrintMode& printMode) : 
                         m_printName(printName), 
                         m_outputPath(outputPath),
                         m_printMode(printMode) {

}

void FakePrinter::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::string value;
        std::stringstream ss(line);

        while (getline(ss, value, ',')) {
            row.push_back(value);
        }

        m_csvData.push_back(row);
    }

    file.close();
}

void FakePrinter::print() {
    if (m_csvData.empty()) {
        std::cout << "Error: CSV data is empty/missing!" << std::endl;
        return;
    }

    switch (m_printMode) {
        case PrintMode::Automatic: {
            handleAutomaticPrint();
            break;
        }
        case PrintMode::Supervised: {
            handleSupervisedPrint();
            break;
        }
        default:
            std::cout << "Error: Unexpected PrintMode; how did we get here?" << std::endl;
        break;
    }
}

void FakePrinter::handleAutomaticPrint() {
    // no need to check for csv data, already handled in `print()`
    std::string filename = "\\output.csv";
    std::string fullpath = m_outputPath + filename;
    
    std::ofstream outfile(fullpath);

    if (!outfile.is_open()) {
        std::cout << "Error: opening file!" << std::endl;
        return;
    }

    // by row
    for (const auto& row : m_csvData) {
        // by element 
        for (size_t i = 0; i < row.size(); ++i) {
            outfile << row[i];
            if (i <= row.size() - 1) {
                outfile << ",";
        }
        }
        outfile << "\n";
    }
    outfile.close();
}

void FakePrinter::handleSupervisedPrint() {
    // no need to check for csv data, already handled in `print()`
    std::string filename = "\\output.csv";
    std::string fullpath = m_outputPath + filename;
    std::ofstream outfile(fullpath);

    if (!outfile.is_open()) {
        std::cout << "Error: opening file!" << std::endl;
        return;
    }

    std::thread inputThread(checkInput);

    // by row
    for (const auto& row : m_csvData) {
        // by element
        for (size_t i = 0; i < row.size(); ++i) {
            outfile << row[i];
            if (i <= row.size() - 1) {
                outfile << ",";
            }
        }
        outfile << "\n";

        if (returnPressed) {
            std::string singleLayerFile = "\\singleLayer.csv";
            std::ofstream outfile2(m_outputPath + singleLayerFile);
            if (!outfile2.is_open()) {
                std::cout << "Error: opening singleLayer file!" << std::endl;
            } else {
                for (size_t i = 0; i < row.size(); ++i) {
                    outfile2 << row[i];
                    if (i <= row.size() - 1) {
                        outfile2 << ",";
                    }
                }
                outfile2 << "\n";
            }
            returnPressed = false; // continue with the other output stream
        }

        // handle error case
        auto it = find_if(row.begin(), row.end(), [](const std::string& layerError) {
            return layerError.find("T") == 0; // find error starting with "T"
        });

        if (it != row.end()) {
            errorRow = true;
            std::cout << "Error: " << *it << std::endl;
            std::cout << "Would you like to end print? y/N" << std::endl;
            std::string endPrint;
            getline(std::cin, endPrint);

            if (endPrint == "y" || endPrint == "Y") {
                std::cout << "Ending print..." << std::endl;
                break;
            }
            errorRow = false;
        } 
    }
    
    breakOut = true;
    inputThread.join();
    outfile.close();
}

int main() {
    std::string name {};
    std::string folder {};
    std::string inputMode {};
    PrintMode mode = PrintMode::Automatic;

    std::cout << "Enter print name: ";
    getline(std::cin, name);

    std::cout << "Enter destination output folder path: ";
    getline(std::cin, folder);

    // I'm skipping error handling on name and path, but I would like to have some here.

    std::cout << "Enter desired print mode: ";
    getline(std::cin, inputMode);

    // need to convert input to enumClass
    if (modeMap.find(inputMode) == modeMap.end()) {
        std::cout << "Error: unexpected mode string provided, assuming automatic. Please use 'supervised' or 'automatic'" << std::endl;
    } else {
        mode = modeMap.at(inputMode);
    }

    std::unique_ptr<FakePrinter> pFakePrinter = std::make_unique<FakePrinter>(name, folder, mode);    
    // Fake printer is created, now we can pull in the data from csv (otherwise, if we were to fail above I'd return out of here skipping these instructions)
    pFakePrinter->loadFromFile("fl_coding_challenge_v1.csv");

    // once data is taken in, I want to call a print function, and then in that we can check for which mode
    // to determine our behavior and if we need to handle the keystroke input or not, etc.
    pFakePrinter->print();


    return 0;
}