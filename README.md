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
