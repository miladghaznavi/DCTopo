//
//  substrategraph.h
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#ifndef __ChainEmbedding__substrategraph__
#define __ChainEmbedding__substrategraph__

#ifndef __ChainEmbedding__graph__
#include "graph.h"
#endif

#ifndef __ChainEmbedding__basics__
#include "basics.h"
#endif

#ifndef __ChainEmbedding__resource__
#include "resource.h"
#endif


#include <map>
using namespace std;

namespace {
    #define SUBSTRATE_GRAPH_HEADER      "SubstrateGraph"
    #define SUBSTRATE_NODE_IS_HOST      "is host"
    #define SUBSTRATE_NODE_CAP          "capacity"
    #define SUBSTRATE_EDGE_CAP          "capacity"
    #define SUBSTRATE_EDGE_LATENCY      "latency"
    
    #define PRECISION_IN_RELEASE        10e-8
}//namespace

class SubstrateNode : public Node {
private:
    ResourceCollection _capacity;
    ResourceCollection _allocated;
    bool               _isHost = false; // Whehter a VNF can be installed in the node
    void _init(ResourceCollection, bool isHost);
    
protected:
    virtual void out_(ostream&);
    virtual void in_ (istream&);
    
public:
    SubstrateNode                       (bool = false);
    SubstrateNode                       (ResourceCollection, bool isHost = false);
    ResourceCollection  capacity        ();
    double          	capacity        (Embedding::Resource);
    void                capacity        (ResourceCollection);
    void                capacity        (Embedding::Resource, double);
    void                allocate        (Embedding::Resource, double);
    void                allocate        (ResourceCollection);
    void            	release         (Embedding::Resource, double);
    void            	release         (ResourceCollection);
    double              allocated       (Embedding::Resource);
    double              residualCapacity(Embedding::Resource);
    ResourceCollection  residualCapacity();
    bool                isHost          ();
    void                isHost          (bool);
    double              utilization     (Embedding::Resource);
    
    virtual Embedding::JsonValue Json     ();
    virtual void                 parseJson(Embedding::JsonValue);
};

class SubstrateEdge : public Edge {
private:
    double _capacity;
    double _allocated;
    double _latency;
    
protected:
    virtual void out_(ostream&);
    virtual void in_ (istream&);
    
public:
    SubstrateEdge(double capacity = 0, double latency = 0);
    SubstrateEdge(Embedding::Identifier src, Embedding::Identifier dst, double capacity = 0, double latency = 0);
    
    double capacity        ();
    void   capacity        (double);
    void   allocate        (double);
    void   release         (double);
    double allocated       ();
    double residualCapacity();
    double latency         ();
    void   latency         (double);
    double utilization     ();
    
    virtual Embedding::JsonValue Json     ();
    virtual void                 parseJson(Embedding::JsonValue);
};

class SubstrateGraph: public Graph<SubstrateNode, SubstrateEdge> {
protected:
    virtual string header_();
    
public:
    SubstrateGraph();
    
    // Host Machines are machines that VNFs can be installed on them
    vector<SubstrateNode>           hostMachines   ();
    vector<Embedding::Identifier>   hostMachineIds ();
    vector<SubstrateNode>           otherMachines  ();
    vector<Embedding::Identifier>   otherMachineIds();
    void                            allocate       (Embedding::Identifier, Embedding::Resource, double);
    void                            allocate       (Embedding::Identifier, ResourceCollection);
    void                            allocate       (Embedding::Identifier, double);
    void                            release        (Embedding::Identifier, Embedding::Resource, double);
    void                            release        (Embedding::Identifier, ResourceCollection);
    void                            release        (Embedding::Identifier, double);
    
    double                          nodesUtilization(Embedding::Resource r);
    double                          edgesUtilization();
};

#endif /* defined(__ChainEmbedding__substrategraph__) */
