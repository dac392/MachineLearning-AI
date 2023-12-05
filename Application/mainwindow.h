#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSoundEffect>
#include "machinelearning.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartButtonClicked();
    void onTrainButtonClicked();
    void onGenerateDataSetClicked();  // Slot for handling the button click
    void onQuitButtonClicked();

private:
    Ui::MainWindow *ui;
    QSoundEffect *sound;
    MachineLearning ml;
};
#endif // MAINWINDOW_H
