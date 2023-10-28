#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>
#include <filesystem>

#include "cGraph.h"
#include "viz.h"

#include "cGrouper.h"

double cGroup::deficitSum(const raven::graph::cGraph &g)
{
    if (!fSumValid)
    {
        myDeficitSum = 0;
        for (int loc : myMembers)
            myDeficitSum += atof(g.rVertexAttr(loc, 0).c_str());
    }

    return myDeficitSum;
}

bool cGroup::isMember(int loc) const
{
    return (std::find(myMembers.begin(), myMembers.end(), loc) != myMembers.end());
}

std::string cGroup::text(const raven::graph::cGraph &g)
{
    std::stringstream ss;
    for (int v : myMembers)
    {
        ss << g.userName(v)
           << " " << g.rVertexAttr(v, 0) << " ";
    }
    ss << "sum " << deficitSum(g) << "\n";
    return ss.str();
}

cGrouper::cGrouper()
{
}

std::string cGrouper::regionsIncluded() const
{
    std::stringstream ss;
    for (int r : vRegionInclude)
        ss << r << " ";
    return ss.str();
}
cAlgoParams &cGrouper::algoParams()
{
    return myAlgoParams;
}

void cGrouper::algoParams(double minSum, double maxSum, int minSize)
{
    myAlgoParams.MinSum = minSum;
    myAlgoParams.MaxSum = maxSum;
    myAlgoParams.MinSize = minSize;
}
void cGrouper::passes(
    bool f,
    double minSum, double maxSum, int minSize)
{
    myAlgoParams.f2pass = f;
    if (f)
    {
        myAlgoParams.MinSum2 = minSum;
        myAlgoParams.MaxSum2 = maxSum;
        myAlgoParams.MinSize2 = minSize;
    }
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
    if (fname.empty())
        return;

    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open " + fname);

    g.clear();
    avLocalDeficit = 0;

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
            {
                vlocalincluded.push_back(atoi(vtoken[0].c_str()));
                avLocalDeficit += atof(vtoken[2].c_str());
            }
        }
        avLocalDeficit /= vlocalincluded.size();

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
        if (!isLocalIncluded(vtoken[3]))
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

void cGrouper::bfs(
    int start)
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

            // the deficit value
            double val = atof(g.rVertexAttr(adj, 0).c_str());

            // ignore localities that take sum further away from target
            if (fabs(sum + val - myAlgoParams.trgSum) >
                fabs(sum - myAlgoParams.trgSum))
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
    cGroup vm;
    for (int kv = 0; kv < g.vertexCount(); kv++)
        if (visited[kv])
        {
            vm.add(kv);
            vMarked[kv] = true;
        }
    vGroup.push_back(vm);
}

bool cGrouper::isGroupAcceptable(
    const std::vector<bool> &visited,
    double sum)
{
    double MinSum, MaxSum;
    int MinSize;
    myAlgoParams.getParams(MinSum, MaxSum, MinSize);

    double delta = sum - myAlgoParams.trgSum;
    if (MinSum > delta || delta > MaxSum)
        return false;
    if (MinSize > std::count(
                      visited.begin(), visited.end(),
                      true))
        return false;
    return true;
}

void cGrouper::sanity()
{
    myAlgoParams.sanity();
}

void cGrouper::assign()
{
    myAlgoParams.sanity();
    myAlgoParams.pass = 1;

    // clear assigned localities
    vMarked.clear();
    vMarked.resize(g.vertexCount(), false);
    vGroup.clear();

    if (myAlgoParams.fComp)
    {
        // loop over components TID15
        std::cout << "Detecting components\n";
        auto vComp = components(g);
        std::cout << vComp.size() << " components detected\n";
        for (auto &comp : vComp)
        {
            // check if component small enough to be a group
            if (comp.size() < myAlgoParams.MinSize)
            {
                std::vector<bool> visited(g.vertexCount(), false);
                for (int vi : comp)
                    visited[vi] = true;
                addSearch(visited);
                continue; // to next component
            }

            std::cout << "Assign Pass 1\n";
            myAlgoParams.pass = 1;

            // loop over localities, starting a modified BFS from each unassigned in the component
            for (int start = 0; start < g.vertexCount(); start++)
            {
                // check start is in component
                if (std::find(
                        comp.begin(), comp.end(), start) != comp.end())
                    bfs(start);
            }

            if (!myAlgoParams.f2pass)
                continue;

            std::cout << "Assign Pass 2\n";
            myAlgoParams.pass = 2;

            // loop over localities, starting a modified BFS from each unassigned
            for (int start = 0; start < g.vertexCount(); start++)
            {
                // check start is in component
                if (std::find(
                        comp.begin(), comp.end(), start) != comp.end())
                    bfs(start);
            }
        }
    }
    else
    {
        std::cout << "Assign Pass 1\n";
        myAlgoParams.pass = 1;

        // loop over localities, starting a modified BFS from each unassigned locality
        for (int start = 0; start < g.vertexCount(); start++)
            bfs(start);

        if (myAlgoParams.f2pass)
        {
            std::cout << "Assign Pass 2\n";
            myAlgoParams.pass = 2;

            // loop over localities, starting a modified BFS from each unassigned
            for (int start = 0; start < g.vertexCount(); start++)
                bfs(start);
        }
    }
    std::cout << vGroup.size() << " groups assigned\n";
}

cAlgoParams::cAlgoParams()
    : trgSum(0),
      MinSum(-5),
      MaxSum(5),
      MinSize(5),
      MinSum2(-25),
      MaxSum2(25),
      MinSize2(2),
      f2pass(false),
      pass(1),
      fComp(false)
{
}

void cAlgoParams::sanity()
{
    // if (fabs(trgSum) > 0.01)
    //     throw std::runtime_error(
    //         "Non-zero group sum target NYI");
    if (MinSum > MaxSum)
        throw std::runtime_error(
            "Bad group sum range");
    if (MinSize < 0)
        throw std::runtime_error(
            "Bad minumum group size");
}

void cAlgoParams::getParams(
    double &minSum,
    double &maxSum,
    int &minSize) const
{
    switch (pass)
    {
    case 1:
        minSum = MinSum;
        maxSum = MaxSum;
        minSize = MinSize;
        break;
    case 2:
        minSum = MinSum2;
        maxSum = MaxSum2;
        minSize = MinSize2;
        break;
    default:
        throw std::runtime_error(
            "sAlgoParams::getParam bad pass");
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
    for (auto &grp : vGroup)
    {
        std::cout << "============\n";
        std::cout << grp.text(g) << "\n";
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
                if (vGroup[k].isMember(v))
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
    auto ap = algoParams();
    ss << "Sum " << ap.trgSum + ap.MinSum << " to " << ap.trgSum + ap.MaxSum
       << ", Min. Size " << ap.MinSize << "\n";
    ss << countAssigned() << " of " << g.vertexCount()
       << " localities assigned to " << vGroup.size()
       << " groups. Average deficit " << avLocalDeficit
       << ".\n";
    return ss.str();
}

std::string cGrouper::text(int region)
{
    return "NYI\n";
    // if (!vGroup.size())
    //     return "\n\n\n     Use menu item File | Adjacency List to select input file";

    // std::stringstream ss;
    // ss << "\n"
    //    << textStats()
    //    << "All groups written to " << myGroupListPath
    //    << "\nAll localities written to " << myAssignTablePath
    //    << "\n\n";

    // ss << "Groups in region " << region << "\n";

    // for (auto &vm : vGroup)
    // {
    //     if (vRegion[vm[0]] != region)
    //         continue;

    //     double sum = 0;
    //     for (int v : vm)
    //     {
    //         ss << g.userName(v) << " ";
    //         sum += atof(g.rVertexAttr(v, 0).c_str());
    //     }
    //     ss << " sum: " << sum << "\n";
    // }
    // return ss.str();
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
    for (auto &grp : vGroup)
    {
        ofs << "============\n";
        ofs << grp.text(g) << "\n";
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

void cGrouper::writeAssignTable()
{
    std::vector<std::vector<std::string>> table;
    int groupID = 0;
    for (auto &grp : vGroup)
    {
        for (int loc : grp)
        {
            std::vector<std::string> row;
            row.push_back(g.userName(loc));
            row.push_back(g.rVertexAttr(loc, 0));
            row.push_back(std::to_string(groupID));
            row.push_back(std::to_string(grp.size()));
            row.push_back(std::to_string(grp.deficitSum(g)));
            table.push_back(row);
        }

        groupID++;
    }
    std::sort(
        table.begin(), table.end(),
        [](const std::vector<std::string> &row1,
           const std::vector<std::string> &row2)
        {
            return (
                atoi(row1[0].c_str()) <
                atoi(row2[0].c_str()));
        });

    myAssignTablePath =
        std::filesystem::current_path().string() +
        "\\assigntable" +
        readableTimeStamp() +
        ".txt";

    std::ofstream ofs(myAssignTablePath);
    if (!ofs.is_open())
    {
        auto fn = myGroupListPath;
        myGroupListPath.clear();
        throw std::runtime_error(
            "Cannot open assign table file " + fn);
    }

    ofs << textStats();
    ofs << "locality deficit group size sum\n";
    for (auto &row : table)
    {
        for (auto &c : row)
        {
            ofs << c << " ";
        }
        ofs << "\n";
    }
}
