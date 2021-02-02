#include "RFrame.h"
#include "qevent.h"
#include "qapplication.h"


RFrame::RFrame(QWidget* parent) :RFrame(RFrame::State::Normal,parent){
//	setMouseTracking(true);
	setFocusPolicy(Qt::FocusPolicy::NoFocus);
}
void RFrame::explore()
{
	connect(this, &RFrame::to_explore, this, &RFrame::explore);
	if (state == State::Blocked)
		return;
	QPalette pal(Qt::GlobalColor::darkGreen);
	setPalette(pal);
}

void RFrame::optimal()
{
	QPalette pal(Qt::GlobalColor::cyan);
	setPalette(pal);
}

void RFrame::reset()
{
	set_state(state);
}

bool RFrame::blocked() const
{
	return state==State::Blocked;
}
RFrame::RFrame(State s,QWidget* parent) :QFrame(parent)
{
	set_state(s);
}

void RFrame::mousePressEvent(QMouseEvent* e)
{
	QFrame::mousePressEvent(e);
	if (e->button() != Qt::MouseButton::RightButton)
		return;
	set_state(state == State::Blocked ? State::Normal : State::Blocked);
}

void RFrame::mouseMoveEvent(QMouseEvent* e)
{
	QFrame::mouseMoveEvent(e);
}

void RFrame::set_state(State s)
{
	state = s;
	QPalette pal;
	switch (s)
	{
	case State::Blocked:
		pal = QPalette(Qt::GlobalColor::black);
		break;
	case State::Normal:
		pal = QPalette(Qt::GlobalColor::lightGray);
		break;
	}
	setPalette(pal);
}