//
// Created by LinjianLi on 2019/9/1.
//

#ifndef BAYESIANNETWORK_ELIMINATIONTREE_H
#define BAYESIANNETWORK_ELIMINATIONTREE_H

#include "JunctionTree.h"
#include "gadget.h"
#include <cmath>

/**
 * @brief: this class is used for regression problem. JunctionTree class is for classification problems.
 */
class EliminationTree : public JunctionTree {
  // Data structures needed for elimination tree
  // (like junction tree) of gaussian Bayesian network.
  // Proposed in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)].

 public:
  Clique *strong_root;

  // The set of discrete clusters that are neighbours to the
  // continuous part will be called the boundary.
  // set<Clique*> boundary;

  map<int, int> map_to_root;

  EliminationTree() = default;
  explicit EliminationTree(Network *net);
  EliminationTree(Network *net, string elim_ord_strategy);
  EliminationTree(Network *net, vector<int> custom_elim_ord);

  void EnterSingleContEvidence(pair<int,double> e);
  void MessagePassingUpdateDiscretePartJT();
  CGRegression CalMarginalOfContinuousVar(int var_index);

 protected:
  void CalcuEachToRoot();
  void InitCGRegressions();

  void MessagePassingUpdateJT() override;
};


#endif //BAYESIANNETWORK_ELIMINATIONTREE_H
