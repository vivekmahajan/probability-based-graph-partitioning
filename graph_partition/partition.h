#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<boost/tokenizer.hpp>
#include<cassert>
#include<algorithm>

using namespace std;

//data strcuture for an edge
struct edge{
    int node;
    int weight;
};

bool compare_function(pair<int, float> p1, pair<int, float> p2){
    return(p1.second>p2.second);
}

class graph{
    vector< vector<struct edge> > grph;
    vector< pair<int, float> > node_weights;
    int no_nodes, no_edges;
    public:
        graph(char *filename);
        void calculate_weights();
        void graph_check();
};
void graph::calculate_weights(){

    for(int i=0; i<this->grph.size(); i++){
        float node_weight = 0.0;
        for(int j=0; j<this->grph[i].size(); j++){
            struct edge e = this->grph[i][j];
            node_weight = node_weight + e.weight;
        }
        assert(this->grph[i].size()!=0);
        node_weight = node_weight/this->grph[i].size();
        node_weights.push_back(make_pair(i, node_weight));
        assert(node_weights.size()==i+1);
    }
    //sorting the nodes by weight
    sort(node_weights.begin(), node_weights.end(), compare_function);
    for(int i=0; i<node_weights.size(); i++){
        //cout<<node_weights[i].second<<endl;

    }

}

void graph::graph_check(){
    assert(grph.size()==this->no_nodes);
    int size=0;
    for(int i=0; i<grph.size(); i++){
        size = size+grph[i].size();
    }
    assert(size/2 == no_edges);
}

graph::graph(char* filename){
    ifstream myfile(filename);
    string str;
    myfile >> this->no_nodes >> this->no_edges;
    getline(myfile, str);//understand this and fix it
    vector<struct edge> temp;
    for(int i=0; i<this->no_nodes;i ++){
        grph.push_back(temp);
    }
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
