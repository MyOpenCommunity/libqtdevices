#ifndef SINGLECHOICECONTENT_H
#define SINGLECHOICECONTENT_H

#include "bannercontent.h"
#include "bann2_buttons.h" // Bann2Buttons

#include <QButtonGroup>


class CheckableBanner : public Bann2Buttons
{
public:
	CheckableBanner(const QString &text, const QString &right_icon = QString());

	BtButton *getButton();
};


namespace SingleChoice
{
	CheckableBanner *createBanner(const QString &text, const QString &right_icon=QString());
}


class SingleChoiceContent : public BannerContent
{
Q_OBJECT
public:
	SingleChoiceContent();

	// Get the active button
	int checkedId() const;
	// add a new banner to the widget
	void addBanner(CheckableBanner *bann, int id);

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
