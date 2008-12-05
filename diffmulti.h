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

#include "sottomenu.h"

#include <QWidget>
#include <QString>
#include <QList>


class SoundDiffusion;
class AudioSources;
class device;
class device_status;


class diffmulti : public sottoMenu
{
Q_OBJECT
public:
	diffmulti(QWidget *parent, QDomNode config_node);
	~diffmulti();

	/*!
	 *  \brief Changes the type of navigation bar present at the
	 *  bsubtree (see bannFrecce). Also calls downstream SoundDiffusion setNavBarMode
	 */
	virtual void setNavBarMode(uchar=0, QString IconBut4=ICON_FRECCIA_DX);
	void setNumRighe(uchar);
	void ripristinaRighe(void);
	virtual void reparent(QWidget * parent, Qt::WindowFlags f, const QPoint & p, bool showIt = FALSE);
	void resizewindows(int x, int y, int w, int h);

	void inizializza();

public slots:
	void ds_closed(SoundDiffusion *);
	void status_changed(QList<device_status*> sl);
	void gestFrame(char*);

protected:
	virtual void hideEvent(QHideEvent *event);

private:
	void loadAmbienti(QDomNode config_node);

	QList<SoundDiffusion*> dslist;
	AudioSources *sorgenti;
	device *matr;

signals:
	void actSrcChanged(int, int);
	void dsClosed();
	void gesFrame(char *);
};


class sveglia;


//! Contenitore diffusione sonora/diffusione sonora multicanale per sveglia
class contdiff : public QObject
{
Q_OBJECT
public:
	contdiff(SoundDiffusion *, diffmulti *);
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

private:
	SoundDiffusion *ds;
	diffmulti *dm;

signals:
	/*!
	 *  \brief Emitted when the object is closed.
	 */
	void Closed();
};


#endif // DIFFMULTI_H
