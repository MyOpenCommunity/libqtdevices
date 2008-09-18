#ifndef BTLABEL_H
#define BTLABEL_H


#include <QLabel>


// TODO: è necessario avere una label che si comporta come quella delle qt, ma
// che quando viene disabilitata non si ingrigisce. Questa soluzione non va bene
// perche' vengono comunque dispatchati gli eventi alla label e ai suoi figli
// (essendo a tutti gli effetti abilitata)
// La soluzione sara' quella di utilizzare una qlabel standard disabilitando
// l'effetto di "ingrigimento" attraverso gli stylesheet.

class BtLabel : public QLabel
{
Q_OBJECT
public:
	BtLabel(QWidget *parent, const char *name = 0, Qt::WindowFlags f = 0);

	// TODO: rimuovere questi metodi qt3!
	void setPaletteForegroundColor(const QColor &c);
	void setPaletteBackgroundColor(const QColor &c);

public slots:
	virtual void setEnabled(bool);
};

#endif
