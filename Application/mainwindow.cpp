#include "mainwindow.h"
#include "startwidget.h"
#include "DataGenerator.h"  // Include the DataGenerator header
#include "./ui_mainwindow.h"

#include <iostream>
#include <QFile>  // Include for QFile
#include <QDir>  // Include for QDir

#define DATASET_NAME "/diagrams.csv"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Generate the path to "diagrams.csv"
    QString datasetPath = QCoreApplication::applicationDirPath() + DATASET_NAME;
    // Convert QString to std::string for printing
    std::string datasetPathStd = datasetPath.toStdString();
    std::cout << datasetPathStd << std::endl;

    // Check if "diagram.csv" exists
    if (!QFile::exists(datasetPath)) {
        ui->startButton->hide();  // Hide the Start button
        ui->trainButton->hide();  // Hide the Train button
    }
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(ui->trainButton, &QPushButton::clicked, this, &MainWindow::onTrainButtonClicked);
    connect(ui->generate, &QPushButton::clicked, this, &MainWindow::onGenerateDataSetClicked);
    connect(ui->quitButton, &QPushButton::clicked, this, &MainWindow::onQuitButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onStartButtonClicked()
{
    StartWidget *newScreen = new StartWidget(this);
    setCentralWidget(newScreen);
}

void MainWindow::onTrainButtonClicked()
{
    std::cout << "Train Button Clicked" << std::endl;
}

// Slot implementation for generating data set
void MainWindow::onGenerateDataSetClicked() {
    int number_of_diagrams = 3000;
    std::cout << "BTW, you are only generating " << number_of_diagrams << " at a time" << std::endl;
    DataGenerator generator;
    // Example: Generate and save 10 diagrams
    for (int i = 0; i < number_of_diagrams; ++i) {
        generator.generateDiagram();
        generator.saveToCSV("diagrams.csv");
    }

    // Unhide the Start and Train buttons
    ui->startButton->show();
    ui->trainButton->show();
}

void MainWindow::onQuitButtonClicked()
{
    QApplication::quit();
}
