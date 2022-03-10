#include <iostream>
#include <unordered_map>
#include <vector>
#include "data_types.h"
#include "genfunc.h"
#include "RectManager.h"
#include "settings.h"
#include <omp.h>

using namespace std;

long long sigs = 0;
int global_n; //Allows us to save presious memory in each GenFunc.
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
#ifdef LIMIT_SIG_64
    if(w >= 52){
        cout << FORMAT_TITLE("WIDTH TOO WIDE");
        cout << FORMAT_ATTR("Size", n);
        cout << FORMAT_ATTR("Width", w);
        cout << FORMAT_ATTR("White mode", wm);
        cout << endl;
        return 0;
    }
#endif
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
            if(PRINT_RES_BY_COL){
                for (int i = 1; i <= n; ++i) {
                    cout << FORMAT_TITLE("RESULT WIDTH COL");
                    cout << FORMAT_ATTR("Run_Size", n);
                    cout << FORMAT_ATTR("Res_Size", i);
                    cout << FORMAT_ATTR("Width", w);
                    cout << FORMAT_ATTR("Column", j);
                    cout << FORMAT_ATTR("White mode", wm);
                    cout << FORMAT_ATTR("Count", (*rm->res)[j]->at(i));
                    cout << endl;
                }
            }
            if(j > w){
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
    gf_type res;
    cout << FORMAT_TITLE("RUNNING SIZE");
    cout << FORMAT_ATTR("Size", n);
    cout << endl;
    for (int w = 2; w < n+1; ++w) {
        if(w%2 == 0){
            res = count_rect(w, n, false, num_of_threads);
            c += res;
            c += res;
        } else {
            for (int wm = 0; wm < 2; ++wm) {
                c += count_rect(w, n, wm, num_of_threads);
            }
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
        cout << "White mode? (y/n):" << endl;
        char wm;
        cin >> wm;
        cout << "Enter num of threads to run on:" << endl;
        cin >> num_of_threads;
//        print_time();
        cout << count_rect(w,n,wm == 'y',num_of_threads) << endl;
//        print_time();
    }
}

int main(){
    int cases;
    cout << "Enter running cases:" << endl;
    cin >> cases;
    for (int i = 0; i < cases; ++i) {
        get_input_and_run();
    }
//    SigDict sd();
//    sig sig_num = 6;
//    GenFunc gf();
//    gf.at

    return 0;
}
