#ifndef PASSWORDSETTINGS_H
#define PASSWORDSETTINGS_H

#include <QString>
#include <QJsonObject>

class PasswordSettings
{
public:
    PasswordSettings();

    int minLen() const;
    int maxLen() const;
    int count() const;
    int maxRepeat() const;

    QString includeChars() const;
    QString excludeChars() const;

    bool preventRepeat() const;
    bool onlyLower() const;
    bool onlyUpper() const;
    bool readable() const;
    bool moreReadable() const;

    bool useDigits() const;
    bool useLower() const;
    bool useUpper() const;
    bool useSymbols() const;

    void setMinLen(int v);
    void setMaxLen(int v);
    void setCount(int v);
    void setMaxRepeat(int v);
    void setInclude(const QString &s);
    void setExclude(const QString &s);
    void setPreventRepeat(bool v);
    void setOnlyLower(bool v);
    void setOnlyUpper(bool v);
    void setReadable(bool v);
    void setMoreReadable(bool v);

    void setUseDigits(bool v);
    void setUseLower(bool v);
    void setUseUpper(bool v);
    void setUseSymbols(bool v);

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &o);

private:
    void rebuildIncludeString();

    int  minLen_ = 8;
    int  maxLen_ = 12;
    int  count_ = 1;
    int  maxRepeat_ = 0;

    QString includeChars_ =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
    QString excludeChars_;

    bool preventRepeat_ = false;
    bool onlyLower_ = false;
    bool onlyUpper_ = false;

    bool readable_ = false;
    bool moreReadable_ = false;

    bool digits_ = true;
    bool lower_ = true;
    bool upper_ = true;
    bool symbols_ = true;
};

#endif // PASSWORDSETTINGS_H
