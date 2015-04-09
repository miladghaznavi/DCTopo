//
//  graph.h
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#ifndef __ChainEmbedding__graph__
#define __ChainEmbedding__graph__

#ifndef __ChainEmbedding__basics__
#include "basics.h"
#endif

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <map>
#include "json/json.h"
using namespace std;

namespace {
    #define GRAPH        "graph"
    
    #define IDENTIFIER   "id"
    #define HEADER       "header"
    #define NODES_COUNT  "nodesCount"
    #define EDGES_COUNT  "edgesCount"
    #define NODES        "nodes"
    #define EDGES        "edges"

    #define SOURCE_NODE  "source"
    #define DEST_NODE    "destination"
}//namespace

class Node : public BaseClass { };

class Edge : public BaseClass {
private:
    Embedding::Identifier _src;
    Embedding::Identifier _dst;
    
protected:
    virtual void out_(ostream&);
    virtual void in_ (istream&);
    
public:
    Edge();
    Edge(Embedding::Identifier, Embedding::Identifier);
    
    Embedding::Identifier source       ();
    void                  source       (Embedding::Identifier);
    Embedding::Identifier destination  ();
    void                  destination  (Embedding::Identifier);
    Embedding::Identifier otherEndpoint(Embedding::Identifier);
    
    virtual Embedding::JsonValue Json     ();
    virtual void                 parseJson(Embedding::JsonValue);
};

/*
    S is the type of Node. S must have two functions:
        -Identifier identifier();
        -void       identifier(Identifier identifier);
            recieve the unique id which is assigned to in graph
 
    T is the type of Edge. T must have eight functions:
        -T(identifier, identifier);
            recieve the id of source and destinaion node
        -Identifier identifier();
        -void       identifier(Identifier identifier);
            recieve the unique id which is assigned to in graph
        -Identifier source       ();
        -void       source       (Identifier);
        -Identifier destination  ();
        -void       destination  (Identifier);
        -Identifier otherEndpoint();
 */
template <typename S, typename T>
class Graph: public BaseClass {
private:
    string _header;
    
protected:
    vector<S>  nodes_;
    vector<T>  edges_;
    
    virtual string header_() {
        return _header;
    }
    
    void header_(string header) {
        _header = header;
    }
    
    virtual void in_ (istream& is) {
        // Read Header
        getline(is >> std::ws, _header);
        // Read Number of nodes and number of edges
        int nodesCount, edgesCount;
        is >> nodesCount >> edgesCount;
        
        edges_.clear();
        nodes_.clear();
        edges_.reserve(edgesCount);
        edges_.reserve(nodesCount);
        
        // Read nodes
        for (int i = 0; i < nodesCount; i++) {
            S node;
            is >> node;
            addNode(node);
        }//for
        
        // Read edges
        for (int i = 0; i < edgesCount; i++) {
            T edge;
            edge.identifier(i);
            is >> edge;
            addEdge(edge);
        }//for
    }
    
    virtual void out_(ostream& os) {
        os << header_() << endl;
        os << nodesCount() << " " << edgesCount();
        
        for (auto n : nodes_) {
            stringstream ss;
            ss << n;
            ss.seekp(0, ios::end);
            if (ss.tellp() > 0)
                os << endl << n;
        }//for
        
        for (auto e : edges_) {
            stringstream ss;
            ss << e;
            ss.seekp(0, ios::end);
            if (ss.tellp() > 0)
                os << endl << e;
        }//for
    }

public:
    Graph() {
        _header = GRAPH;
    }
    
    virtual ~Graph() {
        nodes_.clear();
        edges_.clear();
    }
    
    Embedding::Identifier addNode() {
        S newNode;
        newNode.identifier(nodes_.size());
        nodes_.push_back(newNode);
        return newNode.identifier();
    }
    
    Embedding::Identifier addNode(S newNode) {
        newNode.identifier(nodes_.size());
        nodes_.push_back(newNode);
        return newNode.identifier();
    }
    
    Embedding::Identifier addEdge(Embedding::Identifier src, Embedding::Identifier dst) {
        T edge(src, dst);
        edge.identifier(edges_.size());
        edges_.push_back(edge);
        
        return edge.identifier();
    }
    
    Embedding::Identifier addEdge(T edge) {
        if (edge.source() >= nodes_.size() || edge.destination() >= nodes_.size())
            throw invalid_argument("Invalid edge!");
        
        edge.identifier(edges_.size());
        edges_.push_back(edge);
        return edge.identifier();
    }
    
    size_t edgesCount() {
        return edges_.size();
    }
    
    size_t nodesCount() {
        return nodes_.size();
    }
    
    S node(Embedding::Identifier identifier) {
        return nodes_[identifier];
    }
    
    vector<S> nodes() {
        vector<S> re;
        for (auto n : nodes_)
            re.push_back(n);
        return re;
    }
    
    vector<Embedding::Identifier> nodesIds() {
        vector<Embedding::Identifier> re;
        for(auto v : nodes_)
            re.push_back(v.identifier());
        
        return re;
    }
    
    T edge(Embedding::Identifier edgeId) {
        return edges_[edgeId];
    }
    
    T edge(Embedding::Identifier source, Embedding::Identifier destination) {
        for (auto e : edges_)
            if (e.source() == source && e.destination() == destination)
                return e;
        throw invalid_argument("There is no edge with the source and destination nodes Ids!");
    }
    
    T edge(Embedding::Identifier source, Embedding::Identifier destination, bool undirected) {
        for (auto e : edges_) {
            if (e.source() == source && e.destination() == destination) {
                return e;
            }//if
            else if (undirected && (
                e.source() == source || e.source() == destination) &&
                (e.destination() == source || e.destination() == destination)) {
                return e;
            }//else if
        }//for
        throw invalid_argument("There is no edge with the source and destination nodes Ids!");
    }
    
    vector<T> nodeEdges(Embedding::Identifier nodeId) {
        vector<T> re;
        
        for (auto e : edges_)
            if (e.source() == nodeId || e.destination() == nodeId)
                re.push_back(e);
        
        return re;
    }
    
    vector<T> nodeInEdges(Embedding::Identifier nodeId) {
        vector<T> re;
        for (auto e : edges_)
            if (e.destination() == nodeId)
                re.push_back(e);
        return re;
    }
    
    vector<Embedding::Identifier> nodeInEdgesIds(Embedding::Identifier nodeId) {
        vector<Embedding::Identifier> re;
        for (auto e : edges_)
            if (e.destination() == nodeId)
                re.push_back(e.identifier());
        return re;
    }
    
    vector<T> nodeOutEdges(Embedding::Identifier nodeId) {
        vector<T> re;
        for (auto e : edges_)
            if (e.source() == nodeId)
                re.push_back(e);
        return re;
    }
    
    vector<Embedding::Identifier> nodeOutEdgesIds(Embedding::Identifier nodeId) {
        vector<Embedding::Identifier> re;
        for (auto e : edges_)
            if (e.source() == nodeId)
                re.push_back(e.identifier());
        return re;
    }
    
    vector<Embedding::Identifier> nodeNighbors(Embedding::Identifier nodeId) {
        vector<T> edges = nodeEdges(nodeId);
        vector<Embedding::Identifier> re;
        
        for (size_t i = 0; i < edges.size(); i++)
            re.push_back(edges[i].otherEndPoint(nodeId));
        
        return re;
    }
    
    vector<vector<Embedding::Identifier> > nodesNighbors() {
        vector<vector<Embedding::Identifier> > re(nodesCount());
        
        for (auto e : edges_) {
            re[e.source()     ].push_back(e.destination());
            re[e.destination()].push_back(e.source());
        }//for
        
        return re;
    }
    
    vector<T> edges() {
        vector<T> re;
        for (auto e : edges_)
            re.push_back(e);
        return re;
    }
    
    vector<Embedding::Identifier> edgesIds() {
        vector<T> re;
        for (auto e : edges_)
            re.push_back(e.identifier());
        return re;
    }
    
    vector<vector<T> > adjancyMatrix() {
        vector<vector<T> > re(nodesCount());
        
        for (auto e : edges_) {
            re[e.source()     ].push_back(e);
            re[e.destination()].push_back(e);
        }//for
        
        return re;
    }
    
    Graph<S,T> clone() {
        Graph re;
        
        for (auto n : nodes_) re.addNode(n);
        for (auto e : edges_) re.addEdge(e);
        
        return re;
    }
    
    void merge(const Graph<S,T>& g) {
        size_t count = nodes_.size();
        
        for (auto n : g.nodes_) {
            this->addNode(n);
        }//for
        
        for (size_t i = 0; i < g.edges_.size(); i++) {
            T e = g.edges_[i];
            e.source(e.source() + count);
            e.destination(e.destination() + count);
            this->addEdge(e);
        }//for
    }
    
    void writeToFile(string path) {
        writeToFile(*this, path);
    }
    
    static Graph<S,T> makeGraph(vector<S>& nodes, vector<T>& edges) {
        Graph g;
        for (auto n : nodes) g.addNode(n);
        for (auto e : edges) g.addEdge(e);
        
        return g;
    }
    
    // We assume that the file has the following format
    /*
     header
     #nodes #edges
     node1
     node2
     ... (#nodes times)
     edge1
     edge2
     ... (#edges times)
     */
    
    template <typename G>
    static G readFromFile(string path) {
        ifstream& inputFile = BaseClass::openFile_(path.c_str());
        G re;
        re.readFromStream(inputFile);
        inputFile.close();
        return re;
    }
    
    // The output file will have the following format
    /*
     header
     #nodes #edges
     node1
     node2
     ... (#nodes times)
     edge1
     edge2
     ... (#edges times)
     */
    static void writeToFile (Graph<S,T>& graph, string path) {
        ofstream& outputfile = BaseClass::createFile_(path);
        graph.BaseClass::writeToStream(outputfile);
        outputfile.close();
    }
    
    static Graph<S,T> merge(const Graph<S,T>& g1, const Graph<S,T>& g2) {
        Graph re;
        re.merge(g1);
        re.merge(g2);
        
        return re;
    }
    
    // edge's source and destination must be in the g1 and g2, respectively.
    static Graph<S,T> merge(const Graph<S,T>& g1, const Graph<S,T>& g2, T& edge) {
        Graph re = merge(g1, g2);
        edge.destination(edge.destination() + g1.nodesCount());
        
        re.addEdge(edge);
        
        return re;
    }
    
    // edgees' sourcees and destinations must be in the g1 and g2, respectively.
    static Graph<S,T> merge(const Graph<S,T>& g1, const Graph<S,T>& g2, vector<T>& edges) {
        Graph re = merge(g1, g2);
        
        size_t g1nodesCount = g1.nodesCount();
        for (size_t i = 0; i < edges.size(); i++) {
            edges[i].destination(edges[i].destination() + g1nodesCount);
            re.addEdge(edges[i]);
        }//for
        
        return re;
    }
    
    template <typename G>
    static G readFromJSONFile(string path){
        ifstream& file = BaseClass::openFile_(path.c_str());
        G re;
        re.readFromJsonStream(file);
        file.close();
        return re;
    }
    
    void writeToJSONFile(string path) {
        ofstream& file = BaseClass::createFile_(path.c_str());
        writeToJsonStream(file);
        file.close();
    }
    
    virtual Embedding::JsonValue Json() {
        Embedding::JsonValue graphJson = this->BaseClass::Json();
        
        graphJson[HEADER] = header_();
        graphJson[NODES_COUNT]  = (uint64_t)nodes_.size();
        graphJson[EDGES_COUNT]  = (uint64_t)edges_.size();
        
        Embedding::JsonValue nodesJsonArr;
        for (auto n: nodes_)
            nodesJsonArr.append(n.Json());
        graphJson[NODES] = nodesJsonArr;
        
        Embedding::JsonValue edgesJsonArr;
        for (auto e: edges_)
            edgesJsonArr.append(e.Json());
        graphJson[EDGES] = edgesJsonArr;
        
        return graphJson;
    }
    
    virtual void parseJson(Embedding::JsonValue graphJson) {
        BaseClass::parseJson(graphJson);
        header_(graphJson[HEADER].asString());
        
        for (auto n: graphJson[NODES]) {
            S node;
            node.parseJson(n);
            addNode(node);
        }//for
        
        for (auto e: graphJson[EDGES]) {
            T edge;
            edge.parseJson(e);
            addEdge(edge);
        }//for
    }
};

#endif /* defined(__ChainEmbedding__graph__) */
