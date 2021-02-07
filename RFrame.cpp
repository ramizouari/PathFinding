#include "RFrame.h"
#include "qevent.h"
#include "qapplication.h"
#include "RGrid.h"


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

void RFrame::visit()
{
	setPalette(QPalette(Qt::GlobalColor::green));
}

void RFrame::reset()
{
	set_state(state);
}

void RFrame::set_as_source()
{
	if (state != State::Normal)
		return;
	set_state(State::Source);
}

void RFrame::set_as_destination()
{
	if (state != State::Normal)
		return;
	set_state(State::Destination);
}

void RFrame::set_as_normal()
{
	set_state(State::Normal);
}

void RFrame::set_as_blocked()
{
	set_state(State::Blocked);
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
	if (!blocked() && e->button() == Qt::MouseButton::LeftButton && e->modifiers() == Qt::Modifier::CTRL)
		dynamic_cast<RGrid*>(parentWidget())->set_destination(this);
	else if (!blocked() && e->button() == Qt::MouseButton::LeftButton)
		dynamic_cast<RGrid*>(parentWidget())->set_source(this);
	else if(e->button()==Qt::MouseButton::RightButton && state!=State::Source && state!=State::Destination)
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
	case State::Source:
		pal = QPalette(Qt::GlobalColor::blue);
		break;
	case State::Destination:
		pal = QPalette(Qt::GlobalColor::red);
		break;
	}
	setPalette(pal);
}