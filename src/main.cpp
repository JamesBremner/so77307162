#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <wex.h>
#include "cStarterGUI.h"
#include "GraphTheory.h" // https://github.com/JamesBremner/PathFinder

class cMuni
{
};

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

    void make();

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
    // Mark all the vertices as not visited
    std::vector<bool> visited;
    visited.resize(g.vertexCount(), false);

    // Create a queue for BFS
    std::queue<int> queue;

    // Mark the current node as visited and enqueue it
    visited[start] = true;
    queue.push(start);
    sum = atof(g.rVertexAttr(start, 0).c_str());
    int s = start;

    while (!queue.empty())
    {

        // Dequeue a vertex from queue and print it
        s = queue.front();
        queue.pop();

        for (auto adjacent : g.adjacentOut(s))
        {
            if ((!visited[adjacent]) && (!vMarked[adjacent]))
            {
                sum += atof(g.rVertexAttr(adjacent, 0).c_str());
                if (abs(sum) < 0.5)
                {
                    std::vector<int> vm;
                    for (int kv = 0; kv < g.vertexCount(); kv++)
                        if (visited[kv])
                        {
                            vm.push_back(kv);
                            vMarked[kv] = true;
                        }
                    vm.push_back(adjacent);
                    vMuni.push_back(vm);
                    return;
                }
                visited[adjacent] = true;
                queue.push(adjacent);
            }
        }
    }
}

void cFinder::make()
{
    vMarked.resize(g.vertexCount(), false);
    for (int k = 0; k < 5; k++)
    {
        bfs(rand() % g.vertexCount());
    }
}

void cFinder::display()
{
    // auto& v = vMuni[0];
    // for( auto& l : g.edgeList() )
    // {
    //     std::cout << g.userName(l.first)
    //         << " " << g.userName(l.second );

    //     if(
    //         (std::find( v.begin(),v.end(),l.first) != v.end() )
    //         &&
    //         (std::find( v.begin(),v.end(),l.second) != v.end() ))
    //         std::cout << " ***";
    //     std::cout << "\n";
    // }

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
        for (int v : vm)
        {
            std::cout << g.userName(v) << " " << g.rVertexAttr(v, 0) << "\n";
        }
        std::cout << "============\n";
    }
}

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          lb(wex::maker::make<wex::label>(fm))
    {
        muni.generateRandom(20, 20, 3);
        muni.make();
        muni.display();

        show();
        run();
    }

private:
    wex::label &lb;
    cFinder muni;
};

main()
{
    cGUI theGUI;
    return 0;
}
