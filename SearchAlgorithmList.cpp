#include "SearchAlgorithmList.h"

SearchAlgorithmList::SearchAlgorithmList(QObject* parent):action_group(new QActionGroup(this))
{
	action_group->setExclusive(true);
}

void SearchAlgorithmList::add_algorithm(QString name, std::shared_ptr<SearchAlgorithm> alg)
{
	algorithm_mapper[name].swap(alg);
	auto action = action_group->addAction(name);
	action->setCheckable(true);
	action_mapper[algorithm_mapper[name]]=action;

}

QActionGroup* SearchAlgorithmList::get_action_group()
{
	return action_group;
}

QAction* SearchAlgorithmList::get_action(QString name)
{
	return action_mapper[algorithm_mapper[name]];
}

std::shared_ptr<SearchAlgorithm> SearchAlgorithmList::get_algorithm(QString name)
{
	return algorithm_mapper[name];
}
