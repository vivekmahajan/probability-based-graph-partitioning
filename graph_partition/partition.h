#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<boost/tokenizer.hpp>
#include<cassert>
#include<algorithm>
#include<map>

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
    public:
    vector< vector<struct edge> > grph;
    vector< pair<int, float> > node_weights;
    int no_nodes, no_edges;
        graph(char *filename);
        graph(int no_nodes);//constructor overloading
        void calculate_weights();
        void graph_check();
        graph* cluster_nodes();
};

graph* graph::cluster_nodes(){
    vector<int> cluster(this->no_nodes, 0);
    vector<bool> visited(this->no_nodes, 0);
    int current_cluster_no = 0;
    for(int i=0; i<this->node_weights.size(); i++){
        int current_node = this->node_weights[i].first;
        //first check if clutering that nodes if allowed or not
        if(visited[current_node] == 1){
            continue;
        }
        //else going through all the friends of the selected node and clustering them
        for(int j=0; j<this->grph[current_node].size(); j++){
            struct edge node = grph[current_node][j];
            cluster[node.node] = current_cluster_no;
            //going through all the neighbors of this node and mark them as visited
            for(int k=0; k<this->grph[node.node].size(); k++){
                visited[grph[node.node][k].node] = 1;
            }
        }
        current_cluster_no ++;
    }
    //creating the graph for the next stage
    graph * new_graph = new graph(current_cluster_no);
    vector< map<int, int> > build_graph(current_cluster_no);
    for(int i=0; i< cluster.size(); i++){
        int cur_cluster = cluster[i];
        //for each node visit its neighbors. check whether they lie in different cluster
        for(int j=0; j<this->grph[i].size(); j++){
            struct edge neigh = this->grph[i][j];
            int neigh_cluster = cluster[neigh.node];
            if(neigh_cluster!=cur_cluster){
                map<int, int>::iterator it;
                it = build_graph[cur_cluster].find(neigh_cluster);
                if(it!=build_graph[cur_cluster].end()){
                    build_graph[cur_cluster][neigh_cluster] += neigh.weight;
                }else{
                    build_graph[cur_cluster][neigh_cluster] = neigh.weight;
                }
            }
        }

    }
    //we have the adjancey list in build_graph. Now iterate over it and build the graph
    for(int i=0; i<build_graph.size(); i++){
        map<int, int>::iterator it;
        for(it=build_graph[i].begin(); it!=build_graph[i].end(); it++){
            struct edge temp;
            temp.node = it->first;
            temp.weight = it->second;
            new_graph->grph[i].push_back(temp);
        }

    }

    return new_graph;

}

graph::graph(int no_nodes){
   this->no_nodes = no_nodes;
   vector< struct edge > temp;
   for(int i=0; i<no_nodes;i++){
        grph.push_back(temp);
   }
}

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
