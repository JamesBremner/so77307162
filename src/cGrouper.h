
#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GraphTheory.h" // https://github.com/JamesBremner/PathFinder

/// @brief Assign localities ( municipalities ) to groups
class cGrouper
{
    struct sAlgoParams
    {
        double MinSum;
        double MaxSum;
        int MinSize;
        double MinSum2;
        double MaxSum2;
        int MinSize2;

        bool f2pass; // true if 2 passes required
        int pass;    // current pass

        sAlgoParams();

        /// @brief Check sanity of algorithm parameters, throw exception if fails

        void sanity();

        /// @brief Get parameters for current pass
        /// @param minSum
        /// @param maxSum
        /// @param minSize
        
        void getParams(
            double &minSum,
            double &maxSum,
            int &minSize) const;
    };

    sAlgoParams myAlgoParams;

    raven::graph::cGraph g;               // The links between touching localities
    std::vector<int> vRegion;             // Regions of the localities
    std::vector<int> vRegionInclude;      // Regions to be grouped, empty for all
    std::vector<bool> vMarked;            // The localities that have been assigned to groups
    std::vector<std::vector<int>> vGroup; // Groups of localities
    std::string myGroupListPath;
    std::string myAssignTablePath;

    void bfs(int start);
    void addSearch(const std::vector<bool> &visited);
    bool isGroupAcceptable(
        const std::vector<bool> &visited,
        double sum);
    bool isLocalIncluded(const ::std::string &slocRegion);
    std::string textStats();

    void layout();

    //////////////////////////////////
public:
    cGrouper();

    void readfileAdjancylist(const std::string &fname);
    void writeGroupList();
    void writeAssignTable();

    /// @brief Assign localities to groups
    void assign();

    int countAssigned();

    /// @brief generate layout image
    void display();

    std::string text(int region);

    void sanity();

    // Setters

    void regionsIncluded(const std::string &s);

    /// @brief algorithm parameters for 1st pass
    /// @param minSum
    /// @param maxSum
    /// @param minSize
    void algoParams(double minSum, double maxSum, int minSize);

    /// @brief algorithm parameters for 2nd pass
    /// @param f        true if 2nd pass required
    /// @param minSum
    /// @param maxSum
    /// @param minSize
    void passes(
        bool f,
        double minSum, double maxSum, int minSize);

    // Getters

    std::string regionsIncluded() const;
    sAlgoParams algoParams() const;
};
