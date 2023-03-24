/*
 * Copyright (C) 2020-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACT_ERRORCODES_H
#define ABSTRACT_ERRORCODES_H

#include <QString>
#include "heart_global.h"


namespace QH {

/**
 * @brief ErrorCodes This namesapce contains all error codes of the Heart Library.
 * @note If you want to add won error code in this namespace then you should be create a own child of the AbstractErrorCodesHelper class
 * and override toString method.
 *
 * @note you new enum must be declared in the ErrorCodes namespace and your enum must be beginned of the last value from the ErrorCodes class.
 *
 *
 * Example of the create a own error codes.
 * @code cpp
 *  namespace QH {

    namespace ErrorCodes {

    enum MyErrorCodes: unsigned char {
        /// User not registered because database not inited or other error occurred.
        MyFirstError = AbstractErrorCodes::AbstractErrorCodes, // or another ErrorCodes enum.

        MyErrorCodes,


    };

    class MyErrorCodesHelper {
    public:
        static QString toString(unsigned char enumData) {

            switch (enumData) {

            case ErrorCodes::MyFirstError: {
                return "My Error";
            }

            default: return AbstractErrorCodesHelper::toString(enumData);

        }

    private:
        MyErrorCodesHelper() = default;

    };

    }
    }
 * @endcode
 * @note in place  the AbstractErrorCodes you can use another erorcodes class for example : DBErrorCodesHelper.
 *  Do not forget invoke toString method of the parent erorcodes class (in example case it is DBErrorCodesHelper class).
 */
namespace ErrorCodes {

/**
 * @brief Code This is alias for the unsigned char type.
 */
using Code = unsigned char;

/**
 * @brief The AbstractErrorCodes enum This enum with default error codes.
 */
enum AbstractErrorCodes: Code {

    /// This reqest executed successful.
    NoError = 0,
    /// This is unknown error. Default value.
    UnknownError,
    /// The sender to remote nodes request is invalid. This is default value.
    InvalidRequest,
    /// This case using for inheritance new enum classes.
    AbstractErrorCodes
};

/**
 * @brief The AbstractErrorCodesHelper class is helper class with static methods for translate the ErrorCodes enums to text.
 * This helper know only about the AbstractErrorCodes enum.
 */
class HEARTSHARED_EXPORT AbstractErrorCodesHelper {

public:
    /**
     * @brief toString This method have a cases for the translate all ErrorCodes of the QuasarApp Heart library.
     * @param enumData This is error code.
     * @return string value of the error code.
     */
    static QString toString(Code enumData);

private:
    AbstractErrorCodesHelper() = default;

};

}
}

#endif // ABSTRACT_ERRORCODES_H
