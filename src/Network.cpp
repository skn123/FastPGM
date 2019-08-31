//
// Created by Linjian Li on 2018/11/29.
//

#include "Network.h"

Network::Network(): Network(true) {}

Network::Network(bool pure_disc) {
  this->pure_discrete = pure_disc;
}


void Network::PrintEachNodeParents() {
  for (const auto &node_ptr : set_node_ptr_container) {
    cout << node_ptr->GetNodeIndex() << ":\t";
    for (const auto &par_node_ptr : node_ptr->set_parents_ptrs) {
      cout << par_node_ptr->GetNodeIndex() << '\t';
    }
    cout << endl;
  }
}

void Network::PrintEachNodeChildren() {
  for (const auto &node_ptr : set_node_ptr_container) {
    cout << node_ptr->GetNodeIndex() << ":\t";
    for (const auto &par_node_ptr : node_ptr->set_children_ptrs) {
      cout << par_node_ptr->GetNodeIndex() << '\t';
    }
    cout << endl;
  }
}

Node* Network::FindNodePtrByIndex(const int &index) const {
  if (index<0 || index>num_nodes) {
    fprintf(stderr, "Error in function %s! \nInvalid index!", __FUNCTION__);
    exit(1);
  }
  Node* node_ptr = nullptr;
  for (const auto n_ptr : set_node_ptr_container) {
    if (n_ptr->GetNodeIndex()==index) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}


Node* Network::FindNodePtrByName(const string &name) const {
  Node* node_ptr = nullptr;
  for (const auto n_ptr : set_node_ptr_container) {
    if (n_ptr->node_name==name) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}


void Network::StructLearnCompData(Trainer *trn, bool print_struct) {
  fprintf(stderr, "Not be implemented yet!");
  exit(1);
}


void Network::SetParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  SetParentChild(p,c);
}


void Network::SetParentChild(Node *p, Node *c) {
  p->AddChild(c);
  c->AddParent(p);
}


void Network::RemoveParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  RemoveParentChild(p,c);
}


vector<int> Network::GenTopoOrd() {

  if (this->pure_discrete) {

    // First, convert the network to a directed adjacency matrix.
    int **graph = new int*[num_nodes];
    #pragma omp for
    for (int i=0; i<num_nodes; ++i) {graph[i] = new int[num_nodes]();}
    for (auto &n_p : set_node_ptr_container) {
      for (auto &c_p : n_p->set_children_ptrs) {
        graph[n_p->GetNodeIndex()][c_p->GetNodeIndex()] = 1;
      }
    }

    topo_ord = TopoSortOfDAGZeroInDegreeFirst(graph, num_nodes);

    for (int i=0; i<num_nodes; ++i) { delete[] graph[i]; }
    delete[] graph;

  } else {  // If the network is not pure discrete, then it is conditional Gaussian.
            // Discrete nodes should not have continuous parents.
            // Continuous nodes can have both discrete and continuous parents.
            // In topological ordering, all discrete nodes should
            // occur before any continuous node.
            // todo: test correctness of the case of Gaussian network

    set<Node*> set_disc_node_ptr, set_cont_node_ptr;
    for (const auto &n_p : set_node_ptr_container) {
      if (n_p->is_discrete) {
        set_disc_node_ptr.insert(n_p);
      } else {
        set_cont_node_ptr.insert(n_p);
      }
    }
    int **graph_disc = new int*[set_disc_node_ptr.size()];
    int **graph_cont = new int*[set_cont_node_ptr.size()];
    #pragma omp for
    for (int i=0; i<num_nodes; ++i) {
      graph_disc[i] = new int[set_disc_node_ptr.size()]();
      graph_cont[i] = new int[set_cont_node_ptr.size()]();
    }

    // Generate the ordering for discrete nodes.
    map<int, int> disc_order_index, disc_index_order;
    int disc_ord = 0;
    for (auto &n_p : set_disc_node_ptr) {
      disc_order_index[disc_ord] = n_p->GetNodeIndex();
      disc_index_order[n_p->GetNodeIndex()] = disc_ord;
      ++disc_ord;
    }
    for (auto &n_p : set_disc_node_ptr) {
      for (auto &c_p : n_p->set_children_ptrs) {
        if (!c_p->is_discrete) { continue; }
        graph_disc[ disc_index_order[n_p->GetNodeIndex()] ]
                  [ disc_index_order[c_p->GetNodeIndex()] ] = 1;
      }
    }
    auto topo_ord_disc = TopoSortOfDAGZeroInDegreeFirst(graph_disc, set_disc_node_ptr.size());

    // Generate the ordering for continuous nodes.
    map<int, int> cont_order_index, cont_index_order;
    int cont_ord = 0;
    for (auto &n_p : set_cont_node_ptr) {
      cont_order_index[cont_ord] = n_p->GetNodeIndex();
      cont_index_order[n_p->GetNodeIndex()] = cont_ord;
      ++cont_ord;
    }
    for (auto &n_p : set_cont_node_ptr) {
      for (auto &c_p : n_p->set_children_ptrs) {
        graph_cont[ cont_index_order[n_p->GetNodeIndex()] ]
                  [ cont_index_order[c_p->GetNodeIndex()] ] = 1;
      }
    }
    auto topo_ord_cont = TopoSortOfDAGZeroInDegreeFirst(graph_cont, set_cont_node_ptr.size());

    // Concatinate topo_ord_disc and topo_ord_cont.
    for (const auto &elem : topo_ord_disc) {
      topo_ord.push_back(disc_order_index[elem]);
    }
    for (const auto &elem : topo_ord_cont) {
      topo_ord.push_back(cont_order_index[elem]);
    }

    #pragma omp parallel for
    for (int i=0; i<set_disc_node_ptr.size(); ++i) {
      delete[] graph_disc[i];
    }
    #pragma omp parallel for
    for (int i=0; i<set_cont_node_ptr.size(); ++i) {
      delete[] graph_cont[i];
    }
    delete[] graph_disc;
    delete[] graph_cont;

  }

  return topo_ord;
}


void Network::RemoveParentChild(Node *p, Node *c) {
  p->RemoveChild(c);
  c->RemoveParent(p);
}


void Network::LearnParmsKnowStructCompData(const Trainer *trainer, bool print_params){
  fprintf(stderr, "Need to be implemented in sub-class!");
  exit(1);
}


void Network::ClearParms() {
  for (auto &n_p : this->set_node_ptr_container) {
    if (n_p->set_parents_ptrs.empty()) {
      for (auto &kv : n_p->map_marg_prob_table) {
        kv.second = 0;
      }
    } else {
      for (auto &kv : n_p->map_cond_prob_table) {
        for (auto &kv2 : kv.second) {
          kv2.second = 0;
        }
      }
    }
  }
}


Combination Network::ConstructEvidence(int *nodes_indexes, int *observations, int num_of_observations) {
  Combination result;
  pair<int, int> p;
  for (int i=0; i<num_of_observations; ++i) {
    p.first = nodes_indexes[i];
    p.second = observations[i];
    result.insert(p);
  }
  return result;
}


vector<Factor> Network::ConstructFactors(int *Z, int nz, Node *Y) {
  vector<Factor> factors_list;
  factors_list.push_back(Factor(Y));
  for (int i=0; i<nz; ++i) {
    Node* n = FindNodePtrByIndex(Z[i]);
    factors_list.push_back(Factor(n));
  }
  return factors_list;
}


void Network::LoadEvidence(vector<Factor> *factors_list, Combination E, set<int> all_related_vars) {

  // I do not know why this function cannot use omp to parallel.
  // If I set number of threads more than 1, the accuracy will decrease!

//  int num_cores = omp_get_num_procs();
//  omp_set_num_threads(num_cores);
//  int max_work_per_thread = (factors_list->size()+num_cores-1)/num_cores;
//  #pragma omp parallel
  {
//    for (int i = omp_get_thread_num() * max_work_per_thread;
//         i < (omp_get_thread_num()+1) * max_work_per_thread && i < factors_list->size();
//         ++i) {
    for (int i=0; i<factors_list->size(); ++i) {
      Factor &f = factors_list->at(i);   // For each factor. "std::vector::at" returns reference.
      for (const auto &e : E) {  // For each node's observation in E
        // If this factor is related to this node
        if (f.related_variables.find(e.first) != f.related_variables.end()) {
          // Update each row of map_potentials
          for (const auto &comb : f.set_combinations) {
            // If this entry is not compatible to the evidence.
            if (comb.find(e) == comb.end()) {
              f.map_potentials[comb] = 0;
            }
          }
        }
      }

      //--------------------------------------------------------------------------------
      // This block is to fix the bug occurring when the target node
      // is not the root and the variable elimination order do not start at root.
      // For example:  A --> B --> C
      // When given the markov blanket of node "C", which is "{B}",
      // there is no need to calculate the other nodes, which is "{A}".
      // However, when using this function,
      // the parent of parent of this node, which is "A",
      // still appears in the constructed factor of the parent which is "B".
      // todo: check correctness
      set<int> related_vars_of_f = f.related_variables;
      for (auto &v : related_vars_of_f) {
        if (all_related_vars.find(v) == all_related_vars.end()) {
          f.CopyFactor(f.SumOverVar(v));
        }
      }
      //--------------------------------------------------------------------------------
    }

  }   // end of: #pragma omp parallel
}


Factor Network::SumProductVarElim(vector<Factor> factors_list, int *Z, int nz) {
  for (int i=0; i<nz; ++i) {
    vector<Factor> tempFactorsList;
    Node* nodePtr = FindNodePtrByIndex(Z[i]);
    // Move every factor that is related to the node Z[i] from factors_list to tempFactorsList.
    /*
     * Note: This for loop does not contain "++it" in the parentheses.
     *      Because if we do so, it may cause some logic faults which,
     *      however, may or may not cause runtime error, causing the program hard to debug.
     *      For example:
     *        When "it" reaches the second to last element, and this element is related to the node.
     *        Then this element will be erase from factors_list,
     *        and then "++it" which will move "it" to the end.
     *        Then the for loop will end because "it" has reached the end.
     *        However, at this time, the last element has been ignored,
     *        even if it is related to the node.
     */
    for (auto it=factors_list.begin(); it!=factors_list.end(); /* no ++it */) {
      if ((*it).related_variables.find(nodePtr->GetNodeIndex())!=(*it).related_variables.end()) {
        tempFactorsList.push_back(*it);
        factors_list.erase(it);
        continue;
      } else {
        ++it;
      }
    }
    while(tempFactorsList.size()>1) {
      Factor temp1, temp2, product;
      temp1 = tempFactorsList.back();
      tempFactorsList.pop_back();
      temp2 = tempFactorsList.back();
      tempFactorsList.pop_back();
      product = temp1.MultiplyWithFactor(temp2);
      tempFactorsList.push_back(product);
    }
    Factor newFactor = tempFactorsList.back().SumOverVar(nodePtr);
    factors_list.push_back(newFactor);
  }

  /*
   *   If we are calculating a node's posterior probability given evidence about its children,
   *   then when the program runs to here,
   *   the "factors_list" will contain several factors
   *   about the same node which is the query node Y.
   *   When it happens, we need to multiply these several factors.
   */
  while (factors_list.size()>1) {
    Factor temp1, temp2, product;
    temp1 = factors_list.back();
    factors_list.pop_back();
    temp2 = factors_list.back();
    factors_list.pop_back();
    product = temp1.MultiplyWithFactor(temp2);
    factors_list.push_back(product);
  }
  // After all the processing shown above, the only remaining factor is the factor about Y.
  return factors_list.back();
}


Factor Network::VarElimInferReturnPossib(int *Z, int nz, Combination E, Node *Y) {
  // Z is the array of variable elimination order.
  // E is the evidences.
  vector<Factor> factorsList = ConstructFactors(Z, nz, Y);

  //--------------------------------------------------------------------------------
  // This block is to fix the bug occurring when the target node
  // is not the root and the variable elimination order do not start at root.
  // For example:  A --> B --> C
  // When given the markov blanket, which is "{B}", of node "C",
  // there is no need to calculate the other nodes, which is "{A}".
  // However, when using this function,
  // the parent of parent of this node, which is "A",
  // still appears in the constructed factor of the parent which is "B".
  set<int> all_related_vars;
  all_related_vars.insert(Y->GetNodeIndex());
  for (int i=0; i<nz; ++i) {all_related_vars.insert(Z[i]);}
  //--------------------------------------------------------------------------------


  LoadEvidence(&factorsList, E, all_related_vars);
  Factor F = SumProductVarElim(factorsList, Z, nz);
  F.Normalize();
  return F;
}


Factor Network::VarElimInferReturnPossib(Combination E, Node *Y) {
  pair<int*, int> simplified_elimination_order = SimplifyDefaultElimOrd(E);
  return this->VarElimInferReturnPossib(
                  simplified_elimination_order.first,
                  simplified_elimination_order.second,
                  E,
                  Y
               );
}


int Network::PredictUseVarElimInfer(int *Z, int nz, Combination E, int Y_index) {
  Node *Y = FindNodePtrByIndex(Y_index);
  Factor F = VarElimInferReturnPossib(Z, nz, E, Y);
  double max_prob = 0;
  Combination comb_predict;
  for (auto &comb : F.set_combinations) {
    if (F.map_potentials[comb] > max_prob) {
      max_prob = F.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second;
  return label_predict;
}


int Network::PredictUseVarElimInfer(Combination E, int Y_index) {
  Node *Y = FindNodePtrByIndex(Y_index);
  Factor F = VarElimInferReturnPossib(E, Y);

  double max_prob = 0;
  Combination comb_predict;
  for (auto &comb : F.set_combinations) {
    if (F.map_potentials[comb] > max_prob) {
      max_prob = F.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second;
  return label_predict;
}


double Network::TestNetReturnAccuracy(Trainer *tester) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=tester->num_instance, m20=m/20, progress=0;

//  int num_cores = omp_get_num_procs();
//  omp_set_num_threads(num_cores);
  // For each sample in test set
  #pragma omp parallel for
  for (int i=0; i<m; ++i) {

    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num = num_nodes - 1, *e_index = new int[e_num], *e_value = new int[e_num];
    for (int j = 0; j < e_num; ++j) {
      e_index[j] = j + 1;
      e_value[j] = tester->train_set_X[i][j];
    }
    Combination E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = PredictUseVarElimInfer(E, 0); // The root node (label) has index of 0.
    if (label_predict == tester->train_set_y[i]) {
      #pragma omp critical
      { ++num_of_correct; }
    } else {
      #pragma omp critical
      { ++num_of_wrong; }
    }

    delete[] e_index;
    delete[] e_value;
  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}


double Network::TestNetByApproxInferReturnAccuracy(Trainer *tester, int num_samp) {

  // implement by Gibbs sampling
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=tester->num_instance, m20=m/20, progress=0;

  vector<Combination> samples = this->DrawSamplesByProbLogiSamp(10000);

//  #pragma omp parallel for
  for (int i=0; i<m; ++i) {  // For each sample in test set

//    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num=num_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<e_num; ++j) {
      e_index[j] = j+1;
      e_value[j] = tester->train_set_X[i][j];
    }
    Combination E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = ApproxInferByProbLogiRejectSamp(E, 0, samples); // The root node (label) has index of 0.
    if (label_predict == tester->train_set_y[i]) {
      ++num_of_correct;
    } else {
      ++num_of_wrong;
    }

  }
  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}

double Network::TestAccuracyByLikelihoodWeighting(Trainer *tester, int num_samp) {
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=tester->num_instance, m20=m/20, progress=0;

  #pragma omp parallel for
  for (int i=0; i<m; ++i) {  // For each sample in test set

    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num=num_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<e_num; ++j) {
      e_index[j] = j+1;
      e_value[j] = tester->train_set_X[i][j];
    }
    Combination E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = ApproxinferByLikelihoodWeighting(E, 0, num_samp); // The root node (label) has index of 0.
    if (label_predict == tester->train_set_y[i]) {
      #pragma omp critical
      { ++num_of_correct; }
    } else {
      #pragma omp critical
      { ++num_of_wrong; }
    }

    delete[] e_index;
    delete[] e_value;

  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}



Combination Network::ProbLogicSampleNetwork() {
  if (topo_ord.empty()) {
    this->GenTopoOrd();
  }
  Combination instance;
  // Cannot use OpenMP, because must draw samples in the topological ordering.
  for (const auto &index : topo_ord) {
    Node *n_p = FindNodePtrByIndex(index);
    int drawn_value = n_p->SampleNodeGivenParents(instance);
    instance.insert(pair<int,int>(index, drawn_value));
  }
  return instance;
}

pair<Combination, double> Network::DrawOneLikelihoodWeightingSample(const Combination &evidence) {
  if (topo_ord.empty()) {
    this->GenTopoOrd();
  }
  Combination instance;
  double weight = 1;
  // SHOULD NOT use OpenMP, because must draw samples in the topological ordering.
  for (const auto &index : topo_ord) {
    Node *n_p = FindNodePtrByIndex(index);
    bool observed = false;
    for (const auto &p : evidence) {
      if (index==p.first) {
        observed = true;
        // Set the sample value to be the same as the evidence.
        instance.insert(pair<int, int>(index, p.second));
        // Update the weight.
        if(!n_p->set_parents_ptrs.empty()) {
          set<int> parents_indexes;
          for (const auto &par : n_p->set_parents_ptrs) {
            parents_indexes.insert(par->GetNodeIndex());
          }
          Combination parents_index_value;
          for (const auto &i : instance) {
            if (parents_indexes.find(i.first) != parents_indexes.end()) {
              parents_index_value.insert(i);
            }
          }
          weight *= n_p->map_cond_prob_table[p.second][parents_index_value];
        } else {
          weight *= n_p->map_marg_prob_table[p.second];
        }
        break;
      }
    }
    if (!observed) {
      int drawn_value = n_p->SampleNodeGivenParents(instance);
      instance.insert(pair<int,int>(index, drawn_value));
    }
  }
  return pair<Combination, double>(instance, weight);
}


vector<pair<Combination, double>> Network::DrawSamplesByLikelihoodWeighting(const Combination &evidence, int num_samp) {
  vector<pair<Combination, double>> results;
  #pragma omp parallel for
  for (int i=0; i<num_samp; ++i) {
    auto samp = DrawOneLikelihoodWeightingSample(evidence);
    #pragma omp critical
    { results.push_back(samp); }
  }
  return results;
}

Factor Network::CalcuMargWithLikelihoodWeightingSamples(const vector<pair<Combination, double>> &samples, const int &node_index) {
  map<int, double> value_weight;
  Node *n_p = this->FindNodePtrByIndex(node_index);

  // Initialize the map.
  for (int i=0; i<n_p->num_potential_vals; ++i) {
    value_weight[n_p->potential_vals[i]] = 0;
  }

  // Calculate the sum of weight for each value. Un-normalized.
  for (const auto &samp : samples) {
    for (const auto &feature_value : samp.first) {
      if (node_index==feature_value.first) {
        value_weight[feature_value.second] += samp.second;
        break;
      }
    }
  }

  // Normalization.
  double denominator = 0;
  for (const auto &kv : value_weight) {
    denominator += kv.second;
  }
  for (auto &kv : value_weight) {
    kv.second /= denominator;
  }

  // Construct a factor to return
  Factor f;
  set<int> rv;
  rv.insert(node_index);
  set<Combination> sc;
  for (int i=0; i<n_p->num_potential_vals; ++i) {
    Combination c;
    c.insert(pair<int, int>(node_index, n_p->potential_vals[i]));
    sc.insert(c);
  }
  map<Combination, double> mp;
  for (const auto &c : sc) {
    int value = (*c.begin()).second;
    mp[c] = value_weight[value];
  }
  f.SetMembers(rv, sc, mp);
  return f;
}


int Network::ApproxinferByLikelihoodWeighting(Combination e, const int &node_index, const int &num_samp) {
  vector<pair<Combination, double>> samples_weight = this->DrawSamplesByLikelihoodWeighting(e, num_samp);
  Factor f = CalcuMargWithLikelihoodWeightingSamples(samples_weight, node_index);
  // Find the argmax.
  Combination c;
  double max = -1;
  for (const auto &kv : f.map_potentials) {
    if (kv.second > max) {
      c = kv.first;
      max = kv.second;
    }
  }
  return (*c.begin()).second;
}


vector<Combination> Network::DrawSamplesByProbLogiSamp(int num_samp) {
  vector<Combination> samples;
  samples.reserve(num_samp);
  #pragma omp parallel for
  for (int i=0; i<num_samp; ++i) {
    Combination samp = this->ProbLogicSampleNetwork();
    #pragma omp critical
    { samples.push_back(samp); }
  }
  return samples;
}


set<int> Network::GetMarkovBlanketIndexesOfNode(Node *node_ptr) {
  set<int> markov_blanket_node_index;

  // Add parents.
  for (auto &par_ptr : node_ptr->set_parents_ptrs) {
    markov_blanket_node_index.insert(par_ptr->GetNodeIndex());
  }

  // Add children and parents of children.
  for (auto &chil_ptr : node_ptr->set_children_ptrs) {
    markov_blanket_node_index.insert(chil_ptr->GetNodeIndex());
    for (auto &par_chil_ptr : chil_ptr->set_parents_ptrs) {
      markov_blanket_node_index.insert(par_chil_ptr->GetNodeIndex());
    }
  }


  markov_blanket_node_index.erase(node_ptr->GetNodeIndex());

  return markov_blanket_node_index;
}


vector<Combination> Network::DrawSamplesByGibbsSamp(int num_samp, int num_burn_in) {

  vector<Combination> samples;
  samples.reserve(num_samp);

  Combination single_sample = this->ProbLogicSampleNetwork();

  auto it_node = this->set_node_ptr_container.begin();


  // Need burning in.
//  #pragma omp parallel for
  for (int i=1; i<num_burn_in+num_samp; ++i) {

    Node *node_ptr = *(it_node++);
    if (it_node==set_node_ptr_container.end()) {
      it_node = this->set_node_ptr_container.begin();
    }

    set<int> markov_blanket_node_index = GetMarkovBlanketIndexesOfNode(node_ptr);

    Combination markov_blanket;
    for (auto &p : single_sample) {
      if (markov_blanket_node_index.find(p.first)
          !=
          markov_blanket_node_index.end()) {
        markov_blanket.insert(p);
      }
    }

    int value_index =
            SampleNodeGivenMarkovBlanketReturnValIndex(node_ptr,markov_blanket);

    for (auto p : single_sample) {
      if (p.first == node_ptr->GetNodeIndex()) {
        single_sample.erase(p);
        p.second = node_ptr->potential_vals[value_index];
        single_sample.insert(p);
        break;
      }
    }

    // After burning in, we can store the samples now.
    #pragma omp critical
    { if (i>=num_burn_in) {samples.push_back(single_sample);} }
  }

  return samples;
}


int Network::SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, Combination markov_blanket) {
  int num_elim_ord = markov_blanket.size();
  int *var_elim_ord = new int[num_elim_ord];
  int temp = 0;
  for (auto &n_v : markov_blanket) {
    var_elim_ord[temp++] = n_v.first;
  }

  Factor f = VarElimInferReturnPossib(var_elim_ord, num_elim_ord, markov_blanket, node_ptr);

  vector<int> weights;
  for (int i=0; i<node_ptr->num_potential_vals; ++i) {
    Combination temp;
    temp.insert(pair<int,int>(node_ptr->GetNodeIndex(),node_ptr->potential_vals[i]));
    weights.push_back(f.map_potentials[temp]*10000);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return this_distribution(rand_gen);
}


int Network::ApproxInferByProbLogiRejectSamp(Combination e, Node *node, vector<Combination> &samples) {
  Combination possb_values;
  for (int i=0; i<node->num_potential_vals; ++i) {
    possb_values.insert(pair<int,int>(node->GetNodeIndex(),node->potential_vals[i]));
  }

  int *count_each_value = new int[this->num_nodes]();
  int num_valid_sample = 0;
  for (const auto &samp : samples) {
    if(!Conflict(&e, &samp)) {
      ++num_valid_sample;
      for (const auto &pv : possb_values) {
        if (samp.find(pv)!=samp.end()) {
          ++count_each_value[pv.second];
          break;
        }
      }
    }
  }

  // If there is no valid sample, just take a random guess.
  if (num_valid_sample==0) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rand_gen(seed);
    uniform_int_distribution<int> this_distribution(0,node->num_potential_vals-1);
    return node->potential_vals[this_distribution(rand_gen)];
  }

  // Find the argmax.
  int lable_index_predict = -1;
  int max_occurred = 0;
  for (int i=0; i<node->num_potential_vals; ++i) {
    if (lable_index_predict==-1 || count_each_value[i]>max_occurred) {
      lable_index_predict = i;
      max_occurred = count_each_value[i];
    }
  }

  // Return the predicted label instead of the index.
  return node->potential_vals[lable_index_predict];
}


int Network::ApproxInferByProbLogiRejectSamp(Combination e, int node_index, vector<Combination> &samples) {
  return ApproxInferByProbLogiRejectSamp(e, FindNodePtrByIndex(node_index), samples);
}