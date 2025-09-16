#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include "passwordsettings.h"
#include <QStringList>
#include <QRandomGenerator>

class PasswordGenerator
{
public:
    void setSettings(const PasswordSettings &s) { st_ = s; }
    QStringList generate();

private:
    QString generatePlain(int len) const;
    QString generateReadable(int len) const;
    QString generateMoreReadable(int len) const;

    bool allowed(QChar c, const QString &current) const;

    PasswordSettings st_;
    mutable QRandomGenerator rng_ { QRandomGenerator::securelySeeded() };

    const QString vowels_ = "aeiouy";
    const QString consonants_ = "bcdfghjklmnpqrstvwxz";
};

#endif // PASSWORDGENERATOR_H
