#ifndef LOAD_MANAGEMENT_H
#define LOAD_MANAGEMENT_H

#include "page.h"

#include <QSignalMapper>

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


class ConfirmationPage : public Page
{
Q_OBJECT
public:
	ConfirmationPage(const QString &text);

signals:
	void accept();
	void cancel();
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
	QSignalMapper mapper;

signals:
	void resetActuator(int);
};

class LoadDataPage : public Page
{
Q_OBJECT
public:
	// TODO: add LoadManagement device
	LoadDataPage(const QDomNode &config_node);

private:
	LoadDataContent *content;
	int reset_number;

private slots:
	void resetRequested(int which);
	void reset();
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
	DeactivationTimePage(const QDomNode &config_node);

private slots:
	void sendDeactivateDevice();

private:
	DeactivationTimeContent *content;
};

#endif // LOAD_MANAGEMENT_H
