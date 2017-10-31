#ifndef INIPARSER_H
#define INIPARSER_H

#include <QHash>
#include <QVariant>

class QString;

class IniParser
{
public:
    bool load(const QString& path, QString* error);

    QStringList sectionNames() const;

    QString get(const QString& sectionName, const QString& key) const;

private:
    using Section = QHash<QString, QString>;
    QHash<QString, Section> mSections;
};

#endif /* INIPARSER_H */
