# DSA Assignment 2: Organisational Hierarchy and Department Search

## Aim

Construct a company's organisational tree, display it using level-order
traversal, compare Linear Search and Binary Search, and evaluate the structures.

## Submission contents

This project answers the organisational hierarchy question supplied for this assignment.

| Required item | File or section |
|---|---|
| C source code | [main.c](main.c) |
| Given input data and selected queries | [input_data.txt](input_data.txt) |
| Executed output | [sample_output.txt](sample_output.txt) |
| Intermediate trace tables | [trace_tables.md](trace_tables.md) |
| Time and space complexity analysis | [Complexity](#complexity) below |
| Performance comparison table | [Comparison](comparison_and_conclusion.md#observed-search-results) |
| Justified final conclusion | [Final conclusion](comparison_and_conclusion.md#final-conclusion) |

Repository designated for submission: https://github.com/emmanuelaugustine11/assignment-2

## Run the project

Requires a C11 compiler (GCC, Clang or Microsoft Visual C). No external libraries are needed.

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic main.c -o hierarchy
./hierarchy
./hierarchy --test
./hierarchy --trace > trace_tables.md
```

- `main.c`: tree construction, traversal, height calculation, both searches and self-tests.
- `sample_output.txt`: output captured from an actual execution.

The given hierarchy and four queries are built into the C program and documented
in `input_data.txt`; no interactive input or input-file argument is required.
The `--trace` option records the queue after each visit, each insertion-sort pass,
and every comparison in both searches. Trace printing is diagnostic output and
is excluded from the normal-operation complexity analysis.

## Part (a): Tree representation and construction

Use a **rooted general tree**. Each C struct stores a name, an array of child pointers and a child count. This fixed example allows up to three children per node and stores eight nodes in a caller-owned array; no heap allocation is needed.
A binary tree would not naturally represent the CEO's three direct reports.

```text
CEO
|-- HR
|-- Finance
`-- IT
    |-- Development
    |   |-- Frontend
    |   `-- Backend
    `-- Testing
```

`build_hierarchy()` creates the child nodes and attaches them to their managers.
There are **8 nodes, 7 edges and 5 leaves**: HR, Finance, Testing, Frontend and
Backend. CEO is the root role; the other seven nodes are departments.

### Level-order traversal

1. Insert the root into a FIFO queue.
2. Remove the next node and visit it.
3. Insert its children in their stored order.
4. Repeat until the queue is empty.

The implementation uses an array-backed FIFO queue with head and tail indices, so enqueue and dequeue take constant time. It records depths alongside node pointers to preserve level boundaries. The queue has eight slots with a capacity check.

Actual execution:

```text
Level 0: CEO
Level 1: HR -> Finance -> IT
Level 2: Development -> Testing
Level 3: Frontend -> Backend
```

Arrows on a level indicate traversal order, not reporting relationships.
Overall visit order: CEO, HR, Finance, IT, Development, Testing, Frontend, Backend.
Breadth-first traversal visits all nodes at one depth before going deeper, so it
is useful for reports grouped by organisational level. The tree diagram retains
the specific manager-child relationships that a flat traversal cannot show.

## Part (b): Searchable representation and measured comparisons

Extract departments from the traversal into an array of string pointers, excluding the CEO role, and sort alphabetically using insertion sort. Sorting and searching use the same case-insensitive comparator for these English names:

```text
Index:  0        1            2        3         4   5   6
Name:   Backend  Development  Finance  Frontend  HR  IT  Testing
```

Both algorithms use this same list for a fair comparison. Linear Search checks
successive entries from index 0. Binary Search checks the middle entry, discards
the irrelevant half, and repeats. Binary Search requires the list to be sorted
using the same ordering as its comparisons. Both searches are case-insensitive
and return the matching index or -1, together with their comparison count.

**Counting convention:** one comparison means one list entry compared with the
target. Binary Search counts one logical three-way comparison at each midpoint.
These are entry comparisons, not counts of C operators, individual character
comparisons, or loop-condition checks. Sorting comparisons are preprocessing and
are excluded from the per-search counts.

Results from running `./hierarchy`:

| Target | Result | Linear comparisons | Binary comparisons |
|---|---|---:|---:|
| Backend | Found, index 0 | 1 | 3 |
| HR | Found, index 4 | 5 | 3 |
| Testing | Found, index 6 | 7 | 3 |
| Marketing | Not found | 7 | 3 |

Binary Search's inspected entries explain these counts:

- Backend: Frontend, Development, Backend.
- HR: Frontend, IT, HR.
- Testing: Frontend, IT, Testing.
- Marketing: Frontend, IT, Testing; the remaining search range becomes empty.

Linear Search wins for Backend because it is first. Binary Search uses fewer
comparisons in the other three examples. It does not always take three
comparisons: Frontend, the initial midpoint, takes only one. Across these four
demonstrations, Linear Search makes 20 comparisons and Binary Search makes 12;
these totals describe the chosen examples, not a universal average.

## Part (c): Analysis

### Tree height

Height is the number of edges on the longest root-to-leaf path. The path
CEO -> IT -> Development -> Frontend (or Backend) contains **3 edges**.
Therefore the height is **3**, or **4 levels** if height is expressed in nodes.
`tree_height()` calculates this recursively; a leaf's height is zero.

### Complexity

Let N be the number of hierarchy nodes, D the number of departments, h the tree
height, and w the maximum level width. Here N = 8, D = 7, h = 3 and w = 3.
The table treats a name comparison as constant time.

| Operation | Time | Additional space |
|---|---|---|
| Construct a general hierarchy of N nodes | O(N) | O(N) for stored tree |
| Level-order traversal | O(N) | O(N) allocated queue and depth arrays |
| Calculate height | O(N) | O(h + 1) recursive stack |
| Extract and sort department index | O(N + D^2) worst case | O(N + D) traversal and index storage; sorting uses O(1) extra |
| Linear Search, best case | O(1) | O(1) under fixed-length-name assumption |
| Linear Search, average/worst case | O(D) | O(1) under fixed-length-name assumption |
| Binary Search, best case | O(1) | O(1) under fixed-length-name assumption |
| Binary Search, average/worst case | O(log D) | O(1) under fixed-length-name assumption |

The construction function explicitly builds this fixed example; its work is
constant for these exact eight nodes. O(N) describes extending the same approach
to an arbitrary-sized hierarchy. Every traversal visits each node once, and height
calculation examines every child subtree. The live FIFO contents can mix adjacent levels and are O(w), but this implementation retains visited pointers and allocates O(N) queue storage. Insertion sort costs O(D^2) on average and in the worst case, and O(D) when already sorted; it is sufficient for seven names. All fixed capacities must be increased if the hierarchy grows, or replaced with dynamic storage.

For successful Linear Search with equally likely targets, the average number of
comparisons is (D + 1) / 2 = 4. An unsuccessful Linear Search checks all D = 7
entries. Binary Search examines at most floor(log2 D) + 1 = 3 entries for this
nonempty list; an empty list requires zero comparisons.

Names are strings: if their maximum length is L, case conversion and comparison
can cost O(L) time. The C comparator processes characters directly and uses O(1) additional space. Thus more
precise worst-case search bounds are O(DL) and O(L log D), respectively. Sorting
also has string processing costs. The usual DSA bounds above abstract those costs.

### Suitability and conclusion

The general tree is suitable for organisational reporting because it directly
represents a single manager per node, multiple direct reports, and
clear reporting levels. It would need a graph representation for a matrix
organisation where a department reports to multiple managers.

A tree alone does not provide alphabetically ordered lookup: locating a name
could require O(N) visits. A separate sorted department list supports repeated
Binary Searches in O(log D) after preprocessing. With only seven departments,
Linear Search remains simple and can be faster for an early match or a one-off
query when sorting has not yet been done.

The combined representation is suitable for this small, mostly static company.
When departments are added, removed or renamed, the tree and search index must be
kept consistent; this program rebuilds the index from the tree. Inserting into a
sorted array can take O(D) because entries shift. A larger system needing frequent
exact-name searches could use a dictionary mapping unique names to tree nodes
for expected O(1) lookup, while retaining the tree for reporting. Duplicate names
would require unique department IDs or a mapping to multiple nodes.

## Validation

The C program was compiled and executed using Tiny C Compiler 0.9.27 with `-Wall -Werror`; all C self-tests passed. The installed Microsoft compiler lacked standard headers, so a portable compiler was used for validation. The source uses standard C features compatible with the C11 build commands above. Execution output is saved in `sample_output.txt`. Run `hierarchy --test` to verify reporting links, exact level order, height, sorting, comparison counts, every department, case-insensitive matching, missing entries, empty arrays and single-entry arrays. Assertions must remain enabled (do not define `NDEBUG` when testing).

### Windows build alternative

From a Visual Studio Developer Command Prompt:

```bat
cl /nologo /std:c11 /W4 /WX main.c /Fe:hierarchy.exe
hierarchy.exe
hierarchy.exe --test
```
