import pyper


class T:
    def __init__(self, v):
        self.v = v

def gen(v):
    while True:
        yield v


values = [bool(0), 1, (2,), [3], {4: None}, [5, []], T(6), 7., gen(8), "9"]


def test_per():
    per1, per2, per3 = (
        pyper.PrioritizedExperience(10, seed=0),
        pyper.PrioritizedExperience(10, seed=0),
        pyper.PrioritizedExperience(10, seed=1)
    )

    for v in values:
        per1.push(v)
        per2.push(v)
        per3.push(v)

    assert per1.sample(5) == per2.sample(5)
    assert per1.sample(5) != per3.sample(5)

