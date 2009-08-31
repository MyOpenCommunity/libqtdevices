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

private slots:
	void ds_closed();
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
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf);
	virtual void inizializza();
	virtual void setNumRighe(uchar);

public slots:
	void status_changed(QList<device_status*> sl);

private:
	device *matr;

signals:
	void actSrcChanged(int, int);
};


class MultiSoundDiffAlarm : public MultiSoundDiffInterface
{
Q_OBJECT
public:
	MultiSoundDiffAlarm(const QDomNode &config_node);
	virtual void inizializza() { } // avoid a second initialization (see .cpp for details)
	virtual void showPage();
	virtual SoundDiffusion *createSoundDiffusion(AudioSources *sorgenti, const QDomNode &conf);

private slots:
	void status_changed(QList<device_status*> sl);

signals:
	void actSrcChanged(int, int);
};

#endif // MULTI_SOUNDDIFF_H
