#pragma once
#include <qframe.h>
class RFrame :
    public QFrame
{
    Q_OBJECT
    enum class State
    {
        Normal, Blocked, Source, Destination
    };
    State state;
    //Change the state of this square
    void set_state(State s);
public:   
    RFrame(State s,QWidget* parent = nullptr);
    RFrame(QWidget* parent = nullptr);
    //Get whether this square is blocked or not
    bool blocked()const;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
public slots:
    //Mark the square as explored
    void explore();
    //Marks the square as a part of an optimal walk
    void optimal();
    //Visit the square as visited
    void visit();
    //Reset the square to the Normal state if it is not Blocked
    void reset();
    //Set this square as a source
    void set_as_source();
    //Set this square as a destination
    void set_as_destination();
    //Set this square as normal
    void set_as_normal();
    //Set this square as a blocked one
    void set_as_blocked();
signals:
    void to_explore();
};

