#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <set>
#include <random>
#include <string>
#include <tuple>
#include <vector>

class DataGenerator {
public:
    DataGenerator();
    std::string generateDiagram();
    void saveToCSV(const std::string& filename);

private:
    enum class Color {
        Red, Blue, Yellow, Green
    };

    struct WireInfo {
        std::string orientation; // "Row" or "Column"
        int position; // 1-20
        std::string color; // "Red", "Blue", "Yellow", "Green"
    };

    std::random_device rd;
    std::mt19937 gen;
    std::vector<WireInfo> wireSequence;
    std::string status;

    int randomInRange(int start, int end);
    Color getRandomColor(std::set<Color>& excludedColors);
    std::string colorToString(Color color);
    void colorRowOrColumn(bool isRow, Color& selectedColor, int& selectedPosition, std::set<Color>& usedColors, Color diagram[20][20]);
};

#endif // DATAGENERATOR_H
