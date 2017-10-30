#include "iniparser.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>

bool IniParser::load(const QString& path, QString* error)
{
    Q_ASSERT(error);
    QFile file(path);
    if (!file.exists()) {
        *error = path + " does not exist";
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        *error = "Could not open " + path;
        return false;
    }

    QTextStream stream(&file);
    Section* section = nullptr;

    int lineNumber = 0;
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        ++lineNumber;
        // Skip empty lines and comments
        if (line.isEmpty() || line[0] == '#') {
            continue;
        }
        if (line.at(0) == '[') {
            // We reach the beginning of a new group
            QString sectionName = line.mid(1, line.length() - 2);
            mSections[sectionName] = Section();
            section = &mSections[sectionName];
        } else if (!section) {
            *error = QString("Line %1: found content before a section").arg(lineNumber);
            return false;
        } else {
            // Read a key/value pair
            int idx = line.indexOf('=');
            if (idx == -1) {
                *error = QString("Line %1: missing '=' separator in '%2'").arg(lineNumber).arg(line);
                return false;
            }
            QString key = line.left(idx).trimmed();
            QString value = line.mid(idx + 1).trimmed();
            (*section)[key] = value;
        }
    }

    return true;
}

QStringList IniParser::sectionNames() const
{
    return mSections.keys();
}

QString IniParser::get(const QString& section, const QString& key) const
{
    return mSections.value(section).value(key);
}
