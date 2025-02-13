#include "batch_pir_basic.h"
#include <cmath>





void batch_pir_from_nttform_modswitch(bool isHost, bool isGuest, std::vector<int64_t> & data, vector<int64_t> &new_ids, vector<vector<int64_t>>& result,
size_t feature_num=100, size_t host_log_n_data=20, size_t batch_size = 4096, bool modswitch=false)
{
    vector<string> cipher_x_powers_str;
    vector<Ciphertext> cipher_x_powers;
    shared_ptr<PolyProcessorHost> poly_host = nullptr;
    shared_ptr<PolyProcessorGuest> poly_guest = nullptr;
    // vector<int64_t> coeffs;
    vector<Plaintext> pieces_f;
    // vector<int64_t> new_ids(batch_size);
    result.resize(feature_num);
    vector<vector<int64_t>> reduced_f(feature_num);
    time_t start, end;
std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //-------host offline--------------------
    if (isHost)
    {
        int host_n_data = (1<<host_log_n_data);
        if (data.size()!=host_n_data)
        {
            throw("must have data.size()==host_n_data");
        }        
        poly_host= make_shared<PolyProcessorHost>(host_log_n_data, batch_size);
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        pieces_f = poly_host->encode(data, batch_size, modswitch);
    }

std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //--------------guest----------------------
    if (isGuest)
    {
        poly_guest=make_shared<PolyProcessorGuest>(host_log_n_data, batch_size);
        if (new_ids.size()!=batch_size)
        {
            throw("must hive new_ids.size()==batch_size");
        }
        cipher_x_powers_str = poly_guest->gen_chara_Xpowers_cipher_polys(new_ids);
        std::cout<<"cipher_x_powers.size()="<<cipher_x_powers_str.size()<<std::endl;
        send(cipher_x_powers_str);
        poly_guest->send_public_keys();
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    }

    //--------------host---------------------------
    if (isHost)
    {
        receive(cipher_x_powers_str);
        poly_host->set_chara_and_Xpowers_mod_chara(cipher_x_powers_str, modswitch);
        poly_host->receive_public_keys();   
    }


    vector<Ciphertext> reduced_poly_cipher(feature_num);
    vector<vector<int64_t>> plain_poly(feature_num);
    //--------------host---------------------------

    if (isHost)
    {    start = time(NULL);
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {   
            reduced_poly_cipher[feature_id] = poly_host->poly_reduce(pieces_f);
        }
        end = time(NULL);
        std::cout<<"reduce time in host="<<end-start<<std::endl;
        // send(reduced_poly_cipher);
    }
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    
        //---------------guest------------------------
    if (isGuest)
    {
        start = time(NULL);
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            plain_poly[feature_id] = poly_guest->decrypt_poly(reduced_poly_cipher[feature_id]);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

        result = poly_guest->eval_batch_poly_value(plain_poly,  new_ids, true);

        end = time(NULL);
        std::cout<<"decrypt and evaluation time in guest="<<end-start<<std::endl;
    }

    //--------------------guest for test--------------

    if (isHost & isGuest)
    {
        // for (int feature_id=0; feature_id<feature_num; feature_id++)
        for (int feature_id=0; feature_id<1; feature_id++)
        {
            std::cout<<"result="<<std::endl;
            for (int i=0; i<4; i++)
            {
                std::cout<<result[feature_id][i]<<std::endl;
            }
            std::cout<<"....."<<std::endl;
            for (int i=batch_size-4; i<batch_size; i++)
            {
                std::cout<<result[feature_id][i]<<std::endl;
            }
        }
    
    }

}













void batch_pir_from_nttform_modswitch(bool isHost, bool isGuest, std::vector<std::vector<int64_t>> & data, vector<int64_t> &new_ids, vector<vector<int64_t>>& result,
bool modswitch=false)
{
    vector<string> cipher_x_powers_str;
    vector<Ciphertext> cipher_x_powers;
    shared_ptr<PolyProcessorHost> poly_host = nullptr;
    shared_ptr<PolyProcessorGuest> poly_guest = nullptr;
    size_t feature_num=data.size();
    size_t batch_size=new_ids.size();
    vector<vector<Plaintext>> pieces_f(feature_num);
    result.resize(feature_num);
    vector<vector<int64_t>> reduced_f(feature_num);
    int host_n_data = data[0].size();
    size_t host_log_n_data = log2(host_n_data);
    vector<Ciphertext> reduced_poly_cipher(feature_num);
    vector<vector<int64_t>> plain_poly(feature_num);
    time_t start, end;
std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //-------host offline--------------------
    if (isHost)
    {   start = time(NULL);
  
        poly_host= make_shared<PolyProcessorHost>(host_log_n_data, batch_size);
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        for (size_t i=0; i<feature_num; i++)
        {
            pieces_f[i] = poly_host->encode(data[i], batch_size, modswitch);
        }
        end = time(NULL);
        std::cout<<"preprocess time in host="<<end-start<<std::endl;
        
    }

std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //--------------guest----------------------
    if (isGuest)
    {
        start = time(NULL);
        poly_guest=make_shared<PolyProcessorGuest>(host_log_n_data, batch_size);
        if (new_ids.size()!=batch_size)
        {
            throw("must hive new_ids.size()==batch_size");
        }
        cipher_x_powers_str = poly_guest->gen_chara_Xpowers_cipher_polys(new_ids);
        std::cout<<"cipher_x_powers.size()="<<cipher_x_powers_str.size()<<std::endl;
         end = time(NULL);
        std::cout<<"preprocess time in guest="<<end-start<<std::endl;

        send(cipher_x_powers_str);
        poly_guest->send_public_keys();
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    }

    //--------------host---------------------------
    if (isHost)
    {

        start = time(NULL);
        receive(cipher_x_powers_str);
        poly_host->set_chara_and_Xpowers_mod_chara(cipher_x_powers_str, modswitch);
        poly_host->receive_public_keys();   
    

    //--------------host---------------------------

         
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {   
            reduced_poly_cipher[feature_id] = poly_host->poly_reduce(pieces_f[feature_id]);
        }
        end = time(NULL);
        std::cout<<"reduce time in host="<<end-start<<std::endl;
        send(reduced_poly_cipher);
    }
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    
        //---------------guest------------------------
    if (isGuest)
    {
        receive(reduced_poly_cipher);
        start = time(NULL);

        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            plain_poly[feature_id] = poly_guest->decrypt_poly(reduced_poly_cipher[feature_id]);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

        result = poly_guest->eval_batch_poly_value(plain_poly,  new_ids, true);

        end = time(NULL);
        std::cout<<"decrypt and evaluation time in guest="<<end-start<<std::endl;
    }

    //--------------------guest for test--------------

    if (isHost & isGuest)
    {
        // for (int feature_id=0; feature_id<feature_num; feature_id++)
        for (int feature_id=0; feature_id<1; feature_id++)
        {
            std::cout<<"result="<<std::endl;
            for (int i=0; i<4; i++)
            {
                std::cout<<result[feature_id][i]<<std::endl;
            }
            std::cout<<"....."<<std::endl;
            for (int i=batch_size-4; i<batch_size; i++)
            {
                std::cout<<result[feature_id][i]<<std::endl;
            }
        }
    
    }

}






void batch_pir_from_nttform_type0(bool isHost, bool isGuest, std::vector<std::vector<int64_t>> & data, vector<int64_t> &new_ids, vector<vector<int64_t>>& result)
{
    vector<string> cipher_x_powers_str;
    vector<Ciphertext> cipher_x_powers;
    shared_ptr<PolyProcessorHost> poly_host = nullptr;
    shared_ptr<PolyProcessorGuest> poly_guest = nullptr;
    size_t feature_num=data.size();
    size_t batch_size=new_ids.size();
    vector<vector<Plaintext>> pieces_f(feature_num);
    result.resize(feature_num);
    int host_n_data = data[0].size();
    size_t host_log_n_data = log2(host_n_data);
    vector<Ciphertext> reduced_poly_cipher(feature_num);
    vector<vector<int64_t>> plain_poly(feature_num);
    time_t start, end;
std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //-------host offline--------------------
    if (isHost)
    {   start = time(NULL);
  
        poly_host= make_shared<PolyProcessorHost>(host_log_n_data, batch_size, 0);
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
  
        for (size_t i=0; i<feature_num; i++)
        {
            pieces_f[i] = poly_host->encode(data[i], batch_size, false);
        }
        end = time(NULL);
        std::cout<<"preprocess time in host="<<end-start<<std::endl;
        
    }

std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //--------------guest----------------------
    if (isGuest)
    {
        start = time(NULL);
        poly_guest=make_shared<PolyProcessorGuest>(host_log_n_data, batch_size, 0);
        if (new_ids.size()!=batch_size)
        {
            throw("must hive new_ids.size()==batch_size");
        }
        cipher_x_powers_str = poly_guest->gen_chara_Xpowers_cipher_polys(new_ids);
        std::cout<<"cipher_x_powers.size()="<<cipher_x_powers_str.size()<<std::endl;
         end = time(NULL);
        std::cout<<"preprocess time in guest="<<end-start<<std::endl;

        send(cipher_x_powers_str);
        poly_guest->send_public_keys();
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    }

    //--------------host---------------------------
    if (isHost)
    {

        start = time(NULL);
        receive(cipher_x_powers_str);
        poly_host->set_chara_and_Xpowers_mod_chara(cipher_x_powers_str, false);
        poly_host->receive_public_keys();   
    

    //--------------host---------------------------

         
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {   
            reduced_poly_cipher[feature_id] = poly_host->poly_reduce(pieces_f[feature_id]);
        }
        end = time(NULL);
        std::cout<<"reduce time in host="<<end-start<<std::endl;
        // send(reduced_poly_cipher);
    }
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    
        //---------------guest------------------------
    if (isGuest)
    {
        // receive(reduced_poly_cipher);
        start = time(NULL);

        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            plain_poly[feature_id] = poly_guest->decrypt_poly(reduced_poly_cipher[feature_id]);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

        result = poly_guest->eval_batch_poly_value(plain_poly,  new_ids, true);

        end = time(NULL);
        std::cout<<"decrypt and evaluation time in guest="<<end-start<<std::endl;
    }

    //--------------------guest for test--------------

    if (isHost & isGuest)
    {
        // for (int feature_id=0; feature_id<feature_num; feature_id++)
        for (int feature_id=0; feature_id<1; feature_id++)
        {
            std::cout<<"result="<<std::endl;
            for (int i=0; i<4; i++)
            {
                std::cout<<result[feature_id][i]<<std::endl;
            }
            std::cout<<"....."<<std::endl;
            for (int i=batch_size-4; i<batch_size; i++)
            {
                std::cout<<result[feature_id][i]<<std::endl;
            }
        }
    
    }

}




class BatchPirServer {
    private: 
    size_t feature_num;
    int64_t host_n_data; 
    size_t host_log_n_data;
    vector<vector<Plaintext>> pieces_f;
    shared_ptr<PolyProcessorHost> poly_host = nullptr;
    size_t fake_feature_num;  // 应对内存不足，计算仍按照feature_num进行，存储就只存贮fake_feature_num多维特征
    size_t factor=1;
    


   public:
   BatchPirServer(const vector<vector<int64_t>> & data, size_t batch_size, size_t feature_num): feature_num(feature_num){
        // feature_num=data.size();
        host_n_data = data[0].size();
        host_log_n_data = log2(host_n_data);
        poly_host= make_shared<PolyProcessorHost>(host_log_n_data, batch_size, 0);
        fake_feature_num=data.size();

        if (batch_size==poly_host->half_poly_mod_degree)
        {
            pieces_f.resize(fake_feature_num);
            std::cout<<__FILE__<<":"<<__LINE__<<endl;

            for (size_t i=0; i<feature_num; i++)
            {
                pieces_f[i%fake_feature_num] = poly_host->encode(data[i%fake_feature_num], batch_size, false);
            }
            std::cout<<__FILE__<<":"<<__LINE__<<endl;
        }
        else if (poly_host->half_poly_mod_degree%batch_size==0)
        {
            factor = poly_host->half_poly_mod_degree/batch_size;
            pieces_f.resize(fake_feature_num/factor);
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;

            for (size_t i=0; i<feature_num/factor; i++)
            {
                auto data_tobe_encode = vector<vector<int64_t>>(data.begin()+i%(fake_feature_num/factor)*factor, data.begin()+(i%(fake_feature_num/factor)+1)*factor);
                // std::cout<<i%(fake_feature_num/factor)*factor<<std::endl;
                // std::cout<<(i%(fake_feature_num/factor)+1)*factor<<std::endl;
                // std::cout<<"data_tobe_encode.size()="<<data_tobe_encode.size()<<std::endl;
                pieces_f[i%(fake_feature_num/factor)] = poly_host->encode(data_tobe_encode, false);
            }
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
            // std::cout<<"pieces_f.size()="<<pieces_f.size()<<std::endl;
            // std::cout<<"pieces_f[0].size()="<<pieces_f[0].size()<<std::endl;

         }

   };

   void receive_public_keys()
   {
         poly_host->receive_public_keys();   
   }

   stringstream gen_response(stringstream & cipher_x_powers_str, bool mod_switch=false)
   {
       
        poly_host->set_chara_and_Xpowers_mod_chara(cipher_x_powers_str, false, factor);

        vector<Ciphertext> reduced_poly_cipher(feature_num/factor);

        for (size_t feature_id=0; feature_id<feature_num/factor; feature_id++)
        {   
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            reduced_poly_cipher[feature_id] = poly_host->poly_reduce(pieces_f[feature_id%(fake_feature_num/factor)], mod_switch);            
        }

        return save_cipher_vector(reduced_poly_cipher);
   }

   void load_keys(stringstream & keys_stream)
   {    
        poly_host->load_keys(keys_stream);
   }

 
};







class BatchPirClient {

private:
size_t feature_num;
int64_t host_n_data; 
size_t host_log_n_data;
size_t batch_size;
vector<int64_t> new_ids;
shared_ptr<PolyProcessorGuest> poly_guest = nullptr;
size_t factor;

public:
BatchPirClient(size_t host_log_n_data, size_t feature_num, size_t batch_size): 
host_log_n_data(host_log_n_data), feature_num(feature_num), batch_size(batch_size){
    host_n_data=(1<<host_log_n_data);
    poly_guest=make_shared<PolyProcessorGuest>(host_log_n_data, batch_size, 0);
    factor = poly_guest->half_poly_mod_degree/batch_size;
}

void send_public_keys()
{
    poly_guest->send_public_keys();
}

stringstream save_keys()
{
    return poly_guest->save_keys();
}





stringstream gen_query(const vector<int64_t> & new_ids)
{
    this->new_ids = new_ids;
    if (new_ids.size()!=batch_size)
    {
        throw("must hive new_ids.size()==batch_size");
    }
    auto cipher_x_powers_stream = poly_guest->gen_chara_Xpowers_cipher_polys_stream(new_ids);
    return cipher_x_powers_stream;
}


vector<vector<int64_t>> extract_answer(stringstream & reduced_poly_cipher_stream)
{
    
    // vector<Ciphertext> reduced_poly_cipher(feature_num);

    vector<vector<int64_t>> result;
    if (batch_size==poly_guest->half_poly_mod_degree)
    {
        auto reduced_poly_cipher = load_cipher_vector(poly_guest->get_context(), reduced_poly_cipher_stream, feature_num);

        vector<vector<int64_t>> plain_poly(feature_num);
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            plain_poly[feature_id] = poly_guest->decrypt_poly(reduced_poly_cipher[feature_id]);
        }

        result = poly_guest->eval_batch_poly_value(plain_poly,  new_ids, true);
    }
    else if (poly_guest->half_poly_mod_degree%batch_size==0)
    {

        assert(feature_num%factor==0); 
        auto reduced_poly_cipher = load_cipher_vector(poly_guest->get_context(), reduced_poly_cipher_stream, feature_num/factor);
        vector<vector<int64_t>> plain_poly(feature_num/factor);
        for (size_t feature_id=0; feature_id<feature_num/factor; feature_id++)
        {
            plain_poly[feature_id] = poly_guest->decrypt_poly(reduced_poly_cipher[feature_id]);
        }
        vector<vector<int64_t>> decomposed_plain_poly(feature_num);
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            decomposed_plain_poly[feature_id].resize(2*batch_size);
            for (size_t i=0; i<2*batch_size; i++)
            {
                decomposed_plain_poly[feature_id][i]=plain_poly[feature_id/factor][i*factor+feature_id%factor];
            }
        }
        result = poly_guest->eval_batch_poly_value(decomposed_plain_poly,  new_ids, true);
        // todo
    }


      return result;
}

};


bool test_correctness(const vector<vector<int64_t>> & data, const vector<int64_t> & new_ids, const vector<vector<int64_t>> & result)
{
    bool correctness=true;
    size_t feature_num = data.size();
    for (size_t feature_id=0; feature_id<feature_num; feature_id++)
    {
        for (size_t i=0; i<new_ids.size(); i++)
        {
            correctness &= (data.at(feature_id%data.size()).at(new_ids.at(i))==result.at(feature_id).at(i));
        }
    }
    return correctness;
}