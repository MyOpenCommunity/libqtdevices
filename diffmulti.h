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

#include "device_status.h"
#include "sottomenu.h"

#include <QWidget>
#include <QString>
#include <QList>


class diffSonora;
class device;
class scenEvo_cond;


class dati_ampli_multi
{
public:
	char tipo;
	// NOTE: the ownership of descr strings is mantained by xmlconfhandler.
	QList<QString*> *descr;
	void *indirizzo;
	QString I1, I2, I3, I4, I5;
	int modo;
	dati_ampli_multi(char t, QList<QString*> *d, void *ind, int p1,
			QString _I1, QString _I2, QString _I3, QString _I4, QString _I5 = QString());
	~dati_ampli_multi();
};


class diffmulti : public sottoMenu
{
Q_OBJECT
public:
	diffmulti(QWidget *parent=0, const char *name=0, uchar withNavBar=3, int width=MAX_WIDTH, int height=MAX_HEIGHT, uchar n=NUM_RIGHE-1);
	~diffmulti();

	int addItem(char tipo, QList<QString*> *descrizioni, void* indirizzo, QList<QString*> &icon_names, int periodo=0, int numFrame=0);

	/*!
	 *  \brief Changes the type of navigation bar present at the
	 *  bsubtree (see bannFrecce). Also calls downstream diffSonora setNavBarMode
	 */
	void setNavBarMode(uchar=0, char* IconBut4=ICON_FRECCIA_DX);
	void setNumRighe(uchar);
	void ripristinaRighe(void);
	void forceDraw();
	virtual void reparent(QWidget * parent, Qt::WindowFlags f, const QPoint & p, bool showIt = FALSE);
	void resizewindows(int x, int y, int w, int h);

	/*!
	 *  \brief Sets the geometry passing x,y,w,h.
	 */
	void setGeom(int,int,int,int);
	void inizializza();

public slots:
	void ds_closed(diffSonora *);
	void hide();
	void show();
	void status_changed(QList<device_status*> sl);
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
	QList<diffSonora*> dslist;
	QList<dati_ampli_multi*> datimmulti;
	sottoMenu *sorgenti;
	device *matr;
	/// kemosh FIX
	int _where_address;
};


class sveglia;


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
