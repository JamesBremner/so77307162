
#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GraphTheory.h" // https://github.com/JamesBremner/PathFinder


/// @brief Assign localities ( municipalities ) to groups
class cGrouper
{
    raven::graph::cGraph g;                     // The links between touching localities
    std::vector<int> vRegion;                   // Regions of the localities
    std::vector<int> vRegionInclude;            // Regions to be grouped, empty for all
    std::vector<bool> vMarked;                  // The localities that have been assigned to groups
    std::vector<std::vector<int>> vGroup;       // Groups of localities

    void bfs(int start);
    void addSearch( const std::vector<bool>& visited );

    //////////////////////////////////
public:
    void generateRandom(int range, int width, int height);
    void readfile( const std::string& fname );
    void readfileAdjancylist( const std::string& fname );

    /// @brief Assign localities to groups
    void assign();

    int countAssigned();

    /// @brief generate layout image
    void display();

    std::string text( int region );

    std::string regionsIncluded() const;
    void regionsIncluded( const std::string& s );
};
