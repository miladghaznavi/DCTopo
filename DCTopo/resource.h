//
//  resource.h
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 4/1/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#ifndef __ChainEmbedding__resource__
#define __ChainEmbedding__resource__

#ifndef __ChainEmbedding__basics__
#include "basics.h"
#endif

namespace Embedding {
    enum Resource {
        CPU,
        Memory,
        Storage,
        
        ResourceCount // Don't remove this line
    };//enum
    
    namespace {
        #define CPU_STR "cpu"
        #define MEMORY  "memory"
        #define STORAGE "storage"
    }//namesoace
    
    namespace Coeff {
        const double RESOURCE_COEFFITIENT[Resource::ResourceCount] = { 1, 1, 1 };
        const double BANDWIDTH_COEFFITIENT                         = 1;
    }//namespace
}

struct ResourceCollection: public BaseClass {
    double cpu;
    double memory;
    double storage;
    
private:
    void in_ (istream&);
    void out_(ostream&);
    
public:
    ResourceCollection             (double = 0, double = 0, double = 0);
    Embedding::JsonValue Json      ();
    void                 parseJson (Embedding::JsonValue);
    void                 resource  (Embedding::Resource, double);
    double               resource  (Embedding::Resource);
    double&              operator[](Embedding::Resource);
    
    friend ResourceCollection operator+(ResourceCollection&, ResourceCollection&);
    friend ResourceCollection operator-(ResourceCollection&, ResourceCollection&);
};


#endif /* defined(__ChainEmbedding__resource__) */
