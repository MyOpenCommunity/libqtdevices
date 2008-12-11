/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sounddiffusion.h
 **
 **definizioni della pagina di sottoMenu diffusione Sonora
 **
 ****************************************************************/

#ifndef SOUND_DIFFUSION_H
#define SOUND_DIFFUSION_H

#include "sottomenu.h"

#include <QWidget>
#include <QList>
#include <QString>

class banner;


/// A container for sources (aux, touchscreen, radio..)
class AudioSources : public sottoMenu
{
Q_OBJECT
public:
	AudioSources(QWidget *parent, QDomNode config_node);
	/// add amb to all banners (source)
	void addAmb(QString a);

	/// Sets the index of the list forcing it to the object having the address passed by argument.
	void setIndex(QString addr);

	void mostra_all(char);

private:
	void loadItems(QDomNode config_node);
signals:
	void actSrcChanged(int, int);

	/// amb description changed (for MultiSoundDiff)
	void ambChanged(const QString & newdescr, bool multiwhere, QString where);
};


/// A container for various type of amplifier..
class AmpliContainer : public sottoMenu
{
Q_OBJECT
public:
	AmpliContainer(QWidget *parent, QDomNode config_node);

	/// Sets the index of the list forcing which is the first item to draw.
	void setIndice(char);

private:
	void loadAmplifiers(QDomNode config_node);
};


/*!
 * \class SoundDiffusion
 * \brief This is a class that manage sound diffusion function of the system. 
 * It is composed of two subtree (sottoMenu): amplifiers and sources. 
 * In the first row of the page is shown the source active at a certain 
 * time while in the remaining rows there are all the amplifiers one ca control.
 * \author Davide
 * \date lug 2005
 */
class SoundDiffusion : public Page
{
Q_OBJECT
public:
	SoundDiffusion(QDomNode config_node);
	SoundDiffusion(AudioSources *s, QDomNode config_node);

	/*!
	 * \brief Sets the row's number.
	 */
	void setNumRighe(uchar);

	/*!
	 * \brief Initialize the class.
	 *
	 * It recall the sources' and amplifiers' inizialization procedure.
	 */
	void inizializza();

	/*!
	 * \brief Sets the geometry passing x,y,w,h.
	 */
	void setGeom(int,int,int,int);

	/*!
	 * \brief Changes the navigation bar mode as described in bannFrecce
	 */
	void setNavBarMode(uchar);

	/*!
	 * \brief Recalls a draw for both amplifiers and sources.
	 */
	void draw();

	/*!
	 * \brief Recalls a forced draw for both amplifiers and sources.
	 *
	 * A forced draw force a draw of the subtree(sottoMenu) also if its internal index remain unchanged.
	 */
	void forceDraw();

public slots:
	/*!
	 * \brief Analyze \a Open \a frame to detect which source is active at a certain time.
	 *
	 * When a new source is seen to be active, a change in source is forced an it's redrawn.
	 */
	void gestFrame(char*);

	/*!
	 * \brief sets first source given address
	 */
	void setFirstSource(int addr);

protected:
	/*!
	 * \brief Make sure a source being active before showing the page.
	 */
	virtual void showEvent(QShowEvent *event);

	/*!
	 * \brief Make sure that amplifier index is resetted before hiding.
	 *
	 * Resetting amplifier index guarantees that nex time amplifiers will be shown the first 
	 * banners will be drawn.
	 */
	virtual void hideEvent(QHideEvent *event);

private:
	/*!
	 * \brief Set the "sorgenti" submenu"
	 */
	void setSorgenti(AudioSources *s);
	void init(QDomNode config_node);
	bool isVisual;
	uchar numRighe;
	AudioSources *sorgenti;
	AmpliContainer *amplificatori;

private slots:
	/*!
	 * \brief Emittes the closed signal when amplifiers subtree(sottoMenu) is closed.
	 */
	void fineVis();

signals:
	void closed(SoundDiffusion*);

	/*!
	 * \brief Emitted when there are \a Open \a frame incoming to make them arrive to amplifiers and sources.
	 */
	void gesFrame(char*);
};

#endif // SOUND_DIFFUSION
