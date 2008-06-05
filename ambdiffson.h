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
public:
	ambDiffSon(QWidget *parent=0, const char *name=NULL, void *indirizzo=NULL, char* Icona1="",char *Icona2="", char *Icona3="", QPtrList<dati_ampli_multi> *la = NULL, diffSonora *ds=NULL, sottoMenu *sorg=NULL, diffmulti *dm=NULL);
	void Draw();
	void hide();
	void setDraw(bool d);
	bool isDraw();
public slots:
	void configura(); 
	//! receives amb index and active source index
	void actSrcChanged(int, int);
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
public:
	insAmbDiffSon(QWidget *parent=0, QPtrList<QString> *descrizioni=NULL, void *indirizzo=NULL, char* Icona1="",char *Icona2="", QPtrList<dati_ampli_multi> *la = NULL, diffSonora *ds=NULL, sottoMenu *sorg=NULL, diffmulti *dm=NULL);
	void Draw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
signals:
	void ambChanged(const QString &, bool, char *);
};

#endif
