//
//  Mesh.cpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Mesh.hpp"

// ========================  MESH CLASS MEMBERS  ==============================
// CONSTRUCTOR - USES NODE LOCATIONS FROM meshDataFilename  -------------------
Mesh::Mesh(std::string meshDataFilename){
    parseMeshData(meshDataFilename);
}//----------------------------------------------------------------------------

// PARSES MESH .DAT FILE FOR MESH NODES  --------------------------------------
void Mesh::parseMeshData(std::string meshDataFilename){
    std::ifstream meshData(meshDataFilename);
    int ID;
    double xLoc, yLoc;
    Node currNode(0,0,0);
    std::string header;
    getline(meshData,header);
    while( !meshData.eof() ){
        meshData >> ID >> xLoc >> yLoc;
        currNode.setNode(ID,xLoc,yLoc);
        nodeList.push_back(currNode);
    }
    meshData.close();
}//----------------------------------------------------------------------------


// ========================  NODE CLASS MEMBERS  ==============================
// CONSTRUCTOR  ---------------------------------------------------------------
Node::Node(int ID, double x, double y){
    nodeID = ID;
    loc[0] = x;
    loc[1] = y;
}//----------------------------------------------------------------------------

// SETS NODE DATA TO EXISTING NODE OBJECT  ------------------------------------
void Node::setNode(int ID, double x, double y){
    nodeID = ID;
    loc[0] = x;
    loc[1] = y;
}//----------------------------------------------------------------------------


// =======================  FACET CLASS MEMBERS  ==============================



