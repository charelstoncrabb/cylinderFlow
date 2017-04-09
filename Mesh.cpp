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
        outfile << nodeList[i]->nodeID << " " << nodeList[i]->loc[0] << " " << nodeList[i]->loc[1] << " ";
        for(int j = 0; j < nodeList[i]->adjacent.size(); j++){
            outfile << nodeList[i]->adjacent[j]->nodeID << " ";
        }
        outfile << std::endl;
    }
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE/FACET LISTS (ALREADY TRIANGULATED) ------------------
Mesh::Mesh(std::vector<Node*> nodes, std::vector<Facet*> facets){
    for(int i = 0; i < nodes.size(); i++){
        Node *newNode = new Node(nodes[i]->nodeID,nodes[i]->loc[0],nodes[i]->loc[1]);
        nodeList.push_back(newNode);
    }
    for(int i = 0; i < nodes.size(); i++){
        Facet *newFacet = new Facet(facets[i]->area);
        facetList.push_back(newFacet);
    }
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE LIST (CREATES TRIANGULATION)  -----------------------
Mesh::Mesh(std::vector<Node*> nodes){
    for(int i = 0; i < nodes.size(); i++){
        Node *newNode = new Node(nodes[i]->nodeID,nodes[i]->loc[0],nodes[i]->loc[1]);
        nodeList.push_back(newNode);
    }
    triangulate();
}//----------------------------------------------------------------------------------------------------

// DESTRUCTOR - DELETES ALLOCATED NODES/FACETS  -------------------------------------------------------
Mesh::~Mesh(){
    for(int i = 0; i < nodeList.size(); i++)
        if( nodeList[i] != NULL )
            delete nodeList[i];
    for(int i = 0; i < facetList.size(); i++)
        if( facetList[i] != NULL )
            delete facetList[i];
}

// ASSIGNMENT OPERATOR  -------------------------------------------------------------------------------
//Mesh Mesh::operator=(const Mesh& rhs){
//    nodeList = rhs.nodeList;
//    facetList = rhs.facetList;
//    return *this;
//}//----------------------------------------------------------------------------------------------------

// PARSES MESH .DAT FILE FOR MESH NODES  --------------------------------------------------------------
void Mesh::parseMeshData(std::string meshDataFilename){
    std::ifstream meshData(meshDataFilename);
    int ID;
    double xLoc, yLoc;
    std::string header;
    if( meshData.is_open() ){
        getline(meshData,header);
        while(meshData >> ID >> xLoc >> yLoc){
            Node* currNode = new Node(ID,xLoc,yLoc);
            nodeList.push_back(currNode);
        }
        meshData.close();
    }else{
        std::cout << "ERROR IN parseMeshData(): cannot open file "
        << meshDataFilename << "!" << std::endl;
    }
    sortNodeList();
}//----------------------------------------------------------------------------------------------------

// BUILDS DELAUNAY TRIANGULATION OF NODES IN MESH USING D&C ALGORITHM  --------------------------------
void Mesh::triangulate(void){
    if( nodeList.size() <= 1 ){
        std::cout << "ERROR IN triangulate(): error subdividing mesh!" << std::endl;
        return;
    }else if( nodeList.size() == 2 ){
        setEdges({nodeList[0],nodeList[1]});
    }
    else if( nodeList.size() == 3 ){
        setEdges({nodeList[0],nodeList[1],nodeList[2]});
        Facet* newFacet = new Facet(nodeList);
        facetList.push_back(newFacet);
    }else{
        int halfInd = (int)nodeList.size()/2-1;
        std::vector<Node*> leftNodeList = range<Node>(nodeList,0,halfInd);
        std::vector<Node*> rightNodeList = range<Node>(nodeList,halfInd+1,(int)nodeList.size()-1);
        Mesh *leftSubmesh = new Mesh(leftNodeList);
        Mesh *rightSubmesh = new Mesh(rightNodeList);
        setSubmeshAdjacency(leftSubmesh);
        setSubmeshAdjacency(rightSubmesh);
        mergeMeshes(leftSubmesh,rightSubmesh);
        delete leftSubmesh;
        delete rightSubmesh;
    }
}//----------------------------------------------------------------------------------------------------

// SETS SUBMESH ADJACENCY IN COMBINED MESH WHEN MERGING SUBMESHES  ------------------------------------
void Mesh::setSubmeshAdjacency(const Mesh* submesh){
    for(int i = 0; i < submesh->nodeList.size(); i++){
        for(int j = 0; j < submesh->nodeList[i]->adjacent.size(); j++){
            int currNodeInd = submesh->nodeList[i]->findIndByID(nodeList);
            int currAdjInd = submesh->nodeList[i]->adjacent[j]->findIndByID(nodeList);
            if( !nodeList[currNodeInd]->isAdjacent(*nodeList[currAdjInd]) )
                setEdges({nodeList[currNodeInd],nodeList[currAdjInd]});
        }
    }
}//----------------------------------------------------------------------------------------------------

// HELPER FUNCTION FOR triangulate() - MERGES SUBMESHES  ----------------------------------------------
void Mesh::mergeMeshes(const Mesh* leftSubMesh, const Mesh* rightSubMesh){
    bool noCandidate = false, rightCand = false, leftCand = false;
    Node *leftCandidate = NULL, *rightCandidate = NULL;
//    nodeList = cat(leftSubMesh.nodeList,rightSubMesh.nodeList);
//    facetList = cat(leftSubMesh.facetList,rightSubMesh.facetList);
    Mesh leftMesh(leftSubMesh->nodeList), rightMesh(rightSubMesh->nodeList);
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
    Node* leftBase = leftMesh.nodeList[leftBaseInd];
    Node* rightBase = rightMesh.nodeList[rightBaseInd];
    leftBaseInd = leftBase->findIndByID(nodeList); rightBaseInd = rightBase->findIndByID(nodeList);
    setEdges({nodeList[leftBaseInd],nodeList[rightBaseInd]});
    leftBaseInd = leftMesh.findLeftBaseNode(); rightBaseInd = rightMesh.findRightBaseNode();
    while( noCandidate == false ){
        int itr = 0;
        rightCand = false;
        std::vector<int> potRtCandInds = rightBase->ordCandList(leftBase,"cw");
        std::vector<int> potLtCandInds = leftBase->ordCandList(rightBase,"ccw");
        while( rightCand == false && /*itr*/0 < potRtCandInds.size() ){
            if( potRtCandInds.size() == 1 ){
                rightCandidate = rightBase->adjacent[potRtCandInds[itr]];
                rightCand = true;
            }else if( potRtCandInds.size() >= 2 &&
                     !rightBase->adjacent[potRtCandInds[itr+1]]->isInCirc(*leftBase,*rightBase,*rightBase->adjacent[potRtCandInds[itr]]) ){
                rightCandidate = rightBase->adjacent[potRtCandInds[itr]];
                rightCand = true;
            }else{
//                rmEdges({rightBase,rightBase->adjacent[potRtCandInds[itr]]});
                rmEdges({nodeList[rightBase->findIndByID(nodeList)],nodeList[ rightBase->adjacent[potRtCandInds[itr]]->findIndByID(nodeList)]});
                potRtCandInds.erase(potRtCandInds.begin() + itr);
//                itr++;
            }
        }
        itr = 0;
        leftCand = false;
        while( leftCand == false && /*itr*/0 < potLtCandInds.size() ){
            if( potLtCandInds.size() == 1 ){
                leftCandidate = leftBase->adjacent[potLtCandInds[itr]];
                leftCand = true;
            }else if( potLtCandInds.size() >= 2 &&
                     !leftBase->adjacent[potLtCandInds[itr+1]]->isInCirc(*rightBase,*leftBase,*leftBase->adjacent[potLtCandInds[itr]]) ){
                leftCandidate = leftBase->adjacent[potLtCandInds[itr]];
                leftCand = true;
            }else{
//                rmEdges({leftBase,leftBase->adjacent[potLtCandInds[itr]]});
                rmEdges({nodeList[leftBase->findIndByID(nodeList)],nodeList[ leftBase->adjacent[potLtCandInds[itr]]->findIndByID(nodeList)]});
                potLtCandInds.erase(potLtCandInds.begin() + itr);
//                itr++;
            }
        }
        if( leftCand && rightCand ){
            bool isLtCandInRtCirc = leftCandidate->isInCirc(*leftBase,*rightBase,*rightCandidate);
            bool isRtCandInLtCirc = rightCandidate->isInCirc(*leftBase,*rightBase,*leftCandidate);
            // TODO: check for co-circular points here if isLt/isRt are both true or both false
            int ltBaseInd = leftBase->findIndByID(nodeList), rtBaseInd = rightBase->findIndByID(nodeList);
            if( isLtCandInRtCirc ){
                int ltCandInd = leftCandidate->findIndByID(nodeList);
                setEdges({nodeList[rtBaseInd],nodeList[ltCandInd]});
                Facet *newFacet = new Facet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[ltCandInd]});
                facetList.push_back(newFacet);
                leftBase = leftMesh.nodeList[leftCandidate->findIndByID(leftMesh.nodeList)];
            }else if( isRtCandInLtCirc ){
                int rtCandInd = rightCandidate->findIndByID(nodeList);
                setEdges({nodeList[ltBaseInd],nodeList[rtCandInd]});
                Facet *newFacet = new Facet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[rtCandInd]});
                facetList.push_back(newFacet);
                rightBase = rightMesh.nodeList[rightCandidate->findIndByID(rightMesh.nodeList)];
            }
        }else if( leftCand ){
            int ltBaseInd = leftBase->findIndByID(nodeList), rtBaseInd = rightBase->findIndByID(nodeList),
                ltCandInd = leftCandidate->findIndByID(nodeList);
            setEdges({nodeList[rtBaseInd],nodeList[ltCandInd]});
            Facet *newFacet = new Facet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[ltCandInd]});
            facetList.push_back(newFacet);
            leftBase = leftMesh.nodeList[leftCandidate->findIndByID(leftMesh.nodeList)];
        }else if( rightCand ){
            int ltBaseInd = leftBase->findIndByID(nodeList), rtBaseInd = rightBase->findIndByID(nodeList),
                rtCandInd = rightCandidate->findIndByID(nodeList);
            setEdges({nodeList[ltBaseInd],nodeList[rtCandInd]});
            Facet *newFacet = new Facet({nodeList[ltBaseInd],nodeList[rtBaseInd],nodeList[rtCandInd]});
            facetList.push_back(newFacet);
            rightBase = rightMesh.nodeList[rightCandidate->findIndByID(rightMesh.nodeList)];
        }else{
            noCandidate = true;
        }
    }
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODE FOR LEFT SUBMESH  ------------------------------------------------------------------
int Mesh::findLeftBaseNode(void){
    int baseInd = 0;
    for(int i = 0; i < nodeList.size(); i++){
        if( nodeList[i]->loc[1] < nodeList[baseInd]->loc[1] ){
            baseInd = i;
        }else if( nodeList[i]->loc[1] == nodeList[baseInd]->loc[1] ){
            if( nodeList[i]->loc[0] > nodeList[baseInd]->loc[0] ){
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
        if( nodeList[i]->loc[1] < nodeList[baseInd]->loc[1] ){
            baseInd = i;
        }else if( nodeList[i]->loc[1] == nodeList[baseInd]->loc[1] ){
            if( nodeList[i]->loc[0] < nodeList[baseInd]->loc[0] ){
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
            nodes[i]->adjacent.push_back(nodes[j]);
            nodes[j]->adjacent.push_back(nodes[i]);
        }
    }
}//----------------------------------------------------------------------------------------------------

// REMOVES ALL EDGES BETWEEN GIVEN NODES  -------------------------------------------------------------
void Mesh::rmEdges(std::vector<Node*> nodes){
    for(int i = 0; i < nodes.size(); i++){
        for(int j = i+1; j < nodes.size(); j++){
            std::vector<int> adj = isAdjacent(*nodes[i],*nodes[j]);
            if( adj.size() > 0 ){
                nodes[i]->adjacent.erase(nodes[i]->adjacent.begin() + nodes[j]->findIndByID(nodes[i]->adjacent));
                nodes[j]->adjacent.erase(nodes[j]->adjacent.begin() + nodes[i]->findIndByID(nodes[j]->adjacent));
            }
        }
    }
}//----------------------------------------------------------------------------------------------------

// parseMeshData HELPER - SORTS PARSED MESH DATA  -----------------------------------------------------
void Mesh::sortNodeList(void){
    int N = (int)nodeList.size();
    Node* temp;
    for(int i = N; i > 0; i--){
        for(int j = 0; j < i-1; j++){
            if( *nodeList[j+1] < *nodeList[j] ){
                temp = nodeList[j+1];
                nodeList[j+1] = nodeList[j];
                nodeList[j] = temp;
            }
        }
    }
}//----------------------------------------------------------------------------------------------------

// RETURNS INDICES OF NODES a,b IF THEY ARE ADJACENT IN this MESH
std::vector<int> Mesh::isAdjacent(Node a, Node b){
    std::vector<int> inds;
    int aN = (int)a.adjacent.size(), bN = (int)b.adjacent.size();
    for(int i = 0; i < aN; i++){
        for(int j = 0; j < bN; j++){
            if( a.adjacent[i]->nodeID == b.nodeID && b.adjacent[j]->nodeID == a.nodeID ){
                inds.push_back(j);
                inds.push_back(i);
            }
        }
    }
    return inds;
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
//    nodeID = ID;
//    loc[0] = x;
//    loc[1] = y;
    std::cout << "ERROR: setNode() method is depracated!" << std::endl;
}//----------------------------------------------------------------------------------------------------

// RETURNS ANGLE BETWEEN LINE SEGMENTS this-P AND this-Q  ---------------------------------------------
double Node::calcAngle(Node P, Node Q, std::string orientation){
    double angle = 0;
    std::vector<double> thisP = {P.loc[0]-loc[0],P.loc[1]-loc[1]},
                        thisQ = {Q.loc[0]-loc[0],Q.loc[1]-loc[1]};

    double det = thisP[0]*thisQ[1]-thisP[1]*thisQ[0];

    if( det > 0 )
        angle = acos(dot(thisP,thisQ)/(norm(thisP)*norm(thisQ)));
    else
        angle = 2*acos(-1) - acos(dot(thisP,thisQ)/(norm(thisP)*norm(thisQ)));
    return angle;
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
std::vector<int> Node::ordCandList(Node* node, std::string orientation){
    int N = (int)adjacent.size(), minInd;
    bool minIndSet = false;
    std::vector<double> angles(N);
    std::vector<int> ordered;
    for(int i = 0; i < N; i++){
        if( orientation == "cw")
            angles[i] = calcAngle(*adjacent[i],*node,orientation);
        if( orientation == "ccw" )
            angles[i] = calcAngle(*node,*adjacent[i],orientation);
    }
    
    double theta1 = acos(-1), theta2 = -1, tol = 1e-5;
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
        if( node.nodeID == adjacent[i]->nodeID )
            return true;
    return false;
}//----------------------------------------------------------------------------------------------------

// RETURNS INDEX OF this IN LIST nodes - RETURNS -1 IF this IS NOT IN LIST  ---------------------------
int Node::findIndByID(std::vector<Node*> nodes){
    int ind = -1;
    for(int i = 0; i < nodes.size(); i++){
        if( nodes[i]->nodeID == nodeID ){
            return i;
        }
    }
    return ind;
}//----------------------------------------------------------------------------------------------------

// ASSIGNMENT OPERATOR  -------------------------------------------------------------------------------
//void Node::operator=(Node& rhs){
//    nodeID = rhs.nodeID;
//    loc = rhs.loc;
//}//----------------------------------------------------------------------------------------------------

// COMPARISON OPERATOR - USES STANDARD (X,Y) DICTIONARY ORDER  ----------------------------------------
bool Node::operator<(Node& rhs) const {
    if(loc[0] < rhs.loc[0]){
        return true;
    }else if(loc[0] == rhs.loc[0] && loc[1] < rhs.loc[1]){
        return true;
    }
    return false;
}//----------------------------------------------------------------------------------------------------

// =======================  FACET CLASS MEMBERS  ======================================================
Facet::Facet(std::vector<Node*> nodeList) : nodes(nodeList){
    if( nodeList.size() == 3 ){
        std::vector<double> node1Loc = nodes[0]->getLoc(), node2Loc = nodes[1]->getLoc(),
            node3Loc = nodes[2]->getLoc();
        std::vector<double> matArray = { node2Loc[0]-node1Loc[0], node2Loc[1]-node1Loc[1],
            node3Loc[0]-node1Loc[0], node3Loc[1]-node1Loc[1]};
        area = fabs(matArray[0]*matArray[3]-matArray[1]*matArray[2])/2.0;
    }else{
        area = 0;
        std::cout << "WARNING IN Facet(): degenerate facet constructed." << std::endl;
    }
}//----------------------------------------------------------------------------------------------------

// ASSIGNMENT OPERATOR  -------------------------------------------------------------------------------
//Facet Facet::operator=(Facet& rhs){
//    area = rhs.area;
//    nodes = rhs.nodes;
//    adjacent = rhs.adjacent;
//    return *this;
//}//----------------------------------------------------------------------------------------------------

