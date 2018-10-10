# PrefixSum
Inclusive prefix sum algorithm (Ladner-Fischer implementation) with C++ threads. 
This recursively sums up successive pairs of elements to obtain the sum of all elements in an array, and forks up to seven threads in the array-implemented tree. The tree is padded if the number of elements is not a power of two. The algorithm is two-pass:
1) compute the pairwise sum
2) compute the prefix sum
