#ifndef BTLABELEVO_H
#define BTLABELEVO_H


#include <QLabel>

class BtLabelEvo : public QLabel
{
Q_OBJECT
public:
	BtLabelEvo(QWidget *parent, const char *name = 0, Qt::WindowFlags f = 0)
	: QLabel(parent, f) {}
public slots:
	virtual void setEnabled(bool) {}
};

#endif
