#include "SearchAlgorithm.h"
#include <vector>
#include <algorithm>
#include <queue>
#include <chrono>
#include "RGrid.h"

LSearch::LSearch(double _angle) :theta(_angle), heuristic([](couple x, couple y) 
	{
		return std::sqrt(std::pow(x.first-y.first,2)+std::pow(x.second-y.second,2)); 
	})
{
}

LSearch::LSearch(double _angle, std::function<double(couple, couple)> h):theta(_angle),heuristic(h)
{
}

void LSearch::search(RGrid& G) const
{
	std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));
	std::vector<std::vector<bool>> processed(n, std::vector<bool>(m, false));
	visited[0][0] = true;
	/*
	* This sextuple consists of (c,d,x,y,u,v) where
	* - c is the cost
	* - d is the distance between the vertex s(x,y) and the vertex s0=(0,0)
	* - x the x-coordinate of the vertex s
	* - y is the y-coordinate of the vertex s 
	* - u is the x-coordinate of the vertex r parent of s
	* - v is the y-coordinate of the vertex r parent of s
	*/
	using sextuple = std::tuple<double, double, int, int, int, int >;
	std::priority_queue<sextuple, std::vector<sextuple>, std::greater<void>> Q;
	using couple = std::pair<int, int>;
	std::vector<std::vector<couple>> parent(n, std::vector<couple>(m, { 0,0 }));
	Q.push({ 0,0,0,0,0,0 });
	using namespace  std::chrono_literals;
	auto grid_thread = dynamic_cast<GridThread*>(G.getThread());
	auto cos_theta = std::cos(theta),sin_theta=std::sin(theta);
	while (!Q.empty())
	{
		auto w = Q.top();
		Q.pop();
		auto& [total_cost, distance, i, j, a, b] = w;
		if (processed[i][j])
			continue;
		parent[i][j] = { a,b };
		emit grid_thread->to_explore(i, j);
		if (i == n - 1 && j == m - 1)
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
				Q.push({ cos_theta * (distance + 1) + sin_theta * heuristic({n-1,m-1} ,{p,q}),distance + 1,p,q,i,j });
			}
		std::this_thread::sleep_for(20ms);
	}
	for (std::pair<int, int>w = { n - 1,m - 1 }; w != parent[w.first][w.second]; w = parent[w.first][w.second])
		emit grid_thread->to_set_optimal(w.first, w.second);
	emit grid_thread->to_set_optimal(0, 0);
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
