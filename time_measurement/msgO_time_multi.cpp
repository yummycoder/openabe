#include <iostream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openabe/openabe.h>
#include <openabe/zsymcrypto.h>
#include <fstream>
#include <cassert>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <x86intrin.h>

using namespace std;
using namespace std::chrono;
using namespace oabe;
using namespace oabe::crypto;


std::string genRandomString(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

// main function take 3 args as input, attr_num, attr_length, and message_length
int main(int argc, char* argv[])
{
    // let program run on single core
    cpu_set_t my_set;        
    CPU_ZERO(&my_set);       
    CPU_SET(3, &my_set);     
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set); 
    srand(time(0));
 
    int attr_num = atoi(argv[1]);
    int attr_length = atoi(argv[2]);
    int message_length = atoi(argv[3]);
    
    
    string message = genRandomString(message_length);
  
    string attrs[attr_num];

    std::ofstream fout("../result/msgO_time_multi(attrLen:" + to_string(attr_length) + "msgLen:" + to_string(message_length) + ").txt", std::ios_base::app);
    
    if (attr_num == 1) fout << "# attrNum\t" << "PKE(milliseconds)" << endl;

    InitializeOpenABE();
    OpenPKEContext pkScheme("NIST_P256", false);
    
    //generate attributes
    for (int i = 0; i < attr_num; i++) {
        attrs[i] = genRandomString(attr_length);
        pkScheme.keygen(attrs[i]);
    }

    //cipher text
    string ct;

    // decrypt message
    string dec;

    int iteration = 100;
    
    pkScheme.multiEncrypt(attrs[0], attrs, attr_num, message, ct);

    //warmup
    pkScheme.multiDecrypt(attrs[0], attrs[attr_num-1], ct, dec);
    
    int count = 0;
    // Measure time   
    auto start = steady_clock::now();
    for (int i = 0; i < iteration; i++){
        pkScheme.multiDecrypt(attrs[0], attrs[attr_num-1], ct, dec);        
    }
    auto stop = steady_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    fout << attr_num << "\t" << float(duration.count() / iteration) / 1000 << endl;

    ShutdownOpenABE();

    return 0;
        
}