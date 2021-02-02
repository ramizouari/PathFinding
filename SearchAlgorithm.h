#pragma once
#include "qobject.h"
class RGrid;

namespace metric
{
	using couple = std::pair<double, double>;
	inline constexpr auto lp = [](double p)
	{
		return [p](couple a, couple b)->double
		{
			return
				std::pow(
					std::pow(std::abs(a.first - b.first), p) + std::pow(std::abs(a.second - b.second), p),
				1./p);
		};
	};
	inline constexpr auto l2 = [](couple a, couple b) 
	{
		return 
		std::sqrt(
			std::pow(a.first-b.first, 2) + std::pow(a.second-b.second, 2)
		); 
	};

	inline constexpr auto l1 = [](couple a, couple b)
	{
		return
			std::abs(a.first - b.first) + std::abs(a.second - b.second);	
	};

	inline constexpr auto linf = [](couple a, couple b)
	{
		return
			std::max(a.first - b.first,a.second-b.second);
	};
}

class SearchAlgorithm:public QObject
{
	Q_OBJECT
public:
	virtual ~SearchAlgorithm() = 0;
	virtual void search(RGrid &G) const = 0;
};

class LSearch : public SearchAlgorithm
{
	double theta;
	using couple = std::pair<double, double>;
	std::function<double(couple, couple)>heuristic;
public:
	LSearch(double _angle);
	LSearch(double _angle,std::function<double(couple, couple)> h);
	void search(RGrid& G) const override;
};

class ASharp : public LSearch
{
public:
	using couple = std::pair<double, double>;
	ASharp();
	ASharp(std::function<double(couple, couple)> h);
};

class Djikstra : public LSearch
{
	using couple = std::pair<double, double>;
public:
	Djikstra();
};

class BestSearch : public LSearch
{
	using couple = std::pair<double, double>;
public:
	BestSearch();
	BestSearch(std::function<double(couple, couple)> h);
};
