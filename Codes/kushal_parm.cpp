#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// Class to represent P-tree structure
class PTree {
public:
    int rootCount;
    vector<vector<int>> tree;

    PTree(int size) : rootCount(0) {
        tree.resize(size, vector<int>(size, 0));  // Initialize tree with the given grid size
    }

    // AND operation on P-trees
    static PTree AND(const PTree& ptree1, const PTree& ptree2) {
        int size = ptree1.tree.size();
        PTree result(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result.tree[i][j] = ptree1.tree[i][j] & ptree2.tree[i][j];
                result.rootCount += result.tree[i][j];  // Calculate the root count for the resulting tree
            }
        }
        return result;
    }

    // Initialize P-tree with data
    void buildTree(const vector<vector<int>>& data) {
        int size = data.size();
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                tree[i][j] = data[i][j];
                rootCount += data[i][j];  // Increment the root count as data is added
            }
        }
    }

    void printTree() const {
        cout << "P-tree root count: " << rootCount << endl;
        for (const auto& row : tree) {
            for (int val : row)
                cout << val << " ";
            cout << endl;
        }
    }
};

// Class to represent the PARM algorithm
class PARM {
private:
    vector<PTree> bandPtrees;  // P-trees for each spectral band
    int minSupport;
    int gridSize;

public:
    PARM(int gridSize, int minSupport) : gridSize(gridSize), minSupport(minSupport) {}

    // Build P-trees from the dataset
    void buildPTrees(const vector<vector<vector<int>>>& data) {
        for (const auto& bandData : data) {
            if (bandData.size() != gridSize || bandData[0].size() != gridSize) {
                cerr << "Error: Band data dimensions do not match expected grid size." << endl;
                return;
            }

            PTree ptree(gridSize);
            ptree.buildTree(bandData);
            bandPtrees.push_back(ptree);
        }
    }

    // Function to mine frequent itemsets and generate association rules
    void mineFrequentItemsets() {
        vector<vector<int>> frequentItemsets;
        cout << "Frequent Itemsets and Association Rules:" << endl;
        for (int i = 0; i < bandPtrees.size() - 1; i++) {
            for (int j = i + 1; j < bandPtrees.size(); j++) {
                // Perform AND operation between two bands
                PTree result = PTree::AND(bandPtrees[i], bandPtrees[j]);
                if (result.rootCount >= minSupport) {
                    // Print frequent itemsets
                    cout << "Frequent itemset found between band " << i+1 << " and band " << j+1 << ": Support = " << result.rootCount << endl;
                    result.printTree();
                    frequentItemsets.push_back({i, j});
                    
                    // Print association rule
                    cout << "Association Rule: Band " << i+1 << " -> Band " << j+1 << endl;
                    cout << "Association Rule: Band " << j+1 << " -> Band " << i+1 << endl;
                }
            }
        }
    }
};

// Function to read dataset from a file
vector<vector<vector<int>>> readDataset(const string& filename, int gridSize) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file." << endl;
        return {};
    }

    vector<vector<vector<int>>> data;
    string line;
    vector<vector<int>> bandData;

    while (getline(file, line)) {
        // Ignore empty lines or lines with extra whitespace characters
        if (line.empty() || line == "----") {
            if (!bandData.empty()) {
                data.push_back(bandData);  // Push current band data
                bandData.clear();          // Clear for next band
            }
        } else {
            istringstream iss(line);
            vector<int> row;
            int value;
            while (iss >> value) {
                row.push_back(value);
            }
            if (row.size() == gridSize) {
                bandData.push_back(row);  // Ensure the row matches the grid size
            } else {
                cerr << "Error: Data row size does not match expected grid size." << endl;
                return {};
            }
        }
    }
    // Push the last band data
    if (!bandData.empty()) {
        data.push_back(bandData);
    }

    if (data.empty()) {
        cerr << "Error: No valid data was read from the file." << endl;
    }

    return data;
}

// Main function to test the implementation
int main() {
    int gridSize = 4;  // Expected grid size of each band
    int minSupport = 2;  // Minimum support threshold

    // Read the dataset from file
    string filename = "spatial_data.txt";
    vector<vector<vector<int>>> dataset = readDataset(filename, gridSize);

    if (dataset.empty()) {
        cerr << "Error: Dataset is empty or invalid." << endl;
        return 1;
    }

    // Initialize the PARM algorithm
    PARM parm(gridSize, minSupport);

    // Build P-trees from the dataset
    parm.buildPTrees(dataset);

    // Mine frequent itemsets and generate association rules
    parm.mineFrequentItemsets();

    return 0;
}

