# Problem

I have thousands of localities on a map (vector layer). Each of them present a deficit or a surplus.

I am trying to create groups for which the sum of these deficits and surplus is close to 0 (between -0,5 and 0,5). The municipalities must be contiguous, that is to say that A touches B which touches C without C needing to touch A. The algorithm should try to maximize the size of groups and minimize the amount of localities that belong to no group.

The localities are polygons and two polygons touch if they share one or more edges.

![image](https://github.com/JamesBremner/so77307162/assets/2046227/5fcea3ad-c923-49b1-8a11-cff984c4fd8f)


https://stackoverflow.com/q/77307162/16582

---------

# Algorithm

```
- Convert input to a graph data structure with vertices representing localities and edges connecting localities that share an edge
- Set all vertices unmarked
- Loop S over vertices
- Run a breadth first search starting from S
   - do not visit marked vertices
   - keep sum of vertex values
   - if abs(sum) < 0.5
      - add list of vertices visited in current search to list of groups
      - mark vertices visited in current search
      - end current search
```

   


   

