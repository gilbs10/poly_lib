#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "genfunc.h"
#include "RectManager.h"
#include "settings.h"
#include <omp.h>
#include <iomanip>

using namespace std;

long long sigs = 0;
int global_n; //Allows us to save presious memory in each GenFunc.
#define ALIGN_TAB "\t"
#define FORMAT_ATTR(a,b) " " << (a) <<": "<< (b) << ALIGN_TAB
#define FORMAT_TITLE(a) get_time() << " <" << (a) << ">" << ALIGN_TAB
/*Count the number of polyominoes using transfer matrix algorithm on the diagonal.
 * The code should be highly optimize, this sometime will be instead of readability and making the generic and useful*/

string get_time(){
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    std::string t( ctime( &timenow ) );
    return t.substr( 0, t.length() -1  ) + " ";
}


gf_type count_rect(int w, int n, bool wm, int num_of_threads){
    gf_type c = 0;
    global_n = n;

    cout << FORMAT_TITLE("RUNNING RECT");
    cout << FORMAT_ATTR("Size", n);
    cout << FORMAT_ATTR("Width", w);
    cout << FORMAT_ATTR("White mode", wm);
    cout << endl;
//    RectManager* rm = new RectManager(w, n, bool(wm));
    RectManagerParallel* rm = new RectManagerParallel(w, n, bool(wm), num_of_threads);
    rm->run_rectangle();
    for (int j = w; j <= n; ++j) {
        if (rm->res->find(j) != rm->res->end()){
            c += (*rm->res)[j]->at(n);
            if(j > w){
                if(PRINT_RES_BY_COL){
                    cout << FORMAT_TITLE("RESULT WIDTH COL");
                    cout << FORMAT_ATTR("Size", n);
                    cout << FORMAT_ATTR("Width", w);
                    cout << FORMAT_ATTR("Column", j);
                    cout << FORMAT_ATTR("White mode", wm);
                    cout << FORMAT_ATTR("Count", (*rm->res)[j]->at(n));
                    cout << endl;
                }
                c += (*rm->res)[j]->at(n);
            }
        }
    }
    cout << FORMAT_TITLE("SIGS RECT");
    cout << FORMAT_ATTR("Size", n);
    cout << FORMAT_ATTR("Width", w);
    cout << FORMAT_ATTR("White mode", wm);
    cout << FORMAT_ATTR("Sigs", rm->sig_counter);
    cout << endl;
    sigs += rm->sig_counter;
    cout << FORMAT_TITLE("RESULT RECT");
    cout << FORMAT_ATTR("Size", n);
    cout << FORMAT_ATTR("Width", w);
    cout << FORMAT_ATTR("White mode", wm);
    cout << FORMAT_ATTR("Count", c);
    cout << endl;
    delete rm;
    return c;
}

void count(int n, int num_of_threads){
    sigs = 0;
    gf_type c = 0;
    global_n = n;
    cout << FORMAT_TITLE("RUNNING SIZE");
    cout << FORMAT_ATTR("Size", n);
    cout << endl;
    for (int wm = 0; wm < 2; ++wm) {
        for (int k = 2; k < n+1; ++k) {
            c += count_rect(k, n, wm, num_of_threads);
        }
    }

    cout << FORMAT_TITLE("SIGS SIZE");
    cout << FORMAT_ATTR("Size", n);
    cout << FORMAT_ATTR("Sigs", sigs);
    cout << endl;
    cout << FORMAT_TITLE("RESULT SIZE");
    cout << FORMAT_ATTR("Size", n);
    cout << FORMAT_ATTR("Count", c);
    cout << endl;
}


void run_all(int n1, int n2, int num_of_threads = 1) {
    for (int i = n1; i <= n2; ++i){
        count(i, num_of_threads);
    }
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
//        print_time();
        cout << count_rect(w,n,wm,num_of_threads) << endl;
//        print_time();
    }
}

int main(){
    get_input_and_run();
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
