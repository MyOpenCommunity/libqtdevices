/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** diffmulti.h
 **
 **definizioni della pagina di sottoMenu diffusione Sonora multicanale
 **
 ****************************************************************/

#ifndef DIFFMULTI_H
#define DIFFMULTI_H

#include <qwidget.h>
//#include "banner.h"
#include "items.h"
//#include "main.h"
#include "device.h"
#include "frame_interpreter.h"
#include "diffsonora.h"
#include "sottomenu.h"
#include "openclient.h"
#include <qptrlist.h> 

#include <qcursor.h>

class diffSonora;

class dati_sorgente_multi 
{
public:
	char tipo;
	QPtrList<QString> *descr;
	char ambdescr[50];
	void *indirizzo;
	char I1[50], I2[50], I3[50];
	int modo;
	dati_sorgente_multi(char t, QPtrList<QString> *d, void *ind,
			char *I1, char *I2, char *I3, int p1, char *ad);
	~dati_sorgente_multi();
};

class dati_ampli_multi
{
public:
	char tipo;
	QPtrList<QString> *descr;
	void *indirizzo;
	char I1[50], I2[50], I3[50], I4[50];
	int modo;
	dati_ampli_multi(char t, QPtrList<QString> *d, void *ind,
			char *I1, char *I2, char *I3, char *I4, int p1);
	~dati_ampli_multi();
};

class diffmulti : public sottoMenu
{
Q_OBJECT
public:
	diffmulti( QWidget *parent=0, const char *name=0, uchar withNavBar=3, int width=MAX_WIDTH, int height=MAX_HEIGHT, uchar n=NUM_RIGHE-1 );
	int addItem(char tipo, QPtrList<QString> *nomi, void* indirizzo,
			QPtrList<QString> &icon_names,
			int periodo=0,
			int numFrame=0, QColor  secondFroreground=QColor(0,0,0),
			char* descr1=NULL, char* descr2=NULL, char* descr3=NULL,
			char* descr4=NULL, int par3=0, int par4=0,
			QPtrList<QString> *lt = NULL,
			QPtrList<scenEvo_cond> *lc = NULL,
			QString action="", QString light="", QString key="",
			QString unk="",
			QValueList<int> sstart = QValueList<int>(),
			QValueList<int> sttop = QValueList<int>());
	/*!
	 *  \brief Changes the type of navigation bar present at the
	 *  bsubtree (see bannFrecce). Also calls downstream diffSonora setNavBarMode
	 */
	void setNavBarMode(uchar=0, char* IconBut4=ICON_FRECCIA_DX);
	void setNumRighe(uchar);
	void ripristinaRighe(void);
	void forceDraw();
	void reparent(QWidget *, unsigned int f, QPoint, bool showIt= false);
	void resizewindows(int x, int y, int w, int h);

	/*!
	 *  \brief Sets the geometry passing x,y,w,h.
	 */
	void setGeom(int,int,int,int);
	void inizializza();
public slots:
	//void freezed();
	void ds_closed(diffSonora *);
	void hide();
	void show();
	void status_changed(QPtrList<device_status>);
	/*!
	 *  \brief sets isVisual to false and emits freezed signal
	 */
	void freezed_handler(bool);
	void gestFrame(char*);
signals:
	void actSrcChanged(int, int);
	void dsClosed();
	void freezed(bool);
	void gesFrame(char *);
private:
	QPtrList<diffSonora> *dslist;
	QPtrList<dati_ampli_multi> *datimmulti;
	sottoMenu *sorgenti;
	device *matr;
	/// kemosh FIX
	int _where_address;
};

class sveglia ;

//! Contenitore diffusione sonora/diffusione sonora multicanale per sveglia
class contdiff : public QObject 
{
Q_OBJECT
private:
	diffSonora *ds;
	diffmulti *dm;
public:
	contdiff(diffSonora *, diffmulti *);
	void reparent(QWidget *, unsigned int f, QPoint, bool showIt= false);
	void setNavBarMode(uchar);
	void setNumRighe(uchar);
	void setGeom(int,int,int,int);
	void forceDraw();
	void ripristinaRighe();
	void resizewindows();
	void restorewindows();
	void connectClosed(sveglia *);
	void disconnectClosed(sveglia *);
public slots:
	//! Invoke proper hide method
	void hide();
	void show();
signals:
	/*!
	 *  \brief Emitted when the object is closed.
	 */
	void Closed();

};


#endif // DIFFMULTI_H
