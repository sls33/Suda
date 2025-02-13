#include "include/ec_psi.h"
#include <iostream>
#include <iomanip>
using namespace std;

bool is_party0=true;
bool is_party1=true;

void test_encrypt_decrypt_label()
{
    ECPlayer1 P1=ECPlayer1();
    unsigned char R[crypto_core_ristretto255_BYTES]; 
    unsigned char Q[crypto_core_ristretto255_BYTES];

    bool labels[] = {true, false, false, false, true, false, false, true};

    for (int i=0; i<sizeof(labels); i++)
    {
        P1.ec_encrypt1bit(labels[i],  R, Q);  
        bool result = P1.ec_decrypt1bit(R, Q);
        std::cout<<"result="<<result<<std::endl;

    }
}




void test_encrypt_decrypt_id()
{
    ECPlayer1 P1=ECPlayer1();
    std::vector<string>  batch_ids={"edd", "dsvfdv", "de3e3", "cdfef"};


    std::vector<ECPoint> hashed_points = P1.hash_to_points(batch_ids);

    std::cout<<"hashed_points ="<<std::endl;                    
    print(hashed_points );

    std::vector<ECPoint> encrypt_batch_ids = P1.ec_encryptedID(hashed_points);
    std::vector<ECPoint> decrypt_batch_ids = P1.ec_decryptedID(encrypt_batch_ids);
    std::cout<<"decrypt_batch_ids="<<std::endl;                    
    print(decrypt_batch_ids);
}




void test_encrypt_decrypt_id2()
{
    ECPlayer1 P1=ECPlayer1();
    ECPlayer0 P0=ECPlayer0();
    std::vector<string>  batch_ids={"edd", "dsvfdv", "de3e3", "cdfef"};
    std::vector<ECPoint> hashed_points = P1.hash_to_points(batch_ids);
    std::cout<<"hashed_points="<<std::endl;                    
    print(hashed_points);


    std::vector<ECPoint> encrypt_batch_ids = P1.ec_encryptedID(hashed_points);
    encrypt_batch_ids = P0.ec_encryptedID(encrypt_batch_ids);
    std::vector<ECPoint> decrypt_batch_ids = P1.ec_decryptedID(encrypt_batch_ids);
    std::cout<<"decrypt_batch_ids="<<std::endl;                    
    print(decrypt_batch_ids);


    std::vector<ShortECPoint> encrypt_batch_ids_short = P0.ec_encryptedID_toshort(hashed_points);
    std::cout<<"encrypt_batch_ids_short="<<std::endl;
    print(encrypt_batch_ids_short);

}






void test_encrypt_decrypt_label2()
{
    ECPlayer1 P1=ECPlayer1();
    ECPlayer0 P0=ECPlayer0();
    std::vector<bool> labels = {true, false, false, false, true, false, false, true};

    std::cout<<"labels="<<std::endl;
    for (size_t i=0; i<labels.size(); i++)
    {
        std::cout<<labels[i]<<std::endl;
    }

    std::vector<DoubleECPoint> encrypt_labels = P1.ec_batch_encrypt1bit(labels);
    std::vector<bool> decrypted_labels = P1.ec_batch_decrypt1bit(encrypt_labels);

    std::cout<<"decrypted_labels="<<std::endl;
    for (size_t i=0; i<labels.size(); i++)
    {
        std::cout<<decrypted_labels[i]<<std::endl;
    }


}


void test_encrypt_decrypt_label3()
{
    ECPlayer1 P1=ECPlayer1();
    ECPlayer0 P0=ECPlayer0();
    std::vector<bool> labels = {true, false, false, false, true, false, false, true};

    std::cout<<"labels="<<std::endl;
    for (size_t i=0; i<labels.size(); i++)
    {
        std::cout<<labels[i]<<std::endl;
    }

    std::vector<DoubleECPoint> encrypt_labels = P1.ec_batch_encrypt1bit(labels);

 std::vector<DoubleECPointPlus1bit> R1Q1b0s = P0.ec_batch_cipher_split_bshares(encrypt_labels);

    std::vector<bool> b1s = P1.ec_batch_decrypt1bit(R1Q1b0s);


    std::cout<<"label, result="<<std::endl;
    for (size_t i=0; i<labels.size(); i++)
    {
        std::cout<<labels[i]<<","<< (R1Q1b0s[i].b ^ b1s[i]) <<std::endl;
    }


}





void test_encrypt_decrypt_idlabels()  
{
    ECPlayer1 P1=ECPlayer1();
    ECPlayer0 P0=ECPlayer0();
                          // new_id = 0        1         2       3         4      5        6       7      8       9       10
    std::vector<string>  id_tables = {"edd", "dsvfdv", "grf8", "de3e3", "cdfef", "bfe4", "lkjh", "jit9", "fd9i", "lkg9", "ddw2"};
    std::vector<string>  batch_ids={"edd", "dsvfdv", "de3e3", "cdfef", "tret", "bfe4", "fd9i", "lkg9", "ddw2", "lkjh"};
    std::vector<bool> batch_labels = {true, false, false,      false,   true,   false, false,   true,   true,  false};

    P0.init(id_tables);
    auto query = P1.generate_query(batch_ids, batch_labels);
    // std::vector<bool> b0s;
    auto response = P0.generate_response(query);
    auto intersection_lastids = P1.generate_response(response);
    auto new_id_and_b1s = P1.return_answer();
    auto b0s = P0.return_answer(intersection_lastids);

    std::cout<<"new_id, label="<<std::endl;
    for (size_t i=0; i<b0s.size(); i++)
    {
        std::cout<< new_id_and_b1s[i].a <<","<< (b0s[i] ^ new_id_and_b1s[i].b) <<std::endl;
    }

}


void test_shuffle()
{
        std::random_device rd;
        std::mt19937 g(rd());

        std::vector<int> x = {0, 1,2,3,4,5,6,7};
        std::vector<char> y = {'a','b','c','d','e','f','g', 'h'};

        // 对两个vector进行shuffle
        std::shuffle(x.begin(), x.end(),  g);
        for (size_t i=0; i<x.size(); i++)
        {
            std::cout<<x[i];
        }
        std::cout<<std::endl;
        for (size_t i=0; i<y.size(); i++)
        {
            std::cout<<y[x[i]];
        }
         std::cout<<std::endl;

}


int main(int argc, char **argv) {
// test_encrypt_decrypt();
// test_encrypt_decrypt_id2();
// test_encrypt_decrypt_label2();
// test_encrypt_decrypt_label3();
test_encrypt_decrypt_idlabels();
// test_shuffle();
return 0;

}

