#include "batch_pir_basic.h"
#include "utils.h"
#include <cstdio>

void print(const vector<int64_t> & new_ids)
{
  for (size_t i=0; i<new_ids.size(); i++)
  {
    std::cout<<new_ids[i]<<",";
  }
  std::cout<<std::endl;
}





void batch_pir_to_share_from_nttform_modswitch(bool isHost, bool isGuest, std::vector<int64_t> & data, vector<int64_t> &new_ids, vector<vector<int64_t>>& result0, vector<vector<int64_t>>& result1,
size_t feature_num=100, size_t host_log_n_data=20, size_t batch_size = 4096, bool modswitch=false)
{
    vector<string> cipher_x_powers_str;
    vector<Ciphertext> cipher_x_powers;
    shared_ptr<PolyProcessorHost> poly_host = nullptr;
    shared_ptr<PolyProcessorGuest> poly_guest = nullptr;
    // vector<int64_t> coeffs;
    vector<Plaintext> pieces_f;
    // vector<int64_t> new_ids(batch_size);
    result0.resize(feature_num);
    result1.resize(feature_num);
    vector<vector<int64_t>> reduced_f(feature_num);
    vector<string> row_keepers_str(sqrt(batch_size)), col_keepers_str(sqrt(batch_size));
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
//  vector<string> row_keepers_str(sqrt(batch_size)), col_keepers_str(sqrt(batch_size));
        poly_guest->gen_row_col_basis(new_ids, row_keepers_str, col_keepers_str);






        send(row_keepers_str);
        send(col_keepers_str);

        poly_guest->send_public_keys();
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    }

    //--------------host---------------------------
    if (isHost)
    {
        receive(cipher_x_powers_str);
        poly_host->set_chara_and_Xpowers_mod_chara(cipher_x_powers_str, modswitch);
        receive(row_keepers_str);
        receive(col_keepers_str);
        poly_host->set_evaluate_basis(row_keepers_str, col_keepers_str);
        poly_host->receive_public_keys();   
    }

 std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    Ciphertext reduced_poly_cipher;
    vector<Ciphertext> poly_chiper_share1(feature_num);
    vector<vector<int64_t>> poly_share1(feature_num);
    //--------------host---------------------------

    if (isHost)
    {    start = time(NULL);
        // #pragma omp parallel  for collapse(1)  num_threads(16) private(reduced_poly_cipher)  
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {   
            reduced_poly_cipher = poly_host->poly_reduce(pieces_f);
            //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            result0[feature_id] = poly_host->poly_cipher_toShare(reduced_poly_cipher, poly_chiper_share1[feature_id]);
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
        // #pragma omp parallel for  collapse(1) num_threads(16) 
        for (size_t feature_id=0; feature_id<feature_num; feature_id++)
        {
            poly_share1[feature_id] = poly_guest->decrypt_poly(poly_chiper_share1[feature_id]);
        }
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        auto xi_powers = poly_guest->get_xi_powers(new_ids);

        result1 = poly_guest->eval_batch_poly_value(poly_share1,  new_ids, true);


        end = time(NULL);
        std::cout<<"decrypt and evaluation time in guest="<<end-start<<std::endl;
    }

}




//----------------------------------------------------------------------------------------


void save_plaintexts(const std::vector<Plaintext> & vp, std::ofstream & o_stream)
{
    for (size_t i=0; i< vp.size(); i++)
    {
        vp[i].save(o_stream);
    }
    return;
}


void save_plaintexts(const std::vector<std::vector<Plaintext>> & vvp, std::ofstream & o_stream)
{
    for (size_t i=0; i<vvp.size(); i++)
    {
        save_plaintexts(vvp[i], o_stream);
    }
    return;
}


void load_plaintexts(const SEALContext &context, std::vector<Plaintext> & vp, std::ifstream& i_stream)
{
    for (size_t i=0; i< vp.size(); i++)
    {
        vp[i].load(context, i_stream);
    }
    return;
}

void load_plaintexts(const SEALContext &context, std::vector<std::vector<Plaintext>> & vvp, std::ifstream& i_stream)
{
    for (size_t i=0; i<vvp.size(); i++)
    {
        load_plaintexts(context, vvp[i], i_stream);
    }
    return;
}



class BatchPirToShareServer {
    private: 
    size_t feature_num;
    int64_t host_n_data; 
    size_t host_log_n_data;
    vector<vector<Plaintext>> pieces_f;
    shared_ptr<PolyProcessorHost> poly_host = nullptr;
    size_t fake_feature_num;  // feature_num in mem
        size_t factor=1;
    vector<vector<int64_t>> result0;
    bool use_out_mem;
    string out_mem_file_name="./pieces_f";
    std::ofstream o_stream;
    std::ifstream i_stream;
    
    


   public:
   BatchPirToShareServer(const vector<vector<int64_t>> & data, size_t batch_size, size_t feature_num, bool use_out_mem=false): feature_num(feature_num), use_out_mem(use_out_mem){
        // feature_num=data.size();
        host_n_data = data[0].size();
        host_log_n_data = log2(host_n_data);
        poly_host= make_shared<PolyProcessorHost>(host_log_n_data, batch_size, 1);

        std::cout<<"use_out_mem="<<use_out_mem<<std::endl;
        if (use_out_mem)
        {
            o_stream.open(out_mem_file_name, std::ios::binary | std::ios::trunc);
            fake_feature_num=4;
        }
        else
        {
            fake_feature_num=data.size();
        }

        if (batch_size==poly_host->half_poly_mod_degree)
        {
            pieces_f.resize(fake_feature_num);
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;

            for (size_t i=0; i<feature_num; i++)
            {
                pieces_f[i%fake_feature_num] = poly_host->encode(data[i%fake_feature_num], batch_size, false);
                if (use_out_mem && (i%fake_feature_num==fake_feature_num-1))
                {
                    save_plaintexts(pieces_f, o_stream);
                }
            }
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
        }
        else if (poly_host->half_poly_mod_degree%batch_size==0)
        {
            factor = poly_host->half_poly_mod_degree/batch_size;
            pieces_f.resize(fake_feature_num/factor);
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;

            vector<vector<int64_t>> data_tobe_encode;
             
            for (size_t i=0; i<feature_num/factor; i++)
            {
                data_tobe_encode = vector<vector<int64_t>>(data.begin()+i%(fake_feature_num/factor)*factor, data.begin()+(i%(fake_feature_num/factor)+1)*factor);
                // std::cout<<i%(fake_feature_num/factor)*factor<<std::endl;
                // std::cout<<(i%(fake_feature_num/factor)+1)*factor<<std::endl;
                // std::cout<<"data_tobe_encode.size()="<<data_tobe_encode.size()<<std::endl;
                pieces_f[i%(fake_feature_num/factor)] = poly_host->encode(data_tobe_encode, false);
                if (use_out_mem && (i%(fake_feature_num/factor)==fake_feature_num/factor-1))
                {
                    save_plaintexts(pieces_f, o_stream);
                }
            }
            // std::cout<<__FILE__<<":"<<__LINE__<<endl;
            // std::cout<<"pieces_f.size()="<<pieces_f.size()<<std::endl;
            // std::cout<<"pieces_f[0].size()="<<pieces_f[0].size()<<std::endl;

        }

        if (use_out_mem)
        {
            o_stream.close();
        }
        result0.resize(feature_num);
   };


    ~BatchPirToShareServer()
    {
        if (use_out_mem)
        {
        o_stream.open(out_mem_file_name, std::ios::binary | std::ios::trunc);
        o_stream.close();
        std::remove(out_mem_file_name.c_str()); 
        }
    }


    int64_t get_prime_num()
    {
        return poly_host->get_prime_num();
    }
   void receive_public_keys()
   {
         poly_host->receive_public_keys();   
   }


//    stringstream gen_response(stringstream & cipher_x_powers_str, bool mod_switch=false)
stringstream gen_response(BatchPirToShareQuery & query, bool mod_switch=false)
   {
       
        poly_host->set_chara_and_Xpowers_mod_chara(query.cipher_x_powers_stream, false, factor);
        poly_host->set_evaluate_basis(query.row_keepers_str, query.col_keepers_str);
        vector<Ciphertext> reduced_poly_cipher(feature_num/factor);
        vector<Ciphertext> poly_chiper_share1(feature_num);

        vector<vector<int64_t>> result0_tmp;

        if (use_out_mem)
        {
            i_stream.open(out_mem_file_name, std::ios::binary);
        }

    //    #pragma omp parallel  for collapse(1) num_threads(16) private(result0_tmp)  
        for (size_t feature_id=0; feature_id<feature_num/factor; feature_id++)
        {   
                if (use_out_mem && (feature_id%fake_feature_num==0))
                {
                    load_plaintexts(poly_host->get_context(), pieces_f, i_stream);
                }

            // reduced_poly_cipher = poly_host->poly_reduce(pieces_f[feature_id%fake_feature_num], mod_switch);            
            reduced_poly_cipher[feature_id] = poly_host->poly_reduce(pieces_f[feature_id%(fake_feature_num/factor)], mod_switch);            

            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            if (factor==1)
            {
                result0[feature_id] = poly_host->poly_cipher_toShare(reduced_poly_cipher[feature_id], poly_chiper_share1[feature_id]);
            }
            else
            {
                // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                vector<vector<int64_t>> result0_tmp = poly_host->poly_cipher_toShare(reduced_poly_cipher[feature_id], poly_chiper_share1[feature_id], factor);
                // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                assert(result0_tmp.size()==factor);
                for (size_t j=0; j<factor; j++)
                {
                    // std::cout<<"j="<<j<<std::endl;
                    result0[feature_id*factor+j] = result0_tmp[j];
                }
            }
            
        }
        if (use_out_mem)
        {
            i_stream.close();
        }

        return save_cipher_vector(poly_chiper_share1);
   }







stringstream gen_response(BatchPirToShareQueryStream & query, bool mod_switch=false)
   {
       
        poly_host->set_chara_and_Xpowers_mod_chara(query.cipher_x_powers_stream, false, factor);
        poly_host->set_evaluate_basis(query.row_keepers_stream, query.col_keepers_stream);
        vector<Ciphertext> reduced_poly_cipher(feature_num/factor);
        vector<Ciphertext> poly_chiper_share1(feature_num);
        vector<vector<int64_t>> result0_tmp;
        // #pragma omp parallel  for collapse(1) num_threads(16) private(result0_tmp)  
        for (size_t feature_id=0; feature_id<feature_num/factor; feature_id++)
        {   
            // reduced_poly_cipher = poly_host->poly_reduce(pieces_f[feature_id%fake_feature_num], mod_switch);            
            reduced_poly_cipher[feature_id] = poly_host->poly_reduce(pieces_f[feature_id%(fake_feature_num/factor)], mod_switch);            

            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            if (factor==1)
            {
                result0[feature_id] = poly_host->poly_cipher_toShare(reduced_poly_cipher[feature_id], poly_chiper_share1[feature_id]);
            }
            else
            {
                // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                vector<vector<int64_t>> result0_tmp = poly_host->poly_cipher_toShare(reduced_poly_cipher[feature_id], poly_chiper_share1[feature_id], factor);
                // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                assert(result0_tmp.size()==factor);
                for (size_t j=0; j<factor; j++)
                {
                    // std::cout<<"j="<<j<<std::endl;
                    result0[feature_id*factor+j] = result0_tmp[j];
                }
            }
            
        }

        return save_cipher_vector(poly_chiper_share1);
   }



   void load_keys(stringstream & keys_stream)
   {    
        poly_host->load_keys(keys_stream);
   }

   std::vector<std::vector<int64_t>> extract_answer()
   {
    return result0;
   }

 
};







class BatchPirToShareClient {

    private:
    size_t feature_num;
    int64_t host_n_data; 
    size_t host_log_n_data;
    size_t batch_size;
    vector<int64_t> new_ids;
    shared_ptr<PolyProcessorGuest> poly_guest = nullptr;
size_t factor;

    public:
    BatchPirToShareClient(size_t host_log_n_data, size_t feature_num, size_t batch_size): 
    host_log_n_data(host_log_n_data), feature_num(feature_num), batch_size(batch_size){
        host_n_data=(1<<host_log_n_data);
        poly_guest=make_shared<PolyProcessorGuest>(host_log_n_data, batch_size, 1);
    factor = poly_guest->half_poly_mod_degree/batch_size;
    }


   int64_t get_prime_num()
   {
      return poly_guest->get_prime_num();
   }
    void send_public_keys()
    {
        poly_guest->send_public_keys();
    }

    stringstream save_keys()
    {
        return poly_guest->save_keys();
    }




    BatchPirToShareQuery gen_query(const vector<int64_t> & new_ids)
    {
        BatchPirToShareQuery query;
        this->new_ids = new_ids;
        if (new_ids.size()!=batch_size)
        {
            throw("must hive new_ids.size()==batch_size");
        }
        query.cipher_x_powers_stream = poly_guest->gen_chara_Xpowers_cipher_polys_stream(new_ids);
        //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        //  std::cout<<"new_ids="<<std::endl;
        //  print(new_ids);
        poly_guest->gen_row_col_basis(new_ids, query.row_keepers_str, query.col_keepers_str);
        return query;
    }



    BatchPirToShareQueryStream gen_query_stream(const vector<int64_t> & new_ids)
    {
        BatchPirToShareQueryStream query;
        this->new_ids = new_ids;
        if (new_ids.size()!=batch_size)
        {
            throw("must hive new_ids.size()==batch_size");
        }
        query.cipher_x_powers_stream = poly_guest->gen_chara_Xpowers_cipher_polys_stream(new_ids);
        //  std::cout<<__FILE__<<":"<<__LINE__<<endl;
        //  std::cout<<"new_ids="<<std::endl;
        //  print(new_ids);
        poly_guest->gen_row_col_basis(new_ids, query.row_keepers_stream, query.col_keepers_stream);
        return query;
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


bool test_feature_correctness(const vector<vector<int64_t>> & data, const vector<int64_t> & new_ids, const vector<vector<int64_t>> & result0, const vector<vector<int64_t>> & result1, int64_t prime_num)
{
    bool correctness=true;
    size_t feature_num = data.size();
    for (size_t feature_id=0; feature_id<feature_num; feature_id++)
    {
        std::cout<<std::endl;
        // std::cout<<"feature_id="<<feature_id<<std::endl;
        // std::cout<<"(d,r)="<<std::endl;
        for (size_t i=0; i<new_ids.size(); i++)
        {
            int64_t d = data.at(feature_id%data.size()).at(new_ids.at(i));
            int64_t r = (result0.at(feature_id).at(i)+result1.at(feature_id).at(i))% prime_num;
            // std::cout<<"("<<d<<","<<r<<"),";
            correctness &= (d==r);
        }
    }
    return correctness;
}

bool test_feature_correctness(const vector<vector<int64_t>> & data, const vector<int64_t> & new_ids, const vector<vector<int64_t>> & result0, const vector<vector<int64_t>> & result1, int64_t prime_num, size_t intersection_size)
{
    bool correctness=true;
    size_t feature_num = data.size();
    for (size_t feature_id=0; feature_id<feature_num; feature_id++)
    {
        for (size_t i=0; i<intersection_size; i++)
        {
            correctness &= (data.at(feature_id%data.size()).at(new_ids.at(i))==(result0.at(feature_id).at(i)+result1.at(feature_id).at(i))% prime_num);
        }
    }
    return correctness;
}

bool test_label_correctness(const vector<bool> & client_labels, int start, const vector<bool> & label_shares0, const vector<bool> & label_shares1)
{
    
    size_t sum0 = 0;
    for (size_t i=start; i<client_labels.size(); i++)
    {
        sum0 += int(client_labels[i]);
    }
    std::cout<<"sum0="<<sum0<<std::endl;

    size_t sum1 = 0;
    for (size_t i=0; i<label_shares0.size(); i++)
    {
        sum1 += int(label_shares0[i]^label_shares1[i]);
    }
    std::cout<<"sum1="<<sum1<<std::endl;
    return (sum0 == sum1);

}



