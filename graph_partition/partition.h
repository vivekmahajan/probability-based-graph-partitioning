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
    return(p1.second<p2.second);
}

class graph{
    public:
    vector< vector<struct edge> > grph;
    vector< pair<int, float> > node_weights;
    vector< int > node_w;
    vector< int > cluster;
    vector< int > cluster_weights;
    int no_nodes, no_edges, limit;


    graph(char *, int);
    graph(int , int, vector<int>&);//constructor overloading
    void calculate_weights();
    void graph_check();
    graph* cluster_nodes();
    void update_old_clusters(vector<int>&);
    void display();
    void display_weights();
    void calc_cluster_weights();
    void calc_node_w(vector<int>);
};

void graph::calc_node_w(vector<int> our_cluster){
    this->node_w.clear();
    int max = our_cluster[0];
    for(int i=0; i< our_cluster.size(); i++){
        if(max < our_cluster[i])
            max = our_cluster[i];
    }
    for(int i=0; i<max+1; i++){
        this->node_w.push_back(0);
    }
    for(int i=0; i<our_cluster.size(); i++)
        this->node_w[our_cluster[i]]++;
}


graph::graph(int no_nodes, int limit, vector<int>& new_weights){
   this->limit =  limit;
   this->no_nodes = no_nodes;
   vector< struct edge > temp;
   for(int i=0; i<no_nodes;i++){
        grph.push_back(temp);
   }
   /*
   for(int i=0; i<new_weights.size(); i++){
    this->node_w.push_back(new_weights[i]);
   }*/

}


graph::graph(char* filename, int limit){
    this->limit = limit;
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

    for(int i=0; i < this->no_nodes; i++){
        this->node_w.push_back(1);
    }

}

void graph::calc_cluster_weights(){
    this->cluster_weights.clear();
    for(unsigned int i=0; i<this->grph.size(); i++){

        int sum = node_w[i];
        for(unsigned int j=0; j<this->grph[i].size(); j++){

            sum += this->node_w[this->grph[i][j].node];
        }
        this->cluster_weights.push_back(sum);
    }

}

void graph::update_old_clusters(vector<int>& old_cluster){

    for(unsigned int i=0; i < old_cluster.size(); i++){
        old_cluster[i] = this->cluster[old_cluster[i]];
    }

}

void graph::display_weights(){

    for(unsigned int i=0; i<node_weights.size(); i++){
        cout<<node_weights[i].second<<endl;

    }

}

void graph::display(){
    for(unsigned int i=0; i<this->grph.size(); i++){
        for(unsigned int j=0; j<this->grph[i].size(); j++){
            cout<<this->grph[i][j].node<<","<<grph[i][j].weight<<" ";
        }
        std::cout<<endl;

    }
}
graph* graph::cluster_nodes(){

    this->calc_cluster_weights(); //need do this to avoid unbalanced clusters
    vector<int> new_cluster_weights;

    for(unsigned int i=0; i < this->grph.size(); i++){
        this->cluster.push_back(-1);
    }
    vector<bool> visited(this->grph.size(), 0);
    int current_cluster_no = 0;

    for(unsigned int i=0; i<this->node_weights.size(); i++){
        int current_node = this->node_weights[i].first;

        //first check if clutering that nodes is allowed or not
        //then check whether forming the cluster will result into unbalanced partitions
        if(visited[current_node] == 1 || this->cluster_weights[current_node] > this->limit ){
            continue;
        }
        new_cluster_weights.push_back(this->cluster_weights[current_node]);
        this->cluster[current_node] = current_cluster_no;
        visited[current_node] = 1;
        //else go through all the friends of the selected node and cluster them
        for(unsigned int j=0; j<this->grph[current_node].size(); j++){
            struct edge node = grph[current_node][j];
            cluster[node.node] = current_cluster_no;
            visited[node.node] = 1;

            //going through all the neighbors of this node and mark them as visited
            for(unsigned int k=0; k<this->grph[node.node].size(); k++){
                visited[grph[node.node][k].node] = 1;
            }

        }
        current_cluster_no ++;
    }
    //assign clusters to the unassigned ones
    for(unsigned int i=0; i< cluster.size(); i++){
        if(cluster[i] == -1){
            cluster[i] = current_cluster_no;
            current_cluster_no ++;
        }

    }


    cout<<"Total clusters "<<current_cluster_no<<endl;


    //creating the graph for the next stage
    graph * new_graph = new graph(current_cluster_no, this->limit, new_cluster_weights);
    vector< map<int, int> > build_graph(current_cluster_no);

    for(unsigned int i=0; i< cluster.size(); i++){
        int cur_cluster = cluster[i];

        //for each node visit its neighbors. check whether they lie in different cluster
        for(unsigned int j=0; j < this->grph[i].size(); j++){
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
    for(unsigned int i=0; i<build_graph.size(); i++){
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


void graph::calculate_weights(){

    //this->graph_check();
    for(unsigned int i=0; i < this->grph.size(); i++){
        if(this->grph[i].size() == 0){
            //cout<<i<<endl;
        }
        //assert(this->grph[i].size()!= 0);
        float node_weight = 0.0;
        for(unsigned int j=0; j<this->grph[i].size(); j++){
            struct edge e = this->grph[i][j];
            node_weight = node_weight + e.weight;
        }

        if(this->grph[i].size() != 0){
            node_weight = node_weight/this->grph[i].size();
        }else{
            node_weight = 0.0;
        }
        node_weights.push_back(make_pair(i, node_weight));
        assert(node_weights.size()==i+1);
    }
    //sorting the nodes by weight
    sort(node_weights.begin(), node_weights.end(), compare_function);

}


void graph::graph_check(){
    //checking for the undirected graph
    for(unsigned int i=0; i < this->grph.size(); i++){
        if(this->grph[i].size() == 0){
            //cout<<i<<endl;
        }
        //assert(this->grph[i].size() != 0); //there can't be any disconnected node
        for(unsigned int j=0; j < this->grph[i].size(); j++){
            int neigh = this->grph[i][j].node;
            int flag = 0;

            for(unsigned int k=0; k < this->grph[neigh].size(); k++){

                 if(i==grph[neigh][k].node)
                    flag=1;
            }
            assert(flag==1);

        }

    }

}


