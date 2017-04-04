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

// WRITES MESH DATA TO OUTPUT FILE  -------------------------------------------------------------------
void Mesh::writeMesh(std::string meshOutFile){
    std::fstream outfile(meshOutFile,std::fstream::out | std::fstream::trunc);
    outfile << "ID X Y ADJ" << std::endl;
    for(int i = 0; i < nodeList.size(); i++){
        outfile << nodeList[i].nodeID << " " << nodeList[i].loc[0] << " " << nodeList[i].loc[1] << " ";
        for(int j = 0; j < nodeList[i].adjacent.size(); j++){
            outfile << nodeList[i].adjacent[j].nodeID << " ";
        }
        outfile << std::endl;
    }
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE/FACET LISTS (ALREADY TRIANGULATED) ------------------
Mesh::Mesh(std::vector<Node> nodes, std::vector<Facet> facets) : nodeList(nodes), facetList(facets){
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE LIST (CREATES TRIANGULATION)  -----------------------
Mesh::Mesh(std::vector<Node> nodes) : nodeList(nodes){
    triangulate();
}//----------------------------------------------------------------------------------------------------

// ASSIGNMENT OPERATOR  -------------------------------------------------------------------------------
Mesh Mesh::operator=(const Mesh& rhs){
    nodeList = rhs.nodeList;
    facetList = rhs.facetList;
    return *this;
}//----------------------------------------------------------------------------------------------------

// PARSES MESH .DAT FILE FOR MESH NODES  --------------------------------------------------------------
void Mesh::parseMeshData(std::string meshDataFilename){
    std::ifstream meshData(meshDataFilename);
    int ID, i = 1;
    double xLoc, yLoc;
    Node currNode(0,0,0);
    std::string header;
    if( meshData.is_open() ){
        getline(meshData,header);
        while(meshData >> ID >> xLoc >> yLoc){
            if( ID == i ){
                currNode.setNode(ID,xLoc,yLoc);
                nodeList.push_back(currNode);
            }else{
                std::cout << "WARNING IN parseMeshData(): non-sequential nodes detected and skipped." << std::endl;
            }
            i++;
        }
        meshData.close();
    }else{
        std::cout << "ERROR IN parseMeshData(): cannot open file "
        << meshDataFilename << "!" << std::endl;
    }
// TODO: Sort nodes by (x,y) dictionary order
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
        setEdges({&nodeList[0],&nodeList[1]});
    }
    else if( nodeList.size() == 3 ){
        setEdges({&nodeList[0],&nodeList[1],&nodeList[2]});
        Facet newFacet(nodeList);
        facetList.push_back(newFacet);
    }else{
        int halfInd = ((int)nodeList.size()-1)/2;
        std::vector<Node> leftNodeList = range<Node>(nodeList,0,halfInd),
            rightNodeList = range<Node>(nodeList,halfInd+1,(int)nodeList.size()-1);
        Mesh leftSubmesh(leftNodeList), rightSubmesh(rightNodeList);
        mergeMeshes(leftSubmesh,rightSubmesh);
    }
}//----------------------------------------------------------------------------------------------------

// HELPER FUNCTION FOR triangulate() - MERGES SUBMESHES  ----------------------------------------------
void Mesh::mergeMeshes(Mesh leftMesh, Mesh rightMesh){
    bool noCandidate = false, rightCand = false, leftCand = false;
    Node rightCandidate, leftCandidate;
    nodeList = cat(leftMesh.nodeList,rightMesh.nodeList);
    facetList = cat(leftMesh.facetList,rightMesh.facetList);
    // Find bottom-most nodes from left and right meshes to create base LR edge
    // Find a right candidate:
    //   Potential candidate = Find right base endpoint-adjacent node with smallest angle to base LR edge
    //   Check two criteria:
    //     1. Is the angle less than 180deg?
    //     2. Does the next candidate (next smallest angle to base LR edge) lie outside the circumcirle
    //        of the base LR edge endpoints and the potential candidate?
    //   If yes to both, this is our left candidate and proceed
    //   If no to 1., base right endpoint is candidate
    //   If no to 2., delete edge from right base node to potential candidate,
    //                potential candidate = next candidate, repeat checks
    // Find a left candidate by same process
    // With left/right candidates chosen,
    //   1. Is right candidate outside base-left cand. circumcircle?
    //   2. Is left candidate outside base-right cand. circumcircle?
    //   If yes to 1., create left candidate-right base edge, and set this edge as new base edge
    //   If yes to 2., create right candidate-left base edge, and set this edge as new base edge
    int leftBaseInd = leftMesh.findLeftBaseNode(), rightBaseInd = rightMesh.findRightBaseNode();
    Node leftBase = leftMesh.nodeList[leftBaseInd], rightBase = rightMesh.nodeList[rightBaseInd];
    leftBaseInd = leftBase.findIndByID(nodeList); rightBaseInd = rightBase.findIndByID(nodeList);
    setEdges({&nodeList[leftBaseInd],&nodeList[rightBaseInd]});
    leftBaseInd = leftMesh.findLeftBaseNode(); rightBaseInd = rightMesh.findRightBaseNode();
    while( noCandidate == false ){
        int itr = 0;
        rightCand = false;
        std::vector<int> potRtCandInds = rightMesh.nodeList[rightBaseInd].ordCandList(leftBase);
        std::vector<int> potLtCandInds = leftMesh.nodeList[leftBaseInd].ordCandList(rightBase);
        while( rightCand == false && itr < potRtCandInds.size() ){
            if( potRtCandInds.size() == 1 && rightBase.calcAngle(leftBase,rightBase.adjacent[potRtCandInds[itr]]) < acos(-1) ){
                rightCandidate = rightBase.adjacent[potRtCandInds[itr]];
                rightCand = true;
            }else if( potRtCandInds.size() >= 2 &&
                     !rightBase.adjacent[potRtCandInds[itr+1]].isInCirc(leftBase,rightBase,rightBase.adjacent[potRtCandInds[itr]]) ){
                rightCandidate = rightBase.adjacent[potRtCandInds[itr]];
                rightCand = true;
            }else{
                rightMesh.rmEdges({&rightBase,&rightBase.adjacent[potRtCandInds[itr]]});
                potRtCandInds.erase(potRtCandInds.begin() + itr);
                itr++;
            }
        }
        itr = 0;
        leftCand = false;
        while( leftCand == false && itr < potLtCandInds.size() ){
            if( potLtCandInds.size() == 1 && leftBase.calcAngle(rightBase,leftBase.adjacent[potLtCandInds[itr]]) < acos(-1) ){
                leftCandidate = leftBase.adjacent[potLtCandInds[itr]];
                leftCand = true;
            }else if( potLtCandInds.size() >= 2 &&
                     !leftBase.adjacent[potLtCandInds[itr+1]].isInCirc(rightBase,leftBase,leftBase.adjacent[potLtCandInds[itr]]) ){
                leftCandidate = leftBase.adjacent[potLtCandInds[itr]];
                leftCand = true;
            }else{
                leftMesh.rmEdges({&leftBase,&leftBase.adjacent[potLtCandInds[itr]]});
                potLtCandInds.erase(potLtCandInds.begin() + itr);
                itr++;
            }
        }
        if( leftCand && rightCand ){
            bool isLtCandInRtCirc = leftCandidate.isInCirc(leftBase,rightBase,rightCandidate);
            bool isRtCandInLtCirc = rightCandidate.isInCirc(leftBase,rightBase,leftCandidate);
            int ltBaseInd = leftBase.findIndByID(nodeList), rtBaseInd = rightBase.findIndByID(nodeList);
            if( isLtCandInRtCirc ){
                int ltCandInd = leftCandidate.findIndByID(nodeList);
                setEdges({&nodeList[rtBaseInd],&nodeList[ltCandInd]});
                Facet newFacet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[ltCandInd]});
                facetList.push_back(newFacet);
                leftBase = nodeList[ltCandInd];
                leftBaseInd = leftBase.findIndByID(leftMesh.nodeList);
            }else if( isRtCandInLtCirc ){
                int rtCandInd = rightCandidate.findIndByID(nodeList);
                setEdges({&nodeList[ltBaseInd],&nodeList[rtCandInd]});
                Facet newFacet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[rtCandInd]});
                facetList.push_back(newFacet);
                rightBase = nodeList[rtCandInd];
                rightBaseInd = rightBase.findIndByID(rightMesh.nodeList);
            }
        }else if( leftCand ){
            int ltBaseInd = leftBase.findIndByID(nodeList), rtBaseInd = rightBase.findIndByID(nodeList),
                ltCandInd = leftCandidate.findIndByID(nodeList);
            setEdges({&nodeList[rtBaseInd],&nodeList[ltCandInd]});
            Facet newFacet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[ltCandInd]});
            facetList.push_back(newFacet);
            leftBase = nodeList[ltCandInd];
            leftBaseInd = leftBase.findIndByID(leftMesh.nodeList);
        }else if( rightCand ){
            int ltBaseInd = leftBase.findIndByID(nodeList), rtBaseInd = rightBase.findIndByID(nodeList),
                rtCandInd = rightCandidate.findIndByID(nodeList);
            setEdges({&nodeList[ltBaseInd],&nodeList[rtCandInd]});
            Facet newFacet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[rtCandInd]});
            facetList.push_back(newFacet);
            rightBase = nodeList[rtCandInd];
            rightBaseInd = rightBase.findIndByID(rightMesh.nodeList);
        }else{
            noCandidate = true;
        }
    }
// TODO: Finish debuggin mergeMeshes()
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODE FOR LEFT SUBMESH  ------------------------------------------------------------------
int Mesh::findLeftBaseNode(void){
    int baseInd = 0;
    for(int i = 0; i < nodeList.size(); i++){
        if( nodeList[i].loc[1] < nodeList[baseInd].loc[1] ){
            baseInd = i;
        }else if( nodeList[i].loc[1] == nodeList[baseInd].loc[1] ){
            if( nodeList[i].loc[0] > nodeList[baseInd].loc[0] ){
                baseInd = i;
            }
        }
    }
    return baseInd;
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODE FOR RIGHT SUBMESH  -----------------------------------------------------------------
int Mesh::findRightBaseNode(void){
    int baseInd = 0;
    for(int i = 0; i < nodeList.size(); i++){
        if( nodeList[i].loc[1] < nodeList[baseInd].loc[1] ){
            baseInd = i;
        }else if( nodeList[i].loc[1] == nodeList[baseInd].loc[1] ){
            if( nodeList[i].loc[0] < nodeList[baseInd].loc[0] ){
                baseInd = i;
            }
        }
    }
    return baseInd;
}//----------------------------------------------------------------------------------------------------

// SETS ADJACENCY BETWEEN ALL NODES IN LIST GIVEN  ----------------------------------------------------
void Mesh::setEdges(std::vector<Node*> nodes){
    int N = (int)nodes.size();
    for(int i = 0; i < N-1; i++){
        for(int j = i+1; j < N; j++){
            nodes[i]->adjacent.push_back(*nodes[j]);
            nodes[j]->adjacent.push_back(*nodes[i]);
        }
    }
}//----------------------------------------------------------------------------------------------------

// REMOVES ALL EDGES BETWEEN GIVEN NODES  -------------------------------------------------------------
void Mesh::rmEdges(std::vector<Node*> nodes){
    int N = (int)nodes.size();
    for(int i = 0; i < N; i++){
        int currNodeID = nodes[i]->nodeID;
        for(int j = 0; j < N; j++){
            for(int k = 0; k < nodes[j]->adjacent.size(); k++){
                if( nodes[j]->adjacent[k].nodeID == currNodeID ){
                    nodes[j]->adjacent = rmEl(nodes[j]->adjacent,k);
                }
            }
        }
    }
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

// RETURNS ANGLE BETWEEN LINE SEGMENTS this-P AND this-Q  ---------------------------------------------
double Node::calcAngle(Node P, Node Q){
    std::vector<double> thisP = {P.loc[0]-loc[0],P.loc[1]-loc[1]},
                        thisQ = {Q.loc[0]-loc[0],Q.loc[1]-loc[1]};
    return acos(dot(thisP,thisQ)/(norm(thisP)*norm(thisQ)));
}//----------------------------------------------------------------------------------------------------

// DECIDES IF this IS INSIDE CIRCUMCIRCLE OF TRIANGLE ABC  --------------------------------------------
bool Node::isInCirc(Node A, Node B, Node C){
    double y0 = (pow(A.loc[0],2) - pow(C.loc[0],2) + pow(A.loc[1],2) - pow(C.loc[1],2) -
                ((A.loc[0]-C.loc[0])*(pow(A.loc[0],2) - pow(B.loc[0],2) + pow(A.loc[1],2)
                - pow(B.loc[1],2)))/(A.loc[0]-B.loc[0]) )/(2*(A.loc[1]-C.loc[1]
                - ((A.loc[1]-B.loc[1])*(A.loc[0]-C.loc[0]))/(A.loc[0]-B.loc[0])));
    double x0 = (pow(A.loc[0],2) - pow(B.loc[0],2) + pow(A.loc[1],2) - pow(B.loc[1],2))
                /(2*(A.loc[0]-B.loc[0])) - ((A.loc[1]-B.loc[1])/(A.loc[0]-B.loc[0]))*y0;
    double r = pow(pow(A.loc[0]-x0,2)+pow(A.loc[1]-y0,2),0.5);
    if( pow(pow(loc[0]-x0,2)+pow(loc[1]-y0,2),0.5) > r ){
        return false;
    }else{
        return true;
    }
}//----------------------------------------------------------------------------------------------------

// SORTS ADJACENT NODES BY ANGLE WITH GIVEN NODE  -----------------------------------------------------
std::vector<int> Node::ordCandList(Node node){
    int N = (int)adjacent.size(), minInd;
    bool minIndSet = false;
    std::vector<double> angles(N);
    std::vector<int> ordered;
    Node temp;
    double theta1 = acos(-1), theta2 = -1, tol = 1e-5;
    for(int i = 0; i < N; i++){
        if( loc[1] <= adjacent[i].loc[1] )
            angles[i] = calcAngle(adjacent[i],node);
        else
            angles[i] = 2*acos(-1) - calcAngle(adjacent[i],node);
    }
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if( angles[j] < theta1 - tol && angles[j] > theta2 ){
                minInd = j;
                theta1 = angles[j];
                minIndSet = true;
            }
        }
        if( minIndSet )
            ordered.push_back(minInd);
        theta2 = theta1;
        theta1 = acos(-1);
        minIndSet = false;
    }
    return ordered;
}//----------------------------------------------------------------------------------------------------

// DECIDES IF GIVEN NODE IS ADJACENT TO this
bool Node::isAdjacent(Node node){
    for(int i = 0; i < adjacent.size(); i++)
        if( node.nodeID == adjacent[i].nodeID )
            return true;
    return false;
}//----------------------------------------------------------------------------------------------------

// RETURNS INDEX OF this IN LIST nodes - RETURNS -1 IF this IS NOT IN LIST  ---------------------------
int Node::findIndByID(std::vector<Node> nodes){
    int ind = -1;
    for(int i = 0; i < nodes.size(); i++){
        if( nodes[i].nodeID == nodeID ){
            return i;
        }
    }
    return ind;
}//----------------------------------------------------------------------------------------------------

// ASSIGNMENT OPERATOR  -------------------------------------------------------------------------------
Node Node::operator=(Node& rhs){
    nodeID = rhs.nodeID;
    loc = rhs.loc;
    adjacent = rhs.adjacent;
    return *this;
}//----------------------------------------------------------------------------------------------------


// =======================  FACET CLASS MEMBERS  ======================================================
Facet::Facet(std::vector<Node> nodeList) : nodes(nodeList){
    if( nodeList.size() == 3 ){
        std::vector<double> node1Loc = nodes[0].getLoc(), node2Loc = nodes[1].getLoc(),
            node3Loc = nodes[2].getLoc();
        std::vector<double> matArray = { node2Loc[0]-node1Loc[0], node2Loc[1]-node1Loc[1],
            node3Loc[0]-node1Loc[0], node3Loc[1]-node1Loc[1]};
        area = fabs(matArray[0]*matArray[3]-matArray[1]*matArray[2])/2.0;
    }else{
        area = 0;
        std::cout << "WARNING IN Facet(): degenerate facet constructed." << std::endl;
    }
}//----------------------------------------------------------------------------------------------------

// ASSIGNMENT OPERATOR  -------------------------------------------------------------------------------
Facet Facet::operator=(Facet& rhs){
    area = rhs.area;
    nodes = rhs.nodes;
    adjacent = rhs.adjacent;
    return *this;
}//----------------------------------------------------------------------------------------------------

