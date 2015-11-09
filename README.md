# CPU-Cache

I used two classes, cache and block. I also used a global stamp in my main which is used as an reference to set each block’s LRU value.

I tested the code in the following steps:-

1. tested non-associative cache involving writing into different word fields in the same block

2.tested associative cache involving  writing into two blocks in the same set, fixed LRU problems (added global stamps)

3. fixed flush the whole cache function (was accessing to memory address “-1”)

I used this input text:

# 8 bit addresses
# 2 Bytes/word, 2 words/block, 2 block/set, 2 sets/cache
# 1 cycle/hit, 3 cycles/read, 5 cycles/write
write-req 0 AAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBB
write-req 4 4444
read-req 0
read-req 0
write-req 8 2221
write-req 16 FFFF
write-req 24 EEEE
write-req 32 CCCCC
write-req 4 4INPUT
write-req 6 6INPUT
flush-req
read-req 0
read-req 4
read-req 6
read-req 8
read-req 16
read-req 24
read-req 32
read-req 16
flush-req
debug-req
