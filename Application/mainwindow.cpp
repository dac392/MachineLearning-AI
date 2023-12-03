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
    , ml((QCoreApplication::applicationDirPath()+DATASET_NAME).toStdString())
{
    ui->setupUi(this);

    // Generate the path to "diagrams.csv"
    QString datasetPath = QCoreApplication::applicationDirPath() + DATASET_NAME;

    // Check if "diagram.csv" exists
    if (!QFile::exists(datasetPath)) {
        ui->startButton->hide();  // Hide the Start button
        ui->trainButton->hide();  // Hide the Train button
    }else{
        ml.loadDataset();
        std::cout << "loaded dataset" << std::endl;
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
    StartWidget *newScreen = new StartWidget(this, &ml);
    setCentralWidget(newScreen);
}

void MainWindow::onTrainButtonClicked() {
    QProgressDialog progressDialog("Training in progress...", "Cancel", 0, 100, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();

    // Call the updated training method with the progress dialog
    ml.train(progressDialog);

    // Test the model
    ml.test();

    // Hide button from view
    ui->trainButton->hide();  // Hide the Train button
}


// Slot implementation for generating data set
void MainWindow::onGenerateDataSetClicked() {
    int number_of_diagrams = 10000;

    DataGenerator generator;
    // Example: Generate and save 10 diagrams
    for (int i = 0; i < number_of_diagrams; ++i) {
        generator.generateDiagram();
        generator.saveToCSV("diagrams.csv");
    }

    // Unhide the Start and Train buttons
    ui->startButton->show();
    ui->trainButton->show();
    ml.loadDataset();
    std::cout << "loaded dataset" << std::endl;
    std::cout << "BTW, you are only generating " << number_of_diagrams << " at a time" << std::endl;
}

void MainWindow::onQuitButtonClicked()
{
    QApplication::quit();
}
