#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "genfunc.h"
#include "RectManager.h"
#include <omp.h>


using namespace std;

/*Count the number of polyominoes using transfer matrix algorithm on the diagonal.
 * The code should be highly optimize, this sometime will be instead of readability and making the generic and useful*/

void print_time(){
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout <<  "Time: " << ctime(&timenow) << flush;
}

u128_addable count_rect(int w, int n, bool wm, int num_of_threads){
    u128_addable c = 0;
//    RectManager* rm = new RectManager(w, n, bool(wm));
    RectManagerParallel* rm = new RectManagerParallel(w, n, bool(wm), num_of_threads);
    rm->run_rectangle();
    for (int j = w; j <= n; ++j) {
        if (rm->res->find(j) != rm->res->end()){
            c += (*rm->res)[j]->g_func[n];
            if(j > w){
                c += (*rm->res)[j]->g_func[n];
            }
        }
    }
    delete rm;
    return c;
}

void count(int n, int num_of_threads){
    u128_addable c = 0;
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

int main(){
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
    return 0;
}
