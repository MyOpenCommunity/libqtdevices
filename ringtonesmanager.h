#ifndef RINGTONESMANAGER_H
#define RINGTONESMANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QHash>


enum RingtoneType
{
	RINGTONE_PE1, // call from external unit with ringtone 1
	RINGTONE_PE2,
	RINGTONE_PE3,
	RINGTONE_PE4,
	RINGTONE_PI_INTERCOM,
	RINGTONE_PE_INTERCOM
};


/**
 * Manages ringtones globally.
 *
 * Load/store ringtone preferences from/to flash, play ringtones when needed.
 */
class RingtonesManager : public QObject
{
Q_OBJECT
public:
	RingtonesManager();

	/**
	 * Play the current ringtone set for the given ringtone type.
	 */
	void playRingtone(RingtoneType t);
	// play the ringtone set for the given id
	void playRingtone(int ring);

	/**
	 * Set a new ringtone for a given ringtone type.
	 */
	void setRingtone(RingtoneType t, int ring);

	/**
	 * Return the current ringtone for a given type.
	 */
	int getRingtone(RingtoneType t);

	int getRingtonesNumber();

private:
	QList<QFileInfo> scanDirectory(const QDir &dir, const QStringList &filters);
	QString typeToFilePath(RingtoneType t);
	QHash<int, QString> ringtone_to_file;
	QHash<RingtoneType, int> type_to_ringtone;
};

namespace bt_global { extern RingtonesManager *ringtones; };

#endif // RINGTONESMANAGER_H
