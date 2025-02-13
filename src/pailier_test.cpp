#include "ipcl/ipcl.hpp"
#include <random>
#include <cmath>
#include "include/ipcl_serializable.h"




ipcl::CipherText enc_uint32_t(uint32_t x, ipcl::PublicKey pub_key)
{

ipcl::PlainText a(x);
ipcl::CipherText ct_a = pub_key.encrypt(a);
return ct_a;
}

int64_t dec_uint32_t(ipcl::CipherText ct_a, ipcl::PrivateKey priv_key)
{

ipcl::PlainText dec_a = priv_key.decrypt(ct_a);
std::vector<uint32_t> x = dec_a; 
//   x32=x[0];
return x[0];
}


int main1()
{
//   ipcl::initializeContext("QAT");


// uint32_t val1;
ipcl::PlainText a(1<<31);
ipcl::PlainText b(1<<31);

// BigNumber val3;
// ipcl::PlainText pt3(val3);

ipcl::KeyPair key = ipcl::generateKeypair(1024, true); // previously ipcl::keyPair

// ipcl::CipherText ct = key.pub_key.encrypt(pt); // previously key.pub_key->encrypt(pt)
// ipcl::PlainText dec_pt = key.priv_key.decrypt(ct); 
std::cout<<"n="<<* (key.pub_key.getN())<<std::endl;
BigNumber n=*(key.pub_key.getN());
BigNumber n_shift_64=n/(1<<20)/(1<<20)/(1<<24);
std::cout<<"n_shift_64="<<n_shift_64<<std::endl;;

ipcl::CipherText ct_a = key.pub_key.encrypt(a);
ipcl::CipherText ct_b = key.pub_key.encrypt(b);

// Addition (all three end up being same values after decryption)
ipcl::CipherText ct_c1 = ct_a + ct_b; // ciphertext + ciphertext
ipcl::CipherText ct_c2 = ct_c1 * b; // ciphertext + plaintext
ipcl::CipherText ct_c3 = ct_c2 * a; // ciphertext + plaintext



// Multiplication
ipcl::CipherText ct_d1 = ct_a * b; // ciphertext * plaintext
ipcl::CipherText ct_d2 = ct_b * a;


ipcl::PlainText dec_c1 = key.priv_key.decrypt(ct_c1); 
ipcl::PlainText dec_c2 = key.priv_key.decrypt(ct_c2); 
ipcl::PlainText dec_c3 = key.priv_key.decrypt(ct_c3); 
ipcl::PlainText dec_d1 = key.priv_key.decrypt(ct_d1); 
ipcl::PlainText dec_d2 = key.priv_key.decrypt(ct_d2);

std::cout<<"dec_c1="<<dec_c1[0]<<std::endl;
std::cout<<"dec_c2="<<dec_c2[0]<<std::endl;
std::cout<<"dec_c3="<<dec_c3[0]<<std::endl;
std::cout<<"dec_d1="<<dec_d1[0]<<std::endl;
std::cout<<"dec_d2="<<dec_d2[0]<<std::endl;

std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
for (size_t i=0; i<32; i++)
{
    std::cout<<"i="<<i<<std::endl;
    ct_c1 = ct_c1*b;
    dec_c1 = key.priv_key.decrypt(ct_c1); 
    std::cout<<"dec_c1="<<dec_c1[0]<<std::endl;
}

}


int main_homo()
{
    ipcl::KeyPair key = ipcl::generateKeypair(1024, true); // previously ipcl::keyPair
    ipcl::PublicKey pkey=ipcl::PublicKey();
    uint32_t x = random();


ipcl::CipherText ct = enc_uint32_t(x, key.pub_key);

uint32_t r = random();

ct=ct+ipcl::PlainText(r);

uint64_t y=dec_uint32_t(ct, key.priv_key);

std::cout<<"x="<<std::hex<<x<<std::endl;
std::cout<<"y="<<std::hex<<y<<std::endl;

std::cout<<"r="<<std::hex<<r<<std::endl;
std::cout<<"y-r="<<std::hex<<y-r<<std::endl;
// std::cout<<"x="<<x<<std::endl;
// std::cout<<"y="<<y<<std::endl;
std::cout<<"log2(x)="<<log2(x)<<std::endl;
return 0;
}

int main_serialize()
{
    ipcl::KeyPair key = ipcl::generateKeypair(1024, true); // previously ipcl::keyPair
    ipcl::PublicKey pkey=ipcl::PublicKey();

    uint32_t x = random();

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    std::stringstream pk_stream; 
    ipcl::serializer::serialize<ipcl::PublicKey>(pk_stream, key.pub_key);
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    ipcl::serializer::deserialize<ipcl::PublicKey>(pk_stream, pkey); 
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    return 0;
}


int main_serialize2()
{
    ipcl::KeyPair key = ipcl::generateKeypair(1024, true); // previously ipcl::keyPair
    ipcl::PublicKey pkey=ipcl::PublicKey();

    uint32_t x = random();

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    std::stringstream pk_stream; 
    // ipcl::serializer::serialize<ipcl::PublicKey>(pk_stream, key.pub_key);
    ipcl_encrypt::serialize_pub_key(pk_stream, key.pub_key);
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    // ipcl::serializer::deserialize<ipcl::PublicKey>(pk_stream, pkey); 
    ipcl_encrypt::deserialize_pub_key(pk_stream, pkey);
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;



    ipcl::CipherText ct = enc_uint32_t(x, key.pub_key);

    uint32_t r = random();

    ct=ct+ipcl::PlainText(r);

    uint64_t y=dec_uint32_t(ct, key.priv_key);

    std::cout<<"x="<<std::hex<<x<<std::endl;
    std::cout<<"y="<<std::hex<<y<<std::endl;

    std::cout<<"r="<<std::hex<<r<<std::endl;
    std::cout<<"y-r="<<std::hex<<y-r<<std::endl;
    // std::cout<<"x="<<x<<std::endl;
    // std::cout<<"y="<<y<<std::endl;
    std::cout<<"log2(x)="<<log2(x)<<std::endl;
    return 0;
}

int main()
{
    // main_homo();
    main_serialize2();
    return 0;
}