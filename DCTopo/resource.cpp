//
//  resource.cpp
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 4/1/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#include "resource.h"
#include <stdexcept>

void ResourceCollection::in_ (istream& is) {
    is >> cpu >> memory >> storage;
}
void ResourceCollection::out_(ostream& os) {
    os << cpu << " " << memory << " " << storage;
}

ResourceCollection::ResourceCollection(double cpu, double memory, double storage) {
    this->cpu = cpu;
    this->memory = memory;
    this->storage = storage;
}

Embedding::JsonValue ResourceCollection::Json() {
    Embedding::JsonValue re;
    re[CPU_STR] = cpu;
    re[MEMORY ] = memory;
    re[STORAGE] = storage;
    return re;
}

void ResourceCollection::parseJson(Embedding::JsonValue json) {
    cpu     = json[CPU_STR].asDouble();
    memory  = json[MEMORY ].asDouble();
    storage = json[STORAGE].asDouble();
}

void ResourceCollection::resource (Embedding::Resource r, double d) {
    (*this)[r] = d;
}

double ResourceCollection::resource (Embedding::Resource r) {
    return this->operator[](r);
}

double& ResourceCollection::operator[](Embedding::Resource r) {
    double* re = NULL;
    switch (r) {
        case Embedding::Resource::Memory:
            re = &memory;
            break;
        case Embedding::Resource::CPU:
            re = &cpu;
            break;
        case Embedding::Resource::Storage:
            re = &storage;
            break;
        default:
            break;
    }//switch
    
    if (re == NULL || (*re) < 0) {
        throw invalid_argument("Cannot assign negative value to resource!");
    }//if
    
    return *re;
}

ResourceCollection operator+(ResourceCollection& rc1, ResourceCollection& rc2) {
    ResourceCollection re;
    re.cpu      = rc1.cpu + rc2.cpu;
    re.memory   = rc1.memory + rc2.memory;
    re.storage  = rc1.storage + rc2.storage;
    
    return re;
}

ResourceCollection operator-(ResourceCollection& rc1, ResourceCollection& rc2) {
    ResourceCollection re;
    re.cpu      = rc1.cpu - rc2.cpu;
    re.memory   = rc1.memory - rc2.memory;
    re.storage  = rc1.storage - rc2.storage;
    
    return re;
}
