#ifndef ENERGY_DATA_H
#define ENERGY_DATA_H

#include "sottomenu.h"

class QDomNode;


class EnergyData : public sottoMenu
{
Q_OBJECT
public:
	EnergyData(const QDomNode &config_node);

private:
	void loadTypes(const QDomNode &config_node);
};


class EnergyCost : public sottoMenu
{
Q_OBJECT
public:
	EnergyCost();
};

class EnergyInterface : public sottoMenu
{
Q_OBJECT
public:
	EnergyInterface(const QDomNode &config_node, QWidget *parent = 0);
private:
	void loadItems(const QDomNode &config_node);
};

#endif // ENERGY_DATA_H
