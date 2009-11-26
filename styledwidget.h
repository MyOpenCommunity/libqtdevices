#ifndef STYLEDWIDGET_H
#define STYLEDWIDGET_H

#include <QWidget>

// according to Qt Style Sheets Reference, QWidget subclasses need
// to reimplement paintEvent() in order for the style sheet to be applied
class StyledWidget : public QWidget
{
public:
	StyledWidget(QWidget *parent=0);

protected:
	virtual void paintEvent(QPaintEvent *);
};

#endif // STYLEDWIDGET_H
