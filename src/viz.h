#include <functional> 

namespace raven
{
    namespace graph
    {
        class cViz
        {
            std::string makeDot(const cGraph &g);
            std::function<std::string(int)> fnVertexColor;

        ////////////////////////////////////////    
        public:

            cViz();
            void setVertexColor( std::function<std::string(int)> f);
            void viz(const cGraph &g);
        };
    }
}