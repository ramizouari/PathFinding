#pragma once
#include <qobject.h>
#include <unordered_map>
#include "SearchAlgorithm.h"
#include "qactiongroup.h"
class SearchAlgorithmList :
    public QObject
{
    Q_OBJECT
    std::unordered_map<QString, std::shared_ptr<SearchAlgorithm>> algorithm_mapper;
    std::unordered_map<std::shared_ptr<SearchAlgorithm>, QAction*> action_mapper;
    QActionGroup *action_group;
public:
    SearchAlgorithmList(QObject* parent=nullptr);
    void add_algorithm(QString name, std::shared_ptr<SearchAlgorithm> alg);
    QActionGroup* get_action_group();
    QAction* get_action(QString name);
    std::shared_ptr<SearchAlgorithm> get_algorithm(QString name);
};

