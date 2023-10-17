# Problem

I have thousands of localities on a map (vector layer). Each of them present a deficit or a surplus.

I am trying to create groups for which the sum of these deficits and surplus is close to 0 (between -0,5 and 0,5). The municipalities must be contiguous, that is to say that A touches B which touches C without C needing to touch A. The algorithm should try to maximize the size of groups and minimize the amount of localities that belong to no group.

https://stackoverflow.com/q/77307162/16582

---------

# Algorithm

I will assume that the localities are on a rectangular grid and diagonal adjacent localities touch.

```
- Set xr to to the grid width, yr to the grid height.
- LOOP1
   - Set xt to zero and yt to zero
   - WHILE xt + xr <= grid width and yt + yr <= grid height
       - IF rectangle xt,yt,xt+xr,yt+yr contain marked localities
            - CONTINUE
       - Sum localities in rectangle xt,yt,xt+xr,yt+yr
       - IF abs( sum ) < 0.5
           - Store xt,yt,xt+xr,yt+yr as a municipality
           - Mark localities in xt,yt,xt+xr,yt+yr
       - raster increment xt,yt
       - ENDWHILE
   - Decrement xr,yr  (*)
   - ENDLOOP1
```

   
   
(*) This will only find square municipalities.  If you want to search for rectangular, the 'decrement xr,yr' step will need to somewhat more complex - this should be enough to give you the idea.

   

