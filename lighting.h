/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Lighting section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#ifndef LIGHTING_H
#define LIGHTING_H

#include "sottomenu.h"

#include <QWidget>

class QDomNode;


class Lighting : public sottoMenu
{
Q_OBJECT
public:
	Lighting(QWidget *parent, QDomNode config_node);

	virtual int addItemU(char tipo, const QString &descrizione, void *indirizzo,
		QList<QString*> &icon_names,int periodo = 0, int numFrame = 0,
		char *descr1 = NULL, char *descr2 = NULL, char *descr3 = NULL, char *descr4 = NULL,
		int par3=0, int par4=0,
		QList<QString*> *lt = NULL, QList<scenEvo_cond*> *lc = NULL,
		QString action="", QString light="", QString key="", QString unk="",
		QList<int> sstart = QList<int>(),
		QList<int> sttop = QList<int>(), QString txt1="", QString txt2="", QString txt3="")  { return 0; }

	virtual int addItemU(char tipo, const QString &descrizione, QList<QString> indirizzo,
		QList<QString*> &icon_names,int periodo = 0, int numFrame = 0) { return 0; }

protected:
	virtual void showEvent(QShowEvent *event);

private:
	void init_dimmer();
};

#endif
