/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** diffsonora.h
 **
 **definizioni della pagina di sottoMenu diffusione Sonora
 **
 ****************************************************************/

#ifndef DIFFSONORA_H
#define DIFFSONORA_H

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

private:
	void loadItems(QDomNode config_node);
signals:
	void actSrcChanged(int, int);
};


/// A container for various type of amplifier..
class AmpliContainer : public sottoMenu
{
Q_OBJECT
public:
	AmpliContainer(QWidget *parent);
};


/*!
 * \class diffSonora
 * \brief This is a class that manage sound diffusion function of the system. 
 * It is composed of two subtree (sottoMenu): amplifiers and sources. 
 * In the first row of the page is shown the source active at a certain 
 * time while in the remaining rows there are all the amplifiers one ca control.
 * \author Davide
 * \date lug 2005
 */
class diffSonora : public QWidget
{
Q_OBJECT
public:
	diffSonora(QWidget *parent, QDomNode config_node);
	diffSonora(QWidget *parent, AudioSources *s);
	/*!
	 * \brief Adds an object in the class.
	 *
	 * If it is a source it is added to the sources subtree otherwise it's added to the amplifiers one.
	 * This method reproduce the one implemented in sottoMenù in similar manner.
	 */
	int addItem(char tipo, const QString & nome, char *indirizzo,QList<QString*> &icon_names,
		int periodo=0 , int where=0, const char *ambdescr="");
	int addItem(char tipo, const QString & nome, QList<QString *> *indirizzo,QList<QString*> &icon_names);

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
	void init();
	bool isVisual;
	uchar numRighe;
	AudioSources* sorgenti;
	AmpliContainer* amplificatori;

private slots:
	/*!
	 * \brief Emittes the closed signal when amplifiers subtree(sottoMenu) is closed.
	 */
	void fineVis();

signals:
	/*!
	 * \brief Emitted when the object is closed.
	 */
	void Closed();
	void closed(diffSonora*);

	/*!
	 * \brief Emitted when there are \a Open \a frame incoming to make them arrive to amplifiers and sources.
	 */
	void gesFrame(char*);
};

#endif // DIFFSONORA_H
