//
//  Mesh.cpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Mesh.hpp"

// ========================  MESH CLASS MEMBERS  ======================================================
// CONSTRUCTOR - USES NODE LOCATIONS FROM meshDataFilename  -------------------------------------------
Mesh::Mesh(std::string meshDataFilename){
    parseMeshData(meshDataFilename);
    triangulate();
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE LIST  -----------------------------------------------
Mesh::Mesh(std::vector<Node> nodes) : nodeList(nodes){
    triangulate();
}//----------------------------------------------------------------------------------------------------

// PARSES MESH .DAT FILE FOR MESH NODES  --------------------------------------------------------------
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
}//----------------------------------------------------------------------------------------------------

// BUILDS DELAUNAY TRIANGULATION OF NODES IN MESH USING D&C ALGORITHM  --------------------------------
void Mesh::triangulate(void){
    // PSUEDO:
    // If 2 <= nodeList.size() <= 3
    //   Facet is given triangle
    // If nodeList.size() > 3
    //   Build two submeshes with nodes nodeList[0:size/2] & nodeList[size/2:end]
    //   Combine submeshes
}//----------------------------------------------------------------------------------------------------

// ========================  NODE CLASS MEMBERS  ======================================================
// CONSTRUCTOR  ---------------------------------------------------------------------------------------
Node::Node(int ID, double x, double y){
    nodeID = ID;
    loc.push_back(x);
    loc.push_back(y);
}//----------------------------------------------------------------------------------------------------

// SETS NODE DATA TO EXISTING NODE OBJECT  ------------------------------------------------------------
void Node::setNode(int ID, double x, double y){
    nodeID = ID;
    loc[0] = x;
    loc[1] = y;
}//----------------------------------------------------------------------------------------------------


// =======================  FACET CLASS MEMBERS  ======================================================
Facet::Facet(std::vector<Node> nodeList) : nodes(nodeList){
    std::vector<double> node1Loc = nodes[0].getLoc(), node2Loc = nodes[1].getLoc(),
        node3Loc = nodes[2].getLoc();
    Matrix facetVects(2,2,{ node2Loc[0]-node1Loc[0], node2Loc[1]-node1Loc[1],
        node3Loc[0]-node1Loc[0], node3Loc[1]-node1Loc[1]});
    area = fabs(facetVects.det())/2.0;
}//----------------------------------------------------------------------------------------------------


