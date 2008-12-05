/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** ambdiffson.h
 **
 **definizione degli ambienti di diffusione sonora implementati
 **
 ****************************************************************/

#ifndef AMBDIFFSON_H
#define AMBDIFFSON_H

#include "bannbuticon.h"
#include "bannbut2icon.h"

#include <QWidget>
#include <QString>
#include <QList>

/// Forward Declarations
class diffSonora;
class diffmulti;
class dati_ampli_multi;
class sottoMenu;

/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class ambDiffSon
 * \brief This class represents an env. in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class ambDiffSon : public bannBut2Icon
{
Q_OBJECT
public:
	ambDiffSon(QWidget *parent, QString _name, QString indirizzo, QString Icona1, QString Icona2,
		QString Icona3, diffSonora *ds, sottoMenu *sorg, diffmulti *dm);
	void Draw();
	void setDraw(bool d);
	bool isDraw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
protected:
	virtual void hideEvent(QHideEvent *event);
private:
	diffSonora *diffson;
	diffmulti *diffmul;
	sottoMenu *sorgenti;
	int actSrc;
	bool is_draw;
	QString name;
signals:
	void ambChanged(const QString &, bool, QString);
};


/*****************************************************************
 ** Insieme Ambienti diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class insAmbDiffSon
 * \brief This class represents an env. in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class insAmbDiffSon : public bannButIcon
{
Q_OBJECT
public:
	insAmbDiffSon(QWidget *parent, QString _name, QString Icona1, QString Icona2, diffSonora *ds,
		sottoMenu *sorg, diffmulti *dm);
	void Draw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
private:
	diffSonora *diffson;
	diffmulti *diffmul;
	sottoMenu *sorgenti;
	QString name;
signals:
	void ambChanged(const QString &, bool, QString);
};

#endif
