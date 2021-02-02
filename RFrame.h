#pragma once
#include <qframe.h>
class RFrame :
    public QFrame
{
    Q_OBJECT
    enum class State
    {
        Normal,Blocked
    } state;

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
    void reset();
signals:
    void to_explore();
};

