#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSignalMapper>
#include <vector>
#include <map>
#include <Eigen/Dense>
#include "machinelearning.h"

namespace Ui {
class StartWidget;
}

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartWidget(QWidget *parent = nullptr, MachineLearning *ml = nullptr);
    ~StartWidget();

    enum class State {
        Row,
        Col
    };

private slots:
    void diagramClicked(int id);
    void colorButtonClicked(const QString &color);
    void onSubmitButtonClicked();

private:
    Ui::StartWidget *ui;
    QSignalMapper *signalMapper;
    State currentState;
    std::vector<std::vector<QPushButton*>> buttons; // 2D vector to store grid buttons
    std::vector<QPushButton*> highlightedButtons; // To store highlighted buttons
    std::map<QPushButton*, bool> colorHasBeenSet; // Tracks if a button's color has been set
    std::vector<QPushButton*> currentlySelectedButtons; // To store currently selected buttons
    MachineLearning *ml;

    int current_index_selected;
    std::vector<std::string> diagram;

    void toggleState();
    bool areAllColorButtonsDisabled() const;
    QString getButtonColor(QPushButton* button);
    std::string stateDecoding();
    void resetViewToDefault();
};

#endif // STARTWIDGET_H
