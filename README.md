# Computer Systems hw 2

Collision Resolution: I use an unordered map, which automatically utilizes bucket hashing. Bucket hashing is better than probing because data always belongs/is where it initially hashes to, negating the need to wastefully scan the entire hashtable. This means that insert, for example, is always an O(1) operation not dependent on the fullness of the table and getting or deleting data (not to mention querying deleted data) is much simpler/better.

Evictor Modification: Modification of the evictor requires solely modification of the evictor.add() method. The evictor always evicts the first item in its queue, so inserting new items in queue locations reflecting their eviction priorities ensures correct eviction order.
