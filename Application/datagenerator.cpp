#include "DataGenerator.h"
#include <algorithm>
#include <fstream>

DataGenerator::DataGenerator() : gen(rd()) {}

int DataGenerator::randomInRange(int start, int end) {
    std::uniform_int_distribution<> dis(start, end);
    return dis(gen);
}

DataGenerator::Color DataGenerator::getRandomColor(std::set<Color>& excludedColors) {
    std::vector<Color> colors = {Color::Red, Color::Blue, Color::Yellow, Color::Green};
    colors.erase(std::remove_if(colors.begin(), colors.end(),
                                [&excludedColors](const Color& color) { return excludedColors.count(color) > 0; }),
                 colors.end());
    return colors[randomInRange(0, colors.size() - 1)];
}

std::string DataGenerator::colorToString(Color color) {
    switch(color) {
    case Color::Red: return "Red";
    case Color::Blue: return "Blue";
    case Color::Yellow: return "Yellow";
    case Color::Green: return "Green";
    default: return "Unknown";
    }
}

std::string DataGenerator::generateDiagram() {
    wireSequence.clear();
    Color diagram[20][20] = {}; // 2D array to represent the diagram

    bool startWithRow = randomInRange(0, 1); // 0 for row, 1 for column
    std::set<Color> usedColors;
    int selectedPosition;
    Color selectedColor;

    // Generate wire placements
    for (int i = 0; i < 4; ++i) {
        bool isRow = (i % 2 == 0) ? startWithRow : !startWithRow;
        colorRowOrColumn(isRow, selectedColor, selectedPosition, usedColors, diagram);
    }

    // Determine 'Dangerous' or 'Safe'
    bool redBeforeYellow = false;
    for (const auto& wire : wireSequence) {
        if (wire.color == "Red") {
            redBeforeYellow = true;
        } else if (wire.color == "Yellow" && redBeforeYellow) {
            status = "Dangerous";
            return status;
        }
    }
    status = "Safe";
    return status;
}



void DataGenerator::colorRowOrColumn(bool isRow, Color& selectedColor, int& selectedPosition, std::set<Color>& usedColors, Color diagram[20][20]) {
    selectedPosition = randomInRange(1, 20) - 1;
    selectedColor = getRandomColor(usedColors);
    usedColors.insert(selectedColor);

    if (isRow) {
        // Color the row
        for (int i = 0; i < 20; ++i) {
            diagram[selectedPosition][i] = selectedColor;
        }
        wireSequence.push_back({"Row", selectedPosition + 1, colorToString(selectedColor)});
    } else {
        // Color the column
        for (int i = 0; i < 20; ++i) {
            diagram[i][selectedPosition] = selectedColor;
        }
        wireSequence.push_back({"Column", selectedPosition + 1, colorToString(selectedColor)});
    }
}

void DataGenerator::saveToCSV(const std::string& filename) {
    std::ofstream file(filename, std::ios::app); // Open in append mode
    if (file.is_open()) {
        for (const auto& wire : wireSequence) {
            file << wire.orientation << " " << wire.position << " " << wire.color;
            if (&wire != &wireSequence.back()) {
                file << ",";  // Add comma separator except after the last item
            }
        }
        file << ", " << status << "\n";  // Add status and end the line
    }
}


