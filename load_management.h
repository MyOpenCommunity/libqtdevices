#ifndef LOAD_MANAGEMENT_H
#define LOAD_MANAGEMENT_H

#include "page.h"

class QDomNode;
class banner;
class QLabel;

class LoadManagement : public BannerPage
{
Q_OBJECT
public:
	LoadManagement(const QDomNode &config_node);

	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};



class LoadDataContent : public QWidget
{
Q_OBJECT
public:
	LoadDataContent();

public slots:
	void currentConsumptionChanged(int new_value);

private:
	QLabel *current_consumption;

signals:
	// TODO: is this interface right?
	void firstReset();
	void secondReset();
};

class LoadDataPage : public Page
{
Q_OBJECT
public:
	typedef LoadDataContent ContentType;
	// TODO: add LoadManagement device
	LoadDataPage(const QDomNode &config_node);

private:
	LoadDataContent *content;
};



class DeactivationTimeContent : public QWidget
{
Q_OBJECT
public:
	DeactivationTimeContent();
};

class DeactivationTimePage : public Page
{
Q_OBJECT
public:
	typedef DeactivationTimeContent ContentType;
	DeactivationTimePage(const QDomNode &config_node);

private slots:
	void sendDeactivateDevice();

private:
	DeactivationTimeContent *content;
};

#endif // LOAD_MANAGEMENT_H
