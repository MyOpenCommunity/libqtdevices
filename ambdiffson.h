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
class SoundDiffusion;
class MultiSoundDiff;
class AudioSources;


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
	ambDiffSon(QWidget *parent, QString d, QString indirizzo, QString Icona1, QString Icona2,
		QString Icona3, SoundDiffusion *ds, AudioSources *sorg, MultiSoundDiff *dm);
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
	SoundDiffusion *diffson;
	MultiSoundDiff *diffmul;
	AudioSources *sorgenti;
	int actSrc;
	bool is_draw;
	QString descr;
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
	insAmbDiffSon(QWidget *parent, QString d, QString Icona1, QString Icona2, SoundDiffusion *ds,
		AudioSources *sorg, MultiSoundDiff *dm);
	void Draw();
public slots:
	void configura();
	//! receives amb index and active source index
	void actSrcChanged(int, int);
private:
	SoundDiffusion *diffson;
	MultiSoundDiff *diffmul;
	AudioSources *sorgenti;
	QString descr;
signals:
	void ambChanged(const QString &, bool, QString);
};

#endif
