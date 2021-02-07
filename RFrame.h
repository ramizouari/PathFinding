#pragma once
#include <qframe.h>
class RFrame :
    public QFrame
{
    int a, b;
    Q_OBJECT
    enum class State
    {
        Normal, Blocked, Source, Destination
    };
    State state;
    void set_state(State s);
public:   
    RFrame(State s,QWidget* parent = nullptr);
    RFrame(QWidget* parent = nullptr);
    bool blocked()const;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
public slots:
    void explore();
    void optimal();
    void visit();
    void reset();
    void set_as_source();
    void set_as_destination();
    void set_as_normal();
    void set_as_blocked();
signals:
    void to_explore();
};

