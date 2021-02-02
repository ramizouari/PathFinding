#include "PathFinding.h"

PathFinding::PathFinding(QWidget *parent)
    : QMainWindow(parent), central_widget(new QWidget(this))
{
    r_grid=(new RGrid(central_widget));
    reset = new QPushButton("Reset", this);
    start=(new QPushButton("Start", central_widget));
    auto button_layout = new QHBoxLayout(this);
    button_layout->addWidget(start);
    button_layout->addWidget(reset);
    main_layout = new QVBoxLayout(this);
    main_layout->addLayout(button_layout);
    main_layout->addWidget(r_grid);
    central_widget->setLayout(main_layout);
    central_widget->setMouseTracking(true);
    //setMouseTracking(true);
    //central_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    connect(start, &QPushButton::clicked, r_grid, [this]() 
        {
            static std::unique_ptr<SearchAlgorithm> alg(new LSearch(std::atan2(5,4),metric::l1));
            r_grid->explore_grid_interactive(alg.get());
        });
    connect(reset, &QPushButton::clicked, r_grid,&RGrid::reset);
    setCentralWidget(central_widget);
    //ui.setupUi(this);
}
