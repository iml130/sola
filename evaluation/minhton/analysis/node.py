# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.analysis.treemapper import treeMapper

class Node:
    """
        Simple class for nodes (LogicalNodeInfo and PhysicalNodeInfo) for comparison of nodes.
    """

    def __init__(self, level, number, address, port, fanout):
        self.level = level
        self.number = number
        self.address = address
        self.port = port
        self.fanout = fanout
        self.value = self.calculate_value()

    def calculate_value(self):
        """
            Relative horizontal value of the node, using the tree mapper. 
        """
        return treeMapper(self.level, self.number, self.fanout)

    def __eq__(self, other):
        """
            Equality operator (e.g. node1 == node2)
        """
        return self.level == other.level and self.number == other.number and self.address == other.address and self.port == other.port

    def __ne__(self, other):
        """
            Inequality operator (e.g. node1 != node2)
        """
        return not self == other

    def __lt__(self, other):
        """
            Less-than operator (e.g. node1 < node2)

            For this we use the horizontal value of the node.
            Therefore node1 < node2 means that node1 is to the left of node2.
        """
        return self.value < other.value

    def __gt__(self, other):
        """
            Greater-than operator (e.g. node1 > node2)

            For this we use the horizontal value of the node.
            Therefore node1 > node2 means that node1 is to the right of node2.
        """
        return self.value > other.value

    def __le__(self, other):
        """
            Less-or-equal-than operator (e.g. node1 >= node2)

            For this we use the horizontal value of the node.
            Therefore node1 >= node2 means that node1 is to the left of node2 or on the same position.
        """
        return self.value <= other.value

    def __ge__(self, other):
        """
            Less-or-equal-than operator (e.g. node1 <= node2)

            For this we use the horizontal value of the node.
            Therefore node1 <= node2 means that node1 is to the right of node2 or on the same position.
        """
        return self.value >= other.value
    
    def __hash__(self):
        """
            Simple hash function so that we can use dictionaries. 
            It uses the horizontal value of the node.
        """
        return hash(self.value)

    def __str__(self):
        return "( {}:{} | {}:{} | m={})".format(self.level, self.number, self.address, self.port, self.fanout)
