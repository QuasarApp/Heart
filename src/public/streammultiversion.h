/*
 * Copyright (C) 2024-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef STREAMMULTIVERSION_H
#define STREAMMULTIVERSION_H

#include "streambase.h"

namespace QH {

/**
 * @brief The StreamMultiversion class this parser works with simple multiversion packages.
 *
 * This class used to save and read version of the object in file.
 *
 * @see StreamBase
 *
 * @code{cpp}
 * class myClass: public StreamMultiversion {
 *      protected:
 *      QDataStream &fromStream(QDataStream &stream) override {
 *          char version = versionHeader(0, stream);
 *          readWrite(stream, data);
 *          return stream;
 *      }
 *
 *      QDataStream &toStream(QDataStream &stream) const override {
 *          char version = versionHeader(0, stream);
 *          readWrite(stream, data);
 *          return stream;
 *      }
 *
 *      private:
 *      int data;
 *
 * }
 * @endcode
 *
 * More example of use:
 * @code{cpp}
 * QDataStream &ClassName::fromStream(QDataStream &stream)
    {
        versionHeader(0, stream);

        READ_WRITE_IMPL(fild0,
                        fild1)
    }

    QDataStream &ClassName::toStream(QDataStream &stream) const
    {
        versionHeader(0, stream);

        READ_WRITE_IMPL(fild0,
                        fild1)
    }
 * @endcode
 *
 */
class HEARTSHARED_EXPORT StreamMultiversion: public StreamBase
{
public:

    StreamMultiversion();
    ~StreamMultiversion();
    // StreamBase interface

protected:

    /**
     * @brief saveVersion save version of the object to the stream.
     * @param version - version of the object.
     * @param stream - stream to save.
     *
     * This method used to save version of the object in file.
     * @see toStream
     */
    void saveVersion(char version, QDataStream &stream) const;

    /**
     * @brief readVersion read version of the object from the stream.
     * @param stream - stream to read.
     * @return version of the object.
     * @see fromStream

     */
    char readVersion(QDataStream &stream);

    /**
     * @brief versionHeader this is new function that automaticaly choose what shuld be to-do (read/wirte).
     * @param version this is a version of object.
     * @param stream this is stream cheenel to save or read.
     * @return version of object.
     */
    char versionHeader(char version, QDataStream &stream);

    /**
     * @brief versionHeader this is new function that automaticaly choose what shuld be to-do (read/wirte).
     * @param version this is a version of object.
     * @param stream this is stream cheenel to save or read.
     * @return version of object.
     */
    char versionHeader(char version, QDataStream &stream) const;

    /**
     * @brief readWrite this method automaticaly choose what we need to-do read or write.
     * @param stream
     * @param data
     */
    template <class T>
    void readWrite(QDataStream &stream, const T& data) const {
        stream << data;
    }

    /**
     * @brief readWrite this method automaticaly choose what we need to-do read or write.
     * @param stream
     * @param data
     */
    template <class T>
    void readWrite(QDataStream &stream, T& data) {
        stream >> data;

    }

    /**
     * @brief readWritePack - this is a pack write or read from the srting
     * @param stream - stream cheenel to save or read.
     * @param args - list of arguments
     */
    template <typename... Args>
    void readWritePack(QDataStream &stream, Args&&... args) const {
        (readWrite(stream, std::forward<Args>(args)), ...);
    }

    /**
     * @brief readWritePack - this is a pack write or read from the srting
     * @param stream - stream cheenel to save or read.
     * @param args - list of arguments
     */
    template <typename... Args>
    void readWritePack(QDataStream &stream, Args&&... args) {
        (readWrite(stream, std::forward<Args>(args)), ...);
    }

};

}


#define READ_WRITE_IMPL(...) \
    readWritePack(stream, __VA_ARGS__); \
    return stream; \



#endif // STREAMMULTIVERSION_H
