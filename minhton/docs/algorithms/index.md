# The way to the MINHTON algorithm

## BATON & BATON\*

Internal name: ``baton_star``

BATON [[1]](#references) is a *structured* P2P overlay network using a balanced binary search tree with in-level links for efficiency, fault tolerance, and load balancing.
It is also possible to extend from a *binary* to an *m-ary* tree with *m > 2* (BATON\*), as presented by Jagadish et. al in [[2]](#references).
Each peer in the network corresponds to one node in the *m-ary* tree.
Each node has links to its related nodes like parent, children, adjacent nodes, and selected routing table neighbors as defined in [[2]](#references) and is able to contact them directly.
Since a joining node can contact any node to join, it is possible to exploit the structure and create a network as depicted in [Fig. 1](#fig1).
We will later present you how to avoid this kind of created tree.

<figure markdown>
  <a id="fig1"></a>
  ![BATON tree structure](../img/baton.svg)
  <figcaption markdown>**Figure 1:** Exploited BATON/BATON\* Network (height balanced). Level 4 (5) has 64 (18) (bidirectional) in-level links.</figcaption>
</figure>

## nBATON\*

Internal name: ``nbaton_star``

We extend the functionality by introducing bootstrapping nodes (further described as dominating set nodes (DSNs)) with a more sophisticated *Join* procedure.
Those DSNs handle the forwarding of *Join*-request specifically to keep a null balanced tree [[3]](#references).
Therefore, we tighten the condition of a balanced tree to:

**Definition**: *A tree is a (null-)balanced m-ary tree if all nodes without children differ from their level at most by 1.*

With such a flat tree, the network restructuring, as described in [[2]](#references) can be omitted.
Additionally, a node would more likely keep a position in a tree and a partitioning of the tree gets more difficult since the Nodes are better linked to each other.
The nBATON\* algorithm also features a tree-mapping function that maps each node of the tree into a *1* dimensional axis and is then utilized by the procedures *Search_Exact* and *Leave*, simplifying their implementations.
The proposed adjustments are applicable on the original BATON as well as on BATON\*.
[Fig. 2](#fig2) summarizes the changes we applied to the original BATON structure.
As can be seen, the structure remains the same, including the links between nodes, while the procedures have been changed.
The figure also illustrates the DSNs which have a link to each node in the whole tree, allowing to route the *Join* messages in a specific way to keep a null-balanced tree.

<figure markdown>
  <a id="fig2"></a>
  ![nBATON* tree structure](../img/nbaton.svg)
  <figcaption markdown>**Figure 2:** (Null-) balanced BATON network (nBATON\*). Level 4 (5) has 98 (4) (bidirectional) in-level links.</figcaption>
</figure>

## MINHTON

Internal name: ``minhton``

For improved efficiency, the MINHTON algorithm goes even further than nBATON* and requires a complete balance as defined in [[3]](#references).

**Definition**: *An m-ary tree is complete if it consists of a perfect tree of height h − 1. In a perfect tree, all leaves lie at the same depth and all internal nodes have exactly m children. The remaining nodes are added from left to right and removed from right to left.*

Thus, the constructed tree is as compact as possible, since there are no gaps in between the tree.
However as the tree is filled starting from the leftmost possible position on each level, the amount of links may not be maximal.
Compared to [Fig. 2](#fig2) the node 5:7 would not exist, assuming the amount of nodes *N* stays the same, but 5:2 would exist (see [Fig. 3](#fig3)).

A side effect of MINHTON's complete balance is that there is only one valid position at each time for a joining or leaving node.
If a node wants to join it is inserted right next to the node with the highest number on the level farthest from the root.
When the last level is already filled, it is necessary to create a new level below it.
The leave process works equivalently, whereas only the last node of the tree (the highest number on the last level) can leave.

This algorithm is also capable of handling concurrent operations like multiple leave requests at once.

<figure markdown>
  <a id="fig3"></a>
  ![MINHTON tree structure](../img/minhton.svg)
  <figcaption markdown>**Figure 3:** Minimal height tree overlay network (MINHTON) with complete balance. Level 4 (5) has 98 (6) (bidirectional) in-level links.</figcaption>
</figure>

## References

1. H. V. Jagadish, Beng Chin Ooi, [BATON: a balanced tree structure for peer-to-peer networks](http://www.eecs.umich.edu/db/files/p661-jagadish.pdf) in *Proceedings of the 31st international conference on Very large data bases, Trondheim, Norway*, p.661-672, 2005, ISBN: 1-59593-154-6
2. H. Jagadish et al. “Speeding up search in peer-to-peer
networks with a multi-way tree structure”. In: Jan. 2006,
pp. 1–12. DOI: 10.1145/1142473.1142475
3. Sung-Hyuk Cha. “On Integer Sequences Derived from
Balanced K-Ary Trees”. In: Proceedings of the 6th
WSEAS International Conference on Computer Engineering
and Applications, and Proceedings of the
2012 American Conference on Applied Mathematics.
AMERICAN-MATH’12/CEA’12. Harvard, Cambridge:
World Scientific, Engineering Academy, and Society
(WSEAS), 2012, pp. 377–381. ISBN: 9781618040640.
