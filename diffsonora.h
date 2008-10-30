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

#include <QWidget>
#include <QList>
#include <QString>

class banner;

class sottoMenu;

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
	diffSonora(QWidget *parent=0, sottoMenu *_sorgenti=0);
	/*!
	 * \brief Adds an object in the class.
	 *
	 * If it is a source it is added to the sources subtree otherwise it's added to the amplifiers one.
	 * This method reproduce the one implemented in sottoMenù in similar manner.
	 */
	int addItemU(char tipo, const QString & nome, void* indirizzo,QList<QString*> &icon_names,
		int periodo=0 , int where=0, char *ambdescr="");
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
	sottoMenu* amplificatori;
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
	/*!
	 * \brief Emitted when there are \a Open \a frame outcoming from amplifiers or sources to make them arrive to openClient.
	 */
	void sendFrame(char*);
	void sendInit(char*);
	/*!
	 * \brief Emitted when there is a banner forcing (de)freezing state.
	 */
	void freeze(bool);
	/*!
	 * \brief Emitted thought sources and amplifiers when the device is going into (de)freezing state.
	 */
	void freezed(bool);

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
	/*!
	 * \brief sets isVisual to false and emits freezed signal
	 */
	void freezed_handler(bool);

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

private slots:
	/*!
	 * \brief Emittes the closed signal when amplifiers subtree(sottoMenu) is closed.
	 */
	void fineVis();

private:
	/*!
	 * \brief Set the "sorgenti" submenu"
	 */
	void setSorgenti(sottoMenu *);
	bool isVisual;
	uchar numRighe;
	sottoMenu* sorgenti;
};

#endif // DIFFSONORA_H
