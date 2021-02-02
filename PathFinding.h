#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PathFinding.h"
#include "RGrid.h"
#include "qpushbutton.h"
#include "qboxlayout.h"


class PathFinding : public QMainWindow
{
    Q_OBJECT

public:
    PathFinding(QWidget *parent = Q_NULLPTR);

private:
    RGrid* r_grid;
    QWidget *central_widget;
    QPushButton *start,*reset;
    QVBoxLayout* main_layout;
    Ui::PathFindingClass ui;
};
