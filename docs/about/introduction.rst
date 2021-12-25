.. _about_intro:

Introduction
===============

PER is a small c++ library with Python bindings to provide
the prioritized experience replay buffer as introduced in [1]_.

It provides two classes: the Sum Tree implementation in `per::SumTree` for
efficient data storage and the PER buffer class `per::PrioritizedExperience` to sample from.
Both clases are exported eponymously to Python.

The library is built flexibly to retain any type of value in the buffer due to templatization, simply plug in the
value type upon usage. SumTree is templatized the same way. See section :ref:`_api_cpp`

.. [1] Schaul, Tom, et al. "Prioritized experience replay." arXiv preprint arXiv:1511.05952 (2015).