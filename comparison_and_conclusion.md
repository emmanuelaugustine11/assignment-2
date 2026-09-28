# Performance comparison and final conclusion

## Observed search results

These counts come from executing the C program on the supplied hierarchy.
Both searches use the same sorted department array. Preprocessing is excluded
from per-query counts; these are logical entry comparisons, not elapsed times.

| Query | Result | Linear Search | Binary Search | Fewer comparisons |
|---|---|---:|---:|---|
| Backend | Found at index 0 | 1 | 3 | Linear |
| HR | Found at index 4 | 5 | 3 | Binary |
| Testing | Found at index 6 | 7 | 3 | Binary |
| Marketing | Not found | 7 | 3 | Binary |
| Total | Four queries | 20 | 12 | Binary |

Binary Search used 8 fewer comparisons (40% fewer) for these four queries after
sorting. This does not establish a 40% runtime improvement or include sorting cost.

## Comparison of approaches

| Criterion | General tree | Array with Linear Search | Sorted array with Binary Search |
|---|---|---|---|
| Reporting relationships | Directly stores parent-child links | Names alone lose links | Names alone lose links |
| Level-based reporting | O(N) breadth-first traversal | Not available from names alone | Not available from names alone |
| Lookup worst case | O(N) traversal | O(D) | O(log D) |
| Ordering prerequisite | None | None | Must be sorted |
| Search auxiliary space | O(N) for this BFS implementation | O(1) | O(1), iterative |
| Preprocessing here | O(N) construction | O(N) extraction; sorting unnecessary for linear alone | O(N + D^2) extraction and insertion sort |
| Updates | Update parent-child links within capacity | Append if capacity permits | Insertions may shift O(D) entries |
| Best use | Organisational reporting | Small lists or occasional searches | Repeated searches in mostly static data |

N = number of hierarchy nodes; D = number of departments. Name comparisons are
treated as constant time in this table. See the README complexity section for
string-length costs, height calculation and allocated space details.

## Final conclusion

Use a **general tree for reporting plus a sorted department array with Binary
Search for repeated department lookup**. The tree preserves all seven reporting
links and level-order traversal visits the eight nodes in four levels. Its height
is three edges. The sorted index makes lookup independent of organisational depth.

Binary Search uses three comparisons for each selected query, versus 1, 5, 7 and
7 for Linear Search. Its O(log D) worst-case lookup scales better than O(D).
However, Linear Search wins for Backend and avoids sorting when used alone.
For only one or a few searches in a seven-department organisation, Linear Search
is a reasonable simpler choice; preprocessing must be considered before claiming
an overall speed advantage for Binary Search.

The combined approach is suitable for the assignment's static hierarchy. The C
implementation reserves eight nodes and three child pointers per node; a growing
organisation requires larger capacities or dynamic storage. Changes to the tree
must also update or rebuild the index. Multiple reporting managers require a graph,
and duplicate department names require IDs or a mapping to multiple records.
