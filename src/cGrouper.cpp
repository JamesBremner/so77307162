#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>

#include "cGraph.h"
#include "viz.h"

#include "cPolygon.h"
#include "cGrouper.h"

// void cGrouper::readfile(const std::string &fname)
// {
//     std::ifstream ifs(fname);
//     if (!ifs.is_open())
//         throw std::runtime_error(
//             "Cannot open " + fname);

//     std::string line;
//     while (getline(ifs, line))
//     {
//         // std::cout << line << "\n";

//         int p = line.find("MultiPolygon");
//         if (p == -1)
//             continue;
//         int q = line.find("\"", p + 15);
//         std::string mp = line.substr(p, q - 1);
//         std::string details = line.substr(q);

//         std::cout << mp.substr(16, 30) << "..." << mp.substr(mp.length() - 30) << "\n";
//         std::cout << details << "\n";

//         if (details.length() > 10)
//         {
//             std::vector<double> vd;
//             std::stringstream sst(line);
//             std::string a;
//             while (getline(sst, a, ','))
//                 vd.push_back(atof(a.c_str()));
//             vPolygon.emplace_back(vd);
//         }

//         continue;
//     }
// }

void cGrouper::readfileAdjancylist(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open " + fname);

    g.clear();

    std::string line;
    getline(ifs, line);
    while (getline(ifs, line))
    {
        // std::cout << line << "\n";

        // the file is first delimited by semicolons
        std::vector<std::string> vtoken;
        std::stringstream sst(line);
        std::string a;
        while (getline(sst, a, ';'))
            vtoken.push_back(a);

        if (vtoken.size() != 4)
        {
            std::cout << line << "\n";
            throw std::runtime_error(
                "Format error in adjancies file");
        }

        // std::cout << vtoken[0] << "\n";
        // std::cout << vtoken[1] << "\n";
        // std::cout << vtoken[2] << "\n================\n";

        // check if locality in region to be included
        if (vRegionInclude.size())
        {
            if (std::find(
                    vRegionInclude.begin(), vRegionInclude.end(),
                    atoi(vtoken[3].c_str())) == vRegionInclude.end())
                continue;
        }

        int vi = g.find(vtoken[0]);
        if (vi < 0)
            vi = g.add(vtoken[0]);

        // the decimal point is shown as a comma
        int p = vtoken[2].find(",");
        vtoken[2] = vtoken[2].substr(0, p) +
                    "." + vtoken[2].substr(p + 1);
        g.wVertexAttr(
            vi,
            {vtoken[2]});

        // the adjacencies are space delimited
        std::stringstream ssta(vtoken[1]);
        while (getline(ssta, a, ' '))
            g.add(vtoken[0], a);

        if (!(g.vertexCount() % 1000))
            std::cout << "read " << g.vertexCount() << " localities\n";

        vRegion.push_back(atoi(vtoken[3].c_str()));
    }
    std::cout << "finished reading " << g.vertexCount() << " localities\n";
}

void cGrouper::bfs(int start)
{
    // check start already assigned to a group
    if (vMarked[start])
        return;

    // Mark all the vertices as not visited
    std::vector<bool> visited;
    visited.resize(g.vertexCount(), false);

    // Create a queue for BFS
    std::queue<int> queue;

    // Mark the current node as visited and enqueue it
    visited[start] = true;
    queue.push(start);
    double sum = atof(g.rVertexAttr(start, 0).c_str());

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

            // std::cout << "add potential " << g.userName( adj ) <<" " << val <<" " << sum + val << "\n";

            // add adjacent locality to potential group
            visited[adj] = true;

            // check for acceptable group
            if (std::fabs(sum + val) < 0.5)
            {
                // add this search to the groups
                std::vector<int> vm;
                for (int kv = 0; kv < g.vertexCount(); kv++)
                    if (visited[kv])
                    {
                        vm.push_back(kv);
                        vMarked[kv] = true;
                    }
                vGroup.push_back(vm);

                // return to start a new search somewhere else
                return;
            }

            // update ongoing breadth first search
            sum += val;
            queue.push(adj);
            fadj = true;
        }

        // check potential gropup was added to
        if (!fadj)
        {

            /* All adjacent localities
                were not good candidates for the potential group
                abandon search to start again somewhere else
            */
            return;
        }
    }
}

void cGrouper::assign()
{
    vMarked.resize(g.vertexCount(), false);
    for (int k = 0; k < g.vertexCount(); k++)
    {
        bfs(k);
    }
}

int cGrouper::countAssigned()
{
    return std::count(
        vMarked.begin(), vMarked.end(),
        true);
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
        double sum = 0;
        for (int v : vm)
        {
            std::cout << g.userName(v)
                      << " " << g.rVertexAttr(v, 0) << " ";
            sum += atof(g.rVertexAttr(v, 0).c_str());
        }
        std::cout << "sum " << sum << "\n";
    }

    if (g.vertexCount() > 100)
        return;

    std::vector<std::string> vColor{
        "red", "blue", "green", "aquamarine2", "chocolate2"};
    raven::graph::cViz vz;
    vz.setVertexColor(
        [this, &vColor](int v)
        {
            for (int k = 0; k < vGroup.size(); k++)
            {
                if (std::find(vGroup[k].begin(), vGroup[k].end(), v) != vGroup[k].end())
                {
                    if (k < vColor.size())
                        return std::string(", color = ") + vColor[k];
                    else
                        return std::string("");
                }
            }
            return std::string("");
        });
    vz.viz(g);
}

std::string cGrouper::text(int region)
{
    if (!vGroup.size())
        return "\n\n\n     Use menu item File | Adjacency List to select input file";

    std::stringstream ss;
    ss << "\n"
       << countAssigned() << " of " << g.vertexCount()
       << " localities assigned\n\n";
    ss << "Groups in region " << region << "\n";
    for (auto &vm : vGroup)
    {
        if (vRegion[vm[0]] != region)
            continue;

        double sum = 0;
        for (int v : vm)
        {
            ss << g.userName(v) << " ";
            sum += atof(g.rVertexAttr(v, 0).c_str());
        }
        ss << " sum: " << sum << "\n";
    }
    return ss.str();
}

std::string cGrouper::regionsIncluded() const
{
    std::stringstream ss;
    for (int r : vRegionInclude)
        ss << r << " ";
    return ss.str();
}
void cGrouper::regionsIncluded(const std::string &s)
{
    vRegionInclude.clear();
    std::stringstream sst(s);
    std::string a;
    while (getline(sst, a, ' '))
        vRegionInclude.push_back(atoi(a.c_str()));
}
