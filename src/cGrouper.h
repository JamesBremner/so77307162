
#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GraphTheory.h" // https://github.com/JamesBremner/PathFinder


/// @brief Assign localities ( municipalities ) to groups
class cGrouper
{
    double myMinSum;
    double myMaxSum;
    int myMinSize;
    raven::graph::cGraph g;                     // The links between touching localities
    std::vector<int> vRegion;                   // Regions of the localities
    std::vector<int> vRegionInclude;            // Regions to be grouped, empty for all
    std::vector<bool> vMarked;                  // The localities that have been assigned to groups
    std::vector<std::vector<int>> vGroup;       // Groups of localities
    std::string myGroupListPath;

    void bfs(int start);
    void addSearch( const std::vector<bool>& visited );
    bool isGroupAcceptable(
        const std::vector<bool>& visited,
        double sum     );
    bool isLocalIncluded( const::std::string& slocRegion);
    std::string textStats();

    void layout();

    //////////////////////////////////
public:
    
    cGrouper();

    void readfileAdjancylist( const std::string& fname );
    void writeGroupList();

    /// @brief Assign localities to groups
    void assign();

    int countAssigned();

    /// @brief generate layout image
    void display();

    std::string text( int region );

    /// @brief Check sanity of algorithm parameters, throw exception if fails
    void sanity();

    // Setters

    void regionsIncluded( const std::string& s );
    void minSum( double v );
    void maxSum( double v );
    void minSize( int v );

    // Getters

    std::string regionsIncluded() const;
    double minSum() const;
    double maxSum() const;
    int minSize() const;
};
