//
//  basics.cpp
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#include "basics.h"
#include <sstream>
#include <fstream>
#include <stdexcept>
using namespace Embedding;

Identifier BaseClass::_idCount = 0;

BaseClass::BaseClass() {
    _identifier = _idCount++;
}

BaseClass::BaseClass(Embedding::Identifier id) {
    this->identifier(id);
}

Identifier BaseClass::identifier() {
    return _identifier;
}

void BaseClass::identifier(Identifier identifier) {
    _identifier = identifier;
}

void BaseClass::in_ (istream& is) {
    is >> _identifier;
}

void BaseClass::out_(ostream& os) {
    os << _identifier;
}

ofstream& BaseClass::createFile_(string path) {
    ofstream* file = new ofstream(path.c_str());
    if (!(*file)) {
        stringstream ss;
        ss << "Unable to create file '" << path << "'!";
        throw runtime_error(ss.str());
    }//if
    
    return *file;
}

ifstream& BaseClass::openFile_(string path) {
    ifstream* file = new ifstream(path.c_str());
    if (!(*file)) {
        stringstream ss;
        ss << "Unable to open file '" << path << "'!";
        throw runtime_error(ss.str());
    }//if
    
    return *file;
}

JsonValue BaseClass::Json() {
    JsonValue re;
    re[IDENTIFIER] = _identifier;
    return re;
}

void BaseClass::parseJson(JsonValue json) {
    _identifier = json[IDENTIFIER].asInt64();
}

void BaseClass::readFromStream(istream& is) {
    in_(is);
}

void BaseClass::writeToStream(ostream& os) {
    out_(os);
}

void BaseClass::readFromJsonStream(istream& is) {
    JsonReader reader;
    JsonValue  root;
    reader.parse(is, root);
    parseJson(root);
}

void BaseClass::writeToJsonStream(ostream& os) {
    JsonWriter writer;
    os << writer.write(Json());
}

ostream& operator<<(ostream& os, BaseClass& bc) {
    bc.out_(os);
    return os;
}

istream& operator>>(istream& is, BaseClass& bc) {
    bc.in_(is);
    return is;
}
