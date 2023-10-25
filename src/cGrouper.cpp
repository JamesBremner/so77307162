#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>
#include <filesystem>

#include "cGraph.h"
#include "viz.h"

#include "cGrouper.h"

cGrouper::cGrouper()
    : myMinSum(-0.5),
      myMaxSum(0.5),
      myMinSize(0)
{
}

std::string cGrouper::regionsIncluded() const
{
    std::stringstream ss;
    for (int r : vRegionInclude)
        ss << r << " ";
    return ss.str();
}
double cGrouper::minSum() const
{
    return myMinSum;
}
double cGrouper::maxSum() const
{
    return myMaxSum;
}
int cGrouper::minSize() const
{
    return myMinSize;
}

void cGrouper::minSum(double v)
{
    myMinSum = v;
}
void cGrouper::maxSum(double v)
{
    myMaxSum = v;
}
void cGrouper::minSize(int v)
{
    myMinSize = v;
}

bool cGrouper::isLocalIncluded(const ::std::string &slocRegion)
{
    // check if all regions included
    if (!vRegionInclude.size())
        return true;

    return (std::find(
                vRegionInclude.begin(), vRegionInclude.end(),
                atoi(slocRegion.c_str())) != vRegionInclude.end());
}

void cGrouper::readfileAdjancylist(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open " + fname);

    g.clear();

    std::string line;
    std::vector<int> vlocalincluded;

    // identify localities in the included regions TID11
    if (vRegionInclude.size())
    {
        getline(ifs, line);
        while (getline(ifs, line))
        {
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

            // check if locality in region to be included
            if (isLocalIncluded(vtoken[3]))
                vlocalincluded.push_back(atoi(vtoken[0].c_str()));
        }
        std::cout << "finished 1st pass through input\n";

        // rewind input to start
        // ifs.seekg( 0, ifs.beg );
        ifs.close();
        ifs.open(fname);
    }

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
        if( ! isLocalIncluded(vtoken[3] ))
                continue;

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
        {
            // check if the adjacent locality is in the included regions TID11
            if (vRegionInclude.size())
            {
                if (std::find(
                        vlocalincluded.begin(), vlocalincluded.end(),
                        atoi(a.c_str())) == vlocalincluded.end())
                    continue;
            }

            // add the adjacent locality
            g.add(vtoken[0], a);
        }

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
        bool searchOK = false;
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
            sum += val;

            // check for acceptable group
            if (isGroupAcceptable(visited, sum))
            {
                // add this search to the groups
                addSearch(visited);

                // return to start a new search somewhere else
                return;
            }

            // update ongoing breadth first search
            queue.push(adj);
            searchOK = true;
        }

        // check potential group was added to
        if (!searchOK)
        {

            /* All adjacent localities
                were not good candidates for the potential group
                abandon search to start again somewhere else
            */
            return;
        }
    }
}

void cGrouper::addSearch(const std::vector<bool> &visited)
{
    std::vector<int> vm;
    for (int kv = 0; kv < g.vertexCount(); kv++)
        if (visited[kv])
        {
            vm.push_back(kv);
            vMarked[kv] = true;
        }
    vGroup.push_back(vm);
    std::cout << vGroup.size()<< " groups "
        << countAssigned() << " assigned\n";
}

bool cGrouper::isGroupAcceptable(
    const std::vector<bool> &visited,
    double sum)
{
    if (myMinSum > sum || sum > myMaxSum)
        return false;
    if (myMinSize > std::count(
                        visited.begin(), visited.end(),
                        true))
        return false;
    return true;
}

void cGrouper::assign()
{
    sanity();

    // clear assigned localities
    vMarked.clear();
    vMarked.resize(g.vertexCount(), false);

    vGroup.clear();

    // loop over localities, starting a modified BFS from each unassigned
    for (int start = 0; start < g.vertexCount(); start++)
    {
        bfs(start);
    }
}

void cGrouper::sanity()
{
    if (myMinSum > myMaxSum ||
        myMinSum * myMaxSum > 0)
        throw std::runtime_error(
            "Bad group sum range");
    if (myMinSize < 0)
        throw std::runtime_error(
            "Bad minumum group size");
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

    layout();
}

void cGrouper::layout()
{
    raven::graph::cViz vz;

    // check layout calculation will take a reasonable time
    if (g.vertexCount() > 100)
    {
        raven::graph::cGraph gtest;
        gtest.add(
            std::to_string(g.vertexCount()) + " too many localities to show layout");
        vz.viz(gtest);
        return;
    }

    std::vector<std::string> vColor{
        "red", "blue", "green", "aquamarine2", "chocolate2"};

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

std::string cGrouper::textStats()
{
     std::stringstream ss;
     ss << "Sum " << myMinSum << " to " <<  myMaxSum 
        <<", Min. Size " << myMinSize << "\n";
     ss << countAssigned() << " of " << g.vertexCount()
       << " localities assigned to " << vGroup.size()
       << " groups.\n";
    return ss.str();
}

std::string cGrouper::text(int region)
{
    if (!vGroup.size())
        return "\n\n\n     Use menu item File | Adjacency List to select input file";

    std::stringstream ss;
    ss << "\n" << textStats()
       << "All groups written to " << myGroupListPath
       << "\n\n";

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

static std::string readableTimeStamp()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char buf[100];
    strftime(buf, 99, "%Y_%b_%d_%H_%M_%S", timeinfo);
    return std::string(buf);
}

void cGrouper::writeGroupList()
{
    myGroupListPath =
        std::filesystem::current_path().string() +
        "\\grouplist" +
        readableTimeStamp() +
        ".txt";

    std::ofstream ofs(myGroupListPath);
    if (!ofs.is_open())
    {
        auto fn = myGroupListPath;
        myGroupListPath.clear();
        throw std::runtime_error(
            "Cannot open group list file " + fn);
    }

    ofs << textStats() << "\n";
    for (auto &vm : vGroup)
    {
        ofs << "============\n";
        double sum = 0;
        for (int v : vm)
        {
            ofs << g.userName(v)
                << " " << g.rVertexAttr(v, 0) << " ";
            sum += atof(g.rVertexAttr(v, 0).c_str());
        }
        ofs << "sum " << sum << "\n";
    }
}

void cGrouper::regionsIncluded(const std::string &s)
{
    vRegionInclude.clear();
    std::stringstream sst(s);
    std::string a;
    while (getline(sst, a, ' '))
        vRegionInclude.push_back(atoi(a.c_str()));
}
