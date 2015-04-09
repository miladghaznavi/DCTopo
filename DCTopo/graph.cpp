//
//  graph.cpp
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#include "graph.h"
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
using namespace Embedding;

#define PRINT_PRECISION 2

void Edge::out_(ostream& os) {
    os << _src << " " << _dst;
}

void Edge::in_ (istream& is) {
    is >> _src >> _dst;
}

Edge::Edge(Identifier source, Identifier destination) {
    _src = source;
    _dst = destination;
}

Edge::Edge() : Edge(INVALID_ID, INVALID_ID) { }

Embedding::Identifier Edge::source() {
    return _src;
}

void Edge::source(Identifier source) {
    _src = source;
}

Identifier Edge::destination() {
    return _dst;
}

void Edge::destination(Identifier destination) {
    _dst = destination;
}

Identifier Edge::otherEndpoint(Identifier id) {
    Identifier re;
    if (id == _dst){
        re = _src;
    }//if
    else {
        re = _dst;
    }//else if
    
    return re;
}

JsonValue Edge::Json() {
    JsonValue re = this->BaseClass::Json();
    re[SOURCE_NODE] = source();
    re[DEST_NODE]   = destination();
    
    return re;
}

void Edge::parseJson(Embedding::JsonValue json) {
    BaseClass::parseJson(json);
    this->source(json[SOURCE_NODE].asInt64());
    this->destination(json[DEST_NODE].asInt64());
}
