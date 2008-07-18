/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentimedia.h
 **
 **definizioni dei vari sorgenti multimediali implementati
 **
 ****************************************************************/

#ifndef SORGENTI_MEDIA_H
#define SORGENTI_MEDIA_H

#include "bannciclaz.h"
#include "multimedia_source.h"

class device;

/**
 * \brief This class is made to manage a multimedia source.
 */
class BannerSorgenteMultimedia : public bannCiclaz
{
Q_OBJECT
public:
	BannerSorgenteMultimedia(QWidget *parent, const char *name, char *indirizzo, int where, int nbutt);
	virtual void inizializza(bool forza = false);
public slots:
	void gestFrame(char *);
	void hide();
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
	void menu();
protected:
	MultimediaSource source_menu;
	device *dev;
};

/**
 * \brief This class is made to manage a multichannel multimedia source.
 */
class BannerSorgenteMultimediaMC : public BannerSorgenteMultimedia
{
Q_OBJECT
public:
	BannerSorgenteMultimediaMC(QWidget *parent, const char *name, char *indirizzo, int where,
		const char *icon_onoff, const char *icon_cycle, const char *icon_settings);
	void inizializza(bool forza = false);
public slots:
	void gestFrame(char *);
	void attiva();
	void addAmb(char *);
	void ambChanged(const QString &, bool, char *);
signals:
	void active(int, int);

private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
};

#endif
