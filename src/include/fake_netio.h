#ifndef FAKE_IO
#define FAKE_IO

#include<iostream>
#include<queue>
#include "seal/seal.h"
using namespace seal;
using namespace std;

std::queue<unsigned char> mpc_tee_buffer;
std::queue<PublicKey> pkey_buffer;
std::queue<RelinKeys> rkey_buffer;
std::queue<GaloisKeys> gkey_buffer;
std::queue<vector<string>> vstr_buffer;
std::queue<Ciphertext> cipher_buffer;
std::queue<vector<seal::Ciphertext>> v_cipher_buffer;


void send(const unsigned char * msg, size_t len)
{
    for (size_t i=0; i<len; i++)
    {
        mpc_tee_buffer.push(msg[i]);
    } 
}
void receive(unsigned char * msg, size_t len)
{
    for (size_t i=0; i<len; i++)
    {
        msg[i] = mpc_tee_buffer.front();
        mpc_tee_buffer.pop();
    }
}

void send(const PublicKey& pkey)
{
    pkey_buffer.push(pkey);
}

void receive(PublicKey& pkey)
{
    std::cout<<"receiving pkey..."<<std::endl;
     pkey = pkey_buffer.front();
     pkey_buffer.pop();
}


void send(const RelinKeys& rkey)
{
    rkey_buffer.push(rkey);
}

void receive(RelinKeys& rkey)
{
     rkey = rkey_buffer.front();
     rkey_buffer.pop();
}

void send(const GaloisKeys& gkey)
{
    gkey_buffer.push(gkey);
}

void receive(GaloisKeys& gkey)
{
     gkey = gkey_buffer.front();
     gkey_buffer.pop();
}


void send(const vector<string> & vstr)
{
    vstr_buffer.push(vstr);
}

void receive(vector<string>& vstr)
{
    vstr = vstr_buffer.front();
    vstr_buffer.pop();
}

void send(const Ciphertext & c)
{
    cipher_buffer.push(c);
}

void receive(Ciphertext & c)
{
    c = cipher_buffer.front();
    cipher_buffer.pop();
}

void send(const vector<Ciphertext> & vc)
{
    v_cipher_buffer.push(vc);
}

void receive(vector<Ciphertext> & vc)
{
    vc = v_cipher_buffer.front();
    cipher_buffer.pop();
}

#endif