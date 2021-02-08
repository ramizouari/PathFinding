#include "RGrid.h"
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <thread>
#include <chrono>
#include <random>
#include "Graph.h"

RGrid::RGrid(QWidget* parent) :QWidget(parent), grid(new QGridLayout),
elements(new RFrame* [n])
{
	for (int i = 0; i < n; i++)
	{
		elements[i] = new RFrame[m];
		std::for_each_n(elements[i], m, [this](RFrame& s) {s.setParent(this); });
		for (int j = 0; j < m; j++)
			element_mapper[elements[i] + j] = { i,j };
		
	}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
		{
			elements[i][j].setParent(this);
			grid->addWidget(elements[i] + j,i,j);
			elements[i][j].setAutoFillBackground(true);
			elements[i][j].setFrameShape(QFrame::Shape::Panel);
		}
	grid->setSpacing(0);
	setLayout(grid);
	thread = new GridThread(this,this);
	connect(dynamic_cast<GridThread*>(thread), &GridThread::to_explore,this, [this](int a, int b)
		{
			elements[a][b].explore();
			emit explored();
		});
	connect(dynamic_cast<GridThread*>(thread), &GridThread::to_set_optimal, this, [this](int a, int b)
		{
			elements[a][b].optimal();
		});
	connect(dynamic_cast<GridThread*>(thread), &GridThread::to_visit, this, [this](int a, int b)
		{
			elements[a][b].visit();
			emit visited();
		});
	setMouseTracking(true);
	generate_maze();
	setMinimumSize({ 300,300 });
	setSizeIncrement({ 1,1 });
}

void RGrid::explore_grid()
{
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	std::vector<std::vector<bool>> processed(n, std::vector<bool>(m, false));
	visited[0][0] = true;
	using sextuple = std::tuple<int, int, int, int,int,int >;
	std::priority_queue<sextuple,std::vector<sextuple>,std::greater<void>> Q;
	using couple = std::pair<int, int>;
	std::vector<std::vector<couple>> parent(n, std::vector<couple>(m, { 0,0 }));
	Q.push({0,0,0,0,0,0});
	using namespace  std::chrono_literals;
	auto grid_thread = dynamic_cast<GridThread*>(thread);
	while (!Q.empty())
	{
		auto w = Q.top();
		Q.pop();
		auto &[total_cost,distance, i, j,a,b] = w;
		if (processed[i][j])
			continue;
		parent[i][j] = { a,b };
		emit grid_thread->to_explore(i,j);
		if (i==n-1 && j == m-1)
			break;
		
		processed[i][j] = true;
		for(int p=std::max(i-1,0);p<std::min(i+2,n);p++)
			for (int q = std::max(j - 1, 0); q < std::min(j + 2, m); q++)
			{
				if (i != p && q != j)
					continue;
				if (p == i && q == j)
					continue;
				if (elements[p][q].blocked())
					continue;
				Q.push({distance+n+m-p-q,distance+1,p,q,i,j});
			}
		std::this_thread::sleep_for(50ms);
	}
	for (std::pair<int, int>w = { n - 1,m - 1 }; w != parent[w.first][w.second]; w = parent[w.first][w.second])
		emit grid_thread->to_set_optimal(w.first, w.second);
	emit grid_thread->to_set_optimal(0, 0);
}

bool RGrid::is_blocked(int a, int b)
{
	return elements[a][b].blocked();
}

QThread* RGrid::getThread()
{
	return thread;
}

std::pair<int, int> RGrid::get_source() const
{
	return source;
}

std::pair<int, int> RGrid::get_dest() const
{
	return dest;
}

void RGrid::generate_maze()
{
	GridGraph G((n+1) / 2, (m+1) / 2);
	auto B = G.kruskal().convert();
	auto a= B.size(), b =  B[0].size() ;
	for (int i = 0; i < a; i++)
		for (int j = 0; j < b; j++)
			if (!B[i][j])
				elements[i][j].set_as_blocked();
			else elements[i][j].set_as_normal();
	if (n % 2 == 0) for (int j = 0; j < m; j++)
		if (elements[n - 2][j].blocked()) elements[n - 1][j].set_as_blocked();
	if (m % 2 == 0) for (int i = 0; i < n; i++)
		if (elements[i][m-2].blocked()) elements[i][m-1].set_as_blocked();
	elements[0][0].set_as_source();
	elements[n - 1][m - 1].set_as_destination();
	source = { 0,0 }; 
	dest = { n - 1,m - 1 };
}

void RGrid::set_source(RFrame* e)
{
	auto S = element_mapper[e];
	if (S == dest || e->blocked())
		return;
	elements[source.first][source.second].set_as_normal();
	e->set_as_source();
	source = S;
}

void RGrid::set_destination(RFrame* e)
{
	auto S = element_mapper[e];
	if (S == source || e->blocked())
		return;
	elements[dest.first][dest.second].set_as_normal();
	dest = S;
	e->set_as_destination();
}

void RGrid::explore_grid2()
{
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	std::vector<std::vector<bool>> processed(n, std::vector<bool>(m, false));
	visited[0][0] = true;
	using sextuple = std::tuple<int, int, int,int , int, int > ;
	std::queue<sextuple> Q;
	Q.push({ 0,0,0,0,0,0 });
	using namespace  std::chrono_literals;

	while (!Q.empty())
	{
		auto w = Q.front();
		Q.pop();
		auto& [total_cost, distance,a,b, i, j] = w;
		if (processed[i][j])
			continue;
		emit dynamic_cast<GridThread*>(thread)->to_explore(i, j);
		if (i == n - 1 && j == m - 1)
			break;

		processed[i][j] = true;
		for (int p = std::max(i - 1, 0); p < std::min(i + 2, n); p++)
			for (int q = std::max(j - 1, 0); q < std::min(j + 2, m); q++)
			{
				if (p == i && q == j)
					continue;
				if (elements[p][q].blocked())
					continue;
				if (visited[p][q])
					continue;
				Q.push(std::make_tuple(distance -1 + n + m - p - q,distance + 1,i,j,p,q));
			}
		std::this_thread::sleep_for(50ms);
	}
	auto w = Q.front();
}

void RGrid::explore_grid_interactive()
{		
	thread->start();
}

void RGrid::explore_grid_interactive(SearchAlgorithm* S)
{
	dynamic_cast<GridThread*>(thread)->start(S);
}

void RGrid::explore_grid(SearchAlgorithm*S)
{
	S->search(*this);
}

void RGrid::reset()
{
	thread->terminate();
	while (thread->isRunning());
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			elements[i][j].reset();
}

void RGrid::set_source(int a, int b)
{
	source = { a,b };
}

void RGrid::set_destination(int a, int b)
{
	dest = { a,b };
}

void RGrid::initialize_empty()
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			elements[i][j].set_as_normal();
	elements[0][0].set_as_source();
	elements[n - 1][m - 1].set_as_destination();
	source = { 0,0 };
	dest = { n - 1,m - 1 };
}
