#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>



#include <wex.h> // https://github.com/JamesBremner/windex
#include <window2file.h>

#include "cGraph.h"


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





main()
{
    cGUI theGUI;
    return 0;
}
