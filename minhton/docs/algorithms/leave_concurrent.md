# Concurrent Leave

The leave procedure of MINHTON supports multiple concurrent leaves at once. Therefore an additional communication overhead is required to make sure the established structure stays intact.

## Initiation

- Triggered by the scheduler, a node *v* receives a leave network signal
- The leave algorithm starts because of the received signal
- While processing the signal, a 1000 ms timeout is added. If the node is not in idle mode after the timeout, the signal will be repeated

## Procedure

- The leaving node *v* checks if a successor (may be) necessary or definitely not
    - With successor: Start the search for a successor *s* (according to the MINHTON algorithm the successor is the last node of the tree structure)
    - Without successor: *v* is the last node of the tree structure (*s* = *v*)
    - Abort the procedure, when the last node is already marked to replace another node
- The last node *l* tries to sign off from his parent *p_l*
    - If *p_l* is not locked: *p_l* locks itself and tries to lock the node directly to its right (possibly through Search Exact, if *p_l* is the last node of the level, thus having the next node follow on the next level)
        - Else: *p_l* blocks the leave through its answer
    - If the node to *p_l*'s right is not locked (and *p_l* is not root): *p_l* tries to lock the node directly to its left equivalently
        - Else: *p_l* blocks the leave through its answer
    - If the node to *p_l*'s left is not locked: *p_l* removes *l* from the routing information of itself and its neighbors
        - Else: *p_l* blocks the leave through its answer
    - *p_l* waits for the confirmations of its RT neighbors and lets *l* leave the position
- The last node *l* signs off itself from its RT neighbors and adjacents
    - *l* waits for the confirmations of all RT neighbors and adjacents
- The last node *l* offers itself as a successor to the leaving node *v*
    - If no successor is required, the *l* (then it equals *v*) can leave the network and unlock its parent. The parent unlocks its own direct neighbors left and right as well.
- The leaving node *v* leaves the network and confirms this to the successor *l*
- The successor *l* moves to the position from *v* and updates the RT neighbors, children, and the parent of the new position with the changed network address
    - *l* waits for the confirmation of all updates
    - The parent redirects the update to all its RT neighbors, waits for the confirmations, and informs *l* that the updates on the parent level are done
- *l* unlocks its parent and the parent unlocks its own direct left and right neighbors as well
