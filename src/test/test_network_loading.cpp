////
//// Created by jjt on 25/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "fastpgm/CustomNetwork.h"
//#include "fastpgm/test_common.h"
//
//
//class TestXMLBIFLoading: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        network = new CustomNetwork(true);
//    }
//    CustomNetwork *network;
//};
//
//TEST_F(TestXMLBIFLoading, alarm) {
//    string net_file = dataset_path + "alarm/alarm.xml";
//    network->LoadXMLBIFFile(net_file, ALPHA);
//
//    cout << "network_name = " << network->network_name << endl;
//    cout << "num_nodes = " << network->num_nodes << endl;
//    cout << "num_edges = " << network->num_edges << endl;
//    cout << "pure_discrete = " << network->pure_discrete << endl;
//
//    cout << "topo_ord: ";
//    for (int i = 0; i < network->topo_ord.size(); i++) {
//        cout << network->topo_ord[i] << " ";
//    }
//    cout << endl;
//
//    cout << "vec_default_elim_ord: ";
//    for (int i = 0; i < network->vec_default_elim_ord.size(); i++) {
//        cout << network->vec_default_elim_ord[i] << " ";
//    }
//    cout << endl;
//
//    cout << "map_idx_node_ptr: ";
//    for (const auto &id_n: network->map_idx_node_ptr) {
//        cout << id_n.first << endl;
//        Node* node = id_n.second;
//        cout << "  node_index = " << node->GetNodeIndex() << endl;
//        cout << "  node_name = " << node->node_name << endl;
//        cout << "  is_discrete = " << node->is_discrete << endl;
//        cout << "  set_discrete_parents_combinations: size = " << node->set_discrete_parents_combinations.size() << endl;
//        for (const auto &config: node->set_discrete_parents_combinations) {
//            cout << "  ";
//            for (const auto &varval: config) {
//                cout << varval.first << " = " << varval.second << ", ";
//            }
//            cout << endl;
//        }
//
//        cout << " vec_disc_parent_indexes: ";
//        for (int i = 0; i < node->vec_disc_parent_indexes.size(); ++i) {
//            cout << node->vec_disc_parent_indexes[i] << ", ";
//        }
//        cout << endl;
//
//        cout << "  set_parent_indexes: ";
//        for (const int &p: node->set_parent_indexes) {
//            cout << p << ", ";
//        }
//        cout << endl;
//
//        cout << "  set_children_indexes: ";
//        for (const int &c: node->set_children_indexes) {
//            cout << c << ", ";
//        }
//        cout << endl;
//
//        DiscreteNode* disc_node = (DiscreteNode*)node;
//        cout << "  possible_values_ids: ";
//        for (const auto &val_id: disc_node->possible_values_ids) {
//            cout << val_id.first << ", " << val_id.second << "; ";
//        }
//        cout << endl;
//
//        cout << "map_cond_prob_table_statistics: " << endl;
//        for (const auto &m: disc_node->map_cond_prob_table_statistics) {
//            cout << m.first << ": ";
//            map<DiscreteConfig, int> m2 = m.second;
//            for (const auto &mm: m2) {
//                DiscreteConfig config = mm.first;
//                for (const auto &s: config) {
//                    cout << s.first << " = " << s.second << " ";
//                }
//                cout << ": " << mm.second << ", ";
//            }
//            cout << endl;
//        }
//
//        cout << "map_total_count_under_parents_config: " << endl;
//        for (const auto &mm: disc_node->map_total_count_under_parents_config) {
//            DiscreteConfig config = mm.first;
//            for (const auto &s: config) {
//                cout << s.first << " = " << s.second << " ";
//            }
//            cout << ": " << mm.second << ", ";
//        }
//        cout << endl;
//
//        cout << "num_potential_vals = " << disc_node->GetDomainSize() << endl;
//    }
//
//    cout << "vec_edges: size = " << network->vec_edges.size() << endl;
//}
//
