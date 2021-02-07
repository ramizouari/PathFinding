#pragma once
#include <qwidget.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qpointer.h>
#include <qthread.h>
#include <unordered_map>
#include "RFrame.h"
#include "SearchAlgorithm.h"
inline constexpr int n = 40;
inline constexpr int m = 40;
class RGrid :
    public QWidget
{
    
    Q_OBJECT
public:
    RGrid(QWidget* parent=nullptr);
    bool is_blocked(int a, int b);
    QThread* getThread();
    std::pair<int, int> get_source() const;
    std::pair<int, int> get_dest() const;
    void set_source(RFrame *e);
    void set_destination(RFrame *e);
public slots:
    void explore_grid();
    void explore_grid2();
    void explore_grid_interactive();
    void explore_grid_interactive(SearchAlgorithm *S);
    void explore_grid(SearchAlgorithm *S);
    void reset();
    void set_source(int, int);
    void set_destination(int, int);
signals:
    void explored();
    void visited();
private:
    void generate_maze();
    QGridLayout* grid;
    RFrame** elements;
    std::pair<int, int> source, dest;
    QThread *thread;
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