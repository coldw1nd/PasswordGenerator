#include "passwordgenerator.h"

static const QString ERR_NO_SYMBOLS = QStringLiteral("Ошибка: Нет доступных символов");
static const QString ERR_READABLE = QStringLiteral("Ошибка: Недостаточно символов для читаемого пароля");

bool PasswordGenerator::allowed(QChar c, const QString &current) const
{
    if (!st_.preventRepeat())
        return true;

    if (st_.maxRepeat() <= 0)
        return false;

    int same = 0;
    for (int i = current.size() - 1; i >= 0; --i)
    {
        if (current[i] == c)
            ++same;
        else
            break;
    }

    return same < st_.maxRepeat();
}

static bool buildVowelsAndConsonants(const PasswordSettings &st, const QString &vowelsRef,
                                     const QString &consonantsRef, QString &vowOut, QString &consOut)
{
    vowOut.clear();
    consOut.clear();

    const QString include = st.includeChars();

    for (QChar c : include)
    {
        const QChar lc = c.toLower();

        if (vowelsRef.contains(lc))
        {
            if (!vowOut.contains(c))
                vowOut.append(c);
        }
        else if (consonantsRef.contains(lc))
        {
            if (!consOut.contains(c))
                consOut.append(c);
        }
    }

    for (QChar c : st.excludeChars())
    {
        vowOut.remove(c);
        consOut.remove(c);
    }

    return !(vowOut.isEmpty() || consOut.isEmpty());
}

QString PasswordGenerator::generatePlain(int len) const
{
    QString chars = st_.includeChars();

    if (st_.onlyLower()) chars = chars.toLower();
    else if (st_.onlyUpper()) chars = chars.toUpper();

    for (QChar c : st_.excludeChars())
        chars.remove(c);

    if (chars.isEmpty())
        return ERR_NO_SYMBOLS;

    QString pass;
    int attempts = 0;
    const int maxAttempts = len * 200;

    while (pass.size() < len && attempts++ < maxAttempts)
    {
        QChar c = chars.at(rng_.bounded(chars.length()));
        if (allowed(c, pass))
            pass.append(c);
    }

    return pass.size() == len ? pass : ERR_NO_SYMBOLS;
}

QString PasswordGenerator::generateReadable(int len) const
{
    QString vow, cons;

    if (!buildVowelsAndConsonants(st_, vowels_, consonants_, vow, cons))
        return ERR_READABLE;

    QString pass;
    bool useVowel = rng_.bounded(2);
    int attempts = 0, maxAttempts = 2000;

    while (pass.size() < len && attempts++ < maxAttempts)
    {
        QChar c = useVowel ? vow.at(rng_.bounded(vow.size()))
                           : cons.at(rng_.bounded(cons.size()));

        if (st_.onlyUpper()) c = c.toUpper();
        else if (st_.onlyLower()) c = c.toLower();

        if (st_.excludeChars().contains(c))
            continue;

        if (!st_.onlyUpper() && !st_.onlyLower() &&
            !st_.includeChars().contains(c))
            continue;

        if (allowed(c, pass))
        {
            pass.append(c);
            useVowel = !useVowel;
        }
    }
    return pass.size() == len ? pass : ERR_READABLE;
}

QString PasswordGenerator::generateMoreReadable(int len) const
{
    QString vow, cons;
    if (!buildVowelsAndConsonants(st_, vowels_, consonants_, vow, cons))
        return ERR_READABLE;

    QString pass;
    int patternPos = rng_.bounded(3);
    int attempts = 0, maxAttempts = 3000;

    while (pass.size() < len && attempts++ < maxAttempts)
    {
        QChar c = (patternPos < 2)
                  ? cons.at(rng_.bounded(cons.size()))
                  : vow .at(rng_.bounded(vow .size()));

        if (st_.onlyUpper()) c = c.toUpper();
        else if (st_.onlyLower()) c = c.toLower();

        if (st_.excludeChars().contains(c))
            continue;

        if (!st_.onlyUpper() && !st_.onlyLower() &&
            !st_.includeChars().contains(c))
            continue;

        if (allowed(c, pass))
        {
            pass.append(c);
            patternPos = (patternPos + 1) % 3;
        }
    }
    return pass.size() == len ? pass : ERR_READABLE;
}

QStringList PasswordGenerator::generate()
{
    QStringList list;
    for (int i = 0; i < st_.count(); ++i)
    {
        int len = st_.minLen();
        if (st_.minLen() != st_.maxLen())
            len = rng_.bounded(st_.minLen(), st_.maxLen() + 1);

        QString p = st_.moreReadable() ? generateMoreReadable(len)
                    : st_.readable() ? generateReadable(len) : generatePlain(len);
        list << p;
    }
    return list;
}
