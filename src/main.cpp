#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>



#include <wex.h> // https://github.com/JamesBremner/windex
#include <window2file.h>

#include "cGraph.h"
#include "viz.h" 

#include "cPolygon.h"
#include "cGrouper.h"
#include "cGUI.h"

void cGrouper::generateRandom(int vertexNumber, int edgeNumber, int range)
{
    for (int v = 0; v < vertexNumber; v++)
    {
        g.add(std::to_string(v));
        double d = (rand() % (2 * range)) - range;
        g.wVertexAttr(
            v,
            {std::to_string(d)});
    }

    while (g.edgeCount() < edgeNumber)
        g.add(
            rand() % vertexNumber,
            rand() % vertexNumber);
}
void cGrouper::generate1()
{
}

void cGrouper::assign()
{
    vMarked.resize(g.vertexCount(), false);
    for (int k = 0; k < g.vertexCount(); k++)
    {
        bfs(k);
    }
}

void cGrouper::display()
{

    // for (int v = 0; v < g.vertexCount(); v++)
    // {
    //     std::cout << g.userName(v) << " " << g.rVertexAttr(v, 0) << "\n";
    // }
    // for (auto &l : g.edgeList())
    // {
    //     std::cout << g.userName(l.first)
    //               << " " << g.userName(l.second)
    //               << "\n";
    // }
    for (auto &vm : vGroup)
    {
        std::cout << "============\n";
        for (int v : vm)
        {
            std::cout << g.userName(v) << " " << g.rVertexAttr(v, 0) << "\n";
        }
    }

    std::vector<std::string> vColor {
        "red","blue","green","aquamarine2","chocolate2"    };
    raven::graph::cViz vz;
    vz.setVertexColor(
        [this,&vColor](int v)
        {
            for (int k = 0; k < vGroup.size(); k++)
            {
                if (std::find(vGroup[k].begin(), vGroup[k].end(), v) != vGroup[k].end())
                {
                    if( k < vColor.size() )
                        return std::string(", color = ") + vColor[k];
                    else
                        return std::string("");
                }
            }
            return std::string("");
        });
    vz.viz(g);
}

main()
{
    cGUI theGUI;
    return 0;
}
