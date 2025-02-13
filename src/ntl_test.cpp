// from fedquery
#include "include/poly_tool.h"

using namespace std;
// using namespace seal;

int test_fft_interpolate() {
    long p; // NOLINT
    //    p = GenPrime_long(NTL_SP_NBITS);
    p = 180143985094819841UL;
    int64_t w = 44854428751437740L;
    // p = 17UL;
    // p = GenPrime_long(60);

    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    // fft_test();
    // fft_test3(p);
    //  fft_test4(p, 29);
    //    fft_test4(p, 22);
    // BuildFromRoots_test4(p, 12);
    // PowerMod_test4(p, 12);
    //       zz_pX  f;
    //    f.SetLength(1<<22);
    //    random(f, 1<<22);

    int degree = (1 << 22);
    std::vector<int64_t> data(degree), coeff(degree);
    for (int i = 0; i < degree; i++) {
        data[i] = random() % p;
    }

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    // random((zz_pX&) data, degree);

    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    PolyComputer poly_computer(p, w, 22);
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


void test_row_col_basis()
{
    int p = 167772161UL;
    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    size_t row_num=3;
    size_t col_num=4;
   vec_zz_p mat;
    mat.SetLength(row_num*col_num);
   for (size_t i=0; i<row_num; i++)
   {
        for (size_t j=0; j<col_num; j++)
        {
            random(mat[i*col_num+j]);
        }
    }
    vector<zz_pX> row_keeper,  col_keeper;
    find_row_col_basis(mat, col_num, row_keeper,  col_keeper);
    std::cout<<"row_keeper.size()="<<row_keeper.size()<<std::endl;
    std::cout<<"col_keeper.size()="<<col_keeper.size()<<std::endl;
    for (size_t i=0; i<row_num; i++)
    {
         std::cout<<"row_keeper["<<i<<"]="<<row_keeper[i]<<std::endl;
    }
        for (size_t i=0; i<col_num; i++)
    {
         std::cout<<"col_keeper["<<i<<"]="<<col_keeper[i]<<std::endl;
    }

   


}



void test_row_col_basis_stdvec()
{
    int p = 167772161UL;
    zz_p::init(p);
    std::cout << "p=" << zz_p::modulus() << std::endl;
    //  size_t length=46;
   vector<int64_t> mat(2048);
    size_t col_num=32;
    std::cout<<"col_num="<<col_num<<std::endl;
//    for (size_t i=0; i<length; i++)
//    {
//         for (size_t j=0; j<length; j++)
//         {
//             mat[i*length+j]=random()%p;
//         }
//     }
    for (size_t i=0; i<mat.size(); i++)
    {
        mat[i] = random()%p;
    }
    vector<vector<uint64_t>> row_keeper,  col_keeper;
    find_row_col_basis(mat, col_num, p, row_keeper,  col_keeper);

    std::cout<<"col_keeper[0]="<<col_keeper[0].size()<<std::endl;
}







int main(int argc, char **argv) {
    // int party = std::atoi(argv[1]);
    // int task_id = std::atoi(argv[2]);
    // for (int i = 0; i < 1; i++) {
    //     PolyPsi_test(std::to_string(task_id + i), party);
    // }
    // test_fft_interpolate(); 
    // test_x_powers_mod_new_ids();
    // test_row_col_basis();
    test_row_col_basis_stdvec();

    return 0;
}
