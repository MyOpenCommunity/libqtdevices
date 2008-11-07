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
private:
	diffSonora *diffson;
	diffmulti *diffmul;
	sottoMenu *sorgenti;
	int actSrc;
	bool is_draw;
	QString name;
public:
	ambDiffSon(QWidget *parent=0, QString _name=NULL, void *indirizzo=NULL, QString Icona1="", QString Icona2="", QString Icona3="", QList<dati_ampli_multi*> *la = NULL, diffSonora *ds=NULL, sottoMenu *sorg=NULL, diffmulti *dm=NULL);
	void Draw();
	void setDraw(bool d);
	bool isDraw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
protected:
	virtual void hideEvent(QHideEvent *event);
signals:
	void ambChanged(const QString &, bool, char *);
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
private:
	diffSonora *diffson;
	diffmulti *diffmul;
	sottoMenu *sorgenti;
	QString name;
public:
	insAmbDiffSon(QWidget *parent=0, QList<QString*> *descrizioni=NULL, void *indirizzo=NULL, QString Icona1="", QString Icona2="", QList<dati_ampli_multi*> *la = NULL, diffSonora *ds=NULL, sottoMenu *sorg=NULL, diffmulti *dm=NULL);
	void Draw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
signals:
	void ambChanged(const QString &, bool, char *);
};

#endif
