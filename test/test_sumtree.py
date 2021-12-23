import pyper
import pytest


@pytest.fixture
def default_trees():
    def build(n):
        tree = pyper.SumTree(n)
        for i in range(2 * n):
            tree.insert(i, 2 * i)
        return tree
    return {n: build(n) for n in [10, 20, 30, 50, 100, 200, 500, 1000]}


class TestSumTree:

    def test_constructor(self):
        pyper.SumTree(5)

    def test_constructor_kw(self):
        pyper.SumTree(capacity=5)

    def test_len(self):
        assert len(pyper.SumTree(5)) == 0

    def test_insert(self):
        n = 10
        tree = pyper.SumTree(n)
        for i in range(n-3):
            tree.insert(i, 2 * i)
        assert len(tree) == n - 3

    def test_insert_kw(self):
        n = 10
        tree = pyper.SumTree(n-3)
        for i in range(2 * n):
            tree.insert(value=i, priority=2 * i)
        assert len(tree) == n - 3

    def test_insert_and_get(self, default_trees):
        expected = {
            10: [5, 15, 30.],
            20: [11, 31, 62.],
            30: [17, 47, 94.],
            50: [29, 79, 158.],
            100: [58, 158, 316.],
            200: [116, 316, 632.],
            500: [290, 790, 1580.],
            1000: [581, 1581, 3162.]
        }
        for n, results in expected.items():
            return_value = default_trees[n].get(0.5)
            for k in range(len(results)):
                assert return_value[k] == results[k]

    def test_priority_iter(self, default_trees):
        for n, tree in default_trees.items():
            for k, i in zip(tree.priority_iter(), range(n, 2 * n)):
                assert k == 2 * i

    def test_value_iter(self, default_trees):
        for n, tree in default_trees.items():
            for k, i in zip(tree.value_iter(), range(n, 2 * n)):
                assert k == i

    def test_iter(self, default_trees):
        for n, tree in default_trees.items():
            for (k, i), expected in zip(tree, range(n, 2 * n)):
                assert k == expected, i == 2 * k
