/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** multisounddiff.h
 **
 **definizioni della pagina di sottoMenu diffusione Sonora multicanale
 **
 ****************************************************************/

#ifndef MULTI_SOUNDDIFF_H
#define MULTI_SOUNDDIFF_H

#include "sottomenu.h"

#include <QWidget>
#include <QString>
#include <QList>


class SoundDiffusion;
class AudioSources;
class device;
class device_status;
class AlarmClock;

class MultiSoundDiffInterface : public sottoMenu
{
Q_OBJECT
public:
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf)
		{ return 0; }
	void loadAmbienti(const QDomNode &config_node);
	~MultiSoundDiffInterface();

protected:
	MultiSoundDiffInterface() { }
	QList<SoundDiffusion*> dslist;
	static AudioSources *sorgenti;

signals:
	void actSrcChanged(int, int);
};

class MultiSoundDiff : public MultiSoundDiffInterface
{
Q_OBJECT
public:
	MultiSoundDiff(const QDomNode &config_node);

	/*!
	 *  \brief Changes the type of navigation bar present at the
	 *  bsubtree (see bannFrecce). Also calls downstream SoundDiffusion setNavBarMode
	 */
	virtual void setNavBarMode(uchar=0, QString IconBut4=ICON_FRECCIA_DX);
	void setNumRighe(uchar);
	virtual void reparent(QWidget * parent, Qt::WindowFlags f, const QPoint & p, bool showIt = FALSE);
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf);

	virtual void inizializza();

public slots:
	void ds_closed(SoundDiffusion *);
	void status_changed(QList<device_status*> sl);
	void gestFrame(char*);

private:
	QList<SoundDiffusion*> dslist;
	device *matr;
	void ripristinaRighe();
	void resizewindows(int x, int y, int w, int h);

signals:
	void actSrcChanged(int, int);
	void dsClosed();
	void gesFrame(char *);
};


class MultiSoundDiffAlarm : public MultiSoundDiffInterface
{
Q_OBJECT
public:
	MultiSoundDiffAlarm(const QDomNode &config_node);
	virtual void inizializza() { } // avoid a second initialization
	virtual void showPage();
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf);
	void ripristinaRighe();
	void resizewindows(int x, int y, int w, int h);

signals:
	void actSrcChanged(int, int);
	void ambSelected();
};


//! Contenitore diffusione sonora/diffusione sonora multicanale per sveglia
class contdiff : public QObject
{
Q_OBJECT
public:
	contdiff(SoundDiffusion *, MultiSoundDiffAlarm *);
	void reparent(QWidget *, unsigned int f, QPoint, bool showIt= false);
	void setNavBarMode(uchar);
	void setNumRighe(uchar);
	void setGeom(int,int,int,int);
	void forceDraw();
	void ripristinaRighe();
	void resizewindows();
	void restorewindows();
	void connectClosed(AlarmClock *);
	void disconnectClosed(AlarmClock *);

public slots:
	//! Invoke proper hide method
	void hide();
	void show();

private:
	SoundDiffusion *ds;
	MultiSoundDiffAlarm *dm;

signals:
	/*!
	 *  \brief Emitted when the object is closed.
	 */
	void Closed();
};


#endif // MULTI_SOUNDDIFF_H
