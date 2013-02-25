/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "generic_functions.h"
#include "xml_functions.h"
#include "hardware_functions.h"

#include <QMapIterator>
#include <QTextStream>
#include <QDomElement>
#include <QtGlobal> // Q_ASSERT_X, qRound
#include <QDomNode>
#include <QWidget>
#include <QRegExp>
#include <QDebug>
#include <QDir>
#include <QDate>
#include <QDateTime>
#include <QProcess>
#include <QImageReader>
#include <QTimer>
#include <QtConcurrentRun>

#include <fcntl.h> // open
#include <stdio.h> // rename

#ifdef BT_HARDWARE_DM365
#include <status.h> // set_mystatus_conf_status
#endif

#define DELAYED_WRITE_INTERVAL 3000

// The maximum width & height supported for images (not used for jpeg)
#define IMAGE_MAX_WIDTH 3200
#define IMAGE_MAX_HEIGHT 2100


namespace
{
	QString getDateFormat(char separator = '.')
	{
		QString format;
		bool ok;
		int date_format = (*bt_global::config)[DATE_FORMAT].toInt(&ok);

		// Default in case of error
		if (!ok)
			date_format = EUROPEAN_DATE;

		switch (date_format)
		{
		case EUROPEAN_DATE:
			format = "dd.MM.yy";
			break;
		case USA_DATE:
			format = "MM.dd.yy";
			break;
		case YEAR_FIRST:
			format = "yy.MM.dd";
			break;
		default:
			Q_ASSERT_X(false, "getDateFormat", qPrintable(QString("Format of date %1 not supported!").arg(date_format)));
		}

		if (separator != '.')
			format.replace('.', separator);

		return format;
	}




	#define ARRAY_SIZE(x) int(sizeof(x) / sizeof((x)[0]))
	const char *audio_files[] = {"m3u", "mp3", "wav", "ogg", "wma", 0};
	const char *video_files[] = {"mpg", "avi", "mp4", 0};
	const char *image_files[] = {"png", "gif", "jpg", "jpeg", "tif", "tiff", 0};
#ifdef BUILD_EXAMPLES
	const char *pdf_files[]   = {"pdf", 0};
#endif

	// transforms an extension to a pattern (es. "wav" -> "*.[wW][aA][vV]")
	void addFilters(QStringList &filters, const char **extensions, int size)
	{
		for (int i = 0; extensions[i] != 0; ++i)
		{
			QString pattern = "*.";

			for (const char *c = extensions[i]; *c; ++c)
			{
				QChar letter(*c);
				pattern += QString("[%1%2]").arg(letter).arg(letter.toUpper());
			}
			filters.append(pattern);
		}
	}
}

EntryInfo::EntryInfo(const QString &_name, EntryInfo::Type _type, const QString &_path, const EntryInfo::Metadata &_metadata)
	: name(_name), type(_type), path(_path), metadata(_metadata)
{
	is_null = false;
}

EntryInfo::EntryInfo()
{
	is_null = true;
}

bool EntryInfo::isNull()
{
	return is_null;
}


QStringList getFileExtensions(EntryInfo::Type type)
{
	QStringList exts;
	const char **files = 0;

	switch (type)
	{
	case EntryInfo::AUDIO:
		files = audio_files;
		break;
	case EntryInfo::VIDEO:
		files = video_files;
		break;
	case EntryInfo::IMAGE:
		files = image_files;
		break;
#ifdef BUILD_EXAMPLES
	case EntryInfo::PDF:
		files = pdf_files;
		break;
#endif
	case EntryInfo::UNKNOWN:
	case EntryInfo::DIRECTORY:
		break;
	default:
		Q_ASSERT_X(false, "getFileExtensions", qPrintable(QString("type %1 not handled").arg(type)));
	}

	if (files)
	{
		for (int i = 0; files[i] != 0; ++i)
			exts.append(files[i]);
	}

	return exts;
}

QStringList getFileFilter(EntryInfo::Type type)
{
	QStringList filters;
	const char **files = 0;

	switch (type)
	{
	case EntryInfo::UNKNOWN:
	case EntryInfo::DIRECTORY:
		break;
	case EntryInfo::AUDIO:
		files = audio_files;
		break;
	case EntryInfo::VIDEO:
		files = video_files;
		break;
	case EntryInfo::IMAGE:
		files = image_files;
		break;
#ifdef BUILD_EXAMPLES
	case EntryInfo::PDF:
		files = pdf_files;
		break;
#endif
	default:
		Q_ASSERT_X(false, "getFileFilter", qPrintable(QString("type %1 not handled").arg(type)));
	}

	if (files)
		addFilters(filters, files, ARRAY_SIZE(files));

	return filters;
}

QString getBostikName(const QString &name, const QString &suffix)
{
	int pos = name.indexOf(".");
	if (pos != -1)
		return name.left(pos) + suffix + name.mid(pos);

	return QString();
}

QString getPressName(QString name)
{
	return getBostikName(name, "p");
}

// reads the configuration file into a document
bool prepareWriteCfgFile(QDomDocument &doc, const QString &filename)
{
	QFile config_file(filename);
	if (!config_file.open(QIODevice::ReadOnly))
		return false;

	if (!doc.setContent(&config_file))
	{
		config_file.close();
		return false;
	}
	config_file.close();

	return true;
}

// writes the configuratio to a temporary file and then renames it to the definitive name
bool writeCfgFile(const QDomDocument &doc, const QString &filename)
{
#ifdef BT_HARDWARE_DM365
	const QString tmp_filename = "/var/tmp/appoggio.xml";
#else
	const QString tmp_filename = "cfg/appoggio.xml";
#endif

	if (QFile::exists(tmp_filename))
		QFile::remove(tmp_filename);

	QFile tmp_file(tmp_filename);
	if (!tmp_file.open(QIODevice::WriteOnly))
		return false;

	// Use a text stream to handle unicode properly
	QTextStream tmp_stream(&tmp_file);
	tmp_stream.setCodec("UTF-8");
	QString xml = doc.toString(0);
	// Other processes don't support empty tags on xml, so we replace it with tags
	// with empty content.
	QRegExp empty_tag_reg("<([^>]+)/>");
	empty_tag_reg.setMinimal(true);
	tmp_stream << xml.replace(empty_tag_reg, "<\\1></\\1>");
	tmp_stream.flush();
	tmp_file.close();

	// QDir::rename fails if destination file exists so we use rename system call
	if (!::rename(qPrintable(tmp_file.fileName()), qPrintable(filename)))
	{
#ifdef BT_HARDWARE_DM365
		set_mystatus_conf_status(QString::number(CONF_UPDATED).toAscii().data());
#else
		// Write an empty file to warn other process that the configuration file has changed.
		createFlagFile(FILE_CHANGE_CONF);
#endif
		return true;
	}

	return false;
}

// updates a single configuration item in the given document
void doSetCfgValue(QDomDocument &doc, QMap<QString, QString> data, int item_id)
{
	QDomElement gui = getElement(doc.documentElement(), "gui");
	QDomNode n;

	foreach (const QDomNode &page, getChildren(gui, "page"))
	{
		n = getChildWithId(page, QRegExp("item"), "itemID", item_id);
		if (!n.isNull())
			break;
	}
	Q_ASSERT_X(!n.isNull(), "setCfgValue", qPrintable(QString("No object found with id %1").arg(item_id)));

	// TODO maybe refactor & move to xml_functions.cpp/h
	QMapIterator<QString, QString> it(data);
	while (it.hasNext())
	{
		it.next();
		QDomElement el = getElement(n, it.key());
		Q_ASSERT_X(!el.isNull(), "setCfgValue", qPrintable(QString("No element found: %1").arg(it.key())));
		// To replace the text of the element
		el.replaceChild(doc.createTextNode(it.value()), el.firstChild());
	}
}

// updates a global configuration item in the given document
void doSetGlobalCfgValue(QDomDocument &doc, const QString &root_name, QMap<QString, QString> data, const QString &tag_name, int id_value)
{
	QDomNode root_node = getElement(doc.documentElement(), root_name);

	QDomNode n = findXmlNode(root_node, QRegExp(".*"), tag_name, id_value);
	Q_ASSERT_X(!n.isNull(), "setGlobalCfgValue", qPrintable(QString("No object found with id %1").arg(id_value)));

	// TODO maybe refactor & move to xml_functions.cpp/h
	QMapIterator<QString, QString> it(data);
	while (it.hasNext())
	{
		it.next();
		QDomElement el = getElement(n, it.key());
		Q_ASSERT_X(!el.isNull(), "setGlobalCfgValue", qPrintable(QString("No element found: %1").arg(it.key())));
		// To replace the text of the element
		el.replaceChild(doc.createTextNode(it.value()), el.firstChild());
	}
}

// queues configuration files writes and only performs them in batch after some time
class DelayedConfigWrite : public QTimer // this class should be derived from QObject
{
Q_OBJECT

	// holds the values for a single configuration item
	struct ItemValue
	{
		QMap<QString, QString> data;
		int item_id;

		ItemValue(QMap<QString, QString> _data, int _item_id) :
			data(_data), item_id(_item_id) {}

		bool isSameItem(const ItemValue &other)
		{
			return item_id == other.item_id;
		}
	};

	// holds the values for a global configuration item
	struct GlobalValue
	{
		QString root_name;
		QMap<QString, QString> data;
		QString tag_name;
		int id_value;

		GlobalValue(const QString &_root_name, QMap<QString, QString> _data, const QString &_tag_name, int _id_value) :
			root_name(_root_name), data(_data), tag_name(_tag_name), id_value(_id_value) {}

		bool isSameItem(const GlobalValue &other)
		{
			return root_name == other.root_name && tag_name == other.tag_name && id_value == other.id_value;
		}
	};

	// update list for a configuration file
	struct FileQueue
	{
		QList<ItemValue> item_values;
		QList<GlobalValue> global_values;
	};

public:
	DelayedConfigWrite();

	// enqueue configuration values update and restart the write timer
	void queueCfgValue(QMap<QString, QString> data, int item_id, const QString &filename);
	void queueGlobalValue(const QString &root_name, QMap<QString, QString> data, const QString &tag_name, int id_value, const QString &filename);

private slots:
	void writeConfig();

private:
	static void asyncWriteConfig(QHash<QString, FileQueue> queued_actions);

private:
	QHash<QString, FileQueue> queued_actions;
};

Q_GLOBAL_STATIC(DelayedConfigWrite, delayed_config)


DelayedConfigWrite::DelayedConfigWrite()
{
	connect(this, SIGNAL(timeout()), SLOT(writeConfig()));
	setInterval(DELAYED_WRITE_INTERVAL);
	setSingleShot(true);
}

// adds a new value to the write queue, merging updates to the same configuration item
template<class T>
void addValue(QList<T> &array, const T &value)
{
	for (int i = 0; i < array.size(); ++i)
	{
		if (array[i].isSameItem(value))
		{
			foreach (QString key, value.data.keys())
				array[i].data[key] = value.data[key];

			return;
		}
	}

	array.append(value);
}

void DelayedConfigWrite::queueCfgValue(QMap<QString, QString> data, int item_id, const QString &filename)
{
	addValue(queued_actions[filename].item_values, ItemValue(data, item_id));
	start();
}

void DelayedConfigWrite::queueGlobalValue(const QString &root_name, QMap<QString, QString> data, const QString &tag_name, int id_value, const QString &filename)
{
	addValue(queued_actions[filename].global_values, GlobalValue(root_name, data, tag_name, id_value));
	start();
}

void DelayedConfigWrite::writeConfig()
{
	QtConcurrent::run(asyncWriteConfig, queued_actions);
	queued_actions.clear();
}

Q_GLOBAL_STATIC(QMutex, avoid_mayhem) // just to avoid multiple executions of asyncWriteConfig

void DelayedConfigWrite::asyncWriteConfig(QHash<QString, FileQueue> queued_actions)
{
	QMutexLocker locker(avoid_mayhem());

#ifdef DEBUG
	QTime t;
	t.start();
#endif

	foreach (QString filename, queued_actions.keys())
	{
		const FileQueue &values = queued_actions[filename];

		qDebug() << "Writing" << values.global_values.size() << "global values," << values.item_values.size() << "item value to" << filename;

		QDomDocument doc("config_document");
		if (!prepareWriteCfgFile(doc, filename))
		{
			qWarning("Error writing to %s", qPrintable(filename));
			continue;
		}

		foreach (GlobalValue val, queued_actions[filename].global_values)
			doSetGlobalCfgValue(doc, val.root_name, val.data, val.tag_name, val.id_value);

		foreach (ItemValue val, queued_actions[filename].item_values)
			doSetCfgValue(doc, val.data, val.item_id);

		if (!writeCfgFile(doc, filename))
		{
			qWarning("Error writing to %s", qPrintable(filename));
			continue;
		}
	}

#ifdef DEBUG
	qDebug() << "Writing configuration in:" << t.elapsed() << "ms";
#endif
}

void setCfgValue(QMap<QString, QString> data, int item_id, const QString &filename)
{
	if (!bt_global::config->contains(INIT_COMPLETE))
	{
		qDebug() << "Not writing to configuration during init";
		return;
	}
	delayed_config()->queueCfgValue(data, item_id, filename);
}

// TODO rewrite setCfgValue using setGlobalCfgValue
void setGlobalCfgValue(const QString &root_name, QMap<QString, QString> data, const QString &tag_name, int id_value, const QString &filename)
{
	if (!bt_global::config->contains(INIT_COMPLETE))
	{
		qDebug() << "Not writing to configuration during init";

		return;
	}

	delayed_config()->queueGlobalValue(root_name, data, tag_name, id_value, filename);
}

void setCfgValue(QString field, QString value, int item_id, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = value;
	setCfgValue(m, item_id, filename);
}

void setCfgValue(QString field, int value, int item_id, const QString &filename)
{
	QMap<QString, QString> m;
	m[field] = QString::number(value);
	setCfgValue(m, item_id, filename);
}

int localVolumeToAmplifier(int vol)
{
	return qRound(vol * 31 / 8.0);
}

int scsToLocalVolume(int vol)
{
	if (vol < 0 || vol > 31)
		return -1;

	return qRound(vol * 8 / 31.0);
}


int scsToGraphicalVolume(int vol)
{
	// TODO remove after aligning image names
#ifdef LAYOUT_TS_10
	if (vol < 0 || vol > 31)
		return -1;

	return vol * 8 / 31;
#else
	if (vol < 0)
		return -1;
	if (vol <= 3)
		return 1;
	if (vol <= 7)
		return 2;
	if (vol <= 11)
		return 3;
	if (vol <= 14)
		return 4;
	if (vol <= 17)
		return 5;
	if (vol <= 20)
		return 6;
	if (vol <= 23)
		return 7;
	if (vol <= 27)
		return 8;
	if (vol <= 31)
		return 9;
	return -1;
#endif
}

QString DateConversions::formatDateConfig(const QDate &date, char separator)
{
	return formatDateConfig(date, ALL, separator);
}

QString DateConversions::formatDateConfig(const QDate &date, DateComponents components, char separator)
{
	QStringList components_list;
	bool ok;
	int date_format = (*bt_global::config)[DATE_FORMAT].toInt(&ok);

	// Default in case of error
	if (!ok)
		date_format = EUROPEAN_DATE;

	switch (date_format)
	{
	case EUROPEAN_DATE:
		if (components.testFlag(DAYS)) components_list << "dd";
		if (components.testFlag(MONTHS)) components_list << "MM";
		if (components.testFlag(YEARS)) components_list << "yy";
		break;
	case USA_DATE:
		if (components.testFlag(MONTHS)) components_list << "MM";
		if (components.testFlag(DAYS)) components_list << "dd";
		if (components.testFlag(YEARS)) components_list << "yy";
		break;
	case YEAR_FIRST:
		if (components.testFlag(YEARS)) components_list << "yy";
		if (components.testFlag(MONTHS)) components_list << "MM";
		if (components.testFlag(DAYS)) components_list << "dd";
		break;
	default:
		Q_ASSERT_X(false, "formatDateConfig", qPrintable(QString("Format of date %1 not supported!").arg(date_format)));
	}

	return date.toString(components_list.join(QString(separator)));
}

QString DateConversions::formatDateTimeConfig(const QDateTime &datetime, char separator)
{
	return DateConversions::formatDateConfig(datetime.date(), ALL, separator) + datetime.time().toString(" HH:mm");
}

QDate DateConversions::getDateConfig(const QString &date, char separator)
{
	QString format = getDateFormat(separator);

	// The format string has a two digit for the year so we lose 100 years in the
	// conversion. We re-add them now.
	return QDate::fromString(date, format).addYears(100);
}

QDateTime DateConversions::getDateTimeConfig(const QString &datetime, char separator)
{
	QString format = getDateFormat(separator) + " HH:mm";

	// The format string has a two digit for the year so we lose 100 years in the
	// conversion. We re-add them now.
	return QDateTime::fromString(datetime, format).addYears(100);
}


bool smartExecute(const QString &program, QStringList args)
{
#if DEBUG
	QTime t;
	t.start();
	bool ret = QProcess::execute(program, args);
	qDebug() << "Executed:" << program << args.join(" ") << "in:" << t.elapsed() << "ms";
	return ret;
#else
	return QProcess::execute(program, args);
#endif
}

bool silentExecute(const QString &program, QStringList args)
{
	args << "> /dev/null" << "2>&1";
	return smartExecute(program, args);
}

bool checkImageSize(const QString &path)
{
	QFile file(path);

	QString extension = QFileInfo(path).suffix().toLower();
	// When we load a jpeg image from disk Qt uses an optimization that loads
	// the image using the scaled size (and not the original size). So we have
	// no problem (and no limit) to load jpeg images.
	if (extension == "jpg" || extension == "jpeg")
		return true;

	if (!file.open(QFile::ReadOnly))
	{
		qDebug() << "checkImageSize: can't open" << path;
		return false;
	}

	QImageReader rd(&file);
	QSize sz = rd.size();

	if (!sz.isValid())
	{
		qDebug() << "checkImageSize: invalid size for" << path;
		return false;
	}

	if (sz.width() > IMAGE_MAX_WIDTH || sz.height() > IMAGE_MAX_HEIGHT)
		return false;

	return true;
}

bool checkImageMemory(const QString &path)
{
	QString extension = QFileInfo(path).suffix().toLower();
	// See the comment on checkImageSize
	if (extension == "jpg" || extension == "jpeg")
		return true;

	QFile file(path);

	if (!file.open(QFile::ReadOnly))
	{
		qDebug() << "checkImageMemory: can't open" << path;
		return false;
	}

	QImageReader rd(&file);
	QSize sz = rd.size();

	int byte_per_pixel = 4;
	if (rd.imageFormat() != QImage::Format_RGB32)
		qWarning() << "checkImageMemory: Unknown image format";

	if (!sz.isValid())
	{
		qDebug() << "checkImageMemory: invalid size for" << path;
		return false;
	}

	int required = sz.width() * sz.height() * byte_per_pixel / 1024;
	int available = getMemFree();

#if TRACK_IMAGES_MEMORY
	qDebug() << "checkImageMemory: memory required" << required <<  "for" << path << "available:" << available;
#endif

	// From empirical tests we found that the actual memory usage is about 140% - 160% of
	// the size calculated (every image is loaded from the disk, uncompressed and then
	// scaled to fit the screen).
	if (required * 1.6 > available)
		return false;

	return true;
}

QImage loadImage(const QString &image, int width, int height)
{
	QImageReader r(image);
	QSize s = r.size();
	int w = width > 0 ? width : maxWidth();
	int h = height > 0 ? height : maxHeight();
	s.scale(w, h, Qt::KeepAspectRatio);
	r.setScaledSize(s);
	return r.read();
}

volatile bool stop_track_memory;

void checkMemFree()
{
	static int memfree;
	memfree = getMemFree();
	while (!stop_track_memory)
	{
		memfree = qMin(memfree, getMemFree());
		usleep(50);
	}
	qDebug() << "Minimum free memory:" << memfree;
}

void startTrackMemory()
{
	stop_track_memory = false;

	QtConcurrent::run(&checkMemFree);
}

void stopTrackMemory()
{
	stop_track_memory = true;
}

#include "generic_functions.moc"
