#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <filesystem>

#include <wex.h> // https://github.com/JamesBremner/windex
#include <window2file.h>

#include "cStarterGUI.h"

#include "cPolygon.h"

#include "GraphTheory.h" // https://github.com/JamesBremner/PathFinder
#include "viz.h"

class cFinder
{
    raven::graph::cGraph g;
    std::vector<bool> vMarked;
    std::vector<std::vector<int>> vMuni;
    int sum;

    void bfs(int start);

    //////////////////////////////////
public:
    void generateRandom(int range, int width, int height);
    void generate1();

    /// @brief Assign localities to groups
    void assign();

    void display();
};

void cFinder::generateRandom(int vertexNumber, int edgeNumber, int range)
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
void cFinder::generate1()
{
}

void cFinder::bfs(int start)
{
    // check start already assigned to a group
    if( vMarked[start])
        return;

    // Mark all the vertices as not visited
    std::vector<bool> visited;
    visited.resize(g.vertexCount(), false);

    // Create a queue for BFS
    std::queue<int> queue;

    // Mark the current node as visited and enqueue it
    visited[start] = true;
    queue.push(start);
    sum = atof(g.rVertexAttr(start, 0).c_str());

    while (!queue.empty())
    {
        // Dequeue a vertex from queue
        int s = queue.front();
        queue.pop();

        // loop over adjacent vertices ( touching localities )
        bool fadj = false;
        for (int adj : g.adjacentOut(s))
        {
            // ignore localities assigned to previous groups
            if (vMarked[adj])
                continue;

            // ignore localities already visited in this search
            if (visited[adj])
                continue;

            double val = atof(g.rVertexAttr(adj, 0).c_str());

            // ignore localities that take sum further away from zero
            if (sum * val > 0)
                continue;

            // add adjacent locality to potential group
            visited[adj] = true;
            
            // check for acceptable group
            if (abs(sum + val) < 0.5)
            {
                // add this search to the groups
                std::vector<int> vm;
                for (int kv = 0; kv < g.vertexCount(); kv++)
                    if (visited[kv])
                    {
                        vm.push_back(kv);
                        vMarked[kv] = true;
                    }
                vm.push_back(adj);
                vMuni.push_back(vm);

                // return to start a new search somewhere else
                return;
            }

            // update ongoing breadth first search
            sum += val;
            queue.push(adj);
            fadj = true;
        }

        // check potential gropup was added to
        if ( ! fadj ) {
            
            /* All adjacent localities
                were not good candidates for the potential group
                abandom search to start again somewhere else
            */
            return;
        }
    }
}

void cFinder::assign()
{
    vMarked.resize(g.vertexCount(), false);
    for (int k = 0; k < g.vertexCount(); k++)
    {
        bfs(k);
    }
}

void cFinder::display()
{

    for (int v = 0; v < g.vertexCount(); v++)
    {
        std::cout << g.userName(v) << " " << g.rVertexAttr(v, 0) << "\n";
    }
    for (auto &l : g.edgeList())
    {
        std::cout << g.userName(l.first)
                  << " " << g.userName(l.second)
                  << "\n";
    }
    for (auto &vm : vMuni)
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
            for (int k = 0; k < vMuni.size(); k++)
            {
                if (std::find(vMuni[k].begin(), vMuni[k].end(), v) != vMuni[k].end())
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

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500})
    {
        muni.generateRandom(40, 60, 3);
        muni.assign();
        muni.display();

        fm.events().draw(
            [this](PAINTSTRUCT &ps)
            {
                wex::window2file w2f;
                auto path = std::filesystem::temp_directory_path();
                auto sample = path / "sample.png";
                w2f.draw(fm, sample.string());
            });

        show();
        run();
    }

private:
    cFinder muni;
};

main()
{
    cGUI theGUI;
    return 0;
}
