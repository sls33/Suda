#pragma once

#include <zlib.h>

#include <cstring>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ipcl/ipcl.hpp"
// #include "src/core/crypto_engine/he/utils.h"



namespace ipcl_encrypt {

inline void serialize_bignum(std::ostream &os, const BigNumber &big_num) {
    std::vector<uint32_t> vec;
    big_num.num2vec(vec);
    IppsBigNumSGN sign;
    ippsRef_BN(&sign, nullptr, nullptr, (IppsBigNumState *)big_num);
    int len = vec.size();
    os.write(reinterpret_cast<const char *>(&len), sizeof(int));
    os.write(reinterpret_cast<const char *>(vec.data()),
             len * sizeof(uint32_t));
    os.write(reinterpret_cast<const char *>(&sign), sizeof(IppsBigNumSGN));
}

inline void deserialize_bignum(std::istream &is, BigNumber &big_num) {
    int len;
    is.read(reinterpret_cast<char *>(&len), sizeof(int));
    std::vector<uint32_t> m_data(len);
    is.read(reinterpret_cast<char *>(m_data.data()), len * sizeof(uint32_t));
    IppsBigNumSGN sign;
    is.read(reinterpret_cast<char *>(&sign), sizeof(IppsBigNumSGN));
    big_num = std::move(BigNumber(m_data.data(), len, sign));
}

inline void serialize_basetext(std::ostream &os, const ipcl::BaseText &text) {
    int len = text.getSize();
    os.write(reinterpret_cast<const char *>(&len), sizeof(int));
    auto m_texts = text.getTexts();
    for (const auto &item : m_texts) {
        serialize_bignum(os, item);
    }
}

inline void deserialize_basetext(std::istream &is,
                                 std::vector<BigNumber> &m_texts) {
    int len;
    is.read(reinterpret_cast<char *>(&len), sizeof(int));
    m_texts.resize(len);
    for (auto i = 0; i < len; i++) {
        deserialize_bignum(is, m_texts[i]);
    }
}

inline void serialize_plaintext(std::ostream &os, const ipcl::PlainText &text) {
    serialize_basetext(os, text);
}

inline void deserialize_plaintext(std::istream &is, ipcl::PlainText &text) {
    std::vector<BigNumber> m_texts;
    deserialize_basetext(is, m_texts);
    text = std::move(ipcl::PlainText(m_texts));
}

inline void serialize_ciphertext(std::ostream &os,
                                 const ipcl::CipherText &text) {
    serialize_basetext(os, text);
}

    

inline void deserialize_ciphertext(std::istream &is, const ipcl::PublicKey &pk,
                                   ipcl::CipherText &text) {
    std::vector<BigNumber> m_texts;
    deserialize_basetext(is, m_texts);
    text = std::move(ipcl::CipherText(pk, m_texts));
}

inline void serialize_pub_key(std::ostream &os,
                              const ipcl::PublicKey &pub_key) {
    int bits = pub_key.getBits();
    bool enable_DJN = pub_key.isDJN();
    int randbits = pub_key.getRandBits();
    os.write(reinterpret_cast<const char *>(&bits), sizeof(int));
    os.write(reinterpret_cast<const char *>(&randbits), sizeof(int));
    os.write(reinterpret_cast<const char *>(&enable_DJN), sizeof(bool));
    serialize_bignum(os, *pub_key.getN());
    serialize_bignum(os, pub_key.getHS());
}

inline void deserialize_pub_key(std::istream &is, ipcl::PublicKey &pub_key) {
    int bits, randbits;
    bool enable_DJN;
    BigNumber n, hs;
    is.read(reinterpret_cast<char *>(&bits), sizeof(int));
    is.read(reinterpret_cast<char *>(&randbits), sizeof(int));
    is.read(reinterpret_cast<char *>(&enable_DJN), sizeof(bool));
    deserialize_bignum(is, n);
    deserialize_bignum(is, hs);
    if (enable_DJN) {
        pub_key.create(n, bits, hs, randbits);
    } else {
        pub_key.create(n, bits);
    }
}

}  // namespace ipcl_encrypt


