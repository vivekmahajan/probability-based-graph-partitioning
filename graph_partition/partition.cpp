#include<partition.h>


int main(int argc, char* argv[]){

    graph *g_obj = new graph(argv[1]);
    g_obj->calculate_weights();
    graph *g1_obj = g_obj->cluster_nodes();
}
