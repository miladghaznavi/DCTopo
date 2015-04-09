//
//  substrategraph.cpp
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#include "substrategraph.h"
using namespace Embedding;

namespace {
    ResourceCollection DefaultResourceCollection () {
        return ResourceCollection();
    }
}//namespace

void SubstrateNode::_init(ResourceCollection capacity, bool isHost) {
    _allocated = DefaultResourceCollection();
    _isHost   = isHost;
    _capacity = capacity;
}

SubstrateNode::SubstrateNode(bool isHost) {
    _init(DefaultResourceCollection(), isHost);
}

SubstrateNode::SubstrateNode (ResourceCollection capacity, bool isHost) {
    _init(capacity, isHost);
}

ResourceCollection SubstrateNode::capacity() {
    return _capacity;
}

double SubstrateNode::capacity(Resource r) {
    return _capacity[r];
}

void SubstrateNode::capacity(ResourceCollection rc) {
    _capacity = rc;
}

void SubstrateNode::capacity(Resource r, double amount) {
    _capacity[r] = amount;
}

void SubstrateNode::allocate(ResourceCollection rc) {
    for (size_t r = 0; r < Resource::ResourceCount; r++)
        SubstrateNode::allocate((Resource)r, rc[(Resource)r]);
}

void SubstrateNode::allocate(Resource r, double amount) {
    if (amount < 0)
        throw std::invalid_argument("The negative amount of resource cannot be allocated!");
    else if (amount > _capacity[r] - _allocated[r])
        throw std::invalid_argument("There is no sufficient resource to allocate!");
    
    _allocated[r] += amount;
}

void SubstrateNode::release(Resource r, double amount) {
    if (amount < 0)
        throw std::invalid_argument("The negative amount of resource cannot be released!");
    else if (amount > _allocated[r])
        throw std::invalid_argument("Release amount cannot be greater allocted!");

    _allocated[r] -= amount;
}

void SubstrateNode::release(ResourceCollection rc) {
    for (size_t r = 0; r < Resource::ResourceCount; r++)
        SubstrateNode::release((Resource)r, rc[(Resource)r]);
}

double SubstrateNode::allocated(Resource r) {
    return _allocated[r];
}

double SubstrateNode::residualCapacity(Resource r) {
    return _capacity[r] - _allocated[r];
}

ResourceCollection SubstrateNode::residualCapacity() {
    return _capacity - _allocated;
}

bool SubstrateNode::isHost() {
    return _isHost;
}

void SubstrateNode::isHost(bool isHost){
    _isHost = isHost;
}

double SubstrateNode::utilization(Resource r) {
    return allocated(r) / capacity(r);
}

void SubstrateNode::out_(ostream& os) {
    auto p = os.tellp();
    Node::out_(os);
    if (os.tellp() != p)
        os << " ";
    ResourceCollection tmp = capacity();
    os << tmp;
    
    if (isHost())
        os << " " << true;
}

void SubstrateNode::in_ (istream& is) {
    Node::in_(is);
    
    ResourceCollection tmp;
    is >> tmp;
    capacity(tmp);
    
    char c = is.get();
    if (c == '\n')
        is.unget();
    else if (c == ' ')
        is >> _isHost;
}

Embedding::JsonValue SubstrateNode::Json() {
    JsonValue re = this->Node::Json();
    re[SUBSTRATE_NODE_CAP] = capacity().Json();
    re[SUBSTRATE_NODE_IS_HOST] = (bool)isHost();
    
    return re;
}

void SubstrateNode::parseJson(Embedding::JsonValue json) {
    Node::parseJson(json);
    _capacity.parseJson(json[SUBSTRATE_NODE_CAP]);
    isHost(json[SUBSTRATE_NODE_IS_HOST].asBool());
}

SubstrateEdge::SubstrateEdge(Identifier source, Identifier dest, double capacity, double latency) : Edge(source, dest) {
    if (_capacity < 0.0)
        throw invalid_argument("Capacity of an edge cannot be negative!");
    if (_latency < 0.0)
        throw invalid_argument("Latency of an edge cannot be negative!");
    
    this->_allocated = 0;
    this->capacity(capacity);
    this->latency(latency);
}

SubstrateEdge::SubstrateEdge(double capacity, double latency) : SubstrateEdge(INVALID_ID, INVALID_ID, capacity, latency) { }

double SubstrateEdge::capacity() {
    return _capacity;
}

void   SubstrateEdge::capacity(double amount) {
    if (amount < _allocated)
        throw invalid_argument("Cannot reduce the capacity to lower than what is already allocted!");
    _capacity = amount;
}

void SubstrateEdge::allocate(double amount) {
    if (amount < 0)
        throw std::invalid_argument("The negative amount of edge capacity cannot be allocated!");
    else if (amount > _capacity - _allocated)
        throw std::invalid_argument("There is no sufficient capacity to allocate!");
    
    _allocated += amount;
}

void SubstrateEdge::release(double amount) {
    if (amount < 0)
        throw std::invalid_argument("The negative amount of edge capacity cannot be released!");
    else if (amount - _allocated > PRECISION_IN_RELEASE){
        stringstream ss;
        ss << "Allocated resource is less than release amount!\n";
        ss << _allocated << " < " << amount << " for edge " << this->identifier() << endl;
        
        throw std::invalid_argument(ss.str());
    }//else if
    
    this->_allocated -= amount;
}

double SubstrateEdge::allocated() {
    return _allocated;
}

double SubstrateEdge::residualCapacity() {
    return _capacity - _allocated;
}

double SubstrateEdge::latency() {
    return _latency;
}

void   SubstrateEdge::latency(double latency) {
    if (latency < 0)
        throw invalid_argument("Latency cannot be negative!");
    _latency = latency;
}

double SubstrateEdge::utilization() {
    return allocated() / capacity();
}

void SubstrateEdge::out_(ostream& os) {
    auto pos = os.tellp();
    Edge::out_(os);
    if (os.tellp() != pos)
        os << " ";
    
    os << _capacity << " " << _latency;
}

void SubstrateEdge::in_ (istream& is) {
    Edge::in_(is);
    is >> _capacity >> _latency;
}

Embedding::JsonValue SubstrateEdge::Json() {
    JsonValue re = this->Edge::Json();
    re[SUBSTRATE_EDGE_CAP] = capacity();
    re[SUBSTRATE_EDGE_LATENCY] = latency();
    return re;
}

void SubstrateEdge::parseJson(Embedding::JsonValue json) {
    this->Edge::parseJson(json);
    capacity(json[SUBSTRATE_EDGE_CAP].asDouble());
    latency (json[SUBSTRATE_EDGE_LATENCY].asDouble());
}

SubstrateGraph::SubstrateGraph() {
    this->Graph<SubstrateNode, SubstrateEdge>::header_(SUBSTRATE_GRAPH_HEADER);
}

string SubstrateGraph::header_() {
    return SUBSTRATE_GRAPH_HEADER;
}

vector<SubstrateNode> SubstrateGraph::hostMachines() {
    vector<SubstrateNode> re;
    re.reserve(nodesCount());
    
    for (auto n: nodes_)
        if (n.isHost())
            re.push_back(n);
    
    return re;
}

vector<Identifier> SubstrateGraph::hostMachineIds() {
    vector<Identifier> re;
    re.reserve(nodesCount());
    
    for (auto n: nodes_)
        if (n.isHost())
            re.push_back(n.identifier());
    
    return re;
}

vector<SubstrateNode> SubstrateGraph::otherMachines  () {
    vector<SubstrateNode> re;
    re.reserve(nodesCount());
    
    for (auto n: nodes_)
        if (!n.isHost())
            re.push_back(n);
    
    return re;
}

vector<Identifier> SubstrateGraph::otherMachineIds() {
    vector<Identifier> re;
    re.reserve(nodesCount());
    
    for (auto n: nodes_)
        if (!n.isHost())
            re.push_back(n.identifier());
    
    return re;
}

void SubstrateGraph::allocate(Identifier nodeId, Resource r, double d) {
    nodes_[nodeId].allocate(r, d);
}

void SubstrateGraph::allocate(Identifier nodeId, ResourceCollection D) {
    nodes_[nodeId].allocate(D);
}

void SubstrateGraph::allocate(Identifier edgeId, double d) {
    edges_[edgeId].allocate(d);
}

void SubstrateGraph::release(Identifier nodeId, ResourceCollection D) {
    nodes_[nodeId].release(D);
}

void SubstrateGraph::release(Identifier nodeId, Resource r, double d) {
    nodes_[nodeId].release(r, d);
}

void SubstrateGraph::release(Identifier edgeId, double d) {
    edges_[edgeId].release(d);
}

double SubstrateGraph::nodesUtilization(Embedding::Resource r) {
    double re = 0;
    for (auto n: nodes_)
        re += n.utilization(r);
    re /= (double)nodes_.size();
    return re;
}

double SubstrateGraph::edgesUtilization() {
    double re = 0;
    for (auto e: edges_)
        re += e.utilization();
    re /= (double)edges_.size();
    return re;
}

