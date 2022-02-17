#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "genfunc.h"
#include "RectManager.h"



using namespace std;

/*Count the number of polyominoes using transfer matrix algorithm on the diagonal.
 * The code should be highly optimize, this sometime will be instead of readability and making the generic and useful*/


int res_mul(int i, int j){
    if(j > i){
        return 2;
    }
    return 1;
}

void count(int n, int num_of_threads){
    u128_addable c = 0;
    int t_count = 0;
    #pragma omp parallel for
    for (int wm = 0; wm < 2; ++wm) {
        for (int k = 2; k < n+1; ++k) {
//           RectManager* rm = new RectManager(k, n, bool(wm));
             RectManagerParallel* rm = new RectManagerParallel(k, n, bool(wm), num_of_threads);
            rm->run_rectangle();
            t_count += rm->t_count;
            for (int j = k; j <= n; ++j) {
                if (rm->res->find(j) != rm->res->end()){
                    c += (*rm->res)[j]->g_func[n];
                    if(j > k){
                        c += (*rm->res)[j]->g_func[n];
                    }
                }
            }
            delete rm;
        }
    }
    cout << n << ": " << c << endl;
}

void save_res(int n){
    u128_addable c = 0;
    for (int wm = 0; wm < 2; ++wm) {
        for (int k = 2; k < n+1; ++k) {
            auto timenow =
                    chrono::system_clock::to_time_t(chrono::system_clock::now());
            cout << "Running cols " << k <<", wm="<<wm << ", time: " << ctime(&timenow) << endl;
            RectManager* rm = new RectManager(k, n, bool(wm));
            rm->run_rectangle();
            for (int i = 0; i <= n; ++i) {
                if(rm->res->find(i) != rm->res->end()){
                    cout << n << ", " << wm <<", " << k <<", " << i << ", " << (*rm->res)[i]->g_func[n] << endl;
                }
            }
            delete rm;
        }
    }
}



void run_all(int n = 10, int num_of_threads = 1) {
    for (int i = 3; i <= n; ++i){
        auto timenow =
                chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << "Running " << i << ", time: " << ctime(&timenow) << endl;
        count(i, num_of_threads);
    }
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << ctime(&timenow) << endl;
}

int main(){
    int n, num_of_threads;
    cout << "Enter size to run until:" << endl;
    cin >> n;
    cout << "Enter num of threads to run on:" << endl;
    cin >> num_of_threads;
    run_all(n, num_of_threads);
//    int k = 20;
//    int n = 20;
//    bool wm = false;
//    RectManagerParallel* rm = new RectManagerParallel(k, n, bool(wm), 4);
//    rm->run_rectangle();
    return 0;
}