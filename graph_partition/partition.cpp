#include<evaluation.h>

void load_metis_info(char * filename, vector<int>& metis_cluster){
     ifstream myfile(filename);
     string str;
     while(getline(myfile, str)){
        metis_cluster.push_back(atoi(str.c_str()));
     }


}

int main(int argc, char* argv[]){

    if(argc != 5){
        cout<<"./graph_partition <input_graph> <no_of_partitoins> <limit> <metis_output>"<<endl;
        exit(1);
    }

    int no_partitions = int(argv[2]);
    graph *g_obj = new graph(argv[1], atoi(argv[3]) );
    graph *org_graph = g_obj;


    char *metis_file = argv[4];
    vector<int> metis_cluster;
    load_metis_info(metis_file, metis_cluster);

    vector<int> our_cluster(g_obj->no_nodes);
    for(unsigned int i=0; i < our_cluster.size(); i++){
        our_cluster[i] = i;
    }
    for(int i=0; i < 2; i++){
        g_obj->calculate_weights();
        //g_obj->display_weights();
        graph *temp = g_obj->cluster_nodes();
        g_obj->update_old_clusters(our_cluster);
        g_obj = temp;
    }
    //find the total number of clusters
    int max = our_cluster[0];
    for(unsigned int i=0; i < our_cluster.size(); i++){
        if(max < our_cluster[i]){
            max = our_cluster[i];
        }
    }
    //randomly divide them into partitions
    vector<int> lookup(max);
    for(unsigned int i=0; i<lookup.size(); i++){
        lookup[i] = i%no_partitions;
    }
    for(unsigned int i=0; i < our_cluster.size(); i++){
        our_cluster[i] = lookup[our_cluster[i]];
    }
    //you have the partitions in the cluster vector, now find the probability based evaluation score
    evaluation* eval = new evaluation(org_graph->grph);

    cout<<our_cluster.size()<<" "<<metis_cluster.size()<<endl;
    assert(our_cluster.size() == metis_cluster.size());
    cout<< "ours evaluation score "<<eval->prob_score(our_cluster)<<endl;
    cout<< "Metis evaluation score "<<eval->prob_score(metis_cluster)<<endl;

    cout<< "ours edge score "<<eval->edge_score(our_cluster)<<endl;
    cout<< "Metis edge score "<<eval->edge_score(metis_cluster)<<endl;


}
