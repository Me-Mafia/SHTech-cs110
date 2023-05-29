1. cache size = block size * num_of_blocks
   
## senario 1:
- hit:0  access:16 hit rate = 0
- if increase rep count, hit rate still 0. Because the stepsize is 8 words, block size = 8 bytes. every iteration only change the tag part of the memory address. This makes CPU always try to access the same block of the cache.
- option改为1，step size改为1，By doing that, every address of memory will be mapped to the whole 4 blocks of cache.
hit:192  access:256 ，每四个miss1个，rate= 0.75


## senario 2:
- hit:48  access:64  hit rate:0.75 
(0,4,8,12)(1,5,9,13)每四次访问都是同一次index，第一次miss后三次hit
- if rep goes to infty, hit rate goes to 1. when the repcnt >=2 the compulsory can be avoided, thus access always = hit +16, the rate will go to 1.
- cache blocking... 

## senario 3:
- range：0-0.5 最坏情况0：每一次都随机到同一个位置，每一次miss之后覆盖，然后再次miss最好情况0.5：和lru一样
- associate改一改

#2: 