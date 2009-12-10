#ifndef SINGLECHOICECONTENT_H
#define SINGLECHOICECONTENT_H

#include "bannercontent.h"

#include <QButtonGroup>

class SingleChoiceContent : public BannerContent
{
Q_OBJECT
public:
	SingleChoiceContent(QWidget *parent = 0);

	// Get the active button
	int checkedId() const;
	// add a new banner to the widget
	void addBanner(const QString &text, int id);

	// returns the list of buttons for customization
	QList<QAbstractButton*> getButtons();

public slots:
	// Set the active button
	void setCheckedId(int id);

signals:
	void bannerSelected(int id);

private:
	QButtonGroup buttons;
};

#endif // SINGLECHOICECONTENT_H
