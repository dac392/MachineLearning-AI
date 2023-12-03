#include "startwidget.h"
#include "ui_startwidget.h"

#include <QMessageBox>
#include <iostream>
#include <random>

StartWidget::StartWidget(QWidget *parent, MachineLearning* ml) :
    QWidget(parent),
    ui(new Ui::StartWidget),
    buttons(20, std::vector<QPushButton*>(20, nullptr)),
    ml(ml)
{
    ui->setupUi(this);

    // Initialize the grid buttons and set up signal mapper
    signalMapper = new QSignalMapper(this);
    for(int i = 0; i < 20; i++) {
        for(int j = 0; j < 20; j++) {
            QPushButton *button = new QPushButton(this);
            ui->gridLayout->addWidget(button, i, j);
            buttons[i][j] = button;
            connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
            signalMapper->setMapping(button, i * 20 + j);
        }
    }
    connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mappedInt),
            this, &StartWidget::diagramClicked);

    // Connect color buttons
    connect(ui->redButton, &QPushButton::clicked, [this]{ colorButtonClicked("red"); });
    connect(ui->blueButton, &QPushButton::clicked, [this]{ colorButtonClicked("blue"); });
    connect(ui->greenButton, &QPushButton::clicked, [this]{ colorButtonClicked("green"); });
    connect(ui->yellowButton, &QPushButton::clicked, [this]{ colorButtonClicked("yellow"); });

    // Connect submit Button
    connect(ui->submitButton, &QPushButton::clicked, this, &StartWidget::onSubmitButtonClicked);
    // Initialize colorHasBeenSet map for each button
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            colorHasBeenSet[buttons[i][j]] = false;
        }
    }

    // Randomly set the initial state
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 1);
    currentState = distrib(gen) ? State::Row : State::Col;
}

void StartWidget::diagramClicked(int id) {
    // Clear gray highlighting from currently selected buttons
    for (QPushButton* btn : currentlySelectedButtons) {
        if (!colorHasBeenSet[btn]) {
            btn->setStyleSheet(""); // Reset to default style only if not permanently set
        }
    }

    currentlySelectedButtons.clear();


    int row = id / 20;
    int col = id % 20;
    current_index_selected = (currentState == State::Row)? row : col;


    // Highlight new selection based on current state
    if (currentState == State::Row) {
        for (int j = 0; j < 20; j++) {
            QPushButton* btn = buttons[row][j];
            if (!colorHasBeenSet[btn]) {
                btn->setStyleSheet("background-color: gray");
            }
            currentlySelectedButtons.push_back(btn);
        }
    } else {
        for (int i = 0; i < 20; i++) {
            QPushButton* btn = buttons[i][col];
            if (!colorHasBeenSet[btn]) {
                btn->setStyleSheet("background-color: gray");
            }
            currentlySelectedButtons.push_back(btn);
        }
    }
}


void StartWidget::colorButtonClicked(const QString &color) {
    // Check if there are any buttons currently selected
    if (currentlySelectedButtons.empty()) {
        return; // Do nothing if no buttons are currently selected
    }

    QString styleSheet = QString("background-color: %1").arg(color);

    for (QPushButton* btn : currentlySelectedButtons) {
        btn->setStyleSheet(styleSheet);
        colorHasBeenSet[btn] = true; // Mark this button as having its color permanently set
        btn->disconnect(); // Prevent further changes
        highlightedButtons.push_back(btn); // Add to permanently colored list
    }

    currentlySelectedButtons.clear(); // Clear the currently selected buttons list

    // Now toggle the state as a color has been selected
    toggleState();

    // Disable and gray out the clicked color button
    QPushButton* buttonToDisable = nullptr;
    std::string colorName;

    if (color == "red") {
        buttonToDisable = ui->redButton;
        colorName = "Red";
    } else if (color == "blue") {
        buttonToDisable = ui->blueButton;
        colorName = "Blue";
    } else if (color == "green") {
        buttonToDisable = ui->greenButton;
        colorName = "Green";
    } else if (color == "yellow") {
        buttonToDisable = ui->yellowButton;
        colorName = "Yellow";
    }

    if (buttonToDisable) {
        buttonToDisable->setEnabled(false);
        buttonToDisable->setStyleSheet("background-color: gray");

        std::string entry = stateDecoding() + " " + std::to_string(current_index_selected) + " " + colorName;
        diagram.push_back(entry);
    }
}

std::string StartWidget::stateDecoding(){
    // flipped because
    if(currentState == State::Row)
        return "Column";

    return "Row";
}

void StartWidget::onSubmitButtonClicked() {
    std::string csvString;

    // Check if the diagram is not empty
    if (!diagram.empty()) {
        // Iterate through all but the last element to add a comma after each
        for (size_t i = 0; i < diagram.size() - 1; ++i) {
            csvString += diagram[i] + ",";
        }
        // Add the last element without a comma at the end
        csvString += diagram.back();
    }

    // send this string to ml to decode into a flattened array
    // predict the flattened array
    // make a dialougue  with the prediction results
}


QString StartWidget::getButtonColor(QPushButton* button) {
    if (!button) {
        return QString(); // Return empty if button is null
    }

    QString style = button->styleSheet();

    // Check for different colors based on style sheet properties
    if (style.contains("background-color: red", Qt::CaseInsensitive)) {
        return "Red";
    } else if (style.contains("background-color: blue", Qt::CaseInsensitive)) {
        return "Blue";
    } else if (style.contains("background-color: green", Qt::CaseInsensitive)) {
        return "Green";
    } else if (style.contains("background-color: yellow", Qt::CaseInsensitive)) {
        return "Yellow";
    }

    return QString(); // Return empty string for no color or unrecognized color
}






StartWidget::~StartWidget()
{
    delete ui;
}

void StartWidget::toggleState() {
    if (currentState == State::Row) {
        currentState = State::Col;
    } else {
        currentState = State::Row;
    }
}

bool StartWidget::areAllColorButtonsDisabled() const {
    return !ui->redButton->isEnabled() &&
           !ui->blueButton->isEnabled() &&
           !ui->greenButton->isEnabled() &&
           !ui->yellowButton->isEnabled();
}
