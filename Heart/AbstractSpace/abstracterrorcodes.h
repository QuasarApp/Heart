/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACT_ERRORCODES_H
#define ABSTRACT_ERRORCODES_H

#include <QString>


namespace QH {

/**
 * @brief ErrorCodes This namesapce contains all error codes of the Heart Library.
 * @note If you want to add won error code in this namespace then you shold be create a own child of the AbstractErrorCodesHelper class
 * and override toString method.
 *
 * @note you new enum must be declared in the ErrorCodes namespace and your enum must be beginned of the last value from the ErrorCodes class.
 *
 *
 * Exmample of the create a own error codes.
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
 * @note in place  the AbstractErrorCodes you can use another errocodes class for example : DBErrorCodesHelper.
 *  Do not forget invoke toString method of the parrent eerorcodes class (in example case it is DBErrorCodesHelper class).
 */
namespace ErrorCodes {

/**
 * @brief The AbstractErrorCodes enum This enum with dafault error codes.
 */
enum AbstractErrorCodes: unsigned char {

    /// This reqest executed successful.
    NoError = 0,
    /// This is unknown error. Default value.
    UnknownError,
    /// The sendet to remoute nodes request is invalid. This is default value.
    InvalidRequest,
    /// This case using for inheritance new enum classes.
    AbstractErrorCodes
};

/**
 * @brief The AbstractErrorCodesHelper class is helper class with static methods for translate the ErrorCodes enums to text.
 * This helper know only about the AbstractErrorCodes enum.
 */
class AbstractErrorCodesHelper {

public:
    /**
     * @brief toString This method have a cases for the transate all ErrorCodes of the QuasarApp Heart library.
     * @param enumData This is erorr code.
     * @return string value of the error code.
     */
    static QString toString(unsigned char enumData);

private:
    AbstractErrorCodesHelper() = default;

};

}
}

#endif // ABSTRACT_ERRORCODES_H
