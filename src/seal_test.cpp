// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "include/seal_helper.h"

using namespace std;
using namespace seal;

void example_bgv_basics0()
{
    print_example_banner("Example: BGV Basics");

    /*
    As an example, we evaluate the degree 8 polynomial

        x^8

    over an encrypted x over integers 1, 2, 3, 4. The coefficients of the
    polynomial can be considered as plaintext inputs, as we will see below. The
    computation is done modulo the plain_modulus 1032193.

    Computing over encrypted data in the BGV scheme is similar to that in BFV.
    The purpose of this example is mainly to explain the differences between BFV
    and BGV in terms of ciphertext coefficient modulus selection and noise control.

    Most of the following code are repeated from "BFV basics" and "encoders" examples.
    */

    /*
    Note that scheme_type is now "bgv".
    */
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);

    /*
    We can certainly use BFVDefault coeff_modulus. In later parts of this example,
    we will demonstrate how to choose coeff_modulus that is more useful in BGV.
    */
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
    SEALContext context(parms);

    /*
    Print the parameters that we have chosen.
    */
    print_line(__LINE__);
    cout << "Set encryption parameters and print" << endl;
    print_parameters(context);

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    /*
    Batching and slot operations are the same in BFV and BGV.
    */
    BatchEncoder batch_encoder(context);
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;
    cout << "Plaintext matrix row size: " << row_size << endl;

    /*
    Here we create the following matrix:
        [ 1,  2,  3,  4,  0,  0, ...,  0 ]
        [ 0,  0,  0,  0,  0,  0, ...,  0 ]
    */
    vector<uint64_t> pod_matrix(slot_count, 0ULL);
    pod_matrix[0] = 1ULL;
    pod_matrix[1] = 2ULL;
    pod_matrix[2] = 3ULL;
    pod_matrix[3] = 4ULL;

    cout << "Input plaintext matrix:" << endl;
    print_matrix(pod_matrix, row_size);
    Plaintext x_plain;
    cout << "Encode plaintext matrix to x_plain:" << endl;
    batch_encoder.encode(pod_matrix, x_plain);

    /*
    Next we encrypt the encoded plaintext.
    */
    Ciphertext x_encrypted;
    print_line(__LINE__);
    cout << "Encrypt x_plain to x_encrypted." << endl;
    encryptor.encrypt(x_plain, x_encrypted);
    cout << "    + noise budget in freshly encrypted x: " << decryptor.invariant_noise_budget(x_encrypted) << " bits"
         << endl;
    cout << endl;

    /*
    Then we compute x^2.
    */
    print_line(__LINE__);
    cout << "Compute and relinearize x_squared (x^2)," << endl;
    Ciphertext x_squared;
    evaluator.square(x_encrypted, x_squared);
    cout << "    + size of x_squared: " << x_squared.size() << endl;
    evaluator.relinearize_inplace(x_squared, relin_keys);
    cout << "    + size of x_squared (after relinearization): " << x_squared.size() << endl;
    cout << "    + noise budget in x_squared: " << decryptor.invariant_noise_budget(x_squared) << " bits" << endl;
    Plaintext decrypted_result;
    decryptor.decrypt(x_squared, decrypted_result);
    vector<uint64_t> pod_result;
    batch_encoder.decode(decrypted_result, pod_result);
    cout << "    + result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Next we compute x^4.
    */
    print_line(__LINE__);
    cout << "Compute and relinearize x_4th (x^4)," << endl;
    Ciphertext x_4th;
    evaluator.square(x_squared, x_4th);
    cout << "    + size of x_4th: " << x_4th.size() << endl;
    evaluator.relinearize_inplace(x_4th, relin_keys);
    cout << "    + size of x_4th (after relinearization): " << x_4th.size() << endl;
    cout << "    + noise budget in x_4th: " << decryptor.invariant_noise_budget(x_4th) << " bits" << endl;
    decryptor.decrypt(x_4th, decrypted_result);
    batch_encoder.decode(decrypted_result, pod_result);
    cout << "    + result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Last we compute x^8. We run out of noise budget.
    */
    print_line(__LINE__);
    cout << "Compute and relinearize x_8th (x^8)," << endl;
    Ciphertext x_8th;
    evaluator.square(x_4th, x_8th);
    cout << "    + size of x_8th: " << x_8th.size() << endl;
    evaluator.relinearize_inplace(x_8th, relin_keys);
    cout << "    + size of x_8th (after relinearization): " << x_8th.size() << endl;
    cout << "    + noise budget in x_8th: " << decryptor.invariant_noise_budget(x_8th) << " bits" << endl;
    cout << "NOTE: Decryption can be incorrect if noise budget is zero." << endl;

    cout << endl;
    cout << "~~~~~~ Use modulus switching to calculate x^8 ~~~~~~" << endl;

    /*
    Noise budget has reached 0, which means that decryption cannot be expected
    to give the correct result. BGV requires modulus switching to reduce noise
    growth. In the following demonstration, we will insert a modulus switching
    after each relinearization.
    */
    print_line(__LINE__);
    cout << "Encrypt x_plain to x_encrypted." << endl;
    encryptor.encrypt(x_plain, x_encrypted);
    cout << "    + noise budget in freshly encrypted x: " << decryptor.invariant_noise_budget(x_encrypted) << " bits"
         << endl;
    cout << endl;

    /*
    Then we compute x^2.
    */
    print_line(__LINE__);
    cout << "Compute and relinearize x_squared (x^2)," << endl;
    cout << "    + noise budget in x_squared (previously): " << decryptor.invariant_noise_budget(x_squared) << " bits"
         << endl;
    evaluator.square(x_encrypted, x_squared);
    evaluator.relinearize_inplace(x_squared, relin_keys);
    evaluator.mod_switch_to_next_inplace(x_squared);
    cout << "    + noise budget in x_squared (with modulus switching): " << decryptor.invariant_noise_budget(x_squared)
         << " bits" << endl;
    decryptor.decrypt(x_squared, decrypted_result);
    batch_encoder.decode(decrypted_result, pod_result);
    cout << "    + result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Next we compute x^4.
    */
    print_line(__LINE__);
    cout << "Compute and relinearize x_4th (x^4)," << endl;
    cout << "    + noise budget in x_4th (previously): " << decryptor.invariant_noise_budget(x_4th) << " bits" << endl;
    evaluator.square(x_squared, x_4th);
    evaluator.relinearize_inplace(x_4th, relin_keys);
    evaluator.mod_switch_to_next_inplace(x_4th);
    cout << "    + noise budget in x_4th (with modulus switching): " << decryptor.invariant_noise_budget(x_4th)
         << " bits" << endl;
    decryptor.decrypt(x_4th, decrypted_result);
    batch_encoder.decode(decrypted_result, pod_result);
    cout << "    + result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Last we compute x^8. We still have budget left.
    */
    print_line(__LINE__);
    cout << "Compute and relinearize x_8th (x^8)," << endl;
    cout << "    + noise budget in x_8th (previously): " << decryptor.invariant_noise_budget(x_8th) << " bits" << endl;
    evaluator.square(x_4th, x_8th);
    evaluator.relinearize_inplace(x_8th, relin_keys);
    evaluator.mod_switch_to_next_inplace(x_8th);
    cout << "    + noise budget in x_8th (with modulus switching): " << decryptor.invariant_noise_budget(x_8th)
         << " bits" << endl;
    decryptor.decrypt(x_8th, decrypted_result);
    batch_encoder.decode(decrypted_result, pod_result);
    cout << "    + result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Although with modulus switching x_squared has less noise budget than before,
    noise budget is consumed at a slower rate. To achieve the optimal consumption
    rate of noise budget in an application, one needs to carefully choose the
    location to insert modulus switching and manually choose coeff_modulus.
    */
}



void example_bgv_basics_60()
{
    print_example_banner("Example: BGV Basics");

    /*
    In this example, we demonstrate performing simple computations (a polynomial
    evaluation) on encrypted integers using the BFV encryption scheme.

    The first task is to set up an instance of the EncryptionParameters class.
    It is critical to understand how the different parameters behave, how they
    affect the encryption scheme, performance, and the security level. There are
    three encryption parameters that are necessary to set:

        - poly_modulus_degree (degree of polynomial modulus);
        - coeff_modulus ([ciphertext] coefficient modulus);
        - plain_modulus (plaintext modulus; only for the BFV scheme).

    The BFV scheme cannot perform arbitrary computations on encrypted data.
    Instead, each ciphertext has a specific quantity called the `invariant noise
    budget' -- or `noise budget' for short -- measured in bits. The noise budget
    in a freshly encrypted ciphertext (initial noise budget) is determined by
    the encryption parameters. Homomorphic operations consume the noise budget
    at a rate also determined by the encryption parameters. In BFV the two basic
    operations allowed on encrypted data are additions and multiplications, of
    which additions can generally be thought of as being nearly free in terms of
    noise budget consumption compared to multiplications. Since noise budget
    consumption compounds in sequential multiplications, the most significant
    factor in choosing appropriate encryption parameters is the multiplicative
    depth of the arithmetic circuit that the user wants to evaluate on encrypted
    data. Once the noise budget of a ciphertext reaches zero it becomes too
    corrupted to be decrypted. Thus, it is essential to choose the parameters to
    be large enough to support the desired computation; otherwise the result is
    impossible to make sense of even with the secret key.
    */
    EncryptionParameters parms(scheme_type::bgv);

    /*
    The first parameter we set is the degree of the `polynomial modulus'. This
    must be a positive power of 2, representing the degree of a power-of-two
    cyclotomic polynomial; it is not necessary to understand what this means.

    Larger poly_modulus_degree makes ciphertext sizes larger and all operations
    slower, but enables more complicated encrypted computations. Recommended
    values are 1024, 2048, 4096, 8192, 16384, 32768, but it is also possible
    to go beyond this range.

    In this example we use a relatively small polynomial modulus. Anything
    smaller than this will enable only very restricted encrypted computations.
    */
    size_t poly_modulus_degree = 4096;
    parms.set_poly_modulus_degree(poly_modulus_degree);

    /*
    Next we set the [ciphertext] `coefficient modulus' (coeff_modulus). This
    parameter is a large integer, which is a product of distinct prime numbers,
    each up to 60 bits in size. It is represented as a vector of these prime
    numbers, each represented by an instance of the Modulus class. The
    bit-length of coeff_modulus means the sum of the bit-lengths of its prime
    factors.

    A larger coeff_modulus implies a larger noise budget, hence more encrypted
    computation capabilities. However, an upper bound for the total bit-length
    of the coeff_modulus is determined by the poly_modulus_degree, as follows:

        +----------------------------------------------------+
        | poly_modulus_degree | max coeff_modulus bit-length |
        +---------------------+------------------------------+
        | 1024                | 27                           |
        | 2048                | 54                           |
        | 4096                | 109                          |
        | 8192                | 218                          |
        | 16384               | 438                          |
        | 32768               | 881                          |
        +---------------------+------------------------------+

    These numbers can also be found in native/src/seal/util/hestdparms.h encoded
    in the function SEAL_HE_STD_PARMS_128_TC, and can also be obtained from the
    function

        CoeffModulus::MaxBitCount(poly_modulus_degree).

    For example, if poly_modulus_degree is 4096, the coeff_modulus could consist
    of three 36-bit primes (108 bits).

    Microsoft SEAL comes with helper functions for selecting the coeff_modulus.
    For new users the easiest way is to simply use

        CoeffModulus::BFVDefault(poly_modulus_degree),

    which returns std::vector<Modulus> consisting of a generally good choice
    for the given poly_modulus_degree.
    */
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    //parms.set_coeff_modulus({1073754113});
    //parms.set_coeff_modulus({1073692673}); //最接近30 bit的符合条件的素数
    //parms.set_coeff_modulus({4151297});  //coeff_modulus size: 22  bits OK but noise budget in kx =0
    //parms.set_coeff_modulus({4188161});    // coeff_modulus size: 22 bits OK but noise budget in kx =0
    //parms.set_coeff_modulus({4263937}); // coeff_modulus size: 23  bits OK  noise budget in kx = 1
    //parms.set_coeff_modulus({4323329}); // coeff_modulus size: 23  bits OK  noise budget in kx = 1

    /*
    The plaintext modulus can be any positive integer, even though here we take
    it to be a power of two. In fact, in many cases one might instead want it
    to be a prime number; we will see this in later examples. The plaintext
    modulus determines the size of the plaintext data type and the consumption
    of noise budget in multiplications. Thus, it is essential to try to keep the
    plaintext data type as small as possible for best performance. The noise
    budget in a freshly encrypted ciphertext is

        ~ log2(coeff_modulus/plain_modulus) (bits)

    and the noise budget consumption in a homomorphic multiplication is of the
    form log2(plain_modulus) + (other terms).

    The plaintext modulus is specific to the BFV scheme, and cannot be set when
    using the CKKS scheme.
    */
    //parms.set_plain_modulus(1024);
    //parms.set_plain_modulus(257);
    //parms.set_plain_modulus(67);
    //parms.set_plain_modulus(1UL<<59);   //
    uint64_t plain_module = 1UL<<48;
    parms.set_plain_modulus(plain_module);
    

    /*
    Now that all parameters are set, we are ready to construct a SEALContext
    object. This is a heavy class that checks the validity and properties of the
    parameters we just set.
    */
    SEALContext context(parms);

    /*
    Print the parameters that we have chosen.
    */
    print_line(__LINE__);
    cout << "Set encryption parameters and print" << endl;
    print_parameters(context);

    /*
    When parameters are used to create SEALContext, Microsoft SEAL will first
    validate those parameters. The parameters chosen here are valid.
    */
    cout << "Parameter validation (success): " << context.parameter_error_message() << endl;

    cout << endl;
    cout << "~~~~~~ A naive way to calculate 4(x^2+1)(x+1)^2. ~~~~~~" << endl;

    /*
    The encryption schemes in Microsoft SEAL are public key encryption schemes.
    For users unfamiliar with this terminology, a public key encryption scheme
    has a separate public key for encrypting data, and a separate secret key for
    decrypting data. This way multiple parties can encrypt data using the same
    shared public key, but only the proper recipient of the data can decrypt it
    with the secret key.

    We are now ready to generate the secret and public keys. For this purpose
    we need an instance of the KeyGenerator class. Constructing a KeyGenerator
    automatically generates a secret key. We can then create as many public
    keys for it as we want using KeyGenerator::create_public_key.

    Note that KeyGenerator::create_public_key has another overload that takes
    no parameters and returns a Serializable<PublicKey> object. We will discuss
    this in `6_serialization.cpp'.
    */
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    /*
    To be able to encrypt we need to construct an instance of Encryptor. Note
    that the Encryptor only requires the public key, as expected. It is also
    possible to use Microsoft SEAL in secret-key mode by providing the Encryptor
    the secret key instead. We will discuss this in `6_serialization.cpp'.
    */
    Encryptor encryptor(context, public_key);

    /*
    Computations on the ciphertexts are performed with the Evaluator class. In
    a real use-case the Evaluator would not be constructed by the same party
    that holds the secret key.
    */
    Evaluator evaluator(context);

    /*
    We will of course want to decrypt our results to verify that everything worked,
    so we need to also construct an instance of Decryptor. Note that the Decryptor
    requires the secret key.
    */
    Decryptor decryptor(context, secret_key);

    /*
    As an example, we evaluate the degree 4 polynomial

        4x^4 + 8x^3 + 8x^2 + 8x + 4

    over an encrypted x = 6. The coefficients of the polynomial can be considered
    as plaintext inputs, as we will see below. The computation is done modulo the
    plain_modulus 1024.

    While this examples is simple and easy to understand, it does not have much
    practical value. In later examples we will demonstrate how to compute more
    efficiently on encrypted integers and real or complex numbers.

    Plaintexts in the BFV scheme are polynomials of degree less than the degree
    of the polynomial modulus, and coefficients integers modulo the plaintext
    modulus. For readers with background in ring theory, the plaintext space is
    the polynomial quotient ring Z_T[X]/(X^N + 1), where N is poly_modulus_degree
    and T is plain_modulus.

    To get started, we create a plaintext containing the constant 6. For the
    plaintext element we use a constructor that takes the desired polynomial as
    a string with coefficients represented as hexadecimal numbers.
    */
    print_line(__LINE__);
    //uint64_t x = 66;
    //Plaintext x_plain(uint64_to_hex_string(x));
    string x = "40x^1023 + 30x^3 + 1x^1 + 3";
    //Plaintext x_plain("40x^1023 + 30x^3 + 1x^1 + 3");
    Plaintext x_plain(x);
    cout << "Express x = " + x + " as a plaintext polynomial 0x" + x_plain.to_string() + "." << endl;

    /*
    We then encrypt the plaintext, producing a ciphertext. We note that the
    Encryptor::encrypt function has another overload that takes as input only
    a plaintext and returns a Serializable<Ciphertext> object. We will discuss
    this in `6_serialization.cpp'.
    */
    print_line(__LINE__);
    Ciphertext x_encrypted, x_encrypted1, x_encrypted_sum;
    cout << "Encrypt x_plain to x_encrypted." << endl;
    encryptor.encrypt(x_plain, x_encrypted);

    /*
    In Microsoft SEAL, a valid ciphertext consists of two or more polynomials
    whose coefficients are integers modulo the product of the primes in the
    coeff_modulus. The number of polynomials in a ciphertext is called its `size'
    and is given by Ciphertext::size(). A freshly encrypted ciphertext always
    has size 2.
    */
    cout << "    + size of freshly encrypted x: " << x_encrypted.size() << endl;

    /*
    There is plenty of noise budget left in this freshly encrypted ciphertext.
    */
    cout << "    + noise budget in freshly encrypted x: " << decryptor.invariant_noise_budget(x_encrypted) << " bits"
         << endl;

    /*
    We decrypt the ciphertext and print the resulting plaintext in order to
    demonstrate correctness of the encryption.
    */
    Plaintext x_decrypted;
    cout << "    + decryption of x_encrypted: ";
    decryptor.decrypt(x_encrypted, x_decrypted);
    cout << "0x" << x_decrypted.to_string() << " ...... Correct." << endl;

    /*
    When using Microsoft SEAL, it is typically advantageous to compute in a way
    that minimizes the longest chain of sequential multiplications. In other
    words, encrypted computations are best evaluated in a way that minimizes
    the multiplicative depth of the computation, because the total noise budget
    consumption is proportional to the multiplicative depth. For example, for
    our example computation it is advantageous to factorize the polynomial as

        4x^4 + 8x^3 + 8x^2 + 8x + 4 = 4(x + 1)^2 * (x^2 + 1)

    to obtain a simple depth 2 representation. Thus, we compute (x + 1)^2 and
    (x^2 + 1) separately, before multiplying them, and multiplying by 4.

    First, we compute x^2 and add a plaintext "1". We can clearly see from the
    print-out that multiplication has consumed a lot of noise budget. The user
    can vary the plain_modulus parameter to see its effect on the rate of noise
    budget consumption.
    */

    print_line(__LINE__);
    cout << "Compute x * x_plain." << endl;
    //Ciphertext kx;
    //evaluator.square(x_encrypted, x_sq_plus_one);
    //Plaintext k("40"); //64
    //Plaintext k("1x^120"); // 66
    //Plaintext k("1x^120 + 1x^60 + 1x^30");
    Plaintext k(poly_modulus_degree);
    k[120]=1;
    k[60]=0;
    k[30]=1;

    //Plaintext b("30"); // 
    time_t start_time, end_time;
    time(&start_time);
    evaluator.multiply_plain(x_encrypted, k, x_encrypted_sum);
    //x_encrypted_sum = x_encrypted;
    //#pragma omp parallel for
    // for (int i=0; i<10000000/4096; i++)
    // //for (int i=0; i<100000; i++)
    // {
    //     evaluator.multiply_plain(x_encrypted, k, x_encrypted1);
    //     evaluator.add(x_encrypted1, x_encrypted_sum, x_encrypted_sum);
    // }
    // time(&end_time);
    // time_t dtime = difftime(end_time,start_time);
    // std::cout<<"time="<<dtime<<std::endl;

    //     time(&start_time);
    // for (int i=0; i<1000000; i++)
    // {
    //     auto x = (435435345345UL+i)*(698743454354UL+i);
    // }
    // time(&end_time);
    // dtime = difftime(end_time,start_time);
    // std::cout<<"time="<<dtime<<std::endl;

    //evaluator.add_plain_inplace(x_encrypted, b);
    cout << "    + size of x * x_plain: " << x_encrypted.size() << endl;
    cout << "    + noise budget in x * x_plain: " << decryptor.invariant_noise_budget(x_encrypted) << " bits"
         << endl;
    Plaintext decrypted_result;
    cout << "    + decryption of x * x_plain: ";
    decryptor.decrypt(x_encrypted_sum, decrypted_result);
    cout << "0x" << decrypted_result.to_string() << " ...... Correct." << endl;






    // print_line(__LINE__);
    // cout << "Compute x_sq_plus_one (x^2+1)." << endl;
    // Ciphertext x_sq_plus_one;
    // evaluator.square(x_encrypted, x_sq_plus_one);
    // Plaintext plain_one("1");
    // evaluator.add_plain_inplace(x_sq_plus_one, plain_one);

    /*
    Encrypted multiplication results in the output ciphertext growing in size.
    More precisely, if the input ciphertexts have size M and N, then the output
    ciphertext after homomorphic multiplication will have size M+N-1. In this
    case we perform a squaring, and observe both size growth and noise budget
    consumption.
    */
    // cout << "    + size of x_sq_plus_one: " << x_sq_plus_one.size() << endl;
    // cout << "    + noise budget in x_sq_plus_one: " << decryptor.invariant_noise_budget(x_sq_plus_one) << " bits"
    //      << endl;

    /*
    Even though the size has grown, decryption works as usual as long as noise
    budget has not reached 0.
    */
    // Plaintext decrypted_result;
    // cout << "    + decryption of x_sq_plus_one: ";
    // decryptor.decrypt(x_sq_plus_one, decrypted_result);
    // cout << "0x" << decrypted_result.to_string() << " ...... Correct." << endl;

    // /*
    // Next, we compute (x + 1)^2.
    // */
    // print_line(__LINE__);
    // cout << "Compute x_plus_one_sq ((x+1)^2)." << endl;
    // Ciphertext x_plus_one_sq;
    // evaluator.add_plain(x_encrypted, plain_one, x_plus_one_sq);
    // evaluator.square_inplace(x_plus_one_sq);
    // cout << "    + size of x_plus_one_sq: " << x_plus_one_sq.size() << endl;
    // cout << "    + noise budget in x_plus_one_sq: " << decryptor.invariant_noise_budget(x_plus_one_sq) << " bits"
    //      << endl;
    // cout << "    + decryption of x_plus_one_sq: ";
    // decryptor.decrypt(x_plus_one_sq, decrypted_result);
    // cout << "0x" << decrypted_result.to_string() << " ...... Correct." << endl;

    // /*
    // Finally, we multiply (x^2 + 1) * (x + 1)^2 * 4.
    // */
    // print_line(__LINE__);
    // cout << "Compute encrypted_result (4(x^2+1)(x+1)^2)." << endl;
    // Ciphertext encrypted_result;
    // Plaintext plain_four("4");
    // evaluator.multiply_plain_inplace(x_sq_plus_one, plain_four);
    // evaluator.multiply(x_sq_plus_one, x_plus_one_sq, encrypted_result);
    // cout << "    + size of encrypted_result: " << encrypted_result.size() << endl;
    // cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(encrypted_result) << " bits"
    //      << endl;
    // cout << "NOTE: Decryption can be incorrect if noise budget is zero." << endl;

    // cout << endl;
    // cout << "~~~~~~ A better way to calculate 4(x^2+1)(x+1)^2. ~~~~~~" << endl;

    // /*
    // Noise budget has reached 0, which means that decryption cannot be expected
    // to give the correct result. This is because both ciphertexts x_sq_plus_one
    // and x_plus_one_sq consist of 3 polynomials due to the previous squaring
    // operations, and homomorphic operations on large ciphertexts consume much more
    // noise budget than computations on small ciphertexts. Computing on smaller
    // ciphertexts is also computationally significantly cheaper.

    // `Relinearization' is an operation that reduces the size of a ciphertext after
    // multiplication back to the initial size, 2. Thus, relinearizing one or both
    // input ciphertexts before the next multiplication can have a huge positive
    // impact on both noise growth and performance, even though relinearization has
    // a significant computational cost itself. It is only possible to relinearize
    // size 3 ciphertexts down to size 2, so often the user would want to relinearize
    // after each multiplication to keep the ciphertext sizes at 2.

    // Relinearization requires special `relinearization keys', which can be thought
    // of as a kind of public key. Relinearization keys can easily be created with
    // the KeyGenerator.

    // Relinearization is used similarly in both the BFV and the CKKS schemes, but
    // in this example we continue using BFV. We repeat our computation from before,
    // but this time relinearize after every multiplication.
    // */
    // print_line(__LINE__);
    // cout << "Generate relinearization keys." << endl;
    // RelinKeys relin_keys;
    // keygen.create_relin_keys(relin_keys);

    // /*
    // We now repeat the computation relinearizing after each multiplication.
    // */
    // print_line(__LINE__);
    // cout << "Compute and relinearize x_squared (x^2)," << endl;
    // cout << string(13, ' ') << "then compute x_sq_plus_one (x^2+1)" << endl;
    // Ciphertext x_squared;
    // evaluator.square(x_encrypted, x_squared);
    // cout << "    + size of x_squared: " << x_squared.size() << endl;
    // evaluator.relinearize_inplace(x_squared, relin_keys);
    // cout << "    + size of x_squared (after relinearization): " << x_squared.size() << endl;
    // evaluator.add_plain(x_squared, plain_one, x_sq_plus_one);
    // cout << "    + noise budget in x_sq_plus_one: " << decryptor.invariant_noise_budget(x_sq_plus_one) << " bits"
    //      << endl;
    // cout << "    + decryption of x_sq_plus_one: ";
    // decryptor.decrypt(x_sq_plus_one, decrypted_result);
    // cout << "0x" << decrypted_result.to_string() << " ...... Correct." << endl;

    // print_line(__LINE__);
    // Ciphertext x_plus_one;
    // cout << "Compute x_plus_one (x+1)," << endl;
    // cout << string(13, ' ') << "then compute and relinearize x_plus_one_sq ((x+1)^2)." << endl;
    // evaluator.add_plain(x_encrypted, plain_one, x_plus_one);
    // evaluator.square(x_plus_one, x_plus_one_sq);
    // cout << "    + size of x_plus_one_sq: " << x_plus_one_sq.size() << endl;
    // evaluator.relinearize_inplace(x_plus_one_sq, relin_keys);
    // cout << "    + noise budget in x_plus_one_sq: " << decryptor.invariant_noise_budget(x_plus_one_sq) << " bits"
    //      << endl;
    // cout << "    + decryption of x_plus_one_sq: ";
    // decryptor.decrypt(x_plus_one_sq, decrypted_result);
    // cout << "0x" << decrypted_result.to_string() << " ...... Correct." << endl;

    // print_line(__LINE__);
    // cout << "Compute and relinearize encrypted_result (4(x^2+1)(x+1)^2)." << endl;
    // evaluator.multiply_plain_inplace(x_sq_plus_one, plain_four);
    // evaluator.multiply(x_sq_plus_one, x_plus_one_sq, encrypted_result);
    // cout << "    + size of encrypted_result: " << encrypted_result.size() << endl;
    // evaluator.relinearize_inplace(encrypted_result, relin_keys);
    // cout << "    + size of encrypted_result (after relinearization): " << encrypted_result.size() << endl;
    // cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(encrypted_result) << " bits"
    //      << endl;

    // cout << endl;
    // cout << "NOTE: Notice the increase in remaining noise budget." << endl;

    // /*
    // Relinearization clearly improved our noise consumption. We have still plenty
    // of noise budget left, so we can expect the correct answer when decrypting.
    // */
    // print_line(__LINE__);
    // cout << "Decrypt encrypted_result (4(x^2+1)(x+1)^2)." << endl;
    // decryptor.decrypt(encrypted_result, decrypted_result);
    // cout << "    + decryption of 4(x^2+1)(x+1)^2 = 0x" << decrypted_result.to_string() << " ...... Correct." << endl;
    // cout << endl;

    // /*
    // For x=6, 4(x^2+1)(x+1)^2 = 7252. Since the plaintext modulus is set to 1024,
    // this result is computed in integers modulo 1024. Therefore the expected output
    // should be 7252 % 1024 == 84, or 0x54 in hexadecimal.
    // */

    // /*
    // Sometimes we create customized encryption parameters which turn out to be invalid.
    // Microsoft SEAL can interpret the reason why parameters are considered invalid.
    // Here we simply reduce the polynomial modulus degree to make the parameters not
    // compliant with the HomomorphicEncryption.org security standard.
    // */
    // print_line(__LINE__);
    // cout << "An example of invalid parameters" << endl;
    // parms.set_poly_modulus_degree(2048);
    // context = SEALContext(parms);
    // print_parameters(context);
    // cout << "Parameter validation (failed): " << context.parameter_error_message() << endl << endl;

    // /*
    // This information is helpful to fix invalid encryption parameters.
    // */
}



void see_cipher(const BatchEncoder  &encoder, Decryptor& decryptor, const vector<Ciphertext> & c)
{
    for (int i=0; i<c.size(); i++)
    {   
        Plaintext p_result;
        vector<uint64_t> r;
        decryptor.decrypt(c[i], p_result);
        encoder.decode(p_result, r);
        cout << "    + result ...... i="<<i<< endl;
        print_vector(r, 2048, 7);
    }
}

vector<Ciphertext> decomposition(const vector<Ciphertext> & c, const Evaluator & evaluator, const BatchEncoder &encoder, const GaloisKeys &galois_keys, int cols, int deep=-2)
{

    vector<Ciphertext> result;
    if ((deep<-1) || (deep>=log2(cols)))
    {
        deep=log2(cols)-1;
    }

    std::cout<<"deep="<<deep<<std::endl;
    
    //if (deep==-1)
    if (deep==7)
    {
        return c;
    }
    

    vector<uint64_t> mask_l, mask_r;
    mask_l.resize(cols*2);
    mask_r.resize(cols*2);
    print_line(__LINE__);
    for (int i=0; i<cols; i++)
    {
        if ( ((~i)&(1U<<deep))==(1U<<deep))  // left 
        {
            mask_l[i]=1;
            mask_l[cols+i]=1;
            mask_r[i]=0;
            mask_r[cols+i]=0;
        }
        else   
        {
            mask_l[i]=0;
            mask_l[cols+i]=0;
            mask_r[i]=1;
            mask_r[cols+i]=1;
        }
    }
    // std::cout<<"mask_l="<<std::endl;
    // for (int i=0; i<cols; i++)
    // {
    //     std::cout<<mask_l[i];
    // }
    // std::cout<<std::endl;
    // std::cout<<"mask_r="<<std::endl;
    // for (int i=0; i<cols; i++)
    // {
    //     std::cout<<mask_r[i];
    // }
    // std::cout<<std::endl;
    print_line(__LINE__);
    Plaintext p_mask_l, p_mask_r;
    encoder.encode(mask_l, p_mask_l);
    encoder.encode(mask_r, p_mask_r);
     print_line(__LINE__);
    for (int i=0; i<c.size(); i++)
    {
        //print_line(__LINE__);
        //std::cout<<"i="<<i<<std::endl;
        Ciphertext cl, cr, cl_rotate, cr_rotate;
        //print_line(__LINE__);

        evaluator.multiply_plain(c[i], p_mask_l, cl);
        //print_line(__LINE__);
        evaluator.rotate_rows(cl, 1<<deep, galois_keys, cl_rotate);
        //print_line(__LINE__);
        evaluator.add_inplace(cl, cl_rotate);
        //evaluator.mod_switch_to_next_inplace(cl);
        //print_line(__LINE__);
        result.push_back(cl);
        //print_line(__LINE__);
        evaluator.multiply_plain(c[i], p_mask_r, cr);
        //print_line(__LINE__);
        evaluator.rotate_rows(cr, 1<<deep, galois_keys, cr_rotate);
       // print_line(__LINE__);
        //evaluator.add_inplace(cr, cr_rotate);
        //print_line(__LINE__);
        result.push_back(cr);
        evaluator.mod_switch_to_next_inplace(cr);
        //print_line(__LINE__);
    }
     //print_line(__LINE__);

    result=decomposition(result, evaluator, encoder, galois_keys, cols, deep-1);

    


    return result;
}




// vector<Ciphertext> decomposition(const vector<Ciphertext> & c, const Evaluator & evaluator, const GaloisKeys &galois_keys, int cols, int deep=-2)
// {

//     vector<Ciphertext> result;
//     if ((deep<-1) || (deep>=log2(cols)))
//     {
//         deep=log2(cols)-1;
//     }

//     std::cout<<"deep="<<deep<<std::endl;
    
//     //if (deep==-1)
//     if (deep==7)
//     {
//         return c;
//     }
    

//     vector<uint64_t> mask_l, mask_r;
//     mask_l.resize(cols*2);
//     mask_r.resize(cols*2);
//     print_line(__LINE__);
//     for (int i=0; i<cols; i++)
//     {
//         if ( ((~i)&(1U<<deep))==(1U<<deep))  // left 
//         {
//             mask_l[i]=1;
//             mask_l[cols+i]=1;
//             mask_r[i]=0;
//             mask_r[cols+i]=0;
//         }
//         else   
//         {
//             mask_l[i]=0;
//             mask_l[cols+i]=0;
//             mask_r[i]=1;
//             mask_r[cols+i]=1;
//         }
//     }
//     // std::cout<<"mask_l="<<std::endl;
//     // for (int i=0; i<cols; i++)
//     // {
//     //     std::cout<<mask_l[i];
//     // }
//     // std::cout<<std::endl;
//     // std::cout<<"mask_r="<<std::endl;
//     // for (int i=0; i<cols; i++)
//     // {
//     //     std::cout<<mask_r[i];
//     // }
//     // std::cout<<std::endl;
//     print_line(__LINE__);
//     Plaintext p_mask_l, p_mask_r;
//     encoder.encode(mask_l, p_mask_l);
//     encoder.encode(mask_r, p_mask_r);
//      print_line(__LINE__);
//     for (int i=0; i<c.size(); i++)
//     {
//         //print_line(__LINE__);
//         //std::cout<<"i="<<i<<std::endl;
//         Ciphertext cl, cr, cl_rotate, cr_rotate;
//         //print_line(__LINE__);

//         evaluator.multiply_plain(c[i], p_mask_l, cl);
//         //print_line(__LINE__);
//         evaluator.rotate_rows(cl, 1<<deep, galois_keys, cl_rotate);
//         //print_line(__LINE__);
//         evaluator.add_inplace(cl, cl_rotate);
//         //evaluator.mod_switch_to_next_inplace(cl);
//         //print_line(__LINE__);
//         result.push_back(cl);
//         //print_line(__LINE__);
//         evaluator.multiply_plain(c[i], p_mask_r, cr);
//         //print_line(__LINE__);
//         evaluator.rotate_rows(cr, 1<<deep, galois_keys, cr_rotate);
//        // print_line(__LINE__);
//         //evaluator.add_inplace(cr, cr_rotate);
//         //print_line(__LINE__);
//         result.push_back(cr);
//         evaluator.mod_switch_to_next_inplace(cr);
//         //print_line(__LINE__);
//     }
//      //print_line(__LINE__);

//     result=decomposition(result, evaluator, encoder, galois_keys, cols, deep-1);

    


//     return result;
// }


void test_decomponent_bgv()
{
    EncryptionParameters parms(scheme_type::bgv);

    size_t poly_modulus_degree = 2048*2;
    size_t cols = poly_modulus_degree/2;
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 16));
    parms.set_poly_modulus_degree(poly_modulus_degree);
    //parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));

    /*
    We choose the initial scale to be 2^40. At the last level, this leaves us
    60-40=20 bits of precision before the decimal point, and enough (roughly
    10-20 bits) of precision after the decimal point. Since our intermediate
    primes are 40 bits (in fact, they are very close to 2^40), we can achieve
    scale stabilization as described above.
    */
    // double scale = pow(2.0, 20);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    GaloisKeys galois_keys;
    keygen.create_galois_keys(galois_keys);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    BatchEncoder encoder(context);
    vector<uint64_t> x(cols*2);
    for (int i=0; i<cols*2; i++)
    {
        x[i]=i%3;
    }
    Plaintext p_x;
    Ciphertext c_x;
    encoder.encode(x, p_x);
    encryptor.encrypt(p_x, c_x);

    see_cipher(encoder, decryptor, {c_x});
    auto decomped_querys=decomposition({c_x}, evaluator, encoder, galois_keys, cols, -2);

    auto  budget=decryptor.invariant_noise_budget(decomped_querys[0]);
    std::cout<<"budget="<<budget<<std::endl;
    print_line(__LINE__);
    
    see_cipher(encoder, decryptor, {decomped_querys[0]});

}


void example_batch_encoder8192_167772161_bgv_dedegree()
{
    print_example_banner("Example: Encoders / Batch Encoder");

    /*
    [BatchEncoder] (For BFV or BGV scheme)

    Let N denote the poly_modulus_degree and T denote the plain_modulus. Batching
    allows the BFV plaintext polynomials to be viewed as 2-by-(N/2) matrices, with
    each element an integer modulo T. In the matrix view, encrypted operations act
    element-wise on encrypted matrices, allowing the user to obtain speeds-ups of
    several orders of magnitude in fully vectorizable computations. Thus, in all
    but the simplest computations, batching should be the preferred method to use
    with BFV, and when used properly will result in implementations outperforming
    anything done without batching.

    In a later example, we will demonstrate how to use the BGV scheme. Batching
    works similarly for the BGV scheme to this example for the BFV scheme. For example,
    simply changing `scheme_type::bfv` into `scheme_type::bgv` can make this example
    work for the BGV scheme.
    */
    //int n =  16;
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 8192;
    //size_t poly_modulus_degree = 1<<n;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(4096));
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 41, 42, 42}));


    /*
    To enable batching, we need to set the plain_modulus to be a prime number
    congruent to 1 modulo 2*poly_modulus_degree. Microsoft SEAL provides a helper
    method for finding such a prime. In this example we create a 20-bit prime
    that supports batching.
    */
    parms.set_plain_modulus(167772161);
    // parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 30));
    // parms.set_plain_modulus(257);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    /*
    We can verify that batching is indeed enabled by looking at the encryption
    parameter qualifiers created by SEALContext.
    */
    auto qualifiers = context.first_context_data()->qualifiers();
    cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
    // RelinKeys relin_keys;
    // keygen.create_relin_keys(relin_keys);

   


    // Plaintext plain_matrix("83886080x^1023 + 30x^3 + 83886080x^1 + 3");
    // string x = "a000000x^1023 + 30x^3 + 1x^1 + a000000";
    // Plaintext plain_matrix(x);
    Plaintext plain_matrix(4096);
    // plain_matrix[0]=167772160UL;
    // plain_matrix[1]=1UL;
    // plain_matrix[3]=30UL;
    // plain_matrix[1023]=167772160UL;
    for (int i=0; i<4096; i++)
    {
        plain_matrix[i]=random_uint64()%167772161UL;
    }


    // cout << "Express x  as a plaintext polynomial 0x" + plain_matrix.to_string() + "." << endl;

    print_line(__LINE__);

    /*
    Next we encrypt the encoded plaintext.
    */
    Ciphertext encrypted_matrix, encrypted_matrix2, sum_encrypted_matrix;
    print_line(__LINE__);
    cout << "Encrypt plain_matrix to encrypted_matrix." << endl;
    encryptor.encrypt(plain_matrix, encrypted_matrix);
    print_line(__LINE__);
    std::cout<<"is_ntt_form<<"<<encrypted_matrix.is_ntt_form()<<std::endl;
    cout << "    + Noise budget in encrypted_matrix: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
         << endl;
    // evaluator.transform_to_ntt_inplace(encrypted_matrix);
    // std::cout<<"is_ntt_form<<"<<encrypted_matrix.is_ntt_form()<<std::endl;
    // cout << "    + Noise budget in encrypted_matrix: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
    //      << endl;

    /*
    Operating on the ciphertext results in homomorphic operations being performed
    simultaneously in all 8192 slots (matrix elements). To illustrate this, we
    form another plaintext matrix

        [ 1,  2,  1,  2,  1,  2, ..., 2 ]
        [ 1,  2,  1,  2,  1,  2, ..., 2 ]

    and encode it into a plaintext.
    */


    /*
    We now add the second (plaintext) matrix to the encrypted matrix, and square
    the sum.
    */
    print_line(__LINE__);
    cout << "multiply plaintext and sum" << endl;
    evaluator.transform_to_ntt_inplace(plain_matrix, context.first_parms_id());
     std::cout<<"plain_matrix is_ntt_form<<"<<plain_matrix.is_ntt_form()<<std::endl;
        time_t start_time, end_time;
    time(&start_time);
//    #pragma omp parallel for
    for (int i=0; i<4096*4; i++)
    {
        evaluator.multiply_plain(encrypted_matrix, plain_matrix, encrypted_matrix2);
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix2);
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix2);
        }
    }

    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;
    /*
    How much noise budget do we have left?
    */
    cout << "    + Noise budget in result: " << decryptor.invariant_noise_budget(sum_encrypted_matrix) << " bits" << endl;

    /*
    We decrypt and decompose the plaintext to recover the result as a matrix.
    */
    Plaintext plain_result;
    print_line(__LINE__);
    cout << "Decrypt and decode result." << endl;
    decryptor.decrypt(sum_encrypted_matrix, plain_result);
    cout << "    + Result plaintext matrix ...... Correct." << endl;
    // cout << plain_result.to_string() <<endl;

    /*
    Batching allows us to efficiently use the full plaintext polynomial when the
    desired encrypted computation is highly parallelizable. However, it has not
    solved the other problem mentioned in the beginning of this file: each slot
    holds only an integer modulo plain_modulus, and unless plain_modulus is very
    large, we can quickly encounter data type overflow and get unexpected results
    when integer computations are desired. Note that overflow cannot be detected
    in encrypted form. The CKKS scheme (and the CKKSEncoder) addresses the data
    type overflow issue, but at the cost of yielding only approximate results.
    */
}



void example_batch_encoder8192_167772161_bgv_evaluation()
{
    print_example_banner("Example: Encoders / Batch Encoder");

    /*
    [BatchEncoder] (For BFV or BGV scheme)

    Let N denote the poly_modulus_degree and T denote the plain_modulus. Batching
    allows the BFV plaintext polynomials to be viewed as 2-by-(N/2) matrices, with
    each element an integer modulo T. In the matrix view, encrypted operations act
    element-wise on encrypted matrices, allowing the user to obtain speeds-ups of
    several orders of magnitude in fully vectorizable computations. Thus, in all
    but the simplest computations, batching should be the preferred method to use
    with BFV, and when used properly will result in implementations outperforming
    anything done without batching.

    In a later example, we will demonstrate how to use the BGV scheme. Batching
    works similarly for the BGV scheme to this example for the BFV scheme. For example,
    simply changing `scheme_type::bfv` into `scheme_type::bgv` can make this example
    work for the BGV scheme.
    */
    //int n =  16;
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 8192;
    //size_t poly_modulus_degree = 1<<n;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    // parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 41, 41, 42, 42}));
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 42, 42, 42, 42}));


    /*
    To enable batching, we need to set the plain_modulus to be a prime number
    congruent to 1 modulo 2*poly_modulus_degree. Microsoft SEAL provides a helper
    method for finding such a prime. In this example we create a 20-bit prime
    that supports batching.
    */
    parms.set_plain_modulus(167772161);
    // parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 26));
    // parms.set_plain_modulus(257);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    /*
    We can verify that batching is indeed enabled by looking at the encryption
    parameter qualifiers created by SEALContext.
    */
    auto qualifiers = context.first_context_data()->qualifiers();
    cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);


    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    /*
    Batching is done through an instance of the BatchEncoder class.
    */
    BatchEncoder batch_encoder(context);

    /*
    The total number of batching `slots' equals the poly_modulus_degree, N, and
    these slots are organized into 2-by-(N/2) matrices that can be encrypted and
    computed on. Each slot contains an integer modulo plain_modulus.
    */
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;
    cout << "Plaintext matrix row size: " << row_size << endl;

    /*
    The matrix plaintext is simply given to BatchEncoder as a flattened vector
    of numbers. The first `row_size' many numbers form the first row, and the
    rest form the second row. Here we create the following matrix:

        [ 0,  1,  2,  3,  0,  0, ...,  0 ]
        [ 4,  5,  6,  7,  0,  0, ...,  0 ]
    */
    vector<uint64_t> pod_matrix(slot_count, 0ULL);
    pod_matrix[0] = 0ULL;
    pod_matrix[1] = 1ULL;
    pod_matrix[2] = 2ULL;
    pod_matrix[3] = 3ULL;
    pod_matrix[row_size] = 4ULL;
    pod_matrix[row_size + 1] = 5ULL;
    pod_matrix[row_size + 2] = 6ULL;
    pod_matrix[row_size + 3] = 7ULL;

    cout << "Input plaintext matrix:" << endl;
    print_matrix(pod_matrix, row_size);

    /*
    First we use BatchEncoder to encode the matrix into a plaintext polynomial.
    */
    Plaintext plain_matrix;
    print_line(__LINE__);
    cout << "Encode plaintext matrix:" << endl;
    batch_encoder.encode(pod_matrix, plain_matrix);

    /*
    We can instantly decode to verify correctness of the encoding. Note that no
    encryption or decryption has yet taken place.
    */
    vector<uint64_t> pod_result;
    cout << "    + Decode plaintext matrix ...... Correct." << endl;
    batch_encoder.decode(plain_matrix, pod_result);
    print_matrix(pod_result, row_size);

    /*
    Next we encrypt the encoded plaintext.
    */
    Ciphertext encrypted_matrix, encrypted_matrix2, encrypted_matrix3, sum_encrypted_matrix;
    print_line(__LINE__);
    cout << "Encrypt plain_matrix to encrypted_matrix." << endl;
    encryptor.encrypt(plain_matrix, encrypted_matrix);
    cout << "    + Noise budget in encrypted_matrix: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
         << endl;

    /*
    Operating on the ciphertext results in homomorphic operations being performed
    simultaneously in all 8192 slots (matrix elements). To illustrate this, we
    form another plaintext matrix

        [ 1,  2,  1,  2,  1,  2, ..., 2 ]
        [ 1,  2,  1,  2,  1,  2, ..., 2 ]

    and encode it into a plaintext.
    */
    Plaintext k(1);
    k[0]=167772160UL;

    /*
    We now add the second (plaintext) matrix to the encrypted matrix, and square
    the sum.
    */
    print_line(__LINE__);
    cout << "scala multiply " << endl;
        time_t start_time, end_time;
    time(&start_time);
    std::cout<<"k is ntt form"<<k.is_ntt_form()<<std::endl;
    std::cout<<"encrypted_matrix is_ntt_form<<"<<encrypted_matrix.is_ntt_form()<<std::endl;
    evaluator.transform_from_ntt_inplace(encrypted_matrix);
    std::cout<<"encrypted_matrix is_ntt_form<<"<<encrypted_matrix.is_ntt_form()<<std::endl;

    for (int i=0; i<8192; i++)
    {
        evaluator.multiply_plain(encrypted_matrix, k, encrypted_matrix2);
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix2);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix2);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;

    evaluator.transform_to_ntt_inplace(encrypted_matrix);
    //  cout << "    + Noise budget in result: " << decryptor.invariant_noise_budget(sum_encrypted_matrix) << " bits" << endl;
// evaluator.transform_from_ntt_inplace(sum_encrypted_matrix);

    Ciphertext encrypted_matrix2_ntt;

    print_line(__LINE__);
    cout << " multiply " << endl;
        // time_t start_time, end_time;

    encrypted_matrix2 = Ciphertext(sum_encrypted_matrix);
    
    time(&start_time);


    for (int i=0; i<64; i++)
    {

        evaluator.transform_to_ntt(encrypted_matrix2, encrypted_matrix2_ntt);
        evaluator.multiply(encrypted_matrix, encrypted_matrix2_ntt, encrypted_matrix3);
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix3);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix3);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;



    /*
    How much noise budget do we have left?
    */
    cout << "    + Noise budget in result: " << decryptor.invariant_noise_budget(sum_encrypted_matrix) << " bits" << endl;

    /*
    We decrypt and decompose the plaintext to recover the result as a matrix.
    */
    Plaintext plain_result;
    print_line(__LINE__);
    cout << "Decrypt and decode result." << endl;
    decryptor.decrypt(sum_encrypted_matrix, plain_result);
    batch_encoder.decode(plain_result, pod_result);
    cout << "    + Result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Batching allows us to efficiently use the full plaintext polynomial when the
    desired encrypted computation is highly parallelizable. However, it has not
    solved the other problem mentioned in the beginning of this file: each slot
    holds only an integer modulo plain_modulus, and unless plain_modulus is very
    large, we can quickly encounter data type overflow and get unexpected results
    when integer computations are desired. Note that overflow cannot be detected
    in encrypted form. The CKKS scheme (and the CKKSEncoder) addresses the data
    type overflow issue, but at the cost of yielding only approximate results.
    */
}



void example_batch_encoder8192_167772161_bgv_3coeff_evaluation()
{
    print_example_banner("Example: Encoders / Batch Encoder");

    /*
    [BatchEncoder] (For BFV or BGV scheme)

    Let N denote the poly_modulus_degree and T denote the plain_modulus. Batching
    allows the BFV plaintext polynomials to be viewed as 2-by-(N/2) matrices, with
    each element an integer modulo T. In the matrix view, encrypted operations act
    element-wise on encrypted matrices, allowing the user to obtain speeds-ups of
    several orders of magnitude in fully vectorizable computations. Thus, in all
    but the simplest computations, batching should be the preferred method to use
    with BFV, and when used properly will result in implementations outperforming
    anything done without batching.

    In a later example, we will demonstrate how to use the BGV scheme. Batching
    works similarly for the BGV scheme to this example for the BFV scheme. For example,
    simply changing `scheme_type::bfv` into `scheme_type::bgv` can make this example
    work for the BGV scheme.
    */
    //int n =  16;
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 8192;
    //size_t poly_modulus_degree = 1<<n;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 43, 43, 43, 43}));
    // parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 60, 60})); // 不成， fresh 噪声会变小


    /*
    To enable batching, we need to set the plain_modulus to be a prime number
    congruent to 1 modulo 2*poly_modulus_degree. Microsoft SEAL provides a helper
    method for finding such a prime. In this example we create a 20-bit prime
    that supports batching.
    */
    parms.set_plain_modulus(167772161);
    // parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 26));
    // parms.set_plain_modulus(257);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    /*
    We can verify that batching is indeed enabled by looking at the encryption
    parameter qualifiers created by SEALContext.
    */
    auto qualifiers = context.first_context_data()->qualifiers();
    cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);


    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    /*
    Batching is done through an instance of the BatchEncoder class.
    */
    BatchEncoder batch_encoder(context);

    /*
    The total number of batching `slots' equals the poly_modulus_degree, N, and
    these slots are organized into 2-by-(N/2) matrices that can be encrypted and
    computed on. Each slot contains an integer modulo plain_modulus.
    */
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;
    cout << "Plaintext matrix row size: " << row_size << endl;

    /*
    The matrix plaintext is simply given to BatchEncoder as a flattened vector
    of numbers. The first `row_size' many numbers form the first row, and the
    rest form the second row. Here we create the following matrix:

        [ 0,  1,  2,  3,  0,  0, ...,  0 ]
        [ 4,  5,  6,  7,  0,  0, ...,  0 ]
    */
    vector<uint64_t> pod_matrix(slot_count, 0ULL);
    pod_matrix[0] = 0ULL;
    pod_matrix[1] = 1ULL;
    pod_matrix[2] = 2ULL;
    pod_matrix[3] = 3ULL;
    pod_matrix[row_size] = 4ULL;
    pod_matrix[row_size + 1] = 5ULL;
    pod_matrix[row_size + 2] = 6ULL;
    pod_matrix[row_size + 3] = 7ULL;

    cout << "Input plaintext matrix:" << endl;
    print_matrix(pod_matrix, row_size);

    /*
    First we use BatchEncoder to encode the matrix into a plaintext polynomial.
    */
    Plaintext plain_matrix;
    print_line(__LINE__);
    cout << "Encode plaintext matrix:" << endl;
    batch_encoder.encode(pod_matrix, plain_matrix);

    /*
    We can instantly decode to verify correctness of the encoding. Note that no
    encryption or decryption has yet taken place.
    */
    vector<uint64_t> pod_result;
    cout << "    + Decode plaintext matrix ...... Correct." << endl;
    batch_encoder.decode(plain_matrix, pod_result);
    print_matrix(pod_result, row_size);

    /*
    Next we encrypt the encoded plaintext.
    */
    Ciphertext encrypted_matrix, encrypted_matrix2, encrypted_matrix3, sum_encrypted_matrix, sum_encrypted_matrix2;
    print_line(__LINE__);
    cout << "Encrypt plain_matrix to encrypted_matrix." << endl;
    encryptor.encrypt(plain_matrix, encrypted_matrix);
    encryptor.encrypt(plain_matrix, encrypted_matrix2);
    cout << "    + Noise budget in encrypted_matrix: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
         << endl;
             cout << "    + Noise budget in encrypted_matrix2: " << decryptor.invariant_noise_budget(encrypted_matrix2) << " bits"
         << endl;

    /*
    Operating on the ciphertext results in homomorphic operations being performed
    simultaneously in all 8192 slots (matrix elements). To illustrate this, we
    form another plaintext matrix

        [ 1,  2,  1,  2,  1,  2, ..., 2 ]
        [ 1,  2,  1,  2,  1,  2, ..., 2 ]

    and encode it into a plaintext.
    */
    Plaintext k(1);
    k[0]=167772160UL;

    /*
    We now add the second (plaintext) matrix to the encrypted matrix, and square
    the sum.
    */
    print_line(__LINE__);
    cout << "scala multiply " << endl;
        time_t start_time, end_time;
    time(&start_time);
    std::cout<<"k is ntt form"<<k.is_ntt_form()<<std::endl;
    std::cout<<"encrypted_matrix is_ntt_form<<"<<encrypted_matrix.is_ntt_form()<<std::endl;
    evaluator.transform_from_ntt_inplace(encrypted_matrix);
    std::cout<<"encrypted_matrix is_ntt_form<<"<<encrypted_matrix.is_ntt_form()<<std::endl;
    // cout << "    + Noise budget in encrypted_matrix ntt_form: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
    //      << endl;

    for (int i=0; i<81920; i++)
    {
        evaluator.multiply_plain(encrypted_matrix, k, encrypted_matrix3);
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix3);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix3);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;

    // evaluator.transform_to_ntt_inplace(encrypted_matrix);
    //  cout << "    + Noise budget in result: " << decryptor.invariant_noise_budget(sum_encrypted_matrix) << " bits" << endl;
// evaluator.transform_from_ntt_inplace(sum_encrypted_matrix);

    Ciphertext sum_encrypted_matrix_ntt;

    print_line(__LINE__);
    cout << " multiply " << endl;
        // time_t start_time, end_time;


    
    time(&start_time);

    // evaluator.mod_switch_to_next_inplace(encrypted_matrix2);
    for (int i=0; i<64; i++)
    {

        evaluator.transform_to_ntt(sum_encrypted_matrix, sum_encrypted_matrix_ntt);
        // evaluator.mod_switch_to_next0_inplace(sum_encrypted_matrix_ntt);
 
        evaluator.multiply(encrypted_matrix2, sum_encrypted_matrix_ntt, encrypted_matrix3);
        if (i==0)
        {
            sum_encrypted_matrix2=Ciphertext(encrypted_matrix3);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix2, encrypted_matrix3);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;



    /*
    How much noise budget do we have left?
    */
    cout << "    + Noise budget in result: " << decryptor.invariant_noise_budget(sum_encrypted_matrix2) << " bits" << endl;

    /*
    We decrypt and decompose the plaintext to recover the result as a matrix.
    */
    Plaintext plain_result;
    print_line(__LINE__);
    cout << "Decrypt and decode result." << endl;
    decryptor.decrypt(sum_encrypted_matrix2, plain_result);
    batch_encoder.decode(plain_result, pod_result);
    cout << "    + Result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Batching allows us to efficiently use the full plaintext polynomial when the
    desired encrypted computation is highly parallelizable. However, it has not
    solved the other problem mentioned in the beginning of this file: each slot
    holds only an integer modulo plain_modulus, and unless plain_modulus is very
    large, we can quickly encounter data type overflow and get unexpected results
    when integer computations are desired. Note that overflow cannot be detected
    in encrypted form. The CKKS scheme (and the CKKSEncoder) addresses the data
    type overflow issue, but at the cost of yielding only approximate results.
    */
}




void example_batch_encoder8192_167772161_bgv_3coeff_evaluation_plainciphermul()
{
    print_example_banner("Example: Encoders / Batch Encoder");


    //int n =  16;
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 8192;
    //size_t poly_modulus_degree = 1<<n;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    // parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    std::vector<Modulus> coeff_modulus = CoeffModulus::Create(poly_modulus_degree, { 43, 43, 43, 43});
    parms.set_coeff_modulus(coeff_modulus);
    // parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 60, 60})); // 不成， fresh 噪声会变小

    parms.set_plain_modulus(167772161);
    // parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 26));
    // parms.set_plain_modulus(257);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    /*
    We can verify that batching is indeed enabled by looking at the encryption
    parameter qualifiers created by SEALContext.
    */
    auto qualifiers = context.first_context_data()->qualifiers();
    cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);


    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    /*
    Batching is done through an instance of the BatchEncoder class.
    */
    BatchEncoder batch_encoder(context);

    /*
    The total number of batching `slots' equals the poly_modulus_degree, N, and
    these slots are organized into 2-by-(N/2) matrices that can be encrypted and
    computed on. Each slot contains an integer modulo plain_modulus.
    */
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;
    cout << "Plaintext matrix row size: " << row_size << endl;

    /*
    The matrix plaintext is simply given to BatchEncoder as a flattened vector
    of numbers. The first `row_size' many numbers form the first row, and the
    rest form the second row. Here we create the following matrix:

        [ 0,  1,  2,  3,  0,  0, ...,  0 ]
        [ 4,  5,  6,  7,  0,  0, ...,  0 ]
    */
    vector<uint64_t> pod_matrix(slot_count, 0ULL);
    pod_matrix[0] = 0ULL;
    pod_matrix[1] = 1ULL;
    pod_matrix[2] = 2ULL;
    pod_matrix[3] = 3ULL;
    pod_matrix[row_size] = 4ULL;
    pod_matrix[row_size + 1] = 5ULL;
    pod_matrix[row_size + 2] = 6ULL;
    pod_matrix[row_size + 3] = 7ULL;

    cout << "Input plaintext matrix:" << endl;
    print_matrix(pod_matrix, row_size);

    /*
    First we use BatchEncoder to encode the matrix into a plaintext polynomial.
    */
    Plaintext plain_matrix;
    print_line(__LINE__);
    cout << "Encode plaintext matrix:" << endl;
    batch_encoder.encode(pod_matrix, plain_matrix);

    /*
    We can instantly decode to verify correctness of the encoding. Note that no
    encryption or decryption has yet taken place.
    */
    vector<uint64_t> pod_result;
    cout << "    + Decode plaintext matrix ...... Correct." << endl;
    batch_encoder.decode(plain_matrix, pod_result);
    print_matrix(pod_result, row_size);

    /*
    Next we encrypt the encoded plaintext.
    */
    Ciphertext encrypted_matrix, encrypted_matrix2, encrypted_matrix3, sum_encrypted_matrix, sum_encrypted_matrix2;
    print_line(__LINE__);
    cout << "Encrypt plain_matrix to encrypted_matrix." << endl;
    encryptor.encrypt(plain_matrix, encrypted_matrix);
    encryptor.encrypt(plain_matrix, encrypted_matrix2);
    cout << "    + Noise budget in encrypted_matrix: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
         << endl;
             cout << "    + Noise budget in encrypted_matrix2: " << decryptor.invariant_noise_budget(encrypted_matrix2) << " bits"
         << endl;

    /*
    Operating on the ciphertext results in homomorphic operations being performed
    simultaneously in all 8192 slots (matrix elements). To illustrate this, we
    form another plaintext matrix

        [ 1,  2,  1,  2,  1,  2, ..., 2 ]
        [ 1,  2,  1,  2,  1,  2, ..., 2 ]

    and encode it into a plaintext.
    */
    Plaintext k(1);
    k[0]=167772160UL;

    /*
    We now add the second (plaintext) matrix to the encrypted matrix, and square
    the sum.
    */
    print_line(__LINE__);
    cout << "scala multiply " << endl;
        time_t start_time, end_time;
    
    std::cout<<"k is ntt form: "<<k.is_ntt_form()<<std::endl;
    evaluator.transform_from_ntt_inplace(encrypted_matrix);
    std::cout<<"encrypted_matrix is_ntt_form: "<<encrypted_matrix.is_ntt_form()<<std::endl;
    // cout << "    + Noise budget in encrypted_matrix ntt_form: " << decryptor.invariant_noise_budget(encrypted_matrix) << " bits"
    //      << endl;
time(&start_time);
    for (int i=0; i<81920; i++)
    {
        evaluator.multiply_plain(encrypted_matrix, k, encrypted_matrix3);
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix3);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix3);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;




 std::cout<<"k is ntt form: "<<k.is_ntt_form()<<std::endl;
     std::cout<<"encrypted_matrix is_ntt_form: "<<encrypted_matrix.is_ntt_form()<<std::endl;
time(&start_time);
      MemoryPoolHandle pool = MemoryManager::GetPool();
          size_t encrypted_size = encrypted_matrix.size();
    for (int i=0; i<81920; i++)
    {
        multiply_plain_normal_simple(coeff_modulus, encrypted_matrix, encrypted_size, 54, encrypted_matrix3, pool);
        // multiply_plain_normal_simple2(coeff_modulus, encrypted_matrix, encrypted_size, 54, encrypted_matrix3);

        
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix3);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix3);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;









    evaluator.transform_to_ntt_inplace(k, encrypted_matrix.parms_id());
    evaluator.transform_to_ntt_inplace(encrypted_matrix);
 std::cout<<"k is ntt form: "<<k.is_ntt_form()<<std::endl;
     std::cout<<"encrypted_matrix is_ntt_form: "<<encrypted_matrix.is_ntt_form()<<std::endl;
time(&start_time);
   
    for (int i=0; i<81920; i++)
    {
        evaluator.multiply_plain(encrypted_matrix, k, encrypted_matrix3);
        if (i==0)
        {
            sum_encrypted_matrix=Ciphertext(encrypted_matrix3);   
        }
        else
        {
            evaluator.add_inplace(sum_encrypted_matrix, encrypted_matrix3);
        }
    }
    time(&end_time);

    std::cout<<"time="<<end_time-start_time<<std::endl;



    /*
    How much noise budget do we have left?
    */
    cout << "    + Noise budget in result: " << decryptor.invariant_noise_budget(sum_encrypted_matrix) << " bits" << endl;


    /*
    We decrypt and decompose the plaintext to recover the result as a matrix.
    */
    Plaintext plain_result;
    print_line(__LINE__);
    cout << "Decrypt and decode result." << endl;
    decryptor.decrypt(sum_encrypted_matrix, plain_result);
    batch_encoder.decode(plain_result, pod_result);
    cout << "    + Result plaintext matrix ...... Correct." << endl;
    print_matrix(pod_result, row_size);

    /*
    Batching allows us to efficiently use the full plaintext polynomial when the
    desired encrypted computation is highly parallelizable. However, it has not
    solved the other problem mentioned in the beginning of this file: each slot
    holds only an integer modulo plain_modulus, and unless plain_modulus is very
    large, we can quickly encounter data type overflow and get unexpected results
    when integer computations are desired. Note that overflow cannot be detected
    in encrypted form. The CKKS scheme (and the CKKSEncoder) addresses the data
    type overflow issue, but at the cost of yielding only approximate results.
    */
}


int main()
{
    print_example_banner("Example: BGV");

    /*
    Run all encoder examples.
    */
    // example_bgv_basics0();

    // example_bgv_basics_60();
    // test_decomponent_bgv();
    // example_batch_encoder8192_167772161_bgv_dedegree();  // 27 bit 降次
    // example_batch_encoder8192_167772161_bgv_evaluation();
    // example_batch_encoder8192_167772161_bgv_3coeff_evaluation();
    example_batch_encoder8192_167772161_bgv_3coeff_evaluation_plainciphermul();




}
