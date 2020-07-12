#include "cryptopairkeys.h"
namespace NP {
CryptoPairKeys::CryptoPairKeys() {

}

CryptoPairKeys::CryptoPairKeys(const QByteArray &pubKey, const QByteArray &privKey) {
    setPrivKey(privKey);
    setPublicKey(pubKey);
}

bool CryptoPairKeys::isValid() const {
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

bool operator ==(const CryptoPairKeys &left, const CryptoPairKeys &right) {
    return !(left != right);
}

bool operator !=(const CryptoPairKeys &left, const CryptoPairKeys &right) {
    return left._privKey != right._privKey || left._publicKey != right._publicKey;
}
}
