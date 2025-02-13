#ifndef UTILS
#define UTILS
#include <chrono>
#include "../third_party/libsodium/src/libsodium/include/sodium.h"
#include "ipcl/ipcl.hpp"
#include<iostream>
using namespace std;

 #define short_ECPoint_BYTES 12


void print_vec(vector<string> x)
{
  for (size_t i=0; i<x.size(); i++)
  {
    cout<<x[i]<<",";
  } 
  cout<<endl;
}



struct ECPoint
  {
    unsigned char P[crypto_core_ristretto255_BYTES]; 
    size_t byte_size()
    {
        return crypto_core_ristretto255_BYTES;
    }
    bool operator==(const ECPoint& other)
    {
      bool r=true;
      for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
      {
        r &= (P[i]==other.P[i]);
      }
      return r;
    }
  };


 struct ShortECPoint
  {
    unsigned char P[short_ECPoint_BYTES]; 
    bool operator==(const ShortECPoint& other)
    {
      bool r=true;
      for (size_t i=0; i<short_ECPoint_BYTES; i++)
      {
        r &= (P[i]==other.P[i]);
      }
      return r;
    }
  } ;

      bool operator==(const ShortECPoint & short_ec_point1, const ShortECPoint & short_ec_point2)  {
        bool r = true;
        for (size_t i=0; i<short_ECPoint_BYTES; i++)
        {
            r = r && (short_ec_point1.P[i] == short_ec_point2.P[i]);
        }
        return r;
    }

 struct DoubleECPoint
  {
    ECPoint R;
    ECPoint Q;
    bool operator==(const DoubleECPoint& other)
    {
      return (R==other.R) & (Q==other.Q);
    }
  };




    typedef struct 
  {
        ECPoint R;
    ECPoint Q;
    bool b;
  } DoubleECPointPlus1bit;

  typedef struct 
  {
    ipcl::CipherText Eshare1=ipcl::CipherText();
    int32_t share0;
  } PaillierCipherPlusShare0;


struct ECPSIQuery
  {
    std::vector<ECPoint> EIDs;
    std::vector<DoubleECPoint> Elabels;
    size_t byte_size()
    {
        return EIDs.size()*crypto_core_ristretto255_BYTES + Elabels.size()*crypto_core_ristretto255_BYTES*2;
    }
    size_t size()
    {
      return EIDs.size();
    }
    ECPSIQuery(){}
    ECPSIQuery(size_t size)
    {
      EIDs.resize(size);
      Elabels.resize(size);
    }
    bool operator==(const ECPSIQuery & other)
    {
      bool r=true;
      for (size_t i=0; i<EIDs.size(); i++)
      {
        r &= (EIDs[i]==other.EIDs[i]);
      }
      for (size_t i=0; i<Elabels.size(); i++)
      {
        r &= (Elabels[i]==other.Elabels[i]);
      }
      return r;
    }

  };



struct ECPaillierPSIQuery
  {
    std::vector<ECPoint> EIDs;
// std::vector<ipcl::CipherText> Elabels;
std::vector<std::string> Elabels;

    size_t size()
    {
      return EIDs.size();
    }
        ECPaillierPSIQuery(){}
    ECPaillierPSIQuery(size_t size)
    {
      EIDs.resize(size);
      Elabels.resize(size);
    }
  };



struct ECPaillierPSIResponce
  {
    std::vector<ShortECPoint>  ShortEIDs;
    std::vector<ECPoint> EEIDs;
    // std::vector<ipcl::CipherText> Elabels;
    std::vector<std::string> Elabels;

    std::vector<size_t> sizes()
    {
      std::vector<size_t> r(3);
      r[0] =  ShortEIDs.size();
      r[1] = EEIDs.size();
      r[2] = Elabels.size();
      return r;
    }
    ECPaillierPSIResponce()
    {
    }

    ECPaillierPSIResponce(const std::vector<size_t> & sizes)
    {
      ShortEIDs.resize(sizes[0]);
      EEIDs.resize(sizes[1]);
      Elabels.resize(sizes[2]);
    }
    
  };





struct ECPSIResponce
  {
    std::vector<ShortECPoint>  ShortEIDs;
    std::vector<ECPoint> EEIDs;
    std::vector<DoubleECPoint> R1Q1s;
    size_t byte_size()
    {
        return ShortEIDs.size()*short_ECPoint_BYTES + EEIDs.size()*crypto_core_ristretto255_BYTES + R1Q1s.size()*crypto_core_ristretto255_BYTES*2;
    }

    std::vector<size_t> sizes()
    {
      std::vector<size_t> r(3);
      r[0] =  ShortEIDs.size();
      r[1] = EEIDs.size();
      r[2] = R1Q1s.size();
      return r;
    }
    ECPSIResponce()
    {}

    ECPSIResponce(const std::vector<size_t> & sizes)
    {
      ShortEIDs.resize(sizes[0]);
      EEIDs.resize(sizes[1]);
      R1Q1s.resize(sizes[2]);
    }



     bool operator==(const ECPSIResponce & other)
    {
      bool r=true;
      for (size_t i=0; i<ShortEIDs.size(); i++)
      {
        r &= (ShortEIDs[i]==other.ShortEIDs[i]);
      }
      for (size_t i=0; i<EEIDs.size(); i++)
      {
        r &= (EEIDs[i]==other.EEIDs[i]);
      }
      for (size_t i=0; i<R1Q1s.size(); i++)
      {
        r &= (R1Q1s[i]==other.R1Q1s[i]);
      }
      return r;
    }
  };


struct Int32AndBool
  {
    int32_t a;
    bool b;
  };



struct ShortECPointHash {
    uint32_t operator()(const ShortECPoint& key) const {
        // 这里使用简单的哈希函数来计算字符串的哈希值
        uint32_t hash = key.P[3];
        hash = (hash << 8) | key.P[2];
        hash = (hash << 8) | key.P[1];
        hash = (hash << 8) | key.P[0];
        return hash;
    }
};


struct PK
  {
    DoubleECPoint psi_pk;
    stringstream bps_pk;

string bps_to_str() const
{
  return bps_pk.str();
}

void bps_from_str(string bps_pk_str)
{
  bps_pk<<bps_pk_str;
}

 bool operator==(const PK& other)
    {
      string other_bps_pk_str = other.bps_to_str();
      return (psi_pk==other.psi_pk) & (bps_to_str()==other_bps_pk_str);
    }

  } ;


struct PKPaillier
  {
    // DoubleECPoint psi_pk;
    stringstream psi_pk;
    stringstream bps_pk;

string psi_to_str() const
{
  string psi_pk_str=psi_pk.str();
   std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
  std::cout<<"psi_pk_str="<<std::hash<string>{}(psi_pk_str)<<std::endl;
  return psi_pk_str;
}

string bps_to_str() const
{
  string bps_pk_str=bps_pk.str();
  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
  std::cout<<"bps_pk_str="<<std::hash<string>{}(bps_pk_str)<<std::endl;
  return bps_pk_str;
}

void bps_from_str(string bps_pk_str)
{
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
  std::cout<<"bps_pk_str="<<std::hash<string>{}(bps_pk_str)<<std::endl;
  bps_pk<<bps_pk_str;
}

void psi_from_str(string psi_pk_str)
{
  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
  std::cout<<"psi_pk_str="<<std::hash<string>{}(psi_pk_str)<<std::endl;
  psi_pk<<psi_pk_str;
}


  };


struct BatchPirToShareQuery
  {
    stringstream cipher_x_powers_stream;
    vector<string> row_keepers_str;
    vector<string> col_keepers_str;
    string str()
    {
        string result = cipher_x_powers_stream.str();
        for (string r : row_keepers_str)
        {
            result += r;
        }
        for (string c: col_keepers_str)
        {
            result += c;
        }
        return result;
    }

  size_t byte_size()
  {
    return str().size();
  }

  };


  struct BatchPirToShareQueryStream
  {
    stringstream cipher_x_powers_stream;
    stringstream row_keepers_stream;
    stringstream col_keepers_stream;

  size_t byte_size()
  {
    return cipher_x_powers_stream.str().size()+row_keepers_stream.str().size()+col_keepers_stream.str().size();
  }

  };




class Timer
{
public:
  Timer() { 
    _start = std::chrono::high_resolution_clock::now(); 
    // _start = std::chrono::system_clock::now(); 
    }
    
  inline double elapsed()
  {
    _end = std::chrono::high_resolution_clock::now();
    // _end = std::chrono::system_clock::now(); 
    return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start)
               .count() /
           1000.0;
  }

  inline void reset() { _start = std::chrono::high_resolution_clock::now(); }
  ~Timer(){};

private:
  std::chrono::high_resolution_clock::time_point _start, _end;
};


#endif




