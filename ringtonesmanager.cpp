#include "ringtonesmanager.h"
#include "hardware_functions.h"

#include <QStringList>
#include <QDebug>
#include <QDir>

#include <unistd.h> // lseek, read
#include <fcntl.h>  // open

// TODO: this is not absolute for testing
#define RINGTONE_DIR "cfg/extra/2/"

RingtonesManager::RingtonesManager()
{
	QStringList filters;
	filters << "*.[wW][[aA][vV]";
	int id = 0;
	QDir d(RINGTONE_DIR);
	foreach (const QFileInfo &fi, scanDirectory(d, filters))
	{
		ringtone_to_file[id] = fi.filePath();
		qDebug() << fi.filePath();
		++id;
	}
	// TODO: read ringtones from conf file
}

void RingtonesManager::playRingtone(RingtoneType t)
{
	playSound(typeToFilePath(t));
}

void RingtonesManager::playRingtone(int ring)
{
	Q_ASSERT_X(ring >= 0 && ring < ringtone_to_file.size(), "RingtonesManager::playRingtone(int)",
		"Given ringtone is outside valid range.");
	playSound(ringtone_to_file[ring]);
}

void RingtonesManager::stopRingtone()
{
	stopSound();
}

void RingtonesManager::setRingtone(RingtoneType t, int ring)
{
	Q_ASSERT_X(ring >= 0 && ring < ringtone_to_file.size(), "RingtonesManager::setRingtone",
		"Given ringtone is outside valid range.");
	type_to_ringtone[t] = ring;
	// TODO: save the map to conf
}

int RingtonesManager::getRingtone(RingtoneType t)
{
	return type_to_ringtone[t];
}

int RingtonesManager::getRingtonesNumber()
{
	return ringtone_to_file.size();
}

QString RingtonesManager::typeToFilePath(RingtoneType t)
{
	return ringtone_to_file[type_to_ringtone[t]];
}

// TODO: this can be shared with AudioFileSelector::browseFiles()?
QList<QFileInfo> RingtonesManager::scanDirectory(const QDir &dir, const QStringList &filters)
{
	QList<QFileInfo> temp_files_list;

	foreach (const QFileInfo &fi, dir.entryInfoList(filters))
	{
		if (fi.fileName() != "." && fi.fileName() != "..")
			temp_files_list.append(fi);
	}

	return temp_files_list;
}

RingtonesManager *bt_global::ringtones = 0;
