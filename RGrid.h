#pragma once
#include <qwidget.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qpointer.h>
#include <qthread.h>
#include <unordered_map>
#include "RFrame.h"
#include "SearchAlgorithm.h"
//Number of rows
inline constexpr int n = 41;
//Number of columns
inline constexpr int m = 41;
class RGrid :
    public QWidget
{
    
    Q_OBJECT
public:
    RGrid(QWidget* parent=nullptr);
    //Checks whether a square with coordinates (a,b) is blocked
    bool is_blocked(int a, int b);
    //Get the Thread in which the simulation plays
    QThread* getThread();
    //Get the coordinates of the source
    std::pair<int, int> get_source() const;
    //Get the cooridnates of the destination
    std::pair<int, int> get_dest() const;
    void set_source(RFrame *e);
    void set_destination(RFrame *e);
public slots:
    void explore_grid();
    void explore_grid2();
    void explore_grid_interactive();
    void explore_grid_interactive(SearchAlgorithm *S);
    void explore_grid(SearchAlgorithm *S);
    //Reset the grid
    void reset();
    //Change the source
    void set_source(int, int);
    //Change the destination
    void set_destination(int, int);
    void initialize_empty();
    void generate_maze();
signals:
    //Emitted when a square is explored
    void explored();
    //Emitted when a square is visited
    void visited();
private:
    //The layout of the elements
    QGridLayout* grid;
    //The Squares
    RFrame** elements;
    /*
    * - source: represents the cooridnates of the source
    * - dest: represents the coordinates of the destination
    */
    std::pair<int, int> source, dest;
    /*
    * The thread in which the simulation will play
    */
    QThread *thread;
    //This Mapper will map each frame to its coordinates
    std::unordered_map<RFrame*, std::pair<int, int>> element_mapper;

};

class GridThread :public QThread
{
	RGrid* grid;
    SearchAlgorithm *searchAlgo;
    Q_OBJECT
public:
	GridThread(RGrid* S, QObject* parent = nullptr) :QThread(parent), grid(S),searchAlgo(nullptr) {}
    void start(SearchAlgorithm* S=nullptr)
    {
        searchAlgo = S;
        bool default = S == nullptr;
        if (default)
        {
            std::unique_ptr<SearchAlgorithm> ref_algo(new ASharp);
            searchAlgo = ref_algo.get();
        }
        QThread::start();
    }

	void run() override
	{
		grid->explore_grid(searchAlgo);
	}
signals:
	void to_explore(int a, int b);
    void to_set_optimal(int a, int b);
    void to_visit(int a, int b);
};