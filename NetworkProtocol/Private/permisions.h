#ifndef PERMISIONS_H
#define PERMISIONS_H
namespace NP {

/**
 * @brief The Permission enum
 * permision to data in database
 */
enum class Permission: unsigned char {
    NoPermission = 0x00,
    Read = 0x01,
    Write = 0x02,
};

}
#endif // PERMISIONS_H
