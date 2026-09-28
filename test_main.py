"""Behavioural checks: python -m unittest -v"""

import unittest

from main import (Node, binary_search, build_hierarchy, department_index,
                  level_order, linear_search, tree_height)


class HierarchyTests(unittest.TestCase):
    def test_reporting_relationships(self):
        root = build_hierarchy()
        self.assertEqual([n.name for n in root.children], ["HR", "Finance", "IT"])
        it = root.children[2]
        self.assertEqual([n.name for n in it.children], ["Development", "Testing"])
        self.assertEqual([n.name for n in it.children[0].children],
                         ["Frontend", "Backend"])

    def test_levels_and_height(self):
        self.assertEqual(level_order(build_hierarchy()), [
            ["CEO"], ["HR", "Finance", "IT"],
            ["Development", "Testing"], ["Frontend", "Backend"]])
        self.assertEqual(tree_height(build_hierarchy()), 3)
        self.assertEqual(tree_height(Node("Leaf")), 0)

    def test_searches_and_counts(self):
        names = department_index(build_hierarchy())
        self.assertEqual(names, ["Backend", "Development", "Finance", "Frontend",
                                 "HR", "IT", "Testing"])
        for target, linear_count, binary_count in [
            ("Backend", 1, 3), ("HR", 5, 3), ("Testing", 7, 3),
            ("Marketing", 7, 3)]:
            expected = names.index(target) if target in names else -1
            self.assertEqual(linear_search(names, target), (expected, linear_count))
            self.assertEqual(binary_search(names, target), (expected, binary_count))

    def test_all_names_case_and_missing_boundaries(self):
        names = department_index(build_hierarchy())
        for search in (linear_search, binary_search):
            for index, name in enumerate(names):
                self.assertEqual(search(names, name.lower())[0], index)
            for target in ("AAA", "ZZZ", "CEO"):
                self.assertEqual(search(names, target)[0], -1)
            self.assertEqual(search([], "HR"), (-1, 0))
            self.assertEqual(search(["HR"], "hr"), (0, 1))
            self.assertEqual(search(["HR"], "IT"), (-1, 1))


if __name__ == "__main__":
    unittest.main()
