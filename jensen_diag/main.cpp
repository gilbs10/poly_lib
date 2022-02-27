#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "genfunc.h"
#include "RectManager.h"
#include <omp.h>


using namespace std;
long long sigs = 0;
int global_n; //Allows us to save presious memory in each GenFunc.
/*Count the number of polyominoes using transfer matrix algorithm on the diagonal.
/*Count the number of polyominoes using transfer matrix algorithm on the diagonal.
 * The code should be highly optimize, this sometime will be instead of readability and making the generic and useful*/

void print_time(){
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout <<  "Time: " << ctime(&timenow) << flush;
}

gf_type count_rect(int w, int n, bool wm, int num_of_threads){
    gf_type c = 0;
    global_n = n;
//    RectManager* rm = new RectManager(w, n, bool(wm));
    RectManagerParallel* rm = new RectManagerParallel(w, n, bool(wm), num_of_threads);
    rm->run_rectangle();
    for (int j = w; j <= n; ++j) {
        if (rm->res->find(j) != rm->res->end()){
            c += (*rm->res)[j]->at(n);
            if(j > w){
                c += (*rm->res)[j]->at(n);
            }
        }
    }
    cout << "Num of sigs processed:" << rm->sig_counter << endl;
    cout << "Result: " << c << endl;
    sigs += rm->sig_counter;
    delete rm;
    return c;
}

void count(int n, int num_of_threads){
    gf_type c = 0;
    global_n = n;
    for (int wm = 0; wm < 2; ++wm) {
        for (int k = 2; k < n+1; ++k) {
            cout << "Running cols " << k <<", wm="<<wm << endl;
            print_time();
            c += count_rect(k, n, wm, num_of_threads);
        }
    }
    cout << n << ": " << c << endl;
}


void run_all(int n1, int n2, int num_of_threads = 1) {
    for (int i = n1; i <= n2; ++i){
        auto timenow =
                chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << "Running " << i << ", time: " << ctime(&timenow) << endl;
        count(i, num_of_threads);
    }
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << ctime(&timenow) << endl;
}

void get_input_and_run(){
    cout << "Run rectangle (r) or full size (n)?" << endl;
    char run_type;
    cin >> run_type;
    int n1, n2, num_of_threads;
    int n,w;
    if(run_type == 'n'){
        cout << "Enter sizes to from and to:" << endl;
        cin >> n1 >> n2;
        cout << "Max number of threads: " << omp_get_max_threads() << endl;
        cout << "Enter num of threads to run on:" << endl;
        cin >> num_of_threads;
        run_all(n1, n2, num_of_threads);
    } else {
        cout << "Enter size:" << endl;
        cin >> n;
        cout << "Enter width:" << endl;
        cin >> w;
        bool wm = false;
        cout << "Enter num of threads to run on:" << endl;
        cin >> num_of_threads;
        print_time();
        cout << count_rect(w,n,wm,num_of_threads) << endl;
        print_time();
    }
}

int main(){
    get_input_and_run();
    cout << "Total sigs: " << sigs;
//    global_n = 10;
//    GenFunc gf;
//    unsigned long long x = 1 << 31;
//    x <<= 31;
//    cout << x << endl;
//    u128_addable x128 = x;
//    x128.add(x);
//    x128.add(x);
//    x128.add(x);
//    x128.add(x);
//    x128.add(x);
//    x128.add(x);
//    x128.add(x);
//    cout << x128 << endl;
//    gf.set_at(3, x128);
//    gf.pack();
//    for (int i = 0; i < 6; ++i) {
//        cout << gf.at(i) << ", ";
//    }
//    gf.unpack();
//    cout << endl;
    return 0;
}
