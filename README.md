# mContainers
This is a small set of data structures, mostly just reimplementations of the C++ Standard Library for learning purposes.

Currently implemented are Vectors, Linked Lists, and Dictionaries.

This implementation of a Dictionary provides on average constant time lookup, as well as fast iteration, as the underlying data is stored in contigious memory.
This is achieved by storing the actual key-value pairs in a Vector, and uses a hash table that maps from a key reference to the index of the Vector containing the kv pair.

One of the drawbacks of this is that depending on platform and processor, this costs an extra 8-16 bytes per entry in the Dictionary. 
The other is the cost of removing elements, which has the same time as a Vector, compared to on average constant time for a typical hash table implementation. 