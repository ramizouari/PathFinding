#include "Graph.h"
#include <random>

Graph::Graph(int size):n(size),adjacency_list(size)
{
}

void Graph::connect(int i, int j)
{
	adjacency_list[i].insert(j);
}

void Graph::disconnect(int i, int j)
{
	adjacency_list[i].erase(j);
}

bool Graph::connected(int i, int j) const
{
	return adjacency_list[i].find(j) != adjacency_list[i].end();
}

int PlanarGraph::map(couple u) const
{
	return u.second * rows + u.first;
}

PlanarGraph::couple PlanarGraph::inverse_map(int s) const
{
	auto L = std::div(s, rows);
	return {L.rem,L.quot};
}

PlanarGraph::PlanarGraph(int n, int m):Graph(n*m),rows(n),columns(m)
{
}

void PlanarGraph::connect(couple u, couple v)
{
	Graph::connect(map(u), map(v));
}

void PlanarGraph::disconnect(couple u, couple v)
{
	Graph::disconnect(map(u),map(v));
}

bool PlanarGraph::connected(couple u, couple v)
{
	return 	Graph::connected(map(u),map(v));
}

PlanarGraph PlanarGraph::kruskal() const
{
	PlanarGraph T(rows, columns);
	UnionFind F(rows * columns);
	std::vector<std::pair<couple, couple>> edges;
	for (int i = 0; i < rows * columns; i++)
		for (auto j : adjacency_list[i])
			edges.emplace_back(inverse_map(i), inverse_map(j));
	static std::random_device dev;
	std::mt19937_64 e(dev());
	std::shuffle(edges.begin(), edges.end(), e);
	for(auto &[u,v]:edges)
	{
		if (F.connected(map(u), map(v)))
			continue;
		F.set_union(map(u), map(v));
		T.connect(u, v);
	}
	return T;
}

std::vector<std::vector<bool>> PlanarGraph::convert() const
{
	std::vector<std::vector<bool>> B(2 * rows-1, std::vector<bool>(2 * columns-1));
	for (int i = 0; i < columns * rows; i++)
	{
		auto [a, b] = inverse_map(i);
		B[2*a][2*b] = true;
		for (auto& j : adjacency_list[i])
		{
			auto [c, d] = inverse_map(j);
			B[a+c][b+d] = true;
		}
	}
	return B;
}

int UnionFind::length(int a)
{
	int L=0;
	while (a != find(a))
	{
		L++;
		a = find(a);
	}
	return L;
}

UnionFind::UnionFind(int s):n(s),S(s)
{
	for (int i = 0; i < n; i++)
		S[i] = i;
}

void UnionFind::set_union(int a, int b)
{
	int s1(find(a)), s2(find(b));
	if (s1==s2)
		return;
	int L1 = length(a), L2 = length(b);
	if (L1 > L2)
		S[s2] = s1;
	else
		S[s1] = s2;
}

int UnionFind::find(int s)
{
	int u = s,v=S[u];
	while (u != v)
	{
		S[u] = S[v];
		u = v;
		v = find(v);
	}
	return u;
}

bool UnionFind::connected(int a, int b)
{
	return find(a)==find(b);
}

GridGraph::GridGraph(int n, int m):PlanarGraph(n,m)
{
	for(int i=0;i<n;i++)
		for(int j=0;j<m;j++)
			for(int u=std::max(i-1,0);u<std::min(i+2,n);u++)
				for (int v = std::max(j - 1, 0); v < std::min(j + 2, m); v++)
				{
					if (u==i && v==j || u!=i && v!=j)
						continue;
					connect({ i,j }, { u,v });
				}
}
