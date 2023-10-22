#include <fstream>
#include <sstream>
#include <queue>

#include "cPolygon.h"
#include "cGrouper.h"

void cGrouper::readfile(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open " + fname);

    std::string line;
    while (getline(ifs, line))
    {
        // std::cout << line << "\n";

        int p = line.find("MultiPolygon");
        if (p == -1)
            continue;
        int q = line.find("\"", p + 15);
        std::string mp = line.substr(p, q - 1);
        std::string details = line.substr(q);

        std::cout << mp.substr(16, 30) << "..." << mp.substr(mp.length() - 30) << "\n";
        std::cout << details << "\n";

        if (details.length() > 10)
        {
            std::vector<double> vd;
            std::stringstream sst(line);
            std::string a;
            while (getline(sst, a, ','))
                vd.push_back(atof(a.c_str()));
            vPolygon.emplace_back(vd);
        }

        continue;
    }
}

void cGrouper::readfile2(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open " + fname);

    std::string line;
    while (getline(ifs, line))
    {
        // std::cout << line << "\n";

        // the file is first delimited by semicolons
        std::vector<double> vtoken;
        std::stringstream sst(line);
        std::string a;
        while (getline(sst, a, ';'))
            vtoken.push_back(atof(a.c_str()));
    }
}


void cGrouper::bfs(int start)
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
    int sum = atof(g.rVertexAttr(start, 0).c_str());

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
                
            std::cout << "add potential " << g.userName( adj ) <<" " << val <<" " << sum + val << "\n";

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
        if ( ! fadj ) {
            
            /* All adjacent localities
                were not good candidates for the potential group
                abandom search to start again somewhere else
            */
            return;
        }
    }
}


std::string cGrouper::text()
{
    std::stringstream ss;
    ss << "Groups\n";
    for (auto &vm : vGroup)
    {
        ss << "============\n";
        double sum = 0;
        for (int v : vm)
        {
            ss << g.userName(v) << " ";
            sum += atof( g.rVertexAttr(v, 0).c_str() );
        }
        ss << " sum: " << sum << "\n";
    }
    return ss.str();
}