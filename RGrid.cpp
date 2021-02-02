#include "RGrid.h"
#include <vector>
#include <queue>
#include <tuple>
#include <thread>
#include <chrono>

RGrid::RGrid(QWidget* parent):QWidget(parent),grid(new QGridLayout),
elements(new RFrame*[n])
{
	for (int i = 0; i < n; i++)
		elements[i]=new RFrame[m];
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
		});
	connect(dynamic_cast<GridThread*>(thread), &GridThread::to_set_optimal, this, [this](int a, int b)
		{
			elements[a][b].optimal();
		});
	setMouseTracking(true);
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
