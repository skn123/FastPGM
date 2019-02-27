//
// Created by LinjianLi on 2019/2/20.
//

#ifndef BAYESIANNETWORK_SEPARATOR_H
#define BAYESIANNETWORK_SEPARATOR_H

#include "Clique.h"

class Separator : public Clique {
 public:
  int weight;
  map<Combination, double> map_old_potentials;
  Separator(set<Node*>);
  void UpdateUseMessage(Factor&) override;
  Factor ConstructMessage() override;

 protected:
  Separator() = default;
};


#endif //BAYESIANNETWORK_SEPARATOR_H