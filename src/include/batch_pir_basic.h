#pragma once
// #define BATCH_PIR_BASIC_TEST_FLAG true 
#define USE_NTT_IN_EVALUATION true
#define SMALL_BATCH_OPT true
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <cstddef>
#include <fstream>
// #include <random>
#include "seal_helper.h"
#include "poly_tool.h"
#include "fake_netio.h"
#include <cassert>
#include <cmath>


using namespace seal;
// using namespace boost::multiprecision;


std::vector<string> ciphertext_to_string(
    const std::vector<seal::Ciphertext> &c) {
    stringstream tmp_cipher_stream;
    std::vector<string> cipher_str;
    cipher_str.resize(c.size());
    for (int i = 0; i < c.size(); i++) {
        c[i].save(tmp_cipher_stream);
        // cipher_stream << tmp_cipher_stream;
        cipher_str[i] = tmp_cipher_stream.str();
    }
    return cipher_str;
}

string ciphertext_to_string(const seal::Ciphertext &c) {
    stringstream tmp_cipher_stream;
    string cipher_str;
    c.save(tmp_cipher_stream);
    cipher_str = tmp_cipher_stream.str();
    return cipher_str;
}

string ciphertext_to_string(const seal::Serializable<seal::Ciphertext> &c) {
    stringstream tmp_cipher_stream;
    string cipher_str;
    c.save(tmp_cipher_stream);
    cipher_str = tmp_cipher_stream.str();
    return cipher_str;
}


vector<string> ciphertext_to_string(const vector<seal::Serializable<seal::Ciphertext>> &vc) {
    vector<string> vs;
    vs.resize(vc.size());
    for (size_t i=0; i<vc.size(); i++)
    {
        vs[i] = ciphertext_to_string(vc[i]);
    }
    return vs;
}

void string_to_ciphertext(const SEALContext &context,
                          const std::vector<string> &str_cipher,
                          std::vector<Ciphertext> &c) {    // NOLINT
    c.resize(str_cipher.size());
    stringstream tmp_cipher_stream;
    // #pragma omp parallel for  private(tmp_cipher_stream) 
    for (int i = 0; i < str_cipher.size(); i++) {  // NOLINT
        tmp_cipher_stream << str_cipher[i];
        c[i].load(context, tmp_cipher_stream);
        tmp_cipher_stream.clear();
        tmp_cipher_stream.str("");
    }
}

void string_to_ciphertext(const SEALContext &context, const string &str_cipher,
                          Ciphertext &c) {  // NOLINT
    stringstream tmp_cipher_stream;
    tmp_cipher_stream << str_cipher;
    c.load(context, tmp_cipher_stream);
}




std::vector<Plaintext> interleave(const std::vector<std::vector<int64_t>> & x, size_t new_vecs_length, size_t cycle, int64_t prime_num)
{
    // vec0, vec1, vec2, ...... ---->  （vec0[0], vec1[0], vec2[0], vec3[0], ...,vec0[new_vecs_length/4-1], vec1[new_vecs_length/4-1], vec2[new_vecs_length/4-1], vec3[new_vecs_length/4-1]), ...... () 不足补0
    size_t old_vecs_num = x.size();
    size_t new_vecs_num = old_vecs_num/cycle;
    std::vector<Plaintext> y(new_vecs_num);

        zz_p::init(prime_num);
       


    for (size_t i=0; i<new_vecs_num; i++)
    {
        y[i].resize(new_vecs_length);
        for (size_t j=0; j<new_vecs_length; j++)
        {
            y[i][j]=0;
        }
    }

    for (size_t i=0; i<old_vecs_num; i++)
    {
        for (size_t j=0; j<x[i].size(); j++)
        {
            size_t new_i = i/cycle;
            size_t new_j = j*cycle + i%cycle;
            y[new_i][new_j]=rep(to_zz_p(x[i][j])/to_zz_p(cycle));
        }
    }
    return y;
}




class PolyProcessor {
   protected:
    // shared_ptr<PolyComputer> poly_computer = nullptr;
    /* BGV for poly reduction */
    shared_ptr<SEALContext> context = nullptr;
    shared_ptr<BatchEncoder> batch_encoder = nullptr;
    shared_ptr<Encryptor> encryptor = nullptr;
    shared_ptr<Evaluator> evaluator = nullptr;
    shared_ptr<Decryptor> decryptor = nullptr;
    EncryptionParameters parms;
    SecretKey secret_key;
    PublicKey public_key;
    RelinKeys relin_keys;
    GaloisKeys galois_keys;
  
    size_t poly_mod_degree=8192;
    // const int64_t prime_num = 167772161UL;
    // const int64_t self_defined_w = 140664228L;
    // const int64_t prime_num = 1108307720798209;     //49.97bit
    // const int64_t self_defined_w = 70361716033126;
    const int64_t prime_num = 1337006139375617;   // 50.24 bit
    const int64_t self_defined_w = 155828271966837;
    int64_t host_log_n_data;

   public:
    shared_ptr<PolyComputer> poly_computer = nullptr;
    PolyProcessor(bool init_key, size_t type, int64_t host_log_n_data): 
    host_log_n_data(host_log_n_data)
    {
        
        /* Init poly_computer */
        std::cout << "prime_num = " << prime_num << std::endl;
        std::cout << "self_defined_w = " << self_defined_w << std::endl;
        std::cout << "host_log_n_data = " << host_log_n_data << std::endl;
        poly_computer = make_shared<PolyComputer>(prime_num, self_defined_w,
                                                  host_log_n_data);
        /* Init BGV */
        parms=EncryptionParameters(scheme_type::bgv);
        parms.set_poly_modulus_degree(poly_mod_degree);
        if (type==0)  // pir
        {
            // parms.set_coeff_modulus(
            //     CoeffModulus::Create(poly_mod_degree, {48, 30, 24}));    //batch_pir 最后噪声预算为10
            // parms.set_coeff_modulus(
            //     CoeffModulus::Create(poly_mod_degree, {43, 43, 43}));    //batch_pir 最后噪声预算为10
                //             parms.set_coeff_modulus(
                // CoeffModulus::Create(poly_mod_degree, {54, 54, 54, 54}));
                                            parms.set_coeff_modulus(
                CoeffModulus::Create(poly_mod_degree, {42,42,42,42}));

        }
        else if (type==1)  //pir to share
        {
            // parms.set_coeff_modulus(
            //     CoeffModulus::Create(poly_mod_degree, {43, 43, 43, 43}));
                //             parms.set_coeff_modulus(
                // CoeffModulus::Create(poly_mod_degree, {54, 54, 54, 54, 54}));
                parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_mod_degree));
        }
        else
        {
            parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_mod_degree));
        }

        
        parms.set_plain_modulus(prime_num);
        context = make_shared<SEALContext>(parms);
        print_parameters(*context);

        //--------test----------
        auto context_data = context->key_context_data();
        // auto context_data = context->key_context_data();
        std::cout<<"key_parms().coeff_modulus()"<<std::endl;
        for (const auto &prime : context_data->parms().coeff_modulus())
        {
            cout << prime.value() << " ";
        }
        std::cout<<std::endl;

        //--------test----------


        evaluator = make_shared<Evaluator>(*context);
        batch_encoder = make_shared<BatchEncoder>(*context);
        
        if (init_key) {
            KeyGenerator keygen(*context);
            secret_key = keygen.secret_key();
            keygen.create_public_key(public_key);
            keygen.create_relin_keys(relin_keys);
            encryptor = make_shared<Encryptor>(*context, public_key, secret_key);
            decryptor = make_shared<Decryptor>(*context, secret_key);
            // std::vector<uint32_t> galois_eles = {1+2, 1+4, 1+8, 1+16, 1+32, 1+64, 1+128, 1+256, 1+512, 1+1024, 1+2048, 1+4096, 1+8192};
            std::vector<uint32_t> galois_eles = {1+1024, 1+2048, 1+4096, 1+8192};
            keygen.create_galois_keys(galois_eles, galois_keys);
        }
   
    }

    ~PolyProcessor() {
        // if (net_client_ != nullptr) {
        //     net_client_->Leave();
        // }
    }


stringstream save_keys()
    {
        stringstream keys_stream;
        public_key.save(keys_stream);
        relin_keys.save(keys_stream); 
        galois_keys.save(keys_stream);
        return keys_stream;
    }

void load_keys(stringstream & keys_stream)
{
    public_key.load(*context, keys_stream);
    relin_keys.load(*context, keys_stream);
    galois_keys.load(*context, keys_stream);
}


    void send_public_keys()
    {
        send(public_key);
        send(relin_keys);
    }

    void receive_public_keys()
    {
        receive(public_key);
        receive(relin_keys);
        encryptor = make_shared<Encryptor>(*context, public_key);
    }

    SEALContext get_context()
    {
        return *context;
    }

    int64_t get_prime_num() { return prime_num; }

};

class PolyProcessorHost : public PolyProcessor {
   private:
    size_t batch_size;
    vector<Ciphertext> chara_and_Xpowers_mod_chara;
    vector<Ciphertext> evaluate_basis;

   public:
   size_t half_poly_mod_degree = 4096;
    PolyProcessorHost(size_t host_log_n_data, size_t batch_size, size_t type=1) : batch_size(batch_size), PolyProcessor(0, type, host_log_n_data) { // NOLINT
        // chara_and_Xpowers_mod_chara.resize((1<<host_log_n_data)/half_poly_mod_degree);
        chara_and_Xpowers_mod_chara.resize((1<<host_log_n_data)/batch_size);
        evaluate_basis.resize(batch_size);

    }

    vector<int64_t> fft_interpolate(const vector<int64_t> &data) {
        vector<int64_t> coeff;
        poly_computer->fft_interpolate(data, coeff);
        return coeff;
    }

    vector<Plaintext> encode(const vector<int64_t> &data, size_t batch_size, bool mod_switch_to_next=false)
    {
        vector<int64_t> coeff;
        vector<Plaintext> pieces_f;
        Plaintext piece_f;
        poly_computer->fft_interpolate(data, coeff);
        if (batch_size==half_poly_mod_degree)
        {
            vector<uint64_t> v_piece_f;
            for (size_t i=0; i<coeff.size(); i+=half_poly_mod_degree)
            {
                if (i < coeff.size() / half_poly_mod_degree * half_poly_mod_degree)   
                {
                    v_piece_f = vector<uint64_t>(
                        coeff.begin() + i, coeff.begin() + i + half_poly_mod_degree);
                    piece_f = Plaintext(v_piece_f);
                } else {
                    v_piece_f = vector<uint64_t>(coeff.begin() + i, coeff.end());
                    piece_f = Plaintext(v_piece_f);
                }
                if (i>0)  // 常数项用来相加， 加法中BGV明文不可以是NTT form, 密文必须要是NTT form
                {
                    evaluator->transform_to_ntt_inplace(piece_f, context->first_parms_id());
                    if (mod_switch_to_next)
                    {
                        evaluator->mod_switch_to_next_inplace(piece_f);
                    }
                }
                pieces_f.push_back(piece_f);
            }
        }
        else
        {
            throw("unsupported for batch_size!=half_poly_mod_degree");
        }
        return pieces_f;
    }




  // new-------------
    vector<Plaintext> encode(const vector<vector<int64_t>> &data, bool mod_switch_to_next=false)
    {
        size_t factor = half_poly_mod_degree/batch_size;
        assert(factor*batch_size==half_poly_mod_degree);
        assert(factor == data.size());
        assert(data[0].size()%half_poly_mod_degree==0);
        vector<vector<int64_t>> coeff(factor);
        vector<Plaintext> pieces_f;
        Plaintext piece_f;
        for (size_t j=0; j<factor; j++)
        {
            poly_computer->fft_interpolate(data[j], coeff[j]);
        }

        vector<uint64_t> v_piece_f(half_poly_mod_degree);

        // #pragma omp parallel for  collapse(1) num_threads(16) private(v_piece_f, piece_f) 
        for (size_t i=0; i<coeff[0].size(); i++)
        {
            for (size_t j=0; j<factor; j++)
            {
                v_piece_f[i%batch_size * factor + j] = coeff[j][i];

                if (i%batch_size==batch_size-1 & j==factor-1)
                {
                    piece_f = Plaintext(v_piece_f);
                    if (i>batch_size) // 常数项用来相加， 加法中BGV明文不可以是NTT form, 密文必须要是NTT form
                    {
                        evaluator->transform_to_ntt_inplace(piece_f, context->first_parms_id());
                    }
                    if (mod_switch_to_next)
                    {
                        evaluator->mod_switch_to_next_inplace(piece_f);
                    }
                    
                    pieces_f.push_back(piece_f);
                }
            }

        }          
        return pieces_f;
    }










 void poly_reduce(const vector<Plaintext> &pieces_f, Ciphertext &reduced_cipher) 
                        {  // NOLINT
                        // 这里假设chara_and_Xpowers_mod_chara[0] 是  特征多项式chara,
            // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        if (chara_and_Xpowers_mod_chara.size()!=pieces_f.size()) {
            std::cout<<"chara_and_Xpowers_mod_chara.size()="<<chara_and_Xpowers_mod_chara.size()<<std::endl;
            std::cout<<"pieces_f.size()="<<pieces_f.size()<<std::endl;
            throw std::runtime_error(
                "Check error: xpowers_mod_chara.size()!=pieces_f.size()");
        }
                //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        Plaintext random_plaintext(half_poly_mod_degree);
        for (size_t j=0; j<half_poly_mod_degree; j++)
        {
            random_plaintext[j]=random_uint64()%prime_num;
        }
        Ciphertext tmp_reduced_cipher;
                //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                //  std::cout<<"pieces_f.size()="<<pieces_f.size()<<std::endl;
        for (int64_t i = 0; i < pieces_f.size(); i ++) {
            if (i==0)
            {
                //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                evaluator->multiply_plain(chara_and_Xpowers_mod_chara[i], random_plaintext,
                reduced_cipher);
                //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            }
            else{
                evaluator->multiply_plain(chara_and_Xpowers_mod_chara[i], pieces_f[i], tmp_reduced_cipher);
                evaluator->add_inplace(reduced_cipher, tmp_reduced_cipher);
            }
            // std::cout<<"i="<<i<<std::endl;
        }
        if (parms.scheme()==scheme_type::bfv)
        {
            evaluator->transform_from_ntt_inplace(reduced_cipher);  // 常数项是非NTT形式，bfv下不允许相加
        }
        evaluator->add_plain_inplace(reduced_cipher, pieces_f[0]);
                //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        // evaluator->mod_switch_to_next_inplace(reduced_cipher);
    }


        void set_chara_and_Xpowers_mod_chara(const std::vector<string> & chara_and_Xpowers_mod_chara_str, bool mod_switch_to_next)
        {
            string_to_ciphertext(*context, chara_and_Xpowers_mod_chara_str, chara_and_Xpowers_mod_chara);
                if (mod_switch_to_next)
                {
                    for (int i=0; i<chara_and_Xpowers_mod_chara.size(); i++)
                    {
                        // evaluator->mod_switch_to_inplace(cipher_xpowers_mod_chara[i], context->first_parms_id());
                        evaluator->mod_switch_to_next_inplace(chara_and_Xpowers_mod_chara[i]);
                    }
                }
        } 

#ifndef SMALL_BATCH_OPT
        void set_chara_and_Xpowers_mod_chara(stringstream& chara_and_Xpowers_mod_chara_str, bool mod_switch_to_next, size_t factor=1)
        {
            // string_to_ciphertext(*context, chara_and_Xpowers_mod_chara_str, chara_and_Xpowers_mod_chara);
            size_t vector_size = chara_and_Xpowers_mod_chara.size();
                chara_and_Xpowers_mod_chara = load_cipher_vector(*context, chara_and_Xpowers_mod_chara_str, vector_size);

                    for (int i=0; i<vector_size; i++)
                    {
                        if (!chara_and_Xpowers_mod_chara[i].is_ntt_form())
                        {
                            evaluator->transform_to_ntt_inplace(chara_and_Xpowers_mod_chara[i]);
                        }
                        if (mod_switch_to_next)
                        {
                        // evaluator->mod_switch_to_inplace(cipher_xpowers_mod_chara[i], context->first_parms_id());
                        evaluator->mod_switch_to_next_inplace(chara_and_Xpowers_mod_chara[i]);
                        }
                    }
                
        } 
#else
        void set_chara_and_Xpowers_mod_chara(stringstream& chara_and_Xpowers_mod_chara_str, bool mod_switch_to_next, size_t factor=1)
        {
            // string_to_ciphertext(*context, chara_and_Xpowers_mod_chara_str, chara_and_Xpowers_mod_chara);
            size_t vector_size = chara_and_Xpowers_mod_chara.size();
            // std::cout<<"vector_size="<<vector_size<<std::endl;
            if (factor==1)
            {
                chara_and_Xpowers_mod_chara = load_cipher_vector(*context, chara_and_Xpowers_mod_chara_str, vector_size);
            }
            else
            {
                auto chara_and_Xpowers_mod_chara_tmp = load_cipher_vector(*context, chara_and_Xpowers_mod_chara_str, vector_size/factor);
                // std::cout<<"chara_and_Xpowers_mod_chara_tmp.size()="<<chara_and_Xpowers_mod_chara_tmp.size()<<std::endl;
                chara_and_Xpowers_mod_chara = unzip_vec(*evaluator, chara_and_Xpowers_mod_chara_tmp, galois_keys,  prime_num, poly_mod_degree, factor);
                // std::cout<<"chara_and_Xpowers_mod_chara.size()="<<chara_and_Xpowers_mod_chara.size()<<std::endl;

            }
            

            for (int i=0; i<vector_size; i++)
            {
                if (!chara_and_Xpowers_mod_chara[i].is_ntt_form())
                {
                    evaluator->transform_to_ntt_inplace(chara_and_Xpowers_mod_chara[i]);
                }
                if (mod_switch_to_next)
                {
                // evaluator->mod_switch_to_inplace(cipher_xpowers_mod_chara[i], context->first_parms_id());
                evaluator->mod_switch_to_next_inplace(chara_and_Xpowers_mod_chara[i]);
                }
            }
                
        } 

#endif

        void set_evaluate_basis(const std::vector<string> & row_keepers_str, const std::vector<string> & col_keepers_str)
        {
            vector<Ciphertext> row_keepers, col_keepers;
            string_to_ciphertext(*context, row_keepers_str, row_keepers);
            string_to_ciphertext(*context, col_keepers_str, col_keepers);
            size_t col_num = col_keepers.size();
            std::cout<<"col_num="<<col_num<<std::endl;
            for (size_t i=0; i<batch_size; i++)
            {
                evaluator->multiply(row_keepers[i/col_num], col_keepers[i%col_num], evaluate_basis[i]);
                evaluator->mod_switch_to_next_inplace(evaluate_basis[i]);
                // evaluator->mod_switch_to_next_inplace(evaluate_basis[i]);
#ifndef USE_NTT_IN_EVALUATION                
                evaluator->transform_from_ntt_inplace(evaluate_basis[i]);
#endif
            }
        }

//         void set_evaluate_basis(stringstream & row_keepers_stream, stringstream & col_keepers_stream)
//         {
//             // vector<Ciphertext> row_keepers, col_keepers;
//             // string_to_ciphertext(*context, row_keepers_str, row_keepers);
//             // string_to_ciphertext(*context, col_keepers_str, col_keepers);
//             size_t col_num = sqrt(batch_size);
//             vector<Ciphertext> row_keepers = load_cipher_vector(*context,   row_keepers_stream, col_num);
//             vector<Ciphertext> col_keepers = load_cipher_vector(*context,   col_keepers_stream, col_num);
            
//             std::cout<<"col_num="<<col_num<<std::endl;
//             for (size_t i=0; i<batch_size; i++)
//             {
//                 evaluator->multiply(row_keepers[i/col_num], col_keepers[i%col_num], evaluate_basis[i]);
//                 evaluator->mod_switch_to_next_inplace(evaluate_basis[i]);
// #ifndef USE_NTT_IN_EVALUATION                
//                 evaluator->transform_from_ntt_inplace(evaluate_basis[i]);
// #endif
//             }
//         }

        void set_evaluate_basis(stringstream & row_keepers_stream, stringstream & col_keepers_stream)
        {
            // vector<Ciphertext> row_keepers, col_keepers;
            // string_to_ciphertext(*context, row_keepers_str, row_keepers);
            // string_to_ciphertext(*context, col_keepers_str, col_keepers);
            // size_t batch_sqrt = ceil(sqrt(1.0*batch_size));
            size_t row_num, col_num;
            if (batch_size==2048)
            {
                col_num=64;
            }
            else
            {
                col_num=ceil(sqrt(1.0*batch_size));
            }
            row_num = batch_size/col_num;



            vector<Ciphertext> row_keepers = load_cipher_vector(*context,   row_keepers_stream, row_num);
            vector<Ciphertext> col_keepers = load_cipher_vector(*context,   col_keepers_stream, col_num);
            
            std::cout<<"col_num="<<col_num<<std::endl;
            for (size_t i=0; i<batch_size; i++)
            {
                evaluator->multiply(row_keepers[i/col_num], col_keepers[i%col_num], evaluate_basis[i]);
                evaluator->mod_switch_to_next_inplace(evaluate_basis[i]);
                // evaluator->mod_switch_to_next_inplace(evaluate_basis[i]);
#ifndef USE_NTT_IN_EVALUATION                
                evaluator->transform_from_ntt_inplace(evaluate_basis[i]);
#endif
            }
        }





        Ciphertext poly_reduce(const vector<Plaintext> &pieces_f, bool mod_switch=false)
        {
            Ciphertext reduced_cipher;
            poly_reduce(pieces_f, reduced_cipher);
            if (mod_switch)
            {
                evaluator->mod_switch_to_next_inplace(reduced_cipher);    
            }
            return reduced_cipher;
        }



    vector<int64_t> poly_cipher_toShare(const Ciphertext &poly_cipher, Ciphertext &poly_cipher_share1)
    { // return share0 with size bath_size   (no poly_share0 with size 2*bath_size-1 )
        vector<int64_t> share0(batch_size);
        Ciphertext poly_cipher_share0, tmp_poly_cipher_share0;
        Plaintext share0_plain_tmp(1);
        //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        //    std::cout<<"evaluate_basis[0] is_ntt_form<<"<<evaluate_basis[0].is_ntt_form()<<std::endl;
        //    std::cout<<"share0_plain_tmp is_ntt_form<<"<<share0_plain_tmp.is_ntt_form()<<std::endl;

        for (size_t i=0; i<batch_size; i++)
        {
            share0[i] = random_uint64()%prime_num;
            share0_plain_tmp[0] = share0[i];
            evaluator->multiply_plain(evaluate_basis[i], share0_plain_tmp, tmp_poly_cipher_share0);
            if (i==0)
            {
                poly_cipher_share0 = Ciphertext(tmp_poly_cipher_share0);
            }
            else{
                evaluator->add_inplace(poly_cipher_share0, tmp_poly_cipher_share0);
            }

        }
#ifndef USE_NTT_IN_EVALUATION
        evaluator->transform_to_ntt_inplace(poly_cipher_share0);
#endif
        // evaluator->mod_switch_to_next_inplace(poly_cipher_share0);
        //  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        evaluator->sub(poly_cipher, poly_cipher_share0, poly_cipher_share1);
        return share0;
    }



    vector<vector<int64_t>> poly_cipher_toShare(const Ciphertext &poly_cipher, Ciphertext &poly_cipher_share1, size_t factor)
    { // return share0 with size bath_size   (no poly_share0 with size 2*bath_size-1 )
        vector<vector<int64_t>> share0(factor);
        for (size_t j=0; j<factor; j++)
        {
            share0[j].resize(batch_size);
        }
        Ciphertext poly_cipher_share0, tmp_poly_cipher_share0;
        Plaintext share0_plain_tmp(factor);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        for (size_t i=0; i<batch_size; i++)
        {
            for (size_t j=0; j<factor; j++)
            {
                share0[j][i] = random_uint64()%prime_num;
                share0_plain_tmp[j] = share0[j][i];
            }
            // std::cout<<"share0_plain_tmp="<<share0_plain_tmp.to_string()<<std::endl;
            evaluator->multiply_plain(evaluate_basis[i], share0_plain_tmp, tmp_poly_cipher_share0);
            if (i==0)
            {   
                poly_cipher_share0 = Ciphertext(tmp_poly_cipher_share0);
            }
            else{
                evaluator->add_inplace(poly_cipher_share0, tmp_poly_cipher_share0);
            }
        }






#ifndef USE_NTT_IN_EVALUATION
        evaluator->transform_to_ntt_inplace(poly_cipher_share0);
#endif
        // evaluator->mod_switch_to_next_inplace(poly_cipher_share0);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        evaluator->sub(poly_cipher, poly_cipher_share0, poly_cipher_share1);
        return share0;
    }


   
};

class PolyProcessorGuest : public PolyProcessor {
   private:
    size_t batch_size = 4096;
    zz_pX chara_poly;


   public:
    size_t half_poly_mod_degree = 4096;
    PolyProcessorGuest(size_t host_log_n_data, size_t batch_size, size_t type=1) : batch_size(batch_size), PolyProcessor(1, type, host_log_n_data) { // NOLINT
        chara_poly.SetLength(batch_size+1);
    }

    vector<int64_t> get_xi_powers(const vector<int64_t> &new_ids) {
        vector<int64_t> xi_power_new_ids(new_ids.size());
        poly_computer->get_xi_powers(new_ids, xi_power_new_ids);
        return xi_power_new_ids;
    }



  /**
     * @brief 生成K条密文，其中 第k个为 x^{kn} mod g(x) 的密文 for k>1
     *                       第0个为   g(x) 
     * @param new_ids
     */
    vector<string> gen_chara_Xpowers_cipher_polys(const vector<int64_t> &new_ids) {
        std::cout << "host_log_n_data = " << host_log_n_data << std::endl;
        // size_t K = ceil((1 << host_log_n_data) * 1.0 / half_poly_mod_degree);
        size_t K = ceil((1 << host_log_n_data) * 1.0 / batch_size);
        std::vector<std::vector<int64_t>> x_powers_mod_chara_poly;
        
        std::vector<string> x_powers_mod_chara_poly_cipher(K);

        auto t1 = std::chrono::high_resolution_clock::now();
        // poly_computer->get_chara_Xpowers_mod_new_ids(new_ids, half_poly_mod_degree,
        //                                         K, x_powers_mod_chara_poly);
        poly_computer->get_chara_Xpowers_mod_new_ids(new_ids, batch_size,
                                        K, x_powers_mod_chara_poly);

        for (size_t i=0; i<batch_size+1; i++)
        {
            chara_poly[i] = x_powers_mod_chara_poly[0][i];
        }
        
        
        Plaintext plain_x_power_kn(half_poly_mod_degree+1);
        if (batch_size==half_poly_mod_degree)
        {
            for (int k = 0; k < K; k++) {
                for (int i = 0; i < batch_size; i++) {
                    plain_x_power_kn[i] = x_powers_mod_chara_poly[k][i];
                }
                if (k==0)
                {
                    plain_x_power_kn[batch_size]=x_powers_mod_chara_poly[k][batch_size];
                }
                else
                {
                    plain_x_power_kn[batch_size]=0;
                }

                x_powers_mod_chara_poly_cipher[k] = ciphertext_to_string(encryptor->encrypt_symmetric(plain_x_power_kn));
            }  
        }
        else if (half_poly_mod_degree%batch_size==0)
        {
            size_t factor = half_poly_mod_degree/batch_size;
            assert(factor*batch_size==half_poly_mod_degree);
            for (size_t i=0; i<half_poly_mod_degree+1; i++)
            {
                plain_x_power_kn[i] = 0;
            }

            for (int k = 0; k < K; k++) {
                for (int i = 0; i < batch_size; i++) {
                    plain_x_power_kn[i*factor] = x_powers_mod_chara_poly[k][i];
                }
                if (k==0)
                {
                    plain_x_power_kn[batch_size*factor]=x_powers_mod_chara_poly[k][batch_size];
                }
                else
                {
                    plain_x_power_kn[batch_size*factor]=0;
                }

                x_powers_mod_chara_poly_cipher[k] = ciphertext_to_string(encryptor->encrypt_symmetric(plain_x_power_kn));
            }  
        }
        else
        {
            throw("must have half_poly_mod_degree%batch_size==0");
        }
      
        return x_powers_mod_chara_poly_cipher;
    }




    stringstream gen_chara_Xpowers_cipher_polys_stream(const vector<int64_t> &new_ids)
    {
        stringstream x_powers_mod_chara_poly_cipher_stream;
        std::cout << "host_log_n_data = " << host_log_n_data << std::endl;
        // size_t K = ceil((1 << host_log_n_data) * 1.0 / half_poly_mod_degree);
        size_t K = ceil((1 << host_log_n_data) * 1.0 / batch_size);
        std::vector<std::vector<int64_t>> x_powers_mod_chara_poly;
        
        // std::vector<seal::Serializable<seal::Ciphertext>> x_powers_mod_chara_poly_cipher(K);

        auto t1 = std::chrono::high_resolution_clock::now();
        // poly_computer->get_chara_Xpowers_mod_new_ids(new_ids, half_poly_mod_degree,
        //                                         K, x_powers_mod_chara_poly);
        poly_computer->get_chara_Xpowers_mod_new_ids(new_ids, batch_size,
                                        K, x_powers_mod_chara_poly);

        for (size_t i=0; i<batch_size+1; i++)
        {
            chara_poly[i] = x_powers_mod_chara_poly[0][i];
        }
        
        
        Plaintext plain_x_power_kn;
        if (batch_size==half_poly_mod_degree)
        {
            plain_x_power_kn.resize(half_poly_mod_degree+1);
            for (int k = 0; k < K; k++) {
                for (int i = 0; i < batch_size; i++) {
                    plain_x_power_kn[i] = x_powers_mod_chara_poly[k][i];
                }
                if (k==0)
                {
                    plain_x_power_kn[batch_size]=x_powers_mod_chara_poly[k][batch_size];
                }
                else
                {
                    plain_x_power_kn[batch_size]=0;
                }

                // x_powers_mod_chara_poly_cipher[k] = 
                encryptor->encrypt_symmetric(plain_x_power_kn).save(x_powers_mod_chara_poly_cipher_stream);
            }     
        }
        else if (half_poly_mod_degree%batch_size==0)
        {
            
            size_t factor = half_poly_mod_degree/batch_size;
            assert(factor*batch_size==half_poly_mod_degree);
#ifndef SMALL_BATCH_OPT
            plain_x_power_kn.resize((batch_size+1)*factor);
            for (size_t i=0; i<(batch_size+1)*factor; i++)
            {
                plain_x_power_kn[i] = 0;
            }

            for (int k = 0; k < K; k++) {
                for (int i = 0; i < batch_size; i++) {
                    plain_x_power_kn[i*factor] = x_powers_mod_chara_poly[k][i];
                }
                if (k==0)
                {
                    plain_x_power_kn[batch_size*factor]=x_powers_mod_chara_poly[k][batch_size];
                }
                else
                {
                    plain_x_power_kn[batch_size*factor]=0;
                }
                encryptor->encrypt_symmetric(plain_x_power_kn).save(x_powers_mod_chara_poly_cipher_stream);
            }
#else
            auto plain_x_power_kns = interleave(x_powers_mod_chara_poly, (batch_size+1)*factor, factor, prime_num);
            assert(plain_x_power_kns.size()==K/factor);
            for (size_t i=0; i<plain_x_power_kns.size(); i++)
            {
                encryptor->encrypt_symmetric(plain_x_power_kns[i]).save(x_powers_mod_chara_poly_cipher_stream);
            }

#endif
        }
   
        return x_powers_mod_chara_poly_cipher_stream;
    }



    // void gen_row_col_basis(const vector<int64_t> &new_ids, vector<string>& row_keepers_str, vector<string>& col_keepers_str) {
    //     size_t length = sqrt(batch_size);
    //     if (new_ids.size()!=batch_size)
    //     {
    //         throw std::runtime_error("must have new_ids.size()==batch_size");
    //     }
    //     if (length*length!=batch_size)
    //     {
    //         throw std::runtime_error("must have length*length==batch_size");
    //     }
    //     row_keepers_str.resize(length);
    //     col_keepers_str.resize(length);
    //     vector<vector<uint64_t>> row_keepers(length), col_keepers(length);
    //     std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    //     std::cout<<"length="<<length<<std::endl;
    //     find_row_col_basis(get_xi_powers(new_ids),  length, prime_num, row_keepers, col_keepers);
    //     std::cout<<"row_keepers[0].size()="<<row_keepers[0].size()<<std::endl;

    //     size_t factor = half_poly_mod_degree/batch_size;
    //     if (factor==1)
    //     {
    //         for (size_t i=0; i<length; i++)
    //         {
    //             row_keepers_str[i] =  ciphertext_to_string(encryptor->encrypt_symmetric(Plaintext(row_keepers[i])));
    //             col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(Plaintext(col_keepers[i])));
    //         }
    //     }
    //     else
    //     {
    //         for (size_t i=0; i<length; i++)
    //         {
    //             Plaintext  prow_keeper(half_poly_mod_degree), pcol_keeper(half_poly_mod_degree);
    //             for (size_t j=0; j<batch_size; j++)
    //             {
    //                 prow_keeper[j*factor] = row_keepers[i][j];
    //                 pcol_keeper[j*factor] = col_keepers[i][j];
    //             }
    //             // std::cout<<"i="<<i<<std::endl;
    //             // std::cout<<"prow_keeper="<<prow_keeper.to_string()<<std::endl;
    //             // std::cout<<"pcol_keeper="<<pcol_keeper.to_string()<<std::endl;
    //             row_keepers_str[i] =  ciphertext_to_string(encryptor->encrypt_symmetric(prow_keeper));
    //             col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(pcol_keeper));
    //         }
    //     }

    // }



    void gen_row_col_basis(const vector<int64_t> &new_ids, vector<string>& row_keepers_str, vector<string>& col_keepers_str) {
        // size_t length = ceil(sqrt(1.0*batch_size));
        size_t row_num, col_num;
        if (new_ids.size()!=batch_size)
        {
            throw std::runtime_error("must have new_ids.size()==batch_size");
        }
        switch (batch_size)
        {
            case 2048:
                col_num=64;
                break;
            default:
                col_num=sqrt(batch_size);
                break;
        }
        row_num = batch_size/col_num;
        if (row_num*col_num!=batch_size)
        {
            throw std::runtime_error("must have row_num*col_num==batch_size");
        }
        row_keepers_str.resize(row_num);
        col_keepers_str.resize(col_num);
        vector<vector<uint64_t>> row_keepers(row_num), col_keepers(col_num);
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        std::cout<<"col_num="<<col_num<<std::endl;
        find_row_col_basis(get_xi_powers(new_ids),  col_num, prime_num, row_keepers, col_keepers);
        std::cout<<"row_keepers[0].size()="<<row_keepers[0].size()<<std::endl;

        size_t factor = half_poly_mod_degree/batch_size;
        if (factor==1)
        {
            for (size_t i=0; i<row_num; i++)
            {
                row_keepers_str[i] =  ciphertext_to_string(encryptor->encrypt_symmetric(Plaintext(row_keepers[i])));
            }
            for (size_t i=0; i<col_num; i++)
            {
                col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(Plaintext(col_keepers[i])));
            }
        }
        else
        {
            // for (size_t i=0; i<length; i++)
            // {
            //     Plaintext  prow_keeper(half_poly_mod_degree), pcol_keeper(half_poly_mod_degree);
            //     for (size_t j=0; j<batch_size; j++)
            //     {
            //         prow_keeper[j*factor] = row_keepers[i][j];
            //         pcol_keeper[j*factor] = col_keepers[i][j];
            //     }
            //     // std::cout<<"i="<<i<<std::endl;
            //     // std::cout<<"prow_keeper="<<prow_keeper.to_string()<<std::endl;
            //     // std::cout<<"pcol_keeper="<<pcol_keeper.to_string()<<std::endl;
            //     row_keepers_str[i] =  ciphertext_to_string(encryptor->encrypt_symmetric(prow_keeper));
            //     col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(pcol_keeper));
            // }
            for (size_t i=0; i<row_num; i++)
            {
                Plaintext  prow_keeper(half_poly_mod_degree);
                for (size_t j=0; j<batch_size; j++)
                {
                    prow_keeper[j*factor] = row_keepers[i][j];
                    // pcol_keeper[j*factor] = col_keepers[i][j];
                }
                // std::cout<<"i="<<i<<std::endl;
                // std::cout<<"prow_keeper="<<prow_keeper.to_string()<<std::endl;
                // std::cout<<"pcol_keeper="<<pcol_keeper.to_string()<<std::endl;
                row_keepers_str[i] =  ciphertext_to_string(encryptor->encrypt_symmetric(prow_keeper));
                // col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(pcol_keeper));
            }

            for (size_t i=0; i<col_num; i++)
            {
                Plaintext  pcol_keeper(half_poly_mod_degree);
                for (size_t j=0; j<batch_size; j++)
                {
                    // prow_keeper[j*factor] = row_keepers[i][j];
                    pcol_keeper[j*factor] = col_keepers[i][j];
                }
                col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(pcol_keeper));
            }
        }

    }





    void gen_row_col_basis(const vector<int64_t> &new_ids, stringstream& row_keepers_stream, stringstream& col_keepers_stream) {
        size_t row_num, col_num;
        if (new_ids.size()!=batch_size)
        {
            throw std::runtime_error("must have new_ids.size()==batch_size");
        }
        switch (batch_size)
        {
            case 2048:
                col_num=64;
                break;
            default:
                col_num=sqrt(batch_size);
                break;
        }
        row_num = batch_size/col_num;

        if (row_num*col_num!=batch_size)
        {
            throw std::runtime_error("must have row_num*col_num==batch_size");
        }

        vector<vector<uint64_t>> row_keepers(row_num), col_keepers(col_num);
        // std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        std::cout<<"col_num="<<col_num<<std::endl;
        find_row_col_basis(get_xi_powers(new_ids),  col_num, prime_num, row_keepers, col_keepers);
        std::cout<<"row_keepers[0].size()="<<row_keepers[0].size()<<std::endl;

        size_t factor = half_poly_mod_degree/batch_size;
        if (factor==1)
        {
            for (size_t i=0; i<row_num; i++)
            {
                // row_keepers_str[i] =  ciphertext_to_string(encryptor->encrypt_symmetric(Plaintext(row_keepers[i])));
                // col_keepers_str[i] = ciphertext_to_string(encryptor->encrypt_symmetric(Plaintext(col_keepers[i])));
                encryptor->encrypt_symmetric(Plaintext(row_keepers[i])).save(row_keepers_stream);
            }
            for (size_t i=0; i<col_num; i++)
            {
                encryptor->encrypt_symmetric(Plaintext(col_keepers[i])).save(col_keepers_stream);
            }
        }
        else
        {
            for (size_t i=0; i<row_num; i++)
            {
                Plaintext  prow_keeper(half_poly_mod_degree); //, pcol_keeper(half_poly_mod_degree);
                for (size_t j=0; j<batch_size; j++)
                {
                    prow_keeper[j*factor] = row_keepers[i][j];
                    // pcol_keeper[j*factor] = col_keepers[i][j];
                }
                encryptor->encrypt_symmetric(Plaintext(prow_keeper)).save(row_keepers_stream);
                // encryptor->encrypt_symmetric(Plaintext(pcol_keeper)).save(col_keepers_stream);
            }
            for (size_t i=0; i<col_num; i++)
            {
                Plaintext  pcol_keeper(half_poly_mod_degree);
                for (size_t j=0; j<batch_size; j++)
                {
                    // prow_keeper[j*factor] = row_keepers[i][j];
                    pcol_keeper[j*factor] = col_keepers[i][j];
                }
                // encryptor->encrypt_symmetric(Plaintext(prow_keeper)).save(row_keepers_stream);
                encryptor->encrypt_symmetric(Plaintext(pcol_keeper)).save(col_keepers_stream);
            }
        }

    }





       vector<int64_t> decrypt_poly(const Ciphertext & c)
       {
            // cout<<__FILE__<<":"<<__LINE__<<endl;
            cout << "    + Noise budget in c: " << decryptor->invariant_noise_budget(c) << " bits" << endl;
            Plaintext plain_poly;
            decryptor->decrypt(c, plain_poly);
            std::cout<<"plain_poly.coeff_count()="<<plain_poly.coeff_count()<<std::endl;
            vector<int64_t> poly(poly_mod_degree);
            for (size_t i=0; i<poly_mod_degree; i++)
            {
                if (i<plain_poly.coeff_count())
                {
                    poly[i]=plain_poly[i];
                }
                else
                {
                    poly[i]=0;
                }
                
            }
            return poly;
       }

 
    vector<int64_t> mod_chara_poly(const vector<int64_t> &poly)
    {
        zz_pX poly_zzpx, rem_poly_zzpx;
        poly_zzpx.SetLength(poly.size());
        for (int i=0; i<poly.size(); i++)
        {
            poly_zzpx[i] = zz_p(poly[i]);
        }
        rem_poly_zzpx = poly_zzpx%chara_poly;
        vector<int64_t> rem_poly(batch_size);
        for (int i=0; i<batch_size; i++)
        {
            rem_poly[i] = rep(rem_poly_zzpx[i]);
        }
        return rem_poly;

    }

    vector<int64_t> eveal_poly_value(const vector<int64_t> &revealed_poly,
                                      const vector<int64_t> &index_ids, bool from_ids=true) {

        // std::cout<<"shared_poly_coeff.size()="<<revealed_poly.size()<<std::endl;


        vector<int64_t> eval_res =
            poly_computer->eval_poly_val(index_ids, revealed_poly, from_ids);
        // for (size_t i = 0; i < eval_res.size(); i++) {
        //     eval_res[i] = eval_res[i] > prime_num / 2 ? eval_res[i] - prime_num
        //                                               : eval_res[i];
        // }
        return eval_res;
    }


        vector<vector<int64_t>> eval_batch_poly_value(const vector<vector<int64_t>> &polys,
                                      const vector<int64_t> &index_ids, bool from_ids=true) {
            
            vector<vector<int64_t>> rem_polys(polys.size());
            for (int i=0; i<polys.size(); i++)
            {
                rem_polys[i] = mod_chara_poly(polys[i]);
            }
            auto eval_res =
            poly_computer->eval_batch_poly_val(index_ids, rem_polys, from_ids);    
            return eval_res;                        
                                      
            }


//-------------测试用----------------------------------------
    vector<int64_t> reveal_poly_value(const vector<int64_t> &shared_poly_coeff0,
        const vector<int64_t> &shared_poly_coeff1,
                                      const vector<int64_t> &index_ids) {

        std::cout<<"shared_poly_coeff0.size()="<<shared_poly_coeff0.size()<<std::endl;
        std::cout<<"shared_poly_coeff1.size()="<<shared_poly_coeff1.size()<<std::endl;
        size_t revealed_poly_size = min(shared_poly_coeff0.size(), shared_poly_coeff1.size());
        vector<int64_t> revealed_poly(revealed_poly_size);
        // receive(host_poly_coeff);
        for (size_t i = 0; i < revealed_poly_size; i++) {
            revealed_poly[i] =
                (shared_poly_coeff0[i] + shared_poly_coeff1[i]) % prime_num;
        }

        vector<int64_t> eval_res =
            poly_computer->eval_poly_val(index_ids, revealed_poly);
        // for (size_t i = 0; i < eval_res.size(); i++) {
        //     eval_res[i] = eval_res[i] > prime_num / 2 ? eval_res[i] - prime_num
        //                                               : eval_res[i];
        // }
        return eval_res;
    }



};




