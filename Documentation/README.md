# cylinderFlow
CFD for compressible NS with periodic in/out-flow - method TBD

Architechture:
* class Solver
  > Derived classes:
    * class SolverOptions (TODO)
        - Methods: (TODO)
    * class Matrix
        - Methods: rref()
                   solveAxb()
    * class Mesh (TODO)
        - Methods: (TODO)
  > Methods:
    * stepInTime (TODO)
    * writeState (TODO)

Meshing algorithm:
  Find bottom-most nodes from left and right meshes to create base LR edge
  Find a right candidate:
    Potential candidate = Find right base endpoint-adjacent node with smallest angle to base LR edge
    Check two criteria:
      1. Is the angle less than 180deg?
      2. Does the next candidate (next smallest angle to base LR edge) lie outside the circumcirle
         of the base LR edge endpoints and the potential candidate?
    If yes to both, this is our left candidate and proceed
    If no to 1., base right endpoint is candidate
    If no to 2., delete edge from right base node to potential candidate,
                 potential candidate = next candidate, repeat checks
  Find a left candidate by same process
  With left/right candidates chosen,
    1. Is right candidate outside base-left cand. circumcircle?
    2. Is left candidate outside base-right cand. circumcircle?
    If yes to 1., create left candidate-right base edge, and set this edge as new base edge
    If yes to 2., create right candidate-left base edge, and set this edge as new base edge