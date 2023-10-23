
#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GraphTheory.h" // https://github.com/JamesBremner/PathFinder

#include "cPolygon.h"




/// @brief Assign localities ( municipalities ) to groups
class cGrouper
{
    std::vector<cPolygon> vPolygon;
    raven::graph::cGraph g;                     // The links between touching localities
    std::vector<bool> vMarked;                  // The localities that have been assigned to groups
    std::vector<std::vector<int>> vGroup;       // Groups of localities

    void bfs(int start);

    //////////////////////////////////
public:
    void generateRandom(int range, int width, int height);
    void genPolygon1();
    void generate1();
    void readfile( const std::string& fname );
    void readfileAdjancylist( const std::string& fname );

    /// @brief Assign localities to groups
    void assign();

    int countAssigned();

    /// @brief generate layout image
    void display();

    std::string text();
};
