//
//  Mesh.cpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#include "Mesh.hpp"

// CONTINUOUS RANGE OF VECTOR  ------------------------------------------------------------------------
// RETURNS SUBVECTOR u[min:max]
template<class T>
std::vector<T> range(std::vector<T> u, int min, int max){
    std::vector<T> subU(max-min+1);
    for(int i = 0; i < max-min+1; i++){
        subU[i] = u[i+min];
    }
    return subU;
}//----------------------------------------------------------------------------------------------------


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
    if( nodeList.size() <= 1 ){
        std::cout << "ERROR IN triangulate(): error subdividing mesh!" << std::endl;
        return;
    }else if( nodeList.size() == 2 ){
        // Facet is line segment
    }
    else if( nodeList.size() == 3 ){
        // Facet is triangle
    }else{
        int halfInd = (int)nodeList.size()/2;
        std::vector<Node> leftNodeList = range<Node>(nodeList,0,halfInd),
            rightNodeList = range<Node>(nodeList,halfInd+1,(int)nodeList.size()-1);
        Mesh leftSubmesh(leftNodeList);
        Mesh rightSubmesh(rightNodeList);
        *this = mergeMeshes(leftSubmesh,rightSubmesh);
    }
}//----------------------------------------------------------------------------------------------------

// HELPER FUNCTION FOR triangulate() - MERGES SUBMESHES  ----------------------------------------------
Mesh Mesh::mergeMeshes(Mesh leftMesh, Mesh rightMesh){
    Mesh newMesh;
    return newMesh;
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
// TODO: Catch case if nodeList.size() == 2 
    Matrix facetVects(2,2,{ node2Loc[0]-node1Loc[0], node2Loc[1]-node1Loc[1],
        node3Loc[0]-node1Loc[0], node3Loc[1]-node1Loc[1]});
    area = fabs(facetVects.det())/2.0;
}//----------------------------------------------------------------------------------------------------


