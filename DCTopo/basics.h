//
//  basics.h
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#ifndef __ChainEmbedding__basics__
#define __ChainEmbedding__basics__

#include <vector>

#include <fstream>
#include <stdexcept>

#include "json/json.h"
#include "json/writer.h"
using namespace std;

namespace Embedding {
    #define INVALID_ID -1
    #define BYTE        8
    
    namespace {
        #define IDENTIFIER "id"
    }
    
    typedef unsigned long long Weight;
    typedef long long          Identifier;
    typedef double             Information;
    typedef Json::Value        JsonValue;
    typedef Json::Reader       JsonReader;
    typedef Json::StyledWriter JsonWriter;
}

class BaseClass {
private:
    static  Embedding::Identifier _idCount;
    Embedding::Identifier         _identifier;
    
protected:
    virtual void in_ (istream&);
    virtual void out_(ostream&);
    
    static ofstream& createFile_(string);
    static ifstream& openFile_  (string);
    
public:
    BaseClass                              ();
    BaseClass                              (Embedding::Identifier);
    Embedding::Identifier        identifier();
    void                         identifier(Embedding::Identifier identifier);
    virtual Embedding::JsonValue Json      ();
    virtual void                 parseJson (Embedding::JsonValue);
    
    virtual void readFromStream    (istream&);
    virtual void writeToStream     (ostream&);
    virtual void readFromJsonStream(istream&);
    virtual void writeToJsonStream (ostream&);
    
    friend ostream& operator<<(ostream&, BaseClass&);
    friend istream& operator>>(istream&, BaseClass&);
};

#endif /* defined(__ChainEmbedding__basics__) */
