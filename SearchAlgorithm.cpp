#include "SearchAlgorithm.h"
#include <vector>
#include <algorithm>
#include <queue>
#include <chrono>
#include <stack>
#include "RGrid.h"
constexpr double tol = 1e-5;
using septuplet = std::tuple<double, double, double, int, int, int, int >;
bool gt_function(const septuplet& U, const septuplet& V)
{
	const auto& [a1, b1, c1, d1, e1, f1, g1] = U;
	const auto &[a2, b2, c2, d2, e2, f2, g2] = V;
	if (std::abs(a1 - a2) > tol)
		return a1>a2+tol;
	else if (std::abs(b1 -b2) > + tol)
		return b1>b2+tol;
	else if (std::abs(c1 - c2) > tol)
		return c1>c2+tol;
	else return std::tuple<int,int,int,int>({ d1,e1,f1,g1 }) > std::tuple<int,int,int,int>({d2, e2, f2, g2});
}

struct gt
{
	bool operator()(const septuplet& U, const septuplet& V) const
	{
		return gt_function(U, V);
	}
};

LSearch::LSearch(double _angle) :theta(_angle), heuristic(metric::l1)
{
}

LSearch::LSearch(double _angle, std::function<double(couple, couple)> h):theta(_angle),heuristic(h)
{
}

void LSearch::search(RGrid& G) const
{
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	std::vector<std::vector<bool>> processed(n, std::vector<bool>(m, false));
	auto [x1, y1] = G.get_source();
	auto [x2, y2] = G.get_dest();
	visited[x1][y1] = true;
	/*
	* This septuplet consists of (c,h,d,x,y,u,v) where
	* - c = cos(theta)*h+sin(theta)*d is the cost
	* - h is the value of the heuristic function
	* - d is the distance between the vertex s(x,y) and the vertex s0=(0,0)
	* - x the x-coordinate of the vertex s
	* - y is the y-coordinate of the vertex s 
	* - u is the x-coordinate of the vertex r parent of s
	* - v is the y-coordinate of the vertex r parent of s
	*/
	std::priority_queue<septuplet, std::vector<septuplet>, gt> Q;
	using couple = std::pair<int, int>;
	std::vector<std::vector<couple>> parent(n, std::vector<couple>(m, { x1,y1 }));
	Q.push({ 0,0,0,x1,y1,x1,y1 });
	using namespace  std::chrono_literals;
	auto grid_thread = dynamic_cast<GridThread*>(G.getThread());
	auto cos_theta = std::cos(theta),sin_theta=std::sin(theta);
	while (!Q.empty())
	{
		auto w = Q.top();
		Q.pop();
		auto& [total_cost,h, distance, i, j, a, b] = w;
		if (processed[i][j])
			continue;
		parent[i][j] = { a,b };
		emit grid_thread->to_explore(i, j);
		if (i == x2 && j == y2)
			break;

		processed[i][j] = true;
		for (int p = std::max(i - 1, 0); p < std::min(i + 2, n); p++)
			for (int q = std::max(j - 1, 0); q < std::min(j + 2, m); q++)
			{
				if (i != p && q != j && true)
					continue;
				if (p == i && q == j)
					continue;
				if (G.is_blocked(p, q))
					continue;
				if (!visited[p][q])
					emit grid_thread->to_visit(p,q);
				visited[p][q] = true;

				auto h_r = heuristic({ x2,y2 }, { p,q });
				Q.push({ cos_theta * (distance + 1) + sin_theta * h_r 
					,h_r,distance + 1,p,q,i,j });
			}
		std::this_thread::sleep_for(delay);
	}
	for (std::pair<int, int>w = { x2,y2 }; w != parent[w.first][w.second]; w = parent[w.first][w.second])
		emit grid_thread->to_set_optimal(w.first, w.second);
	emit grid_thread->to_set_optimal(x1, y1);
}

BestSearch::BestSearch():LSearch(std::asin(1))
{
}

BestSearch::BestSearch(std::function<double(couple, couple)> h): LSearch(std::asin(1), h)
{
}

Djikstra::Djikstra():LSearch(0)
{
}

ASharp::ASharp():LSearch(std::atan2(1,1))
{
}

ASharp::ASharp(std::function<double(couple, couple)> h):LSearch(std::atan2(1,1),h)
{
}

SearchAlgorithm::~SearchAlgorithm()
{
}

void BFS::search(RGrid& G) const
{
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	auto [x1, y1] = G.get_source();
	auto [x2, y2] = G.get_dest();
	visited[x1][y1] = true;
	/*
	* This septuplet consists of (c,h,d,x,y,u,v) where
	* - c = cos(theta)*h+sin(theta)*d is the cost
	* - h is the value of the heuristic function
	* - d is the distance between the vertex s(x,y) and the vertex s0=(0,0)
	* - x the x-coordinate of the vertex s
	* - y is the y-coordinate of the vertex s
	* - u is the x-coordinate of the vertex r parent of s
	* - v is the y-coordinate of the vertex r parent of s
	*/
	using quadruplet = std::tuple<int, int, int, int >;
	std::queue<quadruplet> Q;
	using couple = std::pair<int, int>;
	std::vector<std::vector<couple>> parent(n, std::vector<couple>(m, { x1,y1 }));
	Q.push({x1,y1,x1,y1 });
	using namespace  std::chrono_literals;
	auto grid_thread = dynamic_cast<GridThread*>(G.getThread());
	while (!Q.empty())
	{
		auto w = Q.front();
		Q.pop();
		auto& [i, j, a, b] = w;
		parent[i][j] = { a,b };
		emit grid_thread->to_explore(i, j);
		if (i == x2 && j == y2)
			break;
		for (int p = std::max(i - 1, 0); p < std::min(i + 2, n); p++)
			for (int q = std::max(j - 1, 0); q < std::min(j + 2, m); q++)
			{
				if (i != p && q != j && true)
					continue;
				if (p == i && q == j)
					continue;
				if (G.is_blocked(p, q))
					continue;
				if (!visited[p][q])
					emit grid_thread->to_visit(p, q);
				else continue;
				visited[p][q] = true;
				Q.push({p,q,i,j });
			}
		std::this_thread::sleep_for(delay);
	}
	for (std::pair<int, int>w = { x2,y2 }; w != parent[w.first][w.second]; w = parent[w.first][w.second])
		emit grid_thread->to_set_optimal(w.first, w.second);
	emit grid_thread->to_set_optimal(x1, y1);
}

void DFS::search(RGrid& G) const
{
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	auto [x1, y1] = G.get_source();
	auto [x2, y2] = G.get_dest();
	visited[x1][y1] = true;
	/*
	* This heptuplet consists of (c,h,d,x,y,u,v) where
	* - c = cos(theta)*h+sin(theta)*d is the cost
	* - h is the value of the heuristic function
	* - d is the distance between the vertex s(x,y) and the vertex s0=(0,0)
	* - x the x-coordinate of the vertex s
	* - y is the y-coordinate of the vertex s
	* - u is the x-coordinate of the vertex r parent of s
	* - v is the y-coordinate of the vertex r parent of s
	*/
	using couple = std::pair<int, int>;
	using quadruplet = std::tuple<int, int, int, int >;
	std::stack<quadruplet> Q;
	std::vector<std::vector<couple>> parent(n, std::vector<couple>(m, { x1,y1 }));
	Q.push({x1,y1,x1,y1});
	using namespace  std::chrono_literals;
	auto grid_thread = dynamic_cast<GridThread*>(G.getThread());
	while (!Q.empty())
	{
		auto w = Q.top();
		auto& [i, j,a,b] = w;
		emit grid_thread->to_explore(i, j);
		if (i == x2 && j == y2)
			break;
		Q.pop();
		for (int p = std::max(i - 1, 0); p < std::min(i + 2, n); p++)
			for (int q = std::max(j - 1, 0); q < std::min(j + 2, m); q++)
			{
				if (i != p && q != j && true)
					continue;
				if (p == i && q == j)
					continue;
				if (G.is_blocked(p, q))
					continue;
				if (!visited[p][q])
					emit grid_thread->to_visit(p, q);
				else continue;
				parent[p][q] = { i,j };
				visited[p][q] = true;
				Q.push({ p,q,i,j});
			}
		std::this_thread::sleep_for(delay);
	}
	for (std::pair<int, int>w = { x2,y2 }; w != parent[w.first][w.second]; w = parent[w.first][w.second])
		emit grid_thread->to_set_optimal(w.first, w.second);
	emit grid_thread->to_set_optimal(x1, y1);
}
