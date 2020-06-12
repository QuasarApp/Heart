#include "cryptopairkeys.h"
namespace NP {

CryptoPairKeys::CryptoPairKeys() {

}

bool CryptoPairKeys::isValid() {
    return _privKey.size() && _publicKey.size();
}

QByteArray CryptoPairKeys::privKey() const {
    return _privKey;
}

void CryptoPairKeys::setPrivKey(const QByteArray &privKey) {
    _privKey = privKey;
}

QByteArray CryptoPairKeys::publicKey() const {
    return _publicKey;
}

void CryptoPairKeys::setPublicKey(const QByteArray &publicKey) {
    _publicKey = publicKey;
}

int CryptoPairKeys::bits() const {
    return _bits;
}

void CryptoPairKeys::setBits(int bits) {
    _bits = bits;
}
}
