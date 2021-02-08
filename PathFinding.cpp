#include "PathFinding.h"
#include "qmessagebox.h"
#include "qdockwidget.h"
#include <chrono>

PathFinding::PathFinding(QWidget *parent)
    : QMainWindow(parent), central_widget(new QWidget(this))
{
    /*
    * Main Widget
    */
    r_grid=(new RGrid(central_widget));
    reset = new QPushButton(tr("Reset"), this);
    start=(new QPushButton(tr("Start"), central_widget));
    auto lcd_dock = new QDockWidget(this);
    auto lcd_widget = new QWidget(lcd_dock);
    explored = new QLCDNumber(lcd_dock);
    visited = new QLCDNumber(lcd_dock);
    explored->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
    visited->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

    QBoxLayout*button_layout = new QHBoxLayout(this),*lcd_layout=new QBoxLayout(QBoxLayout::Direction::LeftToRight,lcd_dock);
    //Button Layout
    button_layout->addWidget(start);
    button_layout->addWidget(reset);
    //LCD Numbers Layout
    lcd_layout->addWidget(visited);
    lcd_layout->addWidget(explored);
    //Main Widget Layout
    main_layout = new QVBoxLayout(this);
    main_layout->addWidget(r_grid);
    main_layout->addLayout(button_layout);
    central_widget->setLayout(main_layout);
    central_widget->setMouseTracking(true);
    lcd_widget->setLayout(lcd_layout);
    lcd_dock->setWidget(lcd_widget);
    addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, lcd_dock);
    setCentralWidget(central_widget);
    //setMouseTracking(true);
    //central_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    /*
    * MenuBar Initialization
    */
    menu_bar = new QMenuBar(this);
    about = new QMenu(tr("About"),menu_bar);
    option = new QMenu(tr("Options"), menu_bar);
    file = new QMenu(tr("File"),menu_bar);
    menu_bar->addMenu(file);
    menu_bar->addMenu(option);
    menu_bar->addMenu(about);

    /*
    * 1. File Menu
    */
    QAction *new_action = new QAction(tr("New")),
        *new_maze_action = new QAction(tr("New Maze")),
        *load_action= new QAction(tr("Load")),
        *quit_action = new QAction(tr("Quit"));
    file->addAction(new_action);
    file->addAction(new_maze_action);
    file->addAction(load_action);
    file->addSeparator();
    file->addAction(quit_action);

    /*
    * 2. Options Menu
    */
    finding_method_menu = new QMenu(tr("Path Finding Method"));
    option->addMenu(finding_method_menu);

    /*
    * Adding some Known Search Algorithms
    */
    alg_list.add_algorithm(tr(u8"A*: h=L1"), std::shared_ptr<SearchAlgorithm>(new ASharp));
    alg_list.add_algorithm(tr(u8"A*: h=L2"), std::shared_ptr<SearchAlgorithm>(new ASharp(metric::l2)));
    alg_list.add_algorithm(tr(u8"A*: h=L∞"), std::shared_ptr<SearchAlgorithm>(new ASharp(metric::linf)));

    alg_list.add_algorithm(tr(u8"Best Search First: h=L1"), std::shared_ptr<SearchAlgorithm>(new BestSearch));
    alg_list.add_algorithm(tr(u8"Best Search First: h=L2"), 
        std::shared_ptr<SearchAlgorithm>(new BestSearch(metric::l2)));
    alg_list.add_algorithm(tr(u8"Best Search First: h=L∞"), 
        std::shared_ptr<SearchAlgorithm>(new BestSearch(metric::linf)));

    alg_list.add_algorithm(tr(u8"Breadth Search First"), std::shared_ptr<SearchAlgorithm>(new BFS));
    alg_list.add_algorithm(tr(u8"Depth Search First"), std::shared_ptr<SearchAlgorithm>(new DFS));
    alg_list.add_algorithm(tr(u8"Dijkstra"), std::shared_ptr<SearchAlgorithm>(new Djikstra));
    
    const double theta1 = std::atan2(2, 1),theta2=std::atan2(4,3),
        theta3 = std::atan2(3, 2),theta4=std::atan2(5,1);
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(f+2h,L1)"), 
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta1,metric::l1)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(f+2h,L2)"), 
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta1,metric::l2)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(f+2h,L∞)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta1, metric::linf)));

    alg_list.add_algorithm(tr(u8"L*: (c,h)=(3f+4h,L1)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta2, metric::l1)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(3f+4h,L2)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta2, metric::l2)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(3f+4h,L∞)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta2, metric::linf)));

    alg_list.add_algorithm(tr(u8"L*: (c,h)=(3f+2h,L1)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta3, metric::l1)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(3f+2h,L2)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta3, metric::l2)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(3f+2h,L∞)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta3, metric::linf)));

    alg_list.add_algorithm(tr(u8"L*: (c,h)=(1f+5h,L1)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta4, metric::l1)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(1f+5h,L2)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta4, metric::l2)));
    alg_list.add_algorithm(tr(u8"L*: (c,h)=(1f+5h,L∞)"),
        std::shared_ptr<SearchAlgorithm>(new LSearch(theta4, metric::linf)));

    /*
    * Adding Actions of those Algorithms to the submenu
    */
    finding_method_menu->addActions(alg_list.get_action_group()->actions());

    /*
    * A* is the Default Pathfinding  Algorithm
    */
    search_alg = std::dynamic_pointer_cast<SearchAlgorithm>(std::make_shared<ASharp>());
    alg_list.get_action_group()->actions().first()->setChecked(true);

    /*
    * Adding time delay options
    */
    QMenu* time_delay = new QMenu("Time Delay");
    QAction* time_delay_5 = new QAction("5ms"),
        * time_delay_10 = new QAction("10ms"),
        * time_delay_20 = new QAction("20ms"),
        * time_delay_50 = new QAction("50ms");
    time_delay_5->setCheckable(true);
    time_delay_10->setCheckable(true);
    time_delay_20->setCheckable(true);
    time_delay_50->setCheckable(true);
    time_delay_20->setChecked(true);
    QActionGroup* time_group = new QActionGroup(this);
    time_group->setExclusive(true);
    option->addMenu(time_delay);
    time_group->addAction(time_delay_5);
    time_group->addAction(time_delay_10);
    time_group->addAction(time_delay_20);
    time_group->addAction(time_delay_50);
    time_delay->addActions(time_group->actions());

    /*
    * 3. About Menu
    */
    QAction* about_me_action = new QAction(tr("About me...")),
        *about_qt_action = new QAction(tr("About Qt"));
    about->addAction(about_me_action);
    about->addAction(about_qt_action);
    setMenuBar(menu_bar);

    /*
    * Qt MetaObjects Connections configuration
    */
    //Plays the simulation
    connect(start, &QPushButton::clicked, r_grid, [this]() 
        {
            r_grid->explore_grid_interactive(search_alg.get());
        });
    /*
    * Reset the RGrid and the counters
    */
    connect(reset, &QPushButton::clicked, r_grid,&RGrid::reset);
    connect(reset, &QPushButton::clicked, explored, [this] {explored->display(0); });
    connect(reset, &QPushButton::clicked, visited, [this] {visited->display(0); });

    //Quit the application
    connect(quit_action, &QAction::triggered , qApp, &QApplication::quit);

    //Shows About me message
    connect(about_me_action, &QAction::triggered, [this]
        {
            std::unique_ptr<QMessageBox> about_me_box(new QMessageBox(this));
            about_me_box->setWindowTitle(tr("About Me"));
            about_me_box->setText(tr(R"(Path Finding Simulation
Made by: <strong>Rami Zouari</strong><br>
I am a developper currently studying Computer Science at INSAT,Tunisia.<br>
I am an AI enthusiast with some mathematical background who loves to learn new things, the topics that I occasionally work on are:<br>
<ol>
<li> Machine Learning </li>
<li> Advanced Algorithms</li>
<li> Scientific Programming</li>
<li> GUI Programming</li>
<li> Sometimes just Mathematical Problems</li>
</ol>
Git: <a href="https://www.github.com/ramizouari">Rami Zouari </a>)"));
            about_me_box->setTextFormat(Qt::TextFormat::RichText);
            about_me_box->exec();
        });

    //Shows About Qt message
    connect(about_qt_action, &QAction::triggered, &QApplication::aboutQt);

    //Update Counters
    connect(r_grid, &RGrid::explored, explored, [this]
        {
            explored->display(explored->value() + 1);
        });
    connect(r_grid, &RGrid::visited, visited, [this]
        {
            visited->display(visited->value() + 1);
        });

    //Change the pathfinding algorithm
    connect(alg_list.get_action_group(), &QActionGroup::triggered, this, [this](QAction *action)
        {
            search_alg=(alg_list.get_algorithm(action->text()));
        });

    connect(new_action, &QAction::triggered, r_grid, &RGrid::initialize_empty);
    connect(new_maze_action, &QAction::triggered, r_grid, &RGrid::generate_maze);

    using namespace std::chrono_literals;
    connect(time_delay_5, &QAction::triggered, [] {SearchAlgorithm::delay = 5ms; });
    connect(time_delay_10, &QAction::triggered, [] {SearchAlgorithm::delay = 10ms; });
    connect(time_delay_20, &QAction::triggered, [] {SearchAlgorithm::delay = 20ms; });
    connect(time_delay_50, &QAction::triggered, [] {SearchAlgorithm::delay = 50ms; });

    //ui.setupUi(this);
}

void PathFinding::set_algorithm(std::shared_ptr<SearchAlgorithm> alg)
{
    search_alg.swap(alg);
}
