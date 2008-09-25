#ifndef BTLABELEVO_H
#define BTLABELEVO_H

#include "btlabel.h"

// TODO: sara' da rimuovere con la riscrittura della btlabel!

class BtLabelEvo : public BtLabel
{
Q_OBJECT
public:
	BtLabelEvo(QWidget *parent, const char *name = 0, Qt::WindowFlags f = 0)
	: BtLabel(parent, name, f) {}
public slots:
	virtual void setEnabled(bool) {}
};

#endif
