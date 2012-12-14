#include<evaluation.h>

void load_metis_info(char * filename, vector<int>& metis_cluster){
     ifstream myfile(filename);
     string str;
     while(getline(myfile, str)){
        metis_cluster.push_back(atoi(str.c_str()));
     }


}
bool compare_delta(pair< pair<int, int>, float> p1, pair< pair<int,int>, float> p2){

  return(p1.second > p2.second);

}
void kernighan_lin_algo(vector<int> &cluster, vector< vector<struct edge> > grph){

    evaluation * eval = new evaluation(grph);
    for(int count=0; count < 20; count++){
        cout<<"iteration "<<count<<endl;
        cout<<eval->prob_score(cluster)<<" "<<eval->edge_score(cluster)<<endl;

        vector<float> D;
        vector<float> total_int;

        for(int i=0; i<grph.size(); i++){
            float ia=0.0, ea=0.0;
            float total = 0.0;
            for(int j=0; j<grph[i].size(); j++){
                    if(cluster[i]!=cluster[grph[i][j].node])
                        ea += (float)grph[i][j].weight;
                    else
                        ia += (float)grph[i][j].weight;

                    total += (float)grph[i][j].weight;
            }
            //D.push_back((ea-ia)/total);
            //D.push_back((ea-ia));
            D.push_back((ia-ea));
            total_int.push_back(total);
        }
        //only allowing two partitions
        vector<int> parta;
        vector<int> partb;
        for(int i=0; i< cluster.size();i++){
            if(cluster[i]==0)
                parta.push_back(i);
            else
                partb.push_back(i);
        }

        //map< pair<int, int>, float> delta;
        vector< pair< pair<int,int>, float > > delta;
        cout<<parta.size()<<" "<<partb.size()<<endl;
        /*for(int i=0; i<parta.size(); i++){
            for(int j=0; j<partb.size(); j++){
                float c = 0.0;
                for(int k=0; k<grph[parta[i]].size(); k++){
                    if(grph[parta[i]][k].node == partb[j]){
                            //c = grph[parta[i]][k].weight/total_int[parta[i]] + grph[parta[i]][k].weight/total_int[grph[parta[i]][k].node];
                            c = 2*grph[parta[i]][k].weight;
                            break;
                    }
                }
                delta.push_back(make_pair(make_pair(parta[i], partb[j]), D[parta[i]+D[partb[j]]-c]));
                //delta[make_pair(parta[i],partb[j])] = D[parta[i]] + D[partb[j]] - 2*c;
            }
            if(delta.size()>10000000)
                break;
        }*/
        for(int i=0; i< 10000000; i++){

            int randa = rand()%parta.size();
            int randb = rand()%partb.size();

            float c = 0.0;
            for(int k=0; k<grph[parta[randa]].size(); k++){
                if(grph[parta[randa]][k].node == partb[randb]){
                        //c = grph[parta[i]][k].weight/total_int[parta[i]] + grph[parta[i]][k].weight/total_int[grph[parta[i]][k].node];
                        c = 2*grph[parta[randa]][k].weight;
                        break;
                }
            }
            delta.push_back(make_pair(make_pair(parta[randa], partb[randb]), D[parta[randa]+D[partb[randb]]-c]));

        }

        sort(delta.begin(), delta.end(), compare_delta);


        vector<bool> mark(100000, 0);
        int i=-1;
        while(delta[i].second > 0.0){
            i++;
            if(mark[delta[i].first.first] == 1 || mark[delta[i].first.second] == 1)
                continue;
            cluster[delta[i].first.first] = 1;
            cluster[delta[i].first.second] = 0;
            mark[delta[i].first.first] = 1;
            mark[delta[i].first.second] = 1;
        }
    }

}

int main(int argc, char* argv[]){

    if(argc != 5){
        cout<<"./graph_partition <input_graph> <no_of_partitoins> <limit> <metis_output>"<<endl;
        exit(1);
    }

    int no_partitions = atoi(argv[2]);
    graph *g_obj = new graph(argv[1], atoi(argv[3]) );
    graph *org_graph = g_obj;


    //uploading metis clusters
    char *metis_file = argv[4];
    vector<int> metis_cluster;
    load_metis_info(metis_file, metis_cluster);


    //initializing our clusters
    vector<int> our_cluster(g_obj->no_nodes);
    for(unsigned int i=0; i < our_cluster.size(); i++){
        our_cluster[i] = i;
    }
    graph *temp;
    for(int i=0; i < 5; i++){
        g_obj->calculate_weights();
        g_obj->calc_node_w(our_cluster);
        temp = g_obj->cluster_nodes();
        g_obj->update_old_clusters(our_cluster);
        g_obj = temp;
    }
    g_obj->calc_node_w(our_cluster);
    //find the total number of clusters
    int max = our_cluster[0];
    for(unsigned int i=0; i < our_cluster.size(); i++){
        if(max < our_cluster[i]){
            max = our_cluster[i];
        }
    }

    vector<int> partition_nodes(no_partitions, 0);

    //randomly divide them into partitions
    vector<int> lookup(max+1);
    int size_limit = (float)org_graph->no_nodes/(float)no_partitions + 0.10*(float)org_graph->no_nodes;
    //cout<<" size limit "<<size_limit<<endl;
    for(unsigned int i=0; i<lookup.size(); i++){
        while(1){
            int proposed_cluster = rand()%no_partitions;
            int prop_size = partition_nodes[proposed_cluster] + g_obj->node_w[i];
            //cout<<i<<" "<<g_obj->node_w[i]<<" "<<"prop size "<<prop_size<<" size limit "<<size_limit<<"proposed_cluster "<<proposed_cluster<<endl;
            if(prop_size < size_limit)
            {
                lookup[i] = proposed_cluster;
                partition_nodes[proposed_cluster] += g_obj->node_w[i];
                break;

            }else
                continue;

        }
    }
    for(unsigned int i=0; i < our_cluster.size(); i++){
        our_cluster[i] = lookup[our_cluster[i]];
    }
    //Refinement time.
    kernighan_lin_algo(our_cluster, org_graph->grph);



    //you have the partitions in the cluster vector, now find the probability based evaluation score
    evaluation* eval = new evaluation(org_graph->grph);

    cout<<our_cluster.size()<<" "<<metis_cluster.size()<<endl;
    assert(our_cluster.size() == metis_cluster.size());
    cout<< "ours evaluation score "<<eval->prob_score(our_cluster)<<endl;
    cout<< "Metis evaluation score "<<eval->prob_score(metis_cluster)<<endl;

    cout<< "ours edge score "<<eval->edge_score(our_cluster)<<endl;
    cout<< "Metis edge score "<<eval->edge_score(metis_cluster)<<endl;

    cout<< "Cluster statistics "<<endl;
    for(int i=0; i<no_partitions; i++){
        partition_nodes[i] = 0;
    }
    for(int i=0; i< our_cluster.size(); i++){
        partition_nodes[our_cluster[i]]++;
    }

    for(int i=0; i< no_partitions; i++){
        cout<<i<<" "<<partition_nodes[i]<<endl;
    }


}
