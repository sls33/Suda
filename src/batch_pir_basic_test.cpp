#include "include/batch_pir_basic.h"
#include <ctime>

using namespace std;
using namespace seal;


void test_host()
{

    size_t host_log_n_data=24;
    auto poly_host=PolyProcessorHost(host_log_n_data, 4096);
    // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    int host_n_data = (1<<host_log_n_data);
    std::vector<int64_t> data(host_n_data), coeff(host_n_data);
    for (int i = 0; i < host_n_data; i++) {
        data[i] = random() % poly_host.get_prime_num();
    }

    coeff = poly_host.fft_interpolate(data);

    for (int i=0; i<5; i++)
    {
        std::cout<<coeff[i]<<std::endl;
    }
}

void test_guest()
{
        size_t host_log_n_data=24;
        size_t batch_size = 4096;
    auto poly_guest=PolyProcessorGuest(host_log_n_data, batch_size);
    vector<int64_t> new_ids={0, 1,2,3};
    auto cipher_x_powers = poly_guest.gen_chara_Xpowers_cipher_polys(new_ids);

    std::cout<<"cipher_x_powers.size()="<<cipher_x_powers.size()<<std::endl;


}





void poly_reduce_test_from_nttform()
{
    size_t host_log_n_data=20;
    size_t batch_size = 4096;
    size_t feature_num=100;
    //-------host offline--------------------
    int host_n_data = (1<<host_log_n_data);
    std::vector<int64_t> data(host_n_data);
    auto poly_host=PolyProcessorHost(host_log_n_data, batch_size);
    for (int i = 0; i < host_n_data; i++) {
        data[i] = random() % poly_host.get_prime_num();
    }
    std::cout<<"data="<<std::endl;
    for (int i=0; i<4; i++)
    {
        std::cout<<data[i]<<std::endl;
    }
    std::cout<<"...."<<std::endl;
    for (int i=batch_size-4; i<batch_size; i++)
    {
        std::cout<<data[i]<<std::endl;
    }
   vector<Plaintext> pieces_f = poly_host.encode(data, batch_size, true);
   std::cout<<"pieces_f.size()="<<pieces_f.size()<<std::endl;


    //--------------guest----------------------
    auto poly_guest=PolyProcessorGuest(host_log_n_data, batch_size);
    vector<int64_t> new_ids(batch_size);
    for (int i=0; i<batch_size; i++)
    {
        new_ids[i]=i;
    }

    // vector<string> cipher_x_powers = poly_guest.gen_cipher_polys(new_ids);
    vector<string> cipher_x_powers = poly_guest.gen_chara_Xpowers_cipher_polys(new_ids);
    std::cout<<"cipher_x_powers.size()="<<cipher_x_powers.size()<<std::endl;
    send(cipher_x_powers);
    poly_guest.send_public_keys();
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //--------------host---------------------------
    receive(cipher_x_powers);
    poly_host.receive_public_keys();
    Ciphertext reduced_poly_cipher;
    vector<int64_t> revealed_poly;
    time_t  start = time(NULL);
    poly_host.set_chara_and_Xpowers_mod_chara(cipher_x_powers, true);
    for (size_t feature_id=0; feature_id<feature_num; feature_id++)
    {   
        //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        reduced_poly_cipher = poly_host.poly_reduce(pieces_f);
        send(reduced_poly_cipher);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        //---------------guest------------------------
        receive(reduced_poly_cipher);
        revealed_poly = poly_guest.decrypt_poly(reduced_poly_cipher);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    }
    time_t  end = time(NULL);
    std::cout<<"reduce time="<<end-start<<std::endl;
     //---------------guest------------------------
    vector<int64_t> result = poly_guest.eveal_poly_value(revealed_poly, new_ids);
   
    std::cout<<"result="<<std::endl;
    for (int i=0; i<4; i++)
    {
        std::cout<<result[i]<<std::endl;
    }
    std::cout<<"....."<<std::endl;
    for (int i=batch_size-4; i<batch_size; i++)
    {
        std::cout<<result[i]<<std::endl;
    }
}









int main()
{
    // test_host();
    // test_guest();
    // poly_reduce_test();
    poly_reduce_test_from_nttform();


    
}



