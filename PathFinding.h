#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PathFinding.h"
#include "RGrid.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "SearchAlgorithmList.h"
#include "qlcdnumber.h"


class PathFinding : public QMainWindow
{
    Q_OBJECT

public:
    PathFinding(QWidget *parent = Q_NULLPTR);
    void set_algorithm(std::shared_ptr<SearchAlgorithm> alg);
private:
    RGrid* r_grid;
    QWidget *central_widget;
    QPushButton *start,*reset;
    QVBoxLayout* main_layout;
    QMenuBar* menu_bar;
    QLCDNumber *explored, *visited;
    QMenu *file,*option,*about,*finding_method_menu;
    std::shared_ptr<SearchAlgorithm> search_alg;
    SearchAlgorithmList alg_list;
    Ui::PathFindingClass ui;
};
