//
// Created by jjt on 2021/6/17.
//

#ifndef BAYESIANNETWORK_OTT_H
#define BAYESIANNETWORK_OTT_H

#include "StructureLearning.h"
#include "ScoreFunction.h"

class Ott: public StructureLearning {
public:
    virtual void StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents);

    void StructLearnByOtt(Dataset *dts, vector<int> topo_ord_constraint);
    pair<double, set<Node*>> F(Node *node, set<Node*> &candidate_parents, Dataset *dts,
                               map<Node*, map<set<Node*>, double>> &dynamic_program);
    pair<double, vector<pair<Node*, set<Node*>>>> Q(set<Node*> &set_nodes, vector<int> topo_ord, Dataset *dts,
                                                    map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
                                                    map<pair<set<Node*>, vector<int>>,
                                                    pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q);
    vector<int> M(set<Node*> &set_nodes, Dataset *dts, map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
                  map<pair<set<Node*>, vector<int>>, pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q,
                  map<set<Node*>, vector<int>> dynamic_program_for_M);
};

#endif //BAYESIANNETWORK_OTT_H
