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

void count(int n){
    u128_addable c = 0;
    for (int wm = 0; wm < 2; ++wm) {
        for (int k = 2; k < n+1; ++k) {
//           RectManager* rm = new RectManager(k, n, bool(wm));
             RectManagerParallel* rm = new RectManagerParallel(k, n, bool(wm), 1);
            rm->run_rectangle();
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



void run_all() {
    for (int i = 3; i <= 20; ++i) {
        auto timenow =
                chrono::system_clock::to_time_t(chrono::system_clock::now());
        cout << "Running " << i << ", time: " << ctime(&timenow) << endl;
        count(i);
    }
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << ctime(&timenow) << endl;
}

int main(){
    run_all();
//    int k = 20;
//    int n = 20;
//    bool wm = false;
//    RectManagerParallel* rm = new RectManagerParallel(k, n, bool(wm), 4);
//    rm->run_rectangle();

    return 0;
}