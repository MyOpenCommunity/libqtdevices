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

#include <QStringList>
#include <QString>
#include <QWidget>

#define MAX_AMB 9

class QDomNode;


/**
 * \brief This class is made to manage a multimedia source.
 */
class BannerSorgenteMultimedia : public bannCiclaz
{
Q_OBJECT
public:
	BannerSorgenteMultimedia(QWidget *parent, const QDomNode &config_node, QString indirizzo, int where, int nbutt);
	virtual void inizializza(bool forza = false);
	void set_status(bool stat);
	bool get_status();
public slots:
	void gestFrame(char *);
private:
	bool status;
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
protected:
	MultimediaSource source_menu;
};

/**
 * \brief This class is made to manage a multichannel multimedia source.
 */
class BannerSorgenteMultimediaMC : public BannerSorgenteMultimedia
{
Q_OBJECT
public:
	BannerSorgenteMultimediaMC(QWidget *parent, const QDomNode &config_node, QString indirizzo, int where,
		QString icon_onoff, QString icon_cycle, QString icon_settings);
	void inizializza(bool forza = false);
	void setstatusAmb(int Amb, bool status);
	bool statusAmb(int Amb);
	bool get_status();
public slots:
	void gestFrame(char *);
	void attiva();
	void addAmb(QString a);
	void ambChanged(const QString &, bool, QString);

private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
	bool status_Amb[MAX_AMB];
signals:
	void active(int, int);
};

#endif
