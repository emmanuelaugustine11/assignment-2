"""Organisational hierarchy and department search demonstration.

Run with Python 3.10 or newer: python main.py
"""

from collections import deque
from dataclasses import dataclass, field


@dataclass
class Node:
    """A general-tree node: a role or department with any number of children."""

    name: str
    children: list["Node"] = field(default_factory=list)


def build_hierarchy() -> Node:
    development = Node("Development", [Node("Frontend"), Node("Backend")])
    it = Node("IT", [development, Node("Testing")])
    return Node("CEO", [Node("HR"), Node("Finance"), it])


def level_order(root: Node) -> list[list[str]]:
    """Visit nodes breadth-first, retaining the boundaries between levels."""
    queue = deque([root])
    levels = []
    while queue:
        level = []
        for _ in range(len(queue)):
            node = queue.popleft()
            level.append(node.name)
            queue.extend(node.children)
        levels.append(level)
    return levels


def tree_height(root: Node) -> int:
    """Return height in edges: a leaf has height zero."""
    return 1 + max((tree_height(child) for child in root.children), default=-1)


def department_index(root: Node) -> list[str]:
    """Exclude the CEO role and sort departments for binary search."""
    return sorted(
        [name for level in level_order(root)[1:] for name in level],
        key=str.casefold,
    )


def compare_names(left: str, right: str) -> int:
    """One logical three-way name comparison (case-insensitive)."""
    left, right = left.casefold(), right.casefold()
    return (left > right) - (left < right)


def linear_search(names: list[str], target: str) -> tuple[int, int]:
    """Return (index or -1, number of entries compared)."""
    comparisons = 0
    for index, name in enumerate(names):
        comparisons += 1
        if compare_names(name, target) == 0:
            return index, comparisons
    return -1, comparisons


def binary_search(names: list[str], target: str) -> tuple[int, int]:
    """Search a list sorted by casefold(); count each midpoint once."""
    low, high = 0, len(names) - 1
    comparisons = 0
    while low <= high:
        middle = (low + high) // 2
        comparisons += 1
        relation = compare_names(names[middle], target)
        if relation == 0:
            return middle, comparisons
        if relation < 0:
            low = middle + 1
        else:
            high = middle - 1
    return -1, comparisons


def main() -> None:
    root = build_hierarchy()
    print("ORGANISATIONAL HIERARCHY - LEVEL-ORDER TRAVERSAL")
    for depth, level in enumerate(level_order(root)):
        print(f"Level {depth}: {' -> '.join(level)}")
    print("(Arrows within a level indicate visit order, not reporting links.)")
    print(f"\nTree height: {tree_height(root)} edges (4 levels)")
    names = department_index(root)
    print(f"\nSorted departments: {', '.join(names)}")
    print("CEO is a role, so it is excluded from the department index.")
    print("\nSEARCH COMPARISONS")
    print(f"{'Target':<14}{'Result':<12}{'Linear':>8}{'Binary':>8}")
    for target in ("Backend", "HR", "Testing", "Marketing"):
        linear_index, linear_count = linear_search(names, target)
        binary_index, binary_count = binary_search(names, target)
        assert linear_index == binary_index
        result = "Found" if linear_index >= 0 else "Not found"
        print(f"{target:<14}{result:<12}{linear_count:>8}{binary_count:>8}")
    print("\nOne comparison means comparing the target with one list entry.")


if __name__ == "__main__":
    main()
