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

# Sample Input

Randomly generated

```
Localities with values
0 2.000000
1 2.000000
2 1.000000
3 1.000000
4 2.000000
5 1.000000
6 -3.000000
7 -3.000000
8 1.000000
9 -1.000000
10 2.000000
11 2.000000
12 -2.000000
13 0.000000
14 -2.000000
15 2.000000
16 -2.000000
17 -1.000000
18 0.000000
19 -3.000000
20 0.000000
21 -3.000000
22 -1.000000
23 0.000000
24 1.000000
25 1.000000
26 0.000000
27 -1.000000
28 -1.000000
29 2.000000
30 2.000000
31 -3.000000
32 2.000000
33 -3.000000
34 0.000000
35 1.000000
36 2.000000
37 -2.000000
38 -2.000000
39 -3.000000

Touching Localities

0 22
0 36
1 36
2 13
2 30
2 38
2 15
3 21
3 26
4 27
4 34
4 32
4 21
5 6
6 21
6 16
6 30
7 35
7 17
8 26
8 24
8 13
9 18
9 10
9 21
9 38
10 30
10 24
10 26
10 39
11 23
11 28
12 31
13 28
13 17
14 15
17 18
17 26
17 32
18 39
19 37
20 31
21 31
21 25
21 28
22 37
22 26
23 29
23 33
24 33
24 39
25 29
26 33
26 30
27 31
28 33
29 30
33 35
35 36

```

Groups generated

```
============
8 1.000000
9 -1.000000
10 2.000000
11 2.000000
13 0.000000
18 0.000000
21 -3.000000
23 0.000000
24 1.000000
26 0.000000
28 -1.000000
30 2.000000
33 -3.000000
35 1.000000
39 -3.000000
36 2.000000
============
7 -3.000000
17 -1.000000
32 2.000000
4 2.000000
```

![image](https://github.com/JamesBremner/so77307162/assets/2046227/f9b9b06d-6d20-4cd9-9a9a-5421d79b0025)

   

