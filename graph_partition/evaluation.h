#include<partition.h>


class evaluation{
    vector< vector<struct edge> > grph;

    public:
        evaluation(vector< vector<struct edge> >);
        float prob_score(vector<int> &);
        int edge_score(vector<int> &);
};

evaluation::evaluation(vector<vector<struct edge> > ggrph){

    for(unsigned int i=0; i < ggrph.size(); i++){
        vector<struct edge> temp;

        for(unsigned int j=0; j < ggrph[i].size(); j++){
            struct edge edg;
            edg.node = ggrph[i][j].node;
            edg.weight = ggrph[i][j].weight;
            temp.push_back(edg);
        }
        this->grph.push_back(temp);
    }

}

float evaluation::prob_score(vector<int>& cluster){
    vector<float> one_hop_prob;
    vector<float> two_hop_prob;
    vector<float> total_weight;

    //calculate the one hop prob first
    for(unsigned int i=0; i < this->grph.size(); i++){
        float total = 0.0;
        float out = 0.0;
        for(unsigned int j=0; j < this->grph[i].size(); j++){
            if(cluster[i] != cluster[this->grph[i][j].node])
                out += (float)this->grph[i][j].weight;

            total += (float)this->grph[i][j].weight;
        }
        assert(total!=0.0);
        one_hop_prob.push_back(out/total);
        total_weight.push_back(total);
    }


    //calculate the two hop prob now
    for(unsigned int i=0; i < this->grph.size(); i++){
        float total_friends =  (float)this->grph[i].size();
        float two_hop_sum = 0.0;

        for(unsigned int j=0; j < this->grph[i].size(); j++){

            if(cluster[i] != cluster[this->grph[i][j].node]){
                two_hop_sum += this->grph[i][j].weight/total_weight[i];
            }else{
                two_hop_sum += (this->grph[i][j].weight/total_weight[i])*one_hop_prob[this->grph[i][j].node];
            }
        }
        two_hop_prob.push_back(two_hop_sum/total_friends);

    }

    float eval_score = 0.0;
    //now calculate the evaluation score
    for(unsigned int i=0; i<two_hop_prob.size(); i++){
        eval_score += two_hop_prob[i];
    }
    return eval_score/(float)cluster.size();

}

int evaluation::edge_score(vector<int> & cluster){
    int no_edges = 0;
    for(unsigned int i=0; i < this->grph.size(); i++){
            int cur_cluster = cluster[i];
            for(unsigned int j=0; j < this->grph[i].size(); j++){
                if(cur_cluster != cluster[this->grph[i][j].node])
                    no_edges ++;
            }

    }
    return no_edges/2;


}
