//
// Created by jjt on 2021/7/2.
//

#include "CellTable.h"


/**
 * construct a new cell table using the given (fixed) dimensions
 * each dimension must be an integer greater than zero
 * @param dims: an array of length > 0, each element specifies the number of possible values of that dimension (> 0)
 */
CellTable::CellTable(vector<int> dims) {
    Reset(dims);
}

/**
 * resets the table, allowing a different dimensionality
 * all cells are reset to zero
 */
void CellTable::Reset(vector<int> dims) {
    if (!dims.empty()) {
        num_vars_tested = dims.size();

        // calculate length of cells[] array
        num_cells = 1;
        for (int dim : dims) {
            num_cells *= dim;
        }

        // reset cells array
        if (!cells.empty()) {
            for(auto it = cells.begin(); it != cells.end();) {
                cells.erase(it++);
            }
        }
        for (int i = 0; i < num_cells; ++i) {
            // set each counting value to 0
            cells.insert(pair<int, int>(i, 0));
        }

        // store the dimensions
        this->dims = dims;
    }
}

/**
 * construct a new cell for given indices, and traverse the data set to count the occurrence numbers
 * @param indices indices of a subset of features (nodes in the network structure) of the data set
 */
void CellTable::AddToTable(Dataset *dataset, vector<int> indices) {
    vector<int> dims;
    for (int i = 0; i < indices.size(); i++) {
        // get the number of possible values of each feature in indices, from Dataset.num_of_possible_values_of_disc_vars
        int dim = dataset->num_of_possible_values_of_disc_vars.at(indices.at(i));
        dims.push_back(dim);
    }
    Reset(dims); // reset a new cell table using the given dimensions

    vector<int> config;
    config.resize(indices.size());
    for (int i = 0; i < dataset->num_instance; i++) { // for each instance
        for (int j = 0; j < indices.size(); j++) { // for each feature in indices
            config.at(j) = dataset->dataset_all_vars[i][indices.at(j)];
//            coords.at(j) = dataset->vector_dataset_all_vars.at(i).at(indices.at(j)).second.GetInt();
        }
        Increment(config, 1);
    }
}

/**
 * increment the counting value at the given configuration by the specified amount
 * @param config the configuration at the table cell to update
 * @param value the amount by which the table cell of the configuration should be incremented (an integer)
 * @return the new counting value
 */
int CellTable::Increment(vector<int> config, int value) {
    int cell_index = GetCellIndex(config);

    cells[cell_index] += value;
    return cells[cell_index];
}

/**
 * @brief: get the cell index (the row of the table) according to a given configuration
 * @example: 3 features, dims = {2, 3, 2}
 *  cell table: 0 0 0 (row 0)
 *              0 0 1
 *              0 1 0
 *              0 1 1
 *              0 2 0
 *              0 2 1
 *              1 0 0
 *              1 0 1
 *              1 1 0
 *              1 1 1 (row 9)
 *              1 2 0
 *              1 2 1 (row 11 = 2 * 3 * 2 - 1)
 *  given config (x, y, z), the cell index should be (((0 * 2) + x) * 3 + y) * 2 + z
 *  given config (1, 1, 1), the cell index = (((0 * 2) + 1) * 3 + 1) * 2 + 1 = 9
 */
int CellTable::GetCellIndex(vector<int> config) { // TODO: no use
    int cell_index = 0;
    for (int i = 0; i < dims.size(); i++) {
        cell_index *= dims.at(i);
        cell_index += config.at(i);
    }
    return cell_index;
}

/**
 * get the count value of the configuration
 */
long CellTable::GetValue(vector<int> config) {
    return cells[GetCellIndex(config)];
}

/**
 * @brief: calculate a marginal sum for the cell table
 * the variables over which marginal sums should be taken are indicated by placing "-1"
 * in the appropriate positions in the configuration argument
 * @example: to find the margin for v0 = 1, v1 = 3, and v3 = 2,
 *           where the marginal sum ranges over all values of v2 and v4:
 *           the array [1, 3, -1, 2, -1] should be used
 * @param config: a vector of the sort described above.
 * @return the marginal sum specified
 */
long CellTable::ComputeMargin(vector<int> config) {
    // make a copy of the config vector so that the original is not messed up
    config_copy = config;

    long sum = 0;
    int i = -1;

    while (++i < config_copy.size()) { // for each position of the config
        if (config_copy.at(i) == -1) { // the variable of position i is to be marginalized
            for (int j = 0; j < dims.at(i); j++) { // for all possible values of position i
                config_copy.at(i) = j; // fill each possible value in position i
                sum += ComputeMargin(config_copy);
            }
            config_copy[i] = -1;
            return sum;
        }
    } // until every variable is specified a value, then we can obtain the counting value and return
    return GetValue(config_copy);
}

/**
 * an alternative way to specify a marginal calculation
 * config specifies a particular cell in the table,
 * and margin_vars contains the indices of the variables over which the margin sum should be calculated
 * the sum is over the cell specified by 'coord' and
 * all of the cells which differ from that cell in any of the specified coordinates
 */
long CellTable::ComputeMargin(vector<int> config, vector<int> margin_vars) {
    // make a copy of the config vector so that the original is not messed up
    config_copy = config;
    for (int margin_var : margin_vars) {
        config_copy.at(margin_var) = -1;
    }
    return ComputeMargin(config_copy);
}