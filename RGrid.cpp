#include "RGrid.h"
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <thread>
#include <chrono>
#include <random>

RGrid::RGrid(QWidget* parent) :QWidget(parent), grid(new QGridLayout),
elements(new RFrame* [n]), source({ 0,0 }), dest({ n - 1,m - 1 })
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
	elements[0][0].set_as_source();
	elements[n - 1][m - 1].set_as_destination();
	setMouseTracking(true);
	//generate_maze();
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
	using couple = std::pair<int, int>;
	static std::random_device device;
	static auto completely_blocked = [](const std::vector<std::vector<bool>> &B,couple w) 
	{
		int r;
		for(int i=0;i<=1;i++)
			for (int j = 0; j <= 1; j++)
			{
				r = 0;
				for (int a = std::max(w.first - i, 0); a < std::min(w.first - i + 2, n); a++)
					for (int b = std::max(w.second - j, 0); b < std::min(w.second - j + 2, m); b++)
						if (B[a][b])
							r++;
				if (r >= 2)
					return true;
			}
		return false;
	};
	std::mt19937_64 e(device());
	std::vector<std::vector<bool>> blocked(n, std::vector<bool>(m, false));
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	std::stack<couple> T;
	std::uniform_int_distribution d1(0,n-1),d2(0,m-1);
	couple p = { d1(e),d2(e) };
visited[p.first][p.second] = true;
	T.push(p);
	while (!T.empty())
	{
		auto [a,b] = T.top();
		if (!completely_blocked(blocked, T.top()))
		{
			blocked[a][b] = true;
			elements[a][b].set_as_blocked();
		}
		
		T.pop();
		std::vector<couple> A;
		for (int i = std::max(a-1,0); i < std::min(a+2,n);i++)
			for (int j = std::max(b - 1, 0); j < std::min(b + 2, m); j++)
			{
				if (i == a && j == b || i!=a && j!=b)
					continue;
				if (visited[i][j])
					continue;
				if (completely_blocked(blocked, { i,j }))
					continue;
				visited[i][j] = true;
				A.push_back({ i,j });
			}
		std::shuffle(A.begin(), A.end(),e);
		for (const auto& s : A)
			T.push(s);
	}
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
