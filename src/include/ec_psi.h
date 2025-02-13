#include "../third_party/libsodium/src/libsodium/include/sodium.h"
// #include "fake_netio.h"
#include "ipcl/ipcl.hpp"
#include "ipcl/ciphertext.hpp"
#include "utils.h"
#include<queue>
#include <unordered_set>
#include <iomanip> 
#include <boost/functional/hash.hpp>
#include "ipcl_serializable.h"
// #include <pybind11/pybind11.h>
// namespace py = pybind11;







using namespace std;

std::string unsignedCharToString(unsigned char* data, int length) {
    std::string result(data, data + length);
    return result;
}



ShortECPoint cut_off_ec_point(ECPoint P)
{
    ShortECPoint SP;
    for (size_t i=0; i<short_ECPoint_BYTES; i++)
    {
        SP.P[i] = P.P[i];
    }
    return SP;
}






void print(ECPoint P)
{
    for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    {
        std::cout<<std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned>(P.P[i]);
    }
    std::cout<<std::endl;
}

void print(std::vector<ECPoint> Ps)
{
    for (size_t i=0; i<Ps.size(); i++)
    {
        print(Ps[i]);
    }
}

void print(ShortECPoint P)
{
    for (size_t i=0; i<short_ECPoint_BYTES; i++)
    {
        std::cout<<std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned>(P.P[i]);

    }
    std::cout<<std::endl;
    
}

void print(std::vector<ShortECPoint> Ps)
{
    for (size_t i=0; i<Ps.size(); i++)
    {
        print(Ps[i]);
    }
}

//--------P1----------------

class ECPlayer1
{
    private:
    unsigned char one[crypto_core_ristretto255_SCALARBYTES]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char minus_one[crypto_core_ristretto255_SCALARBYTES];
    unsigned char P[crypto_core_ristretto255_BYTES];   // label encrypt base point
    unsigned char s[crypto_core_ristretto255_SCALARBYTES];  // secret key for label encrypt
    unsigned char s_inv[crypto_core_ristretto255_SCALARBYTES];
    unsigned char zeros_cipherR[crypto_core_ristretto255_BYTES];  // public key for label encrypt
    unsigned char zeros_cipherQ[crypto_core_ristretto255_BYTES];  // public key for label encrypt
    // blake3_hasher hasher;
    // uint8_t key1[BLAKE3_KEY_LEN];
    std::vector<Int32AndBool> new_id_and_b1s;


    public:
    ECPlayer1() {
        crypto_core_ristretto255_scalar_negate(minus_one, one);
        crypto_scalarmult_ristretto255_base(P, one);
        generate_private_key();
        generate_public_key(zeros_cipherR, zeros_cipherQ);
        // send_public_key();
        // boost::hash<int> intHash; // 声明一个整数哈希函数对象
        // boost::hash<std::string> stringHash; // 声明一个字符串哈希函数对象  
        // for (size_t i=0; i<BLAKE3_KEY_LEN; i++)
        // {
        //     key1[i]=0x41;
        // }
        // blake3_hasher_init_keyed(&hasher, key1)
        }

    ~ECPlayer1() {
        // if (net_client_ != nullptr) {
        //     net_client_->Leave();
        // }
    }

    void generate_private_key()
    {
        crypto_core_ristretto255_scalar_random(s);
        crypto_core_ristretto255_scalar_invert(s_inv, s);
    }

    void generate_public_key(unsigned char * R, unsigned char * Q)
    {
        crypto_core_ristretto255_random(R);
        crypto_scalarmult_ristretto255(Q, s, R);
    }


    // void send_public_key()
    // {

    //     send(zeros_cipherR, crypto_core_ristretto255_BYTES);
    //     send(zeros_cipherQ, crypto_core_ristretto255_BYTES);
    // }

    void save_public_key(DoubleECPoint & pk)
    {
        // std::cout<<"crypto_core_ristretto255_BYTES="<<crypto_core_ristretto255_BYTES<<std::endl;
        for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
        {
            // std::cout<<"i="<<i<<",";

            // std::cout<<"zeros_cipherR[i]="<<zeros_cipherR[i]<<std::endl;
            // std::cout<<"zeros_cipherQ[i]="<<zeros_cipherQ[i]<<std::endl;
            pk.R.P[i] = zeros_cipherR[i];
            pk.Q.P[i] = zeros_cipherQ[i];
        }
        // std::cout<<std::endl;

    }


    void hash_to_point(const unsigned char * id, unsigned char * ec_point)
    {
        // unsigned char ru[crypto_core_ristretto255_HASHBYTES];
        if (crypto_core_ristretto255_from_hash(ec_point, id) != 0 ||
            crypto_core_ristretto255_is_valid_point(ec_point) != 1) {
            printf("crypto_core_ristretto255_from_hash() failed\n");
        }
    }

    void hash_to_point(const string id, unsigned char * ec_point)
    {
        size_t length = min(id.size(), (size_t) crypto_core_ristretto255_HASHBYTES);
        unsigned char id_array[crypto_core_ristretto255_HASHBYTES];
        for (size_t i=0; i<length; i++)
        {
            id_array[i] = id[i];
        }
        for (size_t i=length; i<crypto_core_ristretto255_HASHBYTES; i++)
        {
            id_array[i] = 0;
        }
        // ec_encryptedID(id_array, ec_point);
        hash_to_point(id_array, ec_point);
    }

  std::vector<ECPoint> hash_to_points(const vector<string> & v_ids)
  {
     size_t length = v_ids.size();
        std::vector<ECPoint> hashed_points(length);
        for (size_t i=0; i<length; i++)
        {
            hash_to_point(v_ids[i], hashed_points[i].P);
        }
        return hashed_points;
  }






    vector<ECPoint> ec_encryptedID(const vector<ECPoint> & ec_points)
    {
        size_t num = ec_points.size();
        vector<ECPoint> encrypted_ec_points(num);
        for (size_t i=0; i<num; i++)
        {
            crypto_scalarmult_ristretto255(encrypted_ec_points[i].P, s, ec_points[i].P);
        }
        return encrypted_ec_points;
    }


    vector<ECPoint> ec_decryptedID(const vector<ECPoint> & ec_points)
    {
        size_t num = ec_points.size();
        vector<ECPoint> ec_decpt_points(num);
        for (size_t i=0; i<num; i++)
        {
            crypto_scalarmult_ristretto255(ec_decpt_points[i].P, s_inv, ec_points[i].P);
        }
        return ec_decpt_points;
    }



    



    void ec_encrypt1bit(bool label,  unsigned char * R, unsigned char * Q)  
    {
        // unsigned char s[crypto_core_ristretto255_SCALARBYTES]
        // unsigned char R[crypto_core_ristretto255_BYTES];
        // unsigned char Q[crypto_core_ristretto255_BYTES];

        // ([r]P, [(-1)^label]P+[rs]P) =(R, Q)
        // unsigned char P[crypto_core_ristretto255_BYTES];
        unsigned char r[crypto_core_ristretto255_SCALARBYTES];

        unsigned char sR[crypto_core_ristretto255_BYTES];
        // crypto_scalarmult_ristretto255_base(P, one);
    
    
        crypto_core_ristretto255_scalar_random(r);
        crypto_scalarmult_ristretto255_base(R, r);
        crypto_scalarmult_ristretto255(sR, s, R);
        if (label)
        {
            crypto_core_ristretto255_sub(Q, sR, P);
        }
        else
        {
            crypto_core_ristretto255_add(Q, sR, P);
        }
        
        
    }



    bool ec_decrypt1bit(const unsigned char * R, const unsigned char * Q)
    {
        unsigned char sR[crypto_core_ristretto255_BYTES];
        unsigned char labelP[crypto_core_ristretto255_BYTES];
        crypto_scalarmult_ristretto255(sR, s, R);
        crypto_core_ristretto255_sub(labelP, Q, sR);
        int equal = sodium_memcmp(labelP, P, crypto_core_ristretto255_BYTES);
        return equal%2;
    }
 



    std::vector<DoubleECPoint> ec_batch_encrypt1bit(const std::vector<bool> & labels)
    {
        size_t batch_size=labels.size();
        std::vector<DoubleECPoint> RsQs(batch_size);
        for (size_t i=0; i<batch_size; i++)
        {
            ec_encrypt1bit(labels[i],  RsQs[i].R.P, RsQs[i].Q.P);
        }
        return RsQs;
    }



    std::vector<bool> ec_batch_decrypt1bit(const std::vector<DoubleECPoint> & RQs)
    {
        size_t batch_size=RQs.size();
        std::vector<bool> labels(batch_size);
        for (size_t i=0; i<batch_size; i++)
        {
            labels[i] = ec_decrypt1bit(RQs[i].R.P, RQs[i].Q.P);
        }
        return labels;
    }

    std::vector<bool> ec_batch_decrypt1bit(const std::vector<DoubleECPointPlus1bit> & RQbs)
    {
        size_t batch_size=RQbs.size();
        std::vector<bool> labels(batch_size);
        for (size_t i=0; i<batch_size; i++)
        {
            labels[i] = ec_decrypt1bit(RQbs[i].R.P, RQbs[i].Q.P);
        }
        return labels;
    }


    ECPSIQuery generate_query(const std::vector<string> & IDs, const std::vector<bool> & labels)
    {
        if (IDs.size()!=labels.size())
        {
            throw ("must have IDs.size()==labels.size()");
        }
        std::vector<ECPoint> hashed_points = hash_to_points(IDs);
        // std::cout<<"hashed_points="<<std::endl;                    
        // print(hashed_points);
        ECPSIQuery ec_psi_query;
        ec_psi_query.EIDs = ec_encryptedID(hashed_points);
        ec_psi_query.Elabels = ec_batch_encrypt1bit(labels);
        return ec_psi_query;
    }



    std::vector<int32_t> generate_response(ECPSIResponce response)
    {
        vector<int32_t>  intersection_lastids;
        vector<ECPoint> EIDs = ec_decryptedID(response.EEIDs);
        vector<bool> b1s = ec_batch_decrypt1bit(response.R1Q1s);

        //  求交 shortEIDs & EIDs
            // std::unordered_set<ShortECPoint> ShortEIDHashedSet;
            std::unordered_map<ShortECPoint, int32_t, ShortECPointHash> ShortEIDHashMap;
 
        // 向哈希集合中插入元素
        for (size_t i=0; i<response.ShortEIDs.size(); i++) {
            ShortEIDHashMap[response.ShortEIDs[i]] = i;
        }

        for (size_t i=0; i<EIDs.size(); i++)
        {
            ShortECPoint shortEID = cut_off_ec_point(EIDs[i]);

            auto it = ShortEIDHashMap.find(shortEID);

            if (it != ShortEIDHashMap.end()) {
                Int32AndBool new_id_and_b1;
                new_id_and_b1.a = it->second;     // new_id
                new_id_and_b1.b = b1s[i];
                new_id_and_b1s.push_back(new_id_and_b1);
                intersection_lastids.push_back(i);
            }
        }

        return intersection_lastids;
    }

    std::vector<Int32AndBool> return_answer()
    {
        return new_id_and_b1s;
    }


};


//--------P0----------------

class ECPlayer0
{
    private:
    unsigned char one[crypto_core_ristretto255_SCALARBYTES]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char minus_one[crypto_core_ristretto255_SCALARBYTES];
    unsigned char s[crypto_core_ristretto255_SCALARBYTES];  // secret key for label encrypt
    unsigned char P[crypto_core_ristretto255_BYTES];
    unsigned char zeros_cipherR[crypto_core_ristretto255_BYTES];
    unsigned char zeros_cipherQ[crypto_core_ristretto255_BYTES];
    // std::unordered_set<ShortECPoint> ShortEIDs;
    std::vector<ShortECPoint> ShortEIDs;
     std::vector<bool>  b0s;

    public:
    ECPlayer0() {
        crypto_core_ristretto255_scalar_negate(minus_one, one);
        crypto_scalarmult_ristretto255_base(P, one);
        generate_private_key();
        // receive_public_key();
    }

    ~ECPlayer0() {
        // if (net_client_ != nullptr) {
        //     net_client_->Leave();
        // }
        
    }

    void generate_private_key()
    {
        crypto_core_ristretto255_scalar_random(s);
    }

    // void receive_public_key()
    // {
    //     receive(zeros_cipherR, crypto_core_ristretto255_BYTES);
    //     receive(zeros_cipherQ, crypto_core_ristretto255_BYTES);
    // }

    void set_public_key(DoubleECPoint pk)
    {
        for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
        {
            zeros_cipherR[i] = pk.R.P[i];
            zeros_cipherQ[i] = pk.Q.P[i];
        }
    }


    void hash_to_point(const unsigned char * id, unsigned char * ec_point)
    {
        // unsigned char ru[crypto_core_ristretto255_HASHBYTES];
        if (crypto_core_ristretto255_from_hash(ec_point, id) != 0 ||
            crypto_core_ristretto255_is_valid_point(ec_point) != 1) {
            printf("crypto_core_ristretto255_from_hash() failed\n");
        }
    }

    void hash_to_point(const string id, unsigned char * ec_point)
    {
        size_t length = min(id.size(), (size_t) crypto_core_ristretto255_HASHBYTES);
        unsigned char id_array[crypto_core_ristretto255_HASHBYTES];
        for (size_t i=0; i<length; i++)
        {
            id_array[i] = id[i];
        }
        for (size_t i=length; i<crypto_core_ristretto255_HASHBYTES; i++)
        {
            id_array[i] = 0;
        }
        // ec_encryptedID(id_array, ec_point);
        hash_to_point(id_array, ec_point);
    }

  std::vector<ECPoint> hash_to_points(const vector<string> & v_ids)
  {
     size_t length = v_ids.size();
        std::vector<ECPoint> hashed_points(length);
            //  #pragma omp parallel for num_threads(16) collapse(1)  //private(reduced_poly_cipher)  
        for (size_t i=0; i<length; i++)
        {
            hash_to_point(v_ids[i], hashed_points[i].P);
        }
        return hashed_points;
  }



    vector<ECPoint> ec_encryptedID(const vector<ECPoint> & ec_points)
    {
        size_t num = ec_points.size();
        vector<ECPoint> encrypted_ec_points(num);
        for (size_t i=0; i<num; i++)
        {
            crypto_scalarmult_ristretto255(encrypted_ec_points[i].P, s, ec_points[i].P);
        }
        return encrypted_ec_points;
    }




    ShortECPoint ec_encryptedID_toshort(ECPoint ec_point)
    {
        ECPoint encrypted_ec_point;
        crypto_scalarmult_ristretto255(encrypted_ec_point.P, s, ec_point.P);
        return cut_off_ec_point(encrypted_ec_point);
    }

    std::vector<ShortECPoint> ec_encryptedID_toshort(vector<ECPoint> ec_points)
    {
        std::vector<ShortECPoint> v_ec_points_short(ec_points.size());
        // #pragma omp parallel for num_threads(16) collapse(1)  // private(reduced_poly_cipher)  
        for (size_t i=0; i<ec_points.size(); i++)
        {
            v_ec_points_short[i] =  ec_encryptedID_toshort(ec_points[i]);
        }
        return v_ec_points_short;
    }

    // void ec_encryptedID_toshort(vector<ECPoint> ec_points)
    // {
    //     for (size_t i=0; i<ec_points.size(); i++)
    //     {
    //         ShortEIDs.inssert(ec_encryptedID_toshort(ec_points[i]));
    //     }
    // }






    void _ec_batch_cipher_split_bshares(const unsigned char * R, const unsigned char * Q,  bool & b0, unsigned char * R1, unsigned char * Q1)
    { // Q = (-1)^b0 Q1    R = (-1)^b0 R1  
        
        // randombytes((unsigned char *) b0s, (batch_size+7)/8);

        b0 = rand()%2;
    
        if(b0==1)
        {
            crypto_scalarmult_ristretto255(R1, minus_one, R);
            crypto_scalarmult_ristretto255(Q1, minus_one, Q);
        }
        else
        {
            crypto_scalarmult_ristretto255(R1, one, R);
            crypto_scalarmult_ristretto255(Q1, one, Q);
        }

        unsigned char r[crypto_core_ristretto255_SCALARBYTES];
        crypto_core_ristretto255_scalar_random(r);
        crypto_scalarmult_ristretto255(zeros_cipherR, r, zeros_cipherR);
        crypto_scalarmult_ristretto255(zeros_cipherQ, r, zeros_cipherQ);

        crypto_core_ristretto255_add(R1, zeros_cipherR, R1);
        crypto_core_ristretto255_add(Q1, zeros_cipherQ, Q1);

    }

    DoubleECPointPlus1bit ec_batch_cipher_split_bshares(const ECPoint & R, const ECPoint & Q)
    {
        DoubleECPointPlus1bit R1Q1b0;
         _ec_batch_cipher_split_bshares(R.P, Q.P, R1Q1b0.b, R1Q1b0.R.P, R1Q1b0.Q.P);
         return R1Q1b0;
    }



    std::vector<DoubleECPointPlus1bit> ec_batch_cipher_split_bshares(const std::vector<DoubleECPoint> & RQs)
    {
        size_t length = RQs.size();

        std::vector<DoubleECPointPlus1bit> R1Q1b0s(length);
        for (size_t i=0; i< length; i++)
        {
            R1Q1b0s[i] = ec_batch_cipher_split_bshares(RQs[i].R, RQs[i].Q);
        }
        return R1Q1b0s;
    }


    void init(std::vector<string> plaintext_ids)
    {
        std::vector<ECPoint> hashed_points = hash_to_points(plaintext_ids);
        ShortEIDs = ec_encryptedID_toshort(hashed_points);
    }

    ECPSIResponce generate_response(ECPSIQuery & query)
    {
        ECPSIResponce ec_psi_responce;
        ec_psi_responce.ShortEIDs = ShortEIDs;
            // std::vector<ECPoint> twice_encrypt_batch_ids 
        size_t query_size = query.EIDs.size();
        std::random_device rd;
        std::mt19937 g(rd());
        std::vector<int32_t> index(query_size);
        for (size_t i=0; i<query_size; i++)
        {
            index[i]=i;
        }

        // // 对两个vector进行shuffle
        std::shuffle(index.begin(), index.end(), g);
    
        std::vector<ECPoint> EIDs(query_size);
        std::vector<DoubleECPoint> Elabels(query_size);
        for (size_t i=0; i<query_size; i++)
        {
            EIDs[i] = query.EIDs[index[i]];
            Elabels[i] = query.Elabels[index[i]];
        }


 
        ec_psi_responce.EEIDs = ec_encryptedID(EIDs);
        std::vector<DoubleECPointPlus1bit> R1Q1b0s = ec_batch_cipher_split_bshares(Elabels);
        b0s.resize(R1Q1b0s.size());
        ec_psi_responce.R1Q1s.resize(R1Q1b0s.size());
        for (size_t i=0; i<R1Q1b0s.size(); i++)
        {
            ec_psi_responce.R1Q1s[i].R = R1Q1b0s[i].R;
            ec_psi_responce.R1Q1s[i].Q = R1Q1b0s[i].Q;
            b0s[i] = R1Q1b0s[i].b;
        }
        return  ec_psi_responce;     
    } 


    std::vector<bool> return_answer(std::vector<int32_t> intersection_lastids)
    {
        std::vector<bool> intersection_b0s(intersection_lastids.size());
        for (size_t i=0; i<intersection_lastids.size(); i++)
        {
            intersection_b0s[i]=b0s[intersection_lastids[i]];
        }
        return intersection_b0s;
    }


};

//--------------------------------------------------------
//------------------------ECPaillier----------------------

void enc_uint32_t(const uint32_t x, const ipcl::PublicKey & pub_key, ipcl::CipherText & ct_a)
{

ipcl::PlainText a(x);
ct_a = pub_key.encrypt(a);
}

uint32_t dec_uint32_t(const ipcl::CipherText & ct_a, const ipcl::PrivateKey & priv_key)
{

ipcl::PlainText dec_a = priv_key.decrypt(ct_a);
std::vector<uint32_t> x = dec_a; 
//   x32=x[0];
return x[0];
}


//---------- ECPaillierPlayer1



class ECPaillierPlayer1
{
    private:
    // unsigned char one[crypto_core_ristretto255_SCALARBYTES]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // unsigned char minus_one[crypto_core_ristretto255_SCALARBYTES];
    // unsigned char P[crypto_core_ristretto255_BYTES];   // label encrypt base point
    unsigned char s[crypto_core_ristretto255_SCALARBYTES];  // secret key for label encrypt
    ipcl::KeyPair key;
    unsigned char s_inv[crypto_core_ristretto255_SCALARBYTES];
    // unsigned char zeros_cipherR[crypto_core_ristretto255_BYTES];  // public key for label encrypt
    // unsigned char zeros_cipherQ[crypto_core_ristretto255_BYTES];  // public key for label encrypt
    // blake3_hasher hasher;
    // uint8_t key1[BLAKE3_KEY_LEN];
    std::vector<std::array<int32_t, 2> > new_id_and_b1s;


    public:
    ECPaillierPlayer1() {
        // crypto_core_ristretto255_scalar_negate(minus_one, one);
        // crypto_scalarmult_ristretto255_base(P, one);
        
        generate_private_key();
        // generate_public_key(zeros_cipherR, zeros_cipherQ);
        key = ipcl::generateKeypair(1024, true); 
        // send_public_key();
        // boost::hash<int> intHash; // 声明一个整数哈希函数对象
        // boost::hash<std::string> stringHash; // 声明一个字符串哈希函数对象  
        // for (size_t i=0; i<BLAKE3_KEY_LEN; i++)
        // {
        //     key1[i]=0x41;
        // }
        // blake3_hasher_init_keyed(&hasher, key1)
        }

    ~ECPaillierPlayer1() {
        // if (net_client_ != nullptr) {
        //     net_client_->Leave();
        // }
    }

    void generate_private_key()
    {
        crypto_core_ristretto255_scalar_random(s);
        crypto_core_ristretto255_scalar_invert(s_inv, s);
 

    }

    // void generate_public_key(unsigned char * R, unsigned char * Q)
    // {
    //     crypto_core_ristretto255_random(R);
    //     crypto_scalarmult_ristretto255(Q, s, R);
    // }


    // void send_public_key(coproto::Socket & socket)
    // {

    //     // send(zeros_cipherR, crypto_core_ristretto255_BYTES);
    //     // send(zeros_cipherQ, crypto_core_ristretto255_BYTES);
    //     stringstream pk_stream;
    //     ipcl_encrypt::serialize_pub_key(pk_stream, s.pub_key);
    //     send_stringstream_socket(pk_stream, socket);
    // }

    // void save_public_key(DoubleECPoint & pk)
    // {
    //     // std::cout<<"crypto_core_ristretto255_BYTES="<<crypto_core_ristretto255_BYTES<<std::endl;
    //     for (size_t i=0; i<crypto_core_ristretto255_BYTES; i++)
    //     {
    //         // std::cout<<"i="<<i<<",";

    //         // std::cout<<"zeros_cipherR[i]="<<zeros_cipherR[i]<<std::endl;
    //         // std::cout<<"zeros_cipherQ[i]="<<zeros_cipherQ[i]<<std::endl;
    //         pk.R.P[i] = zeros_cipherR[i];
    //         pk.Q.P[i] = zeros_cipherQ[i];
    //     }
    //     // std::cout<<std::endl;

    // }


    void save_public_key(stringstream & pk_stream)
    {

            //     stringstream pk_stream;
        ipcl_encrypt::serialize_pub_key(pk_stream, key.pub_key);
    }


    void hash_to_point(const unsigned char * id, unsigned char * ec_point)
    {
        // unsigned char ru[crypto_core_ristretto255_HASHBYTES];
        if (crypto_core_ristretto255_from_hash(ec_point, id) != 0 ||
            crypto_core_ristretto255_is_valid_point(ec_point) != 1) {
            printf("crypto_core_ristretto255_from_hash() failed\n");
        }
    }

    void hash_to_point(const string id, unsigned char * ec_point)
    {
        size_t length = min(id.size(), (size_t) crypto_core_ristretto255_HASHBYTES);
        unsigned char id_array[crypto_core_ristretto255_HASHBYTES];
        for (size_t i=0; i<length; i++)
        {
            id_array[i] = id[i];
        }
        for (size_t i=length; i<crypto_core_ristretto255_HASHBYTES; i++)
        {
            id_array[i] = 0;
        }
        // ec_encryptedID(id_array, ec_point);
        hash_to_point(id_array, ec_point);
    }

  std::vector<ECPoint> hash_to_points(const vector<string> & v_ids)
  {
     size_t length = v_ids.size();
        std::vector<ECPoint> hashed_points(length);
        for (size_t i=0; i<length; i++)
        {
            hash_to_point(v_ids[i], hashed_points[i].P);
        }
        return hashed_points;
  }






    vector<ECPoint> ec_encryptedID(const vector<ECPoint> & ec_points)
    {
        size_t num = ec_points.size();
        vector<ECPoint> encrypted_ec_points(num);
        for (size_t i=0; i<num; i++)
        {
            crypto_scalarmult_ristretto255(encrypted_ec_points[i].P, s, ec_points[i].P);
        }
        return encrypted_ec_points;
    }


    vector<ECPoint> ec_decryptedID(const vector<ECPoint> & ec_points)
    {
        size_t num = ec_points.size();
        vector<ECPoint> ec_decpt_points(num);
        for (size_t i=0; i<num; i++)
        {
            crypto_scalarmult_ristretto255(ec_decpt_points[i].P, s_inv, ec_points[i].P);
        }
        return ec_decpt_points;
    }



    



    // void ec_encrypt1bit(bool label,  unsigned char * R, unsigned char * Q)  
    // {
    //     // unsigned char s[crypto_core_ristretto255_SCALARBYTES]
    //     // unsigned char R[crypto_core_ristretto255_BYTES];
    //     // unsigned char Q[crypto_core_ristretto255_BYTES];

    //     // ([r]P, [(-1)^label]P+[rs]P) =(R, Q)
    //     // unsigned char P[crypto_core_ristretto255_BYTES];
    //     unsigned char r[crypto_core_ristretto255_SCALARBYTES];

    //     unsigned char sR[crypto_core_ristretto255_BYTES];
    //     // crypto_scalarmult_ristretto255_base(P, one);
    
    
    //     crypto_core_ristretto255_scalar_random(r);
    //     crypto_scalarmult_ristretto255_base(R, r);
    //     crypto_scalarmult_ristretto255(sR, s, R);
    //     if (label)
    //     {
    //         crypto_core_ristretto255_sub(Q, sR, P);
    //     }
    //     else
    //     {
    //         crypto_core_ristretto255_add(Q, sR, P);
    //     }
        
        
    // }



    // bool ec_decrypt1bit(const unsigned char * R, const unsigned char * Q)
    // {
    //     unsigned char sR[crypto_core_ristretto255_BYTES];
    //     unsigned char labelP[crypto_core_ristretto255_BYTES];
    //     crypto_scalarmult_ristretto255(sR, s, R);
    //     crypto_core_ristretto255_sub(labelP, Q, sR);
    //     int equal = sodium_memcmp(labelP, P, crypto_core_ristretto255_BYTES);
    //     return equal%2;
    // }
 



    // std::vector<DoubleECPoint> ec_batch_encrypt1bit(const std::vector<bool> & labels)
    // {
    //     size_t batch_size=labels.size();
    //     std::vector<DoubleECPoint> RsQs(batch_size);
    //     for (size_t i=0; i<batch_size; i++)
    //     {
    //         ec_encrypt1bit(labels[i],  RsQs[i].R.P, RsQs[i].Q.P);
    //     }
    //     return RsQs;
    // }

    // std::vector<ipcl::CipherText> pl_batch_encrypt_labels(const std::vector<int32_t> & labels)
    std::vector<std::string> pl_batch_encrypt_labels(const std::vector<int32_t> & labels)

    {
        size_t batch_size=labels.size();
        std::vector<ipcl::CipherText> v_EP(batch_size);
        std::vector<std::string> v_EP_str(batch_size);
        for (size_t i=0; i<batch_size; i++)
        {
            enc_uint32_t(labels[i], key.pub_key, v_EP[i]);
            stringstream tmp_stream;
            ipcl_encrypt::serialize_ciphertext(tmp_stream, v_EP[i]);
            v_EP_str[i]=tmp_stream.str();
        }
        
        
        // return v_EP;
        return v_EP_str;
    }



    // std::vector<bool> ec_batch_decrypt1bit(const std::vector<DoubleECPoint> & RQs)
    // {
    //     size_t batch_size=RQs.size();
    //     std::vector<bool> labels(batch_size);
    //     for (size_t i=0; i<batch_size; i++)
    //     {
    //         labels[i] = ec_decrypt1bit(RQs[i].R.P, RQs[i].Q.P);
    //     }
    //     return labels;
    // }

    // std::vector<bool> ec_batch_decrypt1bit(const std::vector<DoubleECPointPlus1bit> & RQbs)
    // {
    //     size_t batch_size=RQbs.size();
    //     std::vector<bool> labels(batch_size);
    //     for (size_t i=0; i<batch_size; i++)
    //     {
    //         labels[i] = ec_decrypt1bit(RQbs[i].R.P, RQbs[i].Q.P);
    //     }
    //     return labels;
    // }

    std::vector<uint32_t> pl_batch_decrypt_labels(const std::vector<ipcl::CipherText> & Elabels)
    {
        size_t batch_size=Elabels.size();
        std::vector<uint32_t> labels(batch_size);
        for (size_t i=0; i<batch_size; i++)
        {
            // labels[i] = ec_decrypt1bit(RQbs[i].R.P, RQbs[i].Q.P);
            labels[i] = dec_uint32_t(Elabels[i], key.priv_key);
        }
        return labels;
    }

    std::vector<uint32_t> pl_batch_decrypt_labels(std::vector<std::string> & Elabels)
    {
        size_t batch_size=Elabels.size();
        std::vector<uint32_t> labels(batch_size);
        ipcl::CipherText ct_tmp;
        for (size_t i=0; i<batch_size; i++)
        {
            // labels[i] = ec_decrypt1bit(RQbs[i].R.P, RQbs[i].Q.P);
            std::stringstream elabel_stream(Elabels[i]);
            ipcl_encrypt::deserialize_ciphertext(elabel_stream, key.pub_key, ct_tmp);
            labels[i] = dec_uint32_t(ct_tmp, key.priv_key);
        }
        return labels;
    }




    ECPaillierPSIQuery generate_query(const std::vector<string> & IDs, const std::vector<int32_t> & labels)
    {
        if (IDs.size()!=labels.size())
        {
            throw ("must have IDs.size()==labels.size()");
        }
        std::vector<ECPoint> hashed_points = hash_to_points(IDs);
        // std::cout<<"hashed_points="<<std::endl;                    
        // print(hashed_points);
        ECPaillierPSIQuery ec_paillier_psi_query;
        ec_paillier_psi_query.EIDs = ec_encryptedID(hashed_points);
        ec_paillier_psi_query.Elabels = pl_batch_encrypt_labels(labels);
        return ec_paillier_psi_query;
    }



    std::vector<int32_t> generate_response(ECPaillierPSIResponce response)
    {
        vector<int32_t>  intersection_lastids;
        vector<ECPoint> EIDs = ec_decryptedID(response.EEIDs);
        // vector<bool> b1s = ec_batch_decrypt1bit(response.R1Q1s);
        vector<uint32_t> b1s = pl_batch_decrypt_labels(response.Elabels);

        //  求交 shortEIDs & EIDs
            // std::unordered_set<ShortECPoint> ShortEIDHashedSet;
            std::unordered_map<ShortECPoint, int32_t, ShortECPointHash> ShortEIDHashMap;
 
        // 向哈希集合中插入元素
        for (size_t i=0; i<response.ShortEIDs.size(); i++) {
            ShortEIDHashMap[response.ShortEIDs[i]] = i;
        }

        for (size_t i=0; i<EIDs.size(); i++)
        {
            ShortECPoint shortEID = cut_off_ec_point(EIDs[i]);

            auto it = ShortEIDHashMap.find(shortEID);

            if (it != ShortEIDHashMap.end()) {
                // Int32AndBool new_id_and_b1;
                std::array<int32_t, 2> new_id_and_b1;
                new_id_and_b1[0] = it->second;     // new_id
                new_id_and_b1[1] = b1s[i];
                new_id_and_b1s.push_back(new_id_and_b1);
                intersection_lastids.push_back(i);
            }
        }

        return intersection_lastids;
    }

    std::vector<std::array<int32_t, 2>> return_answer()
    {
        return new_id_and_b1s;
    }



};

//---------- ECPaillierPlayer0


class ECPaillierPlayer0
{
    private:
    unsigned char one[crypto_core_ristretto255_SCALARBYTES]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // unsigned char minus_one[crypto_core_ristretto255_SCALARBYTES];
    unsigned char s[crypto_core_ristretto255_SCALARBYTES];  // secret key for label encrypt
    unsigned char P[crypto_core_ristretto255_BYTES];
    // unsigned char zeros_cipherR[crypto_core_ristretto255_BYTES];
    // unsigned char zeros_cipherQ[crypto_core_ristretto255_BYTES];
    // std::unordered_set<ShortECPoint> ShortEIDs;
    ipcl::PublicKey paillier_pub_key=ipcl::PublicKey();
    std::vector<ShortECPoint> ShortEIDs;
     std::vector<uint32_t>  b0s;

    public:
    ECPaillierPlayer0() {
        // crypto_core_ristretto255_scalar_negate(minus_one, one);
        crypto_scalarmult_ristretto255_base(P, one);
        generate_private_key();
        // receive_public_key();
    }

    ~ECPaillierPlayer0() {
        // if (net_client_ != nullptr) {
        //     net_client_->Leave();
        // }
        
    }

    void generate_private_key()
    {
        crypto_core_ristretto255_scalar_random(s);
    }

    // void receive_public_key()
    // {
    //     // receive(zeros_cipherR, crypto_core_ristretto255_BYTES);
    //     // receive(zeros_cipherQ, crypto_core_ristretto255_BYTES);
    //     receive(paillier_pub_key);
    // }

    void set_public_key(std::stringstream & pk_stream)
    {
        // ipcl::serializer::deserialize<ipcl::PublicKey>(pk_stream, paillier_pub_key);
        ipcl_encrypt::deserialize_pub_key(pk_stream, paillier_pub_key);
    }


    void hash_to_point(const unsigned char * id, unsigned char * ec_point)
    {
        // unsigned char ru[crypto_core_ristretto255_HASHBYTES];
        if (crypto_core_ristretto255_from_hash(ec_point, id) != 0 ||
            crypto_core_ristretto255_is_valid_point(ec_point) != 1) {
            printf("crypto_core_ristretto255_from_hash() failed\n");
        }
    }

    void hash_to_point(const string id, unsigned char * ec_point)
    {
        size_t length = min(id.size(), (size_t) crypto_core_ristretto255_HASHBYTES);
        unsigned char id_array[crypto_core_ristretto255_HASHBYTES];
        for (size_t i=0; i<length; i++)
        {
            id_array[i] = id[i];
        }
        for (size_t i=length; i<crypto_core_ristretto255_HASHBYTES; i++)
        {
            id_array[i] = 0;
        }
        // ec_encryptedID(id_array, ec_point);
        hash_to_point(id_array, ec_point);
    }

  std::vector<ECPoint> hash_to_points(const vector<string> & v_ids)
  {
     size_t length = v_ids.size();
        std::vector<ECPoint> hashed_points(length);
            //  #pragma omp parallel for num_threads(16) collapse(1)  //private(reduced_poly_cipher)  
        for (size_t i=0; i<length; i++)
        {
            hash_to_point(v_ids[i], hashed_points[i].P);
        }
        return hashed_points;
  }



    vector<ECPoint> ec_encryptedID(const vector<ECPoint> & ec_points)
    {
        size_t num = ec_points.size();
        vector<ECPoint> encrypted_ec_points(num);
        for (size_t i=0; i<num; i++)
        {
            crypto_scalarmult_ristretto255(encrypted_ec_points[i].P, s, ec_points[i].P);
        }
        return encrypted_ec_points;
    }




    ShortECPoint ec_encryptedID_toshort(ECPoint ec_point)
    {
        ECPoint encrypted_ec_point;
        crypto_scalarmult_ristretto255(encrypted_ec_point.P, s, ec_point.P);
        return cut_off_ec_point(encrypted_ec_point);
    }

    std::vector<ShortECPoint> ec_encryptedID_toshort(vector<ECPoint> ec_points)
    {
        std::vector<ShortECPoint> v_ec_points_short(ec_points.size());
        // #pragma omp parallel for num_threads(16) collapse(1)  // private(reduced_poly_cipher)  
        for (size_t i=0; i<ec_points.size(); i++)
        {
            v_ec_points_short[i] =  ec_encryptedID_toshort(ec_points[i]);
        }
        return v_ec_points_short;
    }

    // void ec_encryptedID_toshort(vector<ECPoint> ec_points)
    // {
    //     for (size_t i=0; i<ec_points.size(); i++)
    //     {
    //         ShortEIDs.inssert(ec_encryptedID_toshort(ec_points[i]));
    //     }
    // }






    // void _ec_batch_cipher_split_bshares(const unsigned char * R, const unsigned char * Q,  bool & b0, unsigned char * R1, unsigned char * Q1)
    // { // Q = (-1)^b0 Q1    R = (-1)^b0 R1  
        
    //     // randombytes((unsigned char *) b0s, (batch_size+7)/8);

    //     b0 = rand()%2;
    
    //     if(b0==1)
    //     {
    //         crypto_scalarmult_ristretto255(R1, minus_one, R);
    //         crypto_scalarmult_ristretto255(Q1, minus_one, Q);
    //     }
    //     else
    //     {
    //         crypto_scalarmult_ristretto255(R1, one, R);
    //         crypto_scalarmult_ristretto255(Q1, one, Q);
    //     }

    //     unsigned char r[crypto_core_ristretto255_SCALARBYTES];
    //     crypto_core_ristretto255_scalar_random(r);
    //     crypto_scalarmult_ristretto255(zeros_cipherR, r, zeros_cipherR);
    //     crypto_scalarmult_ristretto255(zeros_cipherQ, r, zeros_cipherQ);

    //     crypto_core_ristretto255_add(R1, zeros_cipherR, R1);
    //     crypto_core_ristretto255_add(Q1, zeros_cipherQ, Q1);

    // }

    // DoubleECPointPlus1bit ec_batch_cipher_split_bshares(const ECPoint & R, const ECPoint & Q)
    // {
    //     DoubleECPointPlus1bit R1Q1b0;
    //      _ec_batch_cipher_split_bshares(R.P, Q.P, R1Q1b0.b, R1Q1b0.R.P, R1Q1b0.Q.P);
    //      return R1Q1b0;
    // }



    // std::vector<DoubleECPointPlus1bit> ec_batch_cipher_split_bshares(const std::vector<DoubleECPoint> & RQs)
    // {
    //     size_t length = RQs.size();

    //     std::vector<DoubleECPointPlus1bit> R1Q1b0s(length);
    //     for (size_t i=0; i< length; i++)
    //     {
    //         R1Q1b0s[i] = ec_batch_cipher_split_bshares(RQs[i].R, RQs[i].Q);
    //     }
    //     return R1Q1b0s;
    // }



    std::vector<PaillierCipherPlusShare0> ec_batch_cipher_split_ashares(const std::vector<ipcl::CipherText> & pailler_ciphers)
   {
        size_t length = pailler_ciphers.size();
        std::vector<PaillierCipherPlusShare0> pcipher_b0s(length);
        uint32_t r;
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        for (size_t i=0; i< length; i++)
        {
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            // std::cout<<"i="<<i<<std::endl;
            r = random();
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            ipcl::PlainText pt_mr = ipcl::PlainText(-r);
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            pcipher_b0s[i].Eshare1=pailler_ciphers[i]+pt_mr;
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            pcipher_b0s[i].share0=r;
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        }
        return pcipher_b0s;
    }


    void init(std::vector<string> plaintext_ids)
    {
        std::vector<ECPoint> hashed_points = hash_to_points(plaintext_ids);
        ShortEIDs = ec_encryptedID_toshort(hashed_points);
    }

    ECPaillierPSIResponce generate_response(ECPaillierPSIQuery & query)
    {
        ECPaillierPSIResponce ec_psi_responce;
        ec_psi_responce.ShortEIDs = ShortEIDs;
            // std::vector<ECPoint> twice_encrypt_batch_ids 
        size_t query_size = query.EIDs.size();
        std::random_device rd;
        std::mt19937 g(rd());
        std::vector<int32_t> index(query_size);
        for (size_t i=0; i<query_size; i++)
        {
            index[i]=i;
        }

        // // 对两个vector进行shuffle
        std::shuffle(index.begin(), index.end(), g);
    
        std::vector<ECPoint> EIDs(query_size);
        std::vector<ipcl::CipherText> Elabels(query_size);
        // std::vector<std::stringstream> Elabels(query_size);
        for (size_t i=0; i<query_size; i++)
        {
            EIDs[i] = query.EIDs[index[i]];
            stringstream tmp_stream;
            tmp_stream<<query.Elabels[index[i]];
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            // std::cout<<"i="<<i<<std::endl;
            Elabels[i] = ipcl::CipherText();
            ipcl_encrypt::deserialize_ciphertext(tmp_stream, paillier_pub_key,
                                   Elabels[i]);

        }

// std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
 
        ec_psi_responce.EEIDs = ec_encryptedID(EIDs);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        std::vector<PaillierCipherPlusShare0> eb1_b0s = ec_batch_cipher_split_ashares(Elabels);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        b0s.resize(eb1_b0s.size());
        ec_psi_responce.Elabels.resize(eb1_b0s.size());
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        for (size_t i=0; i<eb1_b0s.size(); i++)
        {
            stringstream elabel_stream;
            ipcl_encrypt::serialize_ciphertext(elabel_stream, eb1_b0s[i].Eshare1);
            ec_psi_responce.Elabels[i] = elabel_stream.str();
            b0s[i] = eb1_b0s[i].share0;
        }
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        return  ec_psi_responce;     
    } 


    std::vector<bool> return_answer(std::vector<int32_t> intersection_lastids)
    {
        std::vector<bool> intersection_b0s(intersection_lastids.size());
        for (size_t i=0; i<intersection_lastids.size(); i++)
        {
            intersection_b0s[i]=b0s[intersection_lastids[i]];
        }
        return intersection_b0s;
    }


};