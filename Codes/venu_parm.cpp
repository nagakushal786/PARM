#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <utility>

using namespace std;

// Class to represent P-tree structure
class PTree {
public:
    int rootCount;
    vector<vector<int>> tree;

    PTree(int size) : rootCount(0) {
        tree.resize(size, vector<int>(size, 0));
    }

    // AND operation on P-trees
    static PTree AND(const PTree& ptree1, const PTree& ptree2) {
        int size = ptree1.tree.size();
        PTree result(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result.tree[i][j] = ptree1.tree[i][j] & ptree2.tree[i][j];
                result.rootCount += result.tree[i][j];
            }
        }
        return result;
    }

    // Initialize P-tree with data (for simplicity, assume binary data)
    void buildTree(const vector<vector<int>>& data) {
        int size = data.size();
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                tree[i][j] = data[i][j];
                rootCount += data[i][j];
            }
        }
    }

    void printTree() {
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
    double minConfidence;
    int gridSize;
    vector<pair<vector<int>, int>> frequentItemsets;  // Store frequent itemsets and their support

public:
    PARM(int gridSize, int minSupport, double minConfidence) 
        : gridSize(gridSize), minSupport(minSupport), minConfidence(minConfidence) {}

    // Build P-trees from the dataset
    void buildPTrees(const vector<vector<vector<int>>>& data) {
        for (const auto& bandData : data) {
            PTree ptree(gridSize);
            ptree.buildTree(bandData);
            bandPtrees.push_back(ptree);
        }
    }

    // Function to mine frequent itemsets
    void mineFrequentItemsets() {
        // For simplicity, assume a 2-band association rule mining scenario
        for (int i = 0; i < bandPtrees.size() - 1; i++) {
            for (int j = i + 1; j < bandPtrees.size(); j++) {
                // Perform AND operation between two bands
                PTree result = PTree::AND(bandPtrees[i], bandPtrees[j]);
                if (result.rootCount >= minSupport) {
                    cout << "Frequent itemset found between band " << i << " and band " << j << endl;
                    result.printTree();
                    frequentItemsets.push_back({{i, j}, result.rootCount});
                }
            }
        }
    }

    // Function to generate association rules from frequent itemsets
    void generateAssociationRules() {
        cout << "\nAssociation Rules:\n";
        for (const auto& itemsetSupportPair : frequentItemsets) {
            const vector<int>& itemset = itemsetSupportPair.first;
            int supportXY = itemsetSupportPair.second;

            // For each frequent itemset, generate rules in the form X -> Y
            for (int item : itemset) {
                vector<int> X, Y;
                // Split the itemset into X and Y where Y is a single item
                for (int elem : itemset) {
                    if (elem == item) {
                        Y.push_back(elem);
                    } else {
                        X.push_back(elem);
                    }
                }

                // Calculate support of X (assume single item for simplicity)
                int supportX = bandPtrees[X[0]].rootCount;

                // Calculate confidence and check if it's above the threshold
                double confidence = static_cast<double>(supportXY) / supportX;
                if (confidence >= minConfidence) {
                    // Print the rule
                    cout << "Rule: {";
                    for (int xi : X) cout << xi << " ";
                    cout << "} -> {";
                    for (int yi : Y) cout << yi << " ";
                    cout << "} with confidence: " << confidence * 100 << "%\n";
                }
            }
        }
    }
};

// Main function to test the implementation
int main() {
    int gridSize = 4;
    int minSupport = 2;
    double minConfidence = 0.6;  // Set the minimum confidence threshold

    // Example dataset (binary values for simplicity)
    vector<vector<vector<int>>> dataset = {
        {{1, 0, 0, 1}, {0, 1, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 0}},  // Band 1
        {{0, 1, 0, 0}, {1, 0, 1, 0}, {1, 1, 0, 1}, {0, 1, 0, 1}},  // Band 2
        {{1, 1, 1, 0}, {0, 1, 0, 0}, {1, 0, 1, 1}, {1, 0, 0, 0}}   // Band 3
    };

    // Initialize the PARM algorithm
    PARM parm(gridSize, minSupport, minConfidence);

    // Build P-trees from the dataset
    parm.buildPTrees(dataset);

    // Mine frequent itemsets based on P-tree AND operation
    parm.mineFrequentItemsets();

    // Generate association rules from the frequent itemsets
    parm.generateAssociationRules();

    return 0;
}
