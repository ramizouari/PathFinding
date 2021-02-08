#include "PathFinding.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PathFinding *w=new PathFinding;
    w->show();
    w->setMinimumSize({480,600});
    auto R= a.exec();
    return R;
}
