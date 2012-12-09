#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<boost/tokenizer.hpp>
#include<cassert>
using namespace std;

//data strcuture for an edge
struct edge{
    int node;
    int weight;
};

class graph{
    vector< vector<struct edge> > grph;
    int no_nodes, no_edges;
    public:
        graph(char *filename);
        void graph_check();
};

void graph::graph_check(){
    cout<<grph.size()<<"=="<<this->no_nodes<<endl;
    assert(grph.size()==this->no_nodes);
    int size=0;
    for(int i=0; i<grph.size(); i++){
        size = size+grph[i].size();
    }
    cout<<size<<" == "<<this->no_edges<<endl;
    //assert(size/2 == no_edges);
}

graph::graph(char* filename){
    ifstream myfile(filename);
    myfile >> this->no_nodes >> this->no_edges;
    vector<struct edge> temp;
    for(int i=0; i<this->no_nodes;i ++){
        grph.push_back(temp);
    }
    string str;
    int node_no = 0;
    while(getline(myfile, str)){
        istringstream edges(str);
        string edge;
        while(edges >> edge){
            boost::tokenizer<> tok(edge);
            vector<int> temp;
            for(boost::tokenizer<>::iterator beg = tok.begin(); beg!=tok.end(); ++beg){
                string st1 = *beg;
                temp.push_back(atoi(st1.c_str()));
            }
            assert(temp.size()==2);
            struct edge dir_edge;
            dir_edge.node=temp[0];
            dir_edge.weight=temp[1];
            grph[node_no].push_back(dir_edge);
        }
        node_no++;
    }
    this->graph_check();
}
