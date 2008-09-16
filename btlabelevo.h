#ifndef BTLABELEVO_H
#define BTLABELEVO_H


#include <qlabel.h>

class BtLabelEvo : public QLabel
{
Q_OBJECT
public:
	BtLabelEvo(QWidget *parent, const char *name = 0, WFlags f = 0)
		: QLabel(parent, name, f) {}
public slots:
	virtual void setEnabled(bool) {}
};

#endif
