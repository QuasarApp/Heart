//#
//# Copyright (C) 2025-2025 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#



#ifndef TRANSLATABLESTRING_H
#define TRANSLATABLESTRING_H
#include <streammultiversion.h>


namespace QH {

/**
 * @brief The TranslatableString class  is a wrapper for The QString with boolean flag isKey.
 * @note ise this class if you want to save text into file and want to get correct translations after reading.
 */
class HEARTSHARED_EXPORT TranslatableString: public StreamMultiversion
{
public:
    TranslatableString(const QString& data = "",
                       bool isKey = false);

    TranslatableString(const TranslatableString &) = default;
    TranslatableString(TranslatableString &&) = default;
    TranslatableString &operator=(const TranslatableString &) = default;
    TranslatableString &operator=(TranslatableString &&) = default;

    bool operator==(const TranslatableString& other) const;

    // ValidableObject interface
    bool isValid() const override;

    bool isEmpty() const;

    /**
     * @brief text this method return translated text.
     * @return
     */
    QString text() const;

    /**
     * @brief rawText return raw key source.
     * @return
     */
    const QString& rawText() const;

    /**
     * @brief calcCrc32Hash this method calc hash from raw text.
     * @return
     */
    int calcCrc32Hash() const;

    /**
     * @brief tr this method create a string translation wrapper and save translation key for lupdate tool
     * @param source this is text source.
     * @return
     */
    static TranslatableString tr(const QString& source);

    TranslatableString& setArgs(const QList<TranslatableString> &newArgs);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QString _data;
    bool _isKey = false;
    QList<TranslatableString> _args;

};
}


#endif // TRANSLATABLESTRING_H
