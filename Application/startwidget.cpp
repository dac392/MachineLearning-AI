#include "startwidget.h"
#include "ui_startwidget.h"

#include <iostream>
#include <random>

StartWidget::StartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartWidget),
    buttons(20, std::vector<QPushButton*>(20, nullptr))
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
    if(color == "red") {
        ui->redButton->setEnabled(false);
        ui->redButton->setStyleSheet("background-color: gray");
    } else if(color == "blue") {
        ui->blueButton->setEnabled(false);
        ui->blueButton->setStyleSheet("background-color: gray");
    } else if(color == "green") {
        ui->greenButton->setEnabled(false);
        ui->greenButton->setStyleSheet("background-color: gray");
    } else if(color == "yellow") {
        ui->yellowButton->setEnabled(false);
        ui->yellowButton->setStyleSheet("background-color: gray");
    }
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
