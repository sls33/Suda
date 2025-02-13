// from fedquery
#include "include/poly_tool.h"
#include "include/batch_pir_to_share.h"
#include "seal/seal.h"

using namespace std;
using namespace seal;

int test_fft_interpolate() {
    long p; // NOLINT
    //    p = GenPrime_long(NTL_SP_NBITS);
    p = 167772161UL;
    int64_t w = 140664228L;
    // p = 17UL;
    // p = GenPrime_long(60);

    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    int log_degree = 25;
    int degree = (1 << log_degree);
    std::vector<int64_t> data(degree), coeff(degree);
    for (int i = 0; i < degree; i++) {
        data[i] = random() % p;
    }

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // random((zz_pX&) data, degree);

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // PolyComputer poly_computer(p, w, log_degree);
    PolyComputer poly_computer(p, log_degree);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    poly_computer.fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    poly_computer.check_fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    return 0;
}





int test_fft_interpolate2() {
    long p; // NOLINT
    //    p = GenPrime_long(NTL_SP_NBITS);
    p = 3221225473UL;
    // int64_t w = 140664228L;
    // p = 17UL;
    // p = GenPrime_long(60);

    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    int log_degree = 25;
    int degree = (1 << log_degree);
    std::vector<int64_t> data(degree), coeff(degree);
    for (int i = 0; i < degree; i++) {
        data[i] = random() % p;
    }

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // random((zz_pX&) data, degree);

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // PolyComputer poly_computer(p, w, log_degree);
    PolyComputer poly_computer(p, log_degree);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    poly_computer.fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    poly_computer.check_fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    return 0;
}


int test_fft_interpolate3() {
    long p; // NOLINT
    //    p = GenPrime_long(NTL_SP_NBITS);
    p = 1108307720798209UL; 
    // int64_t w = 140664228L;
    // p = 17UL;
    // p = GenPrime_long(60);

    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    int log_degree = 25;
    int degree = (1 << log_degree);
    std::vector<int64_t> data(degree), coeff(degree);
    for (int i = 0; i < degree; i++) {
        data[i] = random() % p;
    }

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // random((zz_pX&) data, degree);

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // PolyComputer poly_computer(p, w, log_degree);
    PolyComputer poly_computer(p, log_degree);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    poly_computer.fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    poly_computer.check_fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    return 0;
}


int test_fft_interpolate4() {
    long p; // NOLINT
    //    p = GenPrime_long(NTL_SP_NBITS);
    p = 1337006139375617UL;
    // int64_t w = 140664228L;
    // p = 17UL;
    // p = GenPrime_long(60);

    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    int log_degree = 25;
    int degree = (1 << log_degree);
    std::vector<int64_t> data(degree), coeff(degree);
    for (int i = 0; i < degree; i++) {
        data[i] = random() % p;
    }

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // random((zz_pX&) data, degree);

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // PolyComputer poly_computer(p, w, log_degree);
    PolyComputer poly_computer(p, log_degree);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    poly_computer.fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    poly_computer.check_fft_interpolate(data, coeff);
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    return 0;
}



int test_x_powers_mod_new_ids() {
    long p = 17;    // NOLINT

    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;

    PolyComputer poly_computer(p, 4);

    std::vector<std::vector<int64_t>> x_powers_mod_chara_poly;
    poly_computer.get_x_powers_mod_new_ids({1, 2, 3}, 3,
                                           x_powers_mod_chara_poly);
    for (size_t i = 0; i < x_powers_mod_chara_poly.size(); i++) {
        std::cout << "i=" << i << std::endl;
        for (size_t j = 0; j < x_powers_mod_chara_poly[i].size(); j++) {
            std::cout << x_powers_mod_chara_poly[i][j] << ",";
        }
        std::cout << std::endl;
    }
    return 0;
}

void seal_test() {
    size_t poly_mod_degree = 8192;
    EncryptionParameters parms(scheme_type::bgv);
    parms.set_poly_modulus_degree(poly_mod_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_mod_degree));
    parms.set_plain_modulus(PlainModulus::Batching(poly_mod_degree, 50));
    // parms.set_plain_modulus(4222124650659841);
    

    SEALContext context(parms);
    //  print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    Encryptor encryptor(context, public_key, secret_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    BatchEncoder batch_encoder(context);
    GaloisKeys galois_keys;

    Plaintext x_plain("40x^1023 + 30x^3 + 1x^1 + 3");
    cout << "Express x  as a plaintext polynomial 0x" + x_plain.to_string() +
                "."
         << endl;

    //  print_line(__LINE__);
    Ciphertext x_encrypted;
    stringstream x_encrypted_str;
    cout << "Encrypt x_plain to x_encrypted." << endl;
    encryptor.encrypt(x_plain, x_encrypted);
    // encryptor.encrypt_symmetric(x_plain).save(x_encrypted_str);

    //  x_encrypted.load(context, x_encrypted_str);




        cout <<__FILE__<<":"<<__LINE__<<endl;
    cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(x_encrypted) << " bits" << endl;


    std::cout << __FILE__ << ":" << __LINE__ << std::endl;

    vector<string> v_ciphertext_str(3);
    vector<Ciphertext> v_x_encrypted(3);
    for (int i = 0; i < 3; i++) {
        v_x_encrypted[i] = x_encrypted;
    }




    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    for (int i = 0; i < 3; i++) {
                    cout <<__FILE__<<":"<<__LINE__<<endl;
    cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(v_x_encrypted[i]) << " bits" << endl;

        decryptor.decrypt(v_x_encrypted[i], x_plain);
        cout << "Express x  as a plaintext polynomial 0x" +
                    x_plain.to_string() + "."
             << endl;
    }

    evaluator.multiply(v_x_encrypted[0], v_x_encrypted[1], x_encrypted);
    cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(x_encrypted) << " bits" << endl;

    evaluator.multiply_plain_inplace(x_encrypted, Plaintext("FFFFFFFFFFFFx^1024 + FFFFFFFFFFFF"));
    cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(x_encrypted) << " bits" << endl;



}

// void symmetric_cipher_vec_to_string_test() {
//     size_t poly_mod_degree = 8192;
//     EncryptionParameters parms(scheme_type::bfv);
//     parms.set_poly_modulus_degree(poly_mod_degree);
//     parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_mod_degree));
//     parms.set_plain_modulus(PlainModulus::Batching(poly_mod_degree, 60));

//     SEALContext context(parms);
//     //  print_parameters(context);
//     cout << endl;

//     KeyGenerator keygen(context);
//     SecretKey secret_key = keygen.secret_key();
//     PublicKey public_key;
//     keygen.create_public_key(public_key);
//     RelinKeys relin_keys;
//     keygen.create_relin_keys(relin_keys);
//     Encryptor encryptor(context, public_key, secret_key);
//     Evaluator evaluator(context);
//     Decryptor decryptor(context, secret_key);
//     BatchEncoder batch_encoder(context);
//     GaloisKeys galois_keys;

//     Plaintext x_plain("40x^1023 + 30x^3 + 1x^1 + 3");
//     cout << "Express x  as a plaintext polynomial 0x" + x_plain.to_string() +
//                 "."
//          << endl;

//     //  print_line(__LINE__);
//     //  Ciphertext x_encrypted;
//     cout << "Encrypt x_plain to x_encrypted." << endl;
//     //  encryptor.encrypt(x_plain, x_encrypted);
//     auto x_encrypted = encryptor.encrypt_symmetric(x_plain);


//     std::cout << __FILE__ << ":" << __LINE__ << std::endl;

//     vector<string> v_ciphertext_str(3);
//     vector<Ciphertext> v_x_encrypted(3);
//     // for (int i=0;i<3; i++)
//     // {
//     // v_x_encrypted[i] = x_encrypted;
//     // }
//     // v_ciphertext_str = ciphertext_to_string(v_x_encrypted);
//     for (int i = 0; i < 3; i++) {
//         // v_x_encrypted[i] = x_encrypted;
//         v_ciphertext_str[i] = ciphertext_to_string(x_encrypted);
//     }

//     std::cout << __FILE__ << ":" << __LINE__ << std::endl;
//     vector<Ciphertext> v_x_encrypted1;
//     string_to_ciphertext(context, v_ciphertext_str, v_x_encrypted1, false);

//     std::cout << __FILE__ << ":" << __LINE__ << std::endl;
//     for (int i = 0; i < 3; i++) {
//             cout <<__FILE__<<":"<<__LINE__<<endl;
//     cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(v_x_encrypted1[i]) << " bits" << endl;

//         decryptor.decrypt(v_x_encrypted1[i], x_plain);
//         cout << "Express x  as a plaintext polynomial 0x" +
//                     x_plain.to_string() + "."
//              << endl;
//     }
// }

// std::vector<Plaintext> interleave(const std::vector<std::vector<int64_t>> x, size_t new_vecs_length, size_t cycle)
// {
//     // vec0, vec1, vec2, ...... ---->  （vec0[0], vec1[0], vec2[0], vec3[0], ...,vec0[new_vecs_length/4-1], vec1[new_vecs_length/4-1], vec2[new_vecs_length/4-1], vec3[new_vecs_length/4-1]), ...... () 不足补0
//     size_t old_vecs_num = x.size();
//     size_t new_vecs_num = old_vecs_num/cycle;
//     std::vector<Plaintext> y(new_vecs_num);
//     for (size_t i=0; i<new_vecs_num; i++)
//     {
//         y[i].resize(new_vecs_length);
//         for (size_t j=0; j<new_vecs_length; j++)
//         {
//             y[i][j]=0;
//         }
//     }

//     for (size_t i=0; i<old_vecs_num; i++)
//     {
//         for (size_t j=0; j<x[i].size(); j++)
//         {
//             size_t new_i = i/cycle;
//             size_t new_j = j*cycle + i%cycle;
//             y[new_i][new_j]=x[i][j];
//         }
//     }
//     return y;
// }


// std::vector<Ciphertext> unzip(const Evaluator& evaluator, const std::vector<Ciphertext> & vcin, const GaloisKeys & galois_keys,  uint64_t plain_module, size_t poly_mod_degree, size_t start_sparsity)
// {
//     // default: start_sparsity=1 代表初始输入无间隔
//     std::vector<Ciphertext> vcout(vcin.size()*2);
//     Plaintext x_power(poly_mod_degree);
//     for (int i=0; i<poly_mod_degree; i++)
//     {
//         x_power[i]=0;
//     }
//     x_power[poly_mod_degree-start_sparsity]=plain_module-1;

//     for (size_t i=0; i<vcin.size(); i++)
//     {
//         Ciphertext c(vcin[i]);
//         Ciphertext gc, c0, c1;
//         std::cout<<"1+2*poly_mod_degree/start_sparsity="<<1+2*poly_mod_degree/start_sparsity<<std::endl;
//         evaluator.apply_galois(c, 1+poly_mod_degree/start_sparsity, galois_keys, gc);
//         evaluator.add(c, gc, c0);
//         evaluator.sub(c, gc, c1);
//         evaluator.multiply_plain_inplace(c1, x_power);
//         vcout[i]=c0;
//         vcout[i+vcin.size()]=c1;
//     }
//     return vcout;
// }

// std::vector<Ciphertext> unzip(const Evaluator& evaluator, const Ciphertext & vcin, const GaloisKeys & galois_keys,  uint64_t plain_module, size_t poly_mod_degree, size_t start_sparsity, size_t end_sparsity)
// {
//     size_t sparsity = start_sparsity;
//     std::vector<Ciphertext> vcout(1);
//     vcout[0] = vcin;
//     while (sparsity<end_sparsity)
//     {
//         vcout = unzip(evaluator, vcout, galois_keys, plain_module, poly_mod_degree, sparsity);
//         sparsity *=2;
//     }
//     return vcout;
// }


// std::vector<Ciphertext> unzip_vec(const Evaluator& evaluator, const std::vector<Ciphertext> & vcin, const GaloisKeys & galois_keys,  uint64_t plain_module, size_t poly_mod_degree, size_t end_sparsity)
// {
//     std::vector<Ciphertext> vcout;
//     for (Ciphertext c: vcin)
//     {
//         auto vcout1 = unzip(evaluator, c, galois_keys, plain_module, poly_mod_degree, 1, end_sparsity);
//         vcout.insert(vcout.end(), vcout1.begin(), vcout1.end());
//     }
//     return vcout;
// }

void unzip_test()
{
     size_t poly_mod_degree = 8192;
    EncryptionParameters parms(scheme_type::bgv);
    parms.set_poly_modulus_degree(poly_mod_degree);
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_mod_degree));
            //   parms.set_coeff_modulus(
            //     CoeffModulus::Create(poly_mod_degree, {48, 30, 24}));
    parms.set_coeff_modulus(CoeffModulus::Create(poly_mod_degree, {48, 40, 30}));
    uint64_t plain_module = 167772161UL;
    // parms.set_plain_modulus(PlainModulus::Batching(poly_mod_degree, 60));
     parms.set_plain_modulus(plain_module);

    SEALContext context(parms);
    //  print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    Encryptor encryptor(context, public_key, secret_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    BatchEncoder batch_encoder(context);
    GaloisKeys galois_keys;
    std::vector<uint32_t> galois_eles = {1+2, 1+4, 1+8, 1+16, 1+32, 1+64, 1+128, 1+256, 1+512, 1+1024, 1+2048, 1+4096, 1+8192};
    keygen.create_galois_keys(galois_eles, galois_keys);


    Plaintext x_plain("40x^1023 + 5x^5 + 4x^4 + 3x^3 + 2x^2 + 1x^1 + 3");
    Ciphertext x_cipher;
    encryptor.encrypt(x_plain, x_cipher);
    cout << "    + Noise budget in x_cipher: " << decryptor.invariant_noise_budget(x_cipher) << " bits" << endl;



    // auto unziped = unzip(evaluator, x_cipher, galois_keys, plain_module, poly_mod_degree, 1, 4);
    auto unziped = unzip_vec(evaluator, {x_cipher, x_cipher}, galois_keys, plain_module, poly_mod_degree, 4);


    std::cout<<"unziped.size()="<<unziped.size()<<std::endl;
    for (Ciphertext c: unziped)
    {
        Plaintext decripted;
        cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(c) << " bits" << endl;
        decryptor.decrypt(c, decripted);
        std::cout<<"decripted="<<decripted.to_string()<<std::endl;
    }


}





void interleave_unzip_test()
{
     size_t poly_mod_degree = 8192;
    EncryptionParameters parms(scheme_type::bgv);
    parms.set_poly_modulus_degree(poly_mod_degree);
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_mod_degree));
            //   parms.set_coeff_modulus(
            //     CoeffModulus::Create(poly_mod_degree, {48, 30, 24}));
    parms.set_coeff_modulus(CoeffModulus::Create(poly_mod_degree, {48, 40, 30}));
    uint64_t plain_module = 167772161UL;
    // parms.set_plain_modulus(PlainModulus::Batching(poly_mod_degree, 60));
     parms.set_plain_modulus(plain_module);

    SEALContext context(parms);
    //  print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    Encryptor encryptor(context, public_key, secret_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    BatchEncoder batch_encoder(context);
    GaloisKeys galois_keys;
    std::vector<uint32_t> galois_eles = {1+2, 1+4, 1+8, 1+16, 1+32, 1+64, 1+128, 1+256, 1+512, 1+1024, 1+2048, 1+4096, 1+8192};
    keygen.create_galois_keys(galois_eles, galois_keys);


    // Plaintext x_plain("40x^1023 + 5x^5 + 4x^4 + 3x^3 + 2x^2 + 1x^1 + 3");
    size_t vecs_num = 64;
    vector<vector<int64_t>> vecs(vecs_num);
    size_t length=256;
    for (size_t i=0; i<vecs_num; i++)
    {
        if (i==0)
        {
            vecs[i].resize(length+1);
        }
        else
        {
            vecs[i].resize(length);
        }
        for (size_t j=0; j<length; j++)
        {
            vecs[i][j]=(random()%plain_module+plain_module)%plain_module;
            // vecs[i][j]=j;
            // std::cout<<"vecs[i]="<<vecs[i][j]<<" "; 
        }
        if (i==0)
        {
            vecs[i][length]=(random()%plain_module+plain_module)%plain_module;
        }
        // std::cout<<std::endl;
        
    }

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

    auto interleave_plaintexts = interleave(vecs, 4097, 4, plain_module);
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // for (Plaintext p: interleave_plaintexts)
    // {
    //     std::cout<<"p="<<p.to_string()<<std::endl;
    // }



    vector<Ciphertext> v_ciphers(interleave_plaintexts.size());
    for (size_t i=0; i<interleave_plaintexts.size(); i++)
    {
        encryptor.encrypt(interleave_plaintexts[i], v_ciphers[i]);
        cout << "    + Noise budget in x_cipher: " << decryptor.invariant_noise_budget(v_ciphers[i]) << " bits" << endl;
    }

  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // auto unziped = unzip(evaluator, x_cipher, galois_keys, plain_module, poly_mod_degree, 1, 4);
    auto unziped = unzip_vec(evaluator, v_ciphers, galois_keys, plain_module, poly_mod_degree, 4);

  std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    std::cout<<"unziped.size()="<<unziped.size()<<std::endl;
    // for (Ciphertext c: unziped)
    bool correct=true;
    for (size_t i=0; i<unziped.size(); i++)
    {
        Ciphertext c=unziped[i];
        Plaintext decripted;
        // cout << "    + Noise budget in reduced_cipher: " << decryptor.invariant_noise_budget(c) << " bits" << endl;
        decryptor.decrypt(c, decripted);
        // std::cout<<"decripted="<<decripted.to_string()<<std::endl;
        for (size_t j=0; j<length; j++)
        {
            // std::cout<<"decripted[4j]="<<decripted[4*j]<<std::endl;
            // std::cout<<"(vecs[i][j]*4)%plain_module="<<(vecs[i][j]*4)%plain_module<<std::endl;
            correct &= (decripted[4*j]==vecs[i][j]);
        }
    }
    std::cout<<"correct="<<correct<<std::endl;

}

int main(int argc, char **argv) {
    // int party = std::atoi(argv[1]);
    // int task_id = std::atoi(argv[2]);
    // for (int i = 0; i < 1; i++) {
    //     PolyPsi_test(std::to_string(task_id + i), party);
    // }
    // test_fft_interpolate(); 
    // test_x_powers_mod_new_ids();
    seal_test();
    // symmetric_cipher_vec_to_string_test();

    // unzip_test();
// interleave_unzip_test();
// test_fft_interpolate2();
// test_fft_interpolate3();
test_fft_interpolate4();
    return 0;
}
