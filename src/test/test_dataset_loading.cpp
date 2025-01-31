////
//// Created by jjt on 25/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "fastpgm/Dataset.h"
//#include "fastpgm/test_common.h"
//
//
//class TestCSVTrainingLoading : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//    }
//    Dataset *trainer;
//};
//
//TEST_F(TestCSVTrainingLoading, alarm) {
//    string train_set_file = dataset_path + "alarm/alarm_s5000.txt";
//    trainer->LoadCSVTrainingData(train_set_file, true, 0);
//
//    cout << "num_instance = " << trainer->num_instance << endl;
//    cout << "num_vars = " << trainer->num_vars << endl;
//    cout << "class_var_index = " << trainer->class_var_index << endl;
//
//    cout << "vec_var_names: ";
//    for (int i = 0; i < trainer->vec_var_names.size(); i++) {
//        cout << trainer->vec_var_names[i] << " ";
//    }
//    cout << endl;
//
//    cout << "vars_possible_values_ids: " << endl;
//    for (int i = 0; i < trainer->vars_possible_values_ids.size(); i++) {
//        cout << i << ": ";
//        for (const auto &m: trainer->vars_possible_values_ids[i]) {
//            cout << m.first << ", " << m.second << "; ";
//        }
//        cout << endl;
//    }
//
//    cout << "num_of_possible_values_of_disc_vars: ";
//    for (int i = 0; i < trainer->num_of_possible_values_of_disc_vars.size(); i++) {
//        cout << trainer->num_of_possible_values_of_disc_vars[i] << " ";
//    }
//    cout << endl;
//
//    cout << "dataset_all_vars:" << endl;
//    for (int i = 0; i < trainer->num_instance; i++) {
//        for (int j = 0; j < trainer->num_vars; j++) {
//            cout << trainer->dataset_all_vars[i][j] << " ";
//        }
//        cout << endl;
//    }
//
////    cout << "dataset_columns: " << endl;
////    for (int i = 0; i < trainer->num_vars; i++) {
////        for (int j = 0; j < trainer->num_instance; j++) {
////            cout << trainer->dataset_columns[i][j] << " ";
////        }
////        cout << endl;
////    }
////
////    cout << "vector_dataset_all_vars: " << endl;
////    for (int i = 0; i < trainer->vector_dataset_all_vars.size(); i++) {
////        for (int j = 0; j < trainer->vector_dataset_all_vars[i].size(); j++) {
////            VarVal vv = trainer->vector_dataset_all_vars[i][j];
////            cout << vv.first << ", " << vv.second.GetInt() << " ";
////        }
////        cout << endl;
////    }
////
////    for (int i = 0; i < trainer->num_instance; i++) {
////        for (int j = 0; j < trainer->num_vars; j++) {
////            EXPECT_EQ(trainer->dataset_all_vars[i][j], trainer->dataset_columns[j][i]);
////        }
////    }
//
//    SAFE_DELETE(trainer);
//}