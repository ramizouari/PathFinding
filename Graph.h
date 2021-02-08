#pragma once
#include <vector>
#include <unordered_set>
class Graph
{
protected:
	std::vector<std::unordered_set<int>> adjacency_list;
	int n;
public:
	Graph(int);
	void connect(int i, int j);
	void disconnect(int i, int j);
	bool connected(int i, int j) const;
};

class PlanarGraph : public Graph
{
	using couple = std::pair<int, int>;
	int rows, columns;
	int map(couple u) const;
	couple inverse_map(int s) const;
public:
	PlanarGraph(int n,int m);
	void connect(int, int) = delete;
	void disconnect(int, int) = delete;
	bool connected(int, int) = delete;
	void connect(couple u, couple v);
	void disconnect(couple u, couple v);
	bool connected(couple u, couple v);
	PlanarGraph kruskal() const;
	std::vector<std::vector<bool>> convert() const;
};

class GridGraph :public PlanarGraph
{
public:
	GridGraph(int n, int m);
};

class UnionFind
{
	int n;
	std::vector<int> S;
	int length(int a);
public:
	UnionFind(int s);
	void set_union(int a, int b);
	int find(int s);
	bool connected(int a, int b);
};