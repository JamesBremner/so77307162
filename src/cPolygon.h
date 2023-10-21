#include "cxy.h"        https://github.com/JamesBremner/raven-set

class cPolygon
{
    std::vector<cxy> myVertices;

    /////////////////////////////
public:
    bool touching(const cPolygon &other)
    {
        for (int p1v1 = 0; p1v1 < myVertices.size() - 1; p1v1++)
        {
            cxy p1v1xy = myVertices[p1v1];
            cxy p1v2xy = myVertices[p1v1 + 1];
            for (int p2v1 = 0; p2v1 < other.myVertices.size() - 1; p2v1++)
            {
                cxy p2v1xy = myVertices[p2v1];
                cxy p2v2xy = myVertices[p2v1 + 1];
                if( p1v1xy == p2v1xy && p1v2xy == p2v2xy )
                    return true;
            }
        }
        return false;
    }
};