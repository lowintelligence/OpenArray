#ifndef __JIT_DRIVER_HPP__
#define __JIT_DRIVER_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <functional>
#include "Jit.hpp"
#include <vector>
typedef std::shared_ptr<Jit> JitPtr;

using namespace std;

typedef void fusion_kernel_rawptr (void**&, int);
typedef void (*kernel_func)(void**&, int);
typedef std::function<fusion_kernel_rawptr> FusionKernelPtr;

typedef unordered_map<size_t, FusionKernelPtr> FKPtrMap;
typedef unordered_map<size_t, JitPtr> JitPoolMap;

class Jit_Driver{
private:
  FKPtrMap kernel_dict;
  JitPoolMap m_jit_pool;

public:
  FusionKernelPtr get(size_t hash) {
    if (kernel_dict.find(hash) == kernel_dict.end()) return NULL;
    return kernel_dict[hash];
  }

  void insert(size_t hash, const stringstream& code) {
    FusionKernelPtr fk_ptr = get(hash);
    if (fk_ptr != NULL) return ;

    std::vector<string> opvs;

    opvs.push_back("-O3");
    opvs.push_back("-Ofast");
    opvs.push_back("-ffast-math");
    opvs.push_back("-march=core-avx2");
    opvs.push_back("-m64");
    char **fake_argv = new char*[opvs.size()];

    for (int i = 0; i < opvs.size(); i++) {
        fake_argv[i] = new char[256];
        strcpy(fake_argv[i], opvs[i].c_str());
    }
    
    stringstream ss;
    ss<<"kernel_"<<hash;
    
    const string& scode = code.str();  
    const char* cccode = scode.c_str(); 
    const string& sname = ss.str();  
    const char* ccname = sname.c_str(); 
    
    char *ccode = new char[strlen(cccode)+1];
    char *cname = new char[strlen(ccname)+1];
    strcpy(ccode, cccode);
    strcpy(cname, ccname);

    JitPtr jit_ptr = JitPtr(new Jit(opvs.size(), fake_argv, cname, ccode));
    m_jit_pool[hash] = jit_ptr;

    uint64_t Entry = jit_ptr->compile();
    
    fk_ptr = (kernel_func)Entry;
    kernel_dict[hash] = fk_ptr;

    delete []ccode;
    delete []cname;
    for (int i = 0; i < opvs.size(); i++) delete[] fake_argv[i];
    delete[] fake_argv;

    return ;
  }

  static Jit_Driver* global() {
    static Jit_Driver jit;
    return &jit;
  }
};

#endif
