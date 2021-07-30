import pyper


def test_sumtree():
    expected = dict()
    expected[10] = [5, 15, 30.]
    expected[20] = [11, 31, 62.]
    expected[30] = [17, 47, 94.]
    expected[50] = [29, 79, 158.]
    expected[100] = [58, 158, 316.]
    expected[200] = [116, 316, 632.]
    expected[500] = [290, 790, 1580.]
    expected[1000] = [581, 1581, 3162.]

    for n, results in expected.items():
        tree = pyper.SumTree(n)
        for i in range(2 * n):
            tree.insert(i, 2 * i)

        return_value = tree.get(0.5)
        for k in range(len(results)):
            assert return_value[k] == results[k]


def test_sumtree_priority_iter():
    n = 10
    tree = pyper.SumTree(n)
    for i in range(2 * n):
        tree.insert(i, 2 * i)

    for k, i in zip(tree.priority_iter(), range(10, 2 * n)):
        assert k == 2 * i
