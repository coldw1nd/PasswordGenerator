#include "passwordsettings.h"

void PasswordSettings::rebuildIncludeString()
{
    QString s;
    if (lower_) s += "abcdefghijklmnopqrstuvwxyz";
    if (upper_) s += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (digits_) s += "0123456789";
    if (symbols_) s += "!@#$%^&*()_+-=[]{}|;:,.<>?";

    includeChars_ = s;
}

PasswordSettings::PasswordSettings() {}

#define SIMPLE_GET(name,type) type PasswordSettings::name() const { return name##_; }
SIMPLE_GET(minLen,int)
SIMPLE_GET(maxLen,int)
SIMPLE_GET(count,int)
SIMPLE_GET(maxRepeat,int)
SIMPLE_GET(includeChars,QString)
SIMPLE_GET(excludeChars,QString)
SIMPLE_GET(preventRepeat,bool)
SIMPLE_GET(onlyLower,bool)
SIMPLE_GET(onlyUpper,bool)
SIMPLE_GET(readable,bool)
SIMPLE_GET(moreReadable,bool)
bool PasswordSettings::useDigits() const { return digits_; }
bool PasswordSettings::useLower() const { return lower_; }
bool PasswordSettings::useUpper() const { return upper_; }
bool PasswordSettings::useSymbols() const { return symbols_; }

void PasswordSettings::setMinLen(int v) { minLen_ = qMax(0, v); }
void PasswordSettings::setMaxLen(int v) { maxLen_ = qMax(0, v); }
void PasswordSettings::setCount(int v) { count_ = qMax(0, v); }
void PasswordSettings::setMaxRepeat(int v) { maxRepeat_ = qMax(0, v); }

void PasswordSettings::setInclude(const QString &s) { includeChars_ = s; }
void PasswordSettings::setExclude(const QString &s) { excludeChars_ = s; }
void PasswordSettings::setPreventRepeat(bool v) { preventRepeat_ = v; }

void PasswordSettings::setOnlyLower(bool v)
{
    onlyLower_ = v;
    if (v) onlyUpper_ = false;
}
void PasswordSettings::setOnlyUpper(bool v)
{
    onlyUpper_ = v;
    if (v) onlyLower_ = false;
}
void PasswordSettings::setReadable(bool v)
{
    readable_ = v;
    if (v) moreReadable_ = false;
}
void PasswordSettings::setMoreReadable(bool v)
{
    moreReadable_ = v;
    if (v) readable_ = false;
}

void PasswordSettings::setUseDigits(bool v) { digits_ = v; rebuildIncludeString(); }
void PasswordSettings::setUseLower(bool v) { lower_ = v; rebuildIncludeString(); }
void PasswordSettings::setUseUpper(bool v) { upper_ = v; rebuildIncludeString(); }
void PasswordSettings::setUseSymbols(bool v) { symbols_ = v; rebuildIncludeString(); }

QJsonObject PasswordSettings::toJson() const
{
    QJsonObject o;
    o["minLen"] = minLen_; o["maxLen"] = maxLen_; o["count"] = count_;
    o["maxRepeat"] = maxRepeat_;
    o["include"] = includeChars_; o["exclude"] = excludeChars_;

    o["preventRepeat"] = preventRepeat_;
    o["onlyLower"] = onlyLower_; o["onlyUpper"] = onlyUpper_;
    o["readable"] = readable_; o["moreReadable"] = moreReadable_;

    o["digits"] = digits_; o["lower"] = lower_;
    o["upper"] = upper_; o["symbols"] = symbols_;
    return o;
}

void PasswordSettings::fromJson(const QJsonObject &o)
{
    setMinLen(o["minLen"].toInt(minLen_));
    setMaxLen(o["maxLen"].toInt(maxLen_));
    setCount(o["count"].toInt(count_));
    setMaxRepeat(o["maxRepeat"].toInt(maxRepeat_));

    setUseDigits(o["digits"].toBool(digits_));
    setUseLower(o["lower"].toBool(lower_));
    setUseUpper(o["upper"].toBool(upper_));
    setUseSymbols(o["symbols"].toBool(symbols_));

    setInclude(o["include"].toString(includeChars_));
    setExclude(o["exclude"].toString(excludeChars_));

    setPreventRepeat(o["preventRepeat"].toBool(preventRepeat_));

    setOnlyLower(o["onlyLower"].toBool(onlyLower_));
    setOnlyUpper(o["onlyUpper"].toBool(onlyUpper_));
    setReadable(o["readable"].toBool(readable_));
    setMoreReadable(o["moreReadable"].toBool(moreReadable_));
}
