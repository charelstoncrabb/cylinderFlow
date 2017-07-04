//
//  FEMesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 7/4/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#ifndef FEMesh_hpp
#define FEMesh_hpp

#include "Mesh.hpp"

class FEMesh : public Mesh{
    FEMesh(const char *meshDataFilename) : Mesh(meshDataFilename) {};
    virtual ~FEMesh();
};

#endif
