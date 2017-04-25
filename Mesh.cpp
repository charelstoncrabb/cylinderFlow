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
Mesh::Mesh(const char* meshDataFilename, bool rotFlag) : rotateFlag(rotFlag){
    double theta = 0.1;
    Matrix rot(2,2,{cos(theta),-sin(theta),sin(theta),cos(theta)}), rotInv(2,2,{cos(theta),sin(theta),-sin(theta),cos(theta)});
    rotation = rot;
    parseMeshData(meshDataFilename);
    if( rotateFlag ){
        for(int i = 0; i < nodeList.size(); i++){
            nodeList[i]->loc = rotation*nodeList[i]->loc;
        }
    }
    triangulate();
    if( rotateFlag ){
        for(int i = 0; i < nodeList.size(); i++){
            nodeList[i]->loc = rotInv*nodeList[i]->loc;
        }
    }
    buildFacetList();
//TODO: Set boundary node list here
}//----------------------------------------------------------------------------------------------------

// WRITES MESH DATA TO OUTPUT FILE  -------------------------------------------------------------------
void Mesh::writeMesh(const char* meshOutFile){
    std::fstream outfile(meshOutFile,std::fstream::out | std::fstream::trunc);
    std::fstream mainOut("Meshing.out",std::fstream::out | std::fstream::trunc);
    mainOut << output.str();
    outfile << "NODE DATA:" << std::endl << "ID X Y ADJ" << std::endl;
    for(int i = 0; i < nodeList.size(); i++){
        outfile << nodeList[i]->nodeID << " " << nodeList[i]->loc[0] << " " << nodeList[i]->loc[1] << " ";
        for(int j = 0; j < nodeList[i]->adjacent.size(); j++){
            outfile << nodeList[i]->adjacent[j]->nodeID << " ";
        }
        outfile << std::endl;
    }
    outfile << std::endl << "FACET DATA:" << std::endl << "ID AREA CENTROID VERTICES" << std::endl;
    for(int i = 0; i < facetList.size(); i++){
        outfile << facetList[i]->ID << " " << facetList[i]->area << " (" << facetList[i]->centroid[0] << "," << facetList[i]->centroid[1] << ") " << facetList[i]->nodes[0]->nodeID << " " << facetList[i]->nodes[1]->nodeID << " "<< facetList[i]->nodes[2]->nodeID << std::endl;
    }
    outfile << std::endl << "NODE VERTEX DATA:" << std::endl << "NODE_ID FACET_IDS" << std::endl;
    for(int i = 0; i < nodeList.size(); i++){
        outfile << nodeList[i]->nodeID << " ";
        for(int j = 0; j < nodeList[i]->isVertexOf.size(); j++){
            outfile << nodeList[i]->isVertexOf[j]->ID << " ";
        }
        outfile << std::endl;
    }
    outfile.close();
    mainOut.close();
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
}//----------------------------------------------------------------------------------------------------

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
        if( !areColinear(*nodeList[0],*nodeList[1],*nodeList[2]) )
        {
            setEdges({nodeList[0],nodeList[1],nodeList[2]});
        }else{
            bool _0lt1_ = *nodeList[0] < *nodeList[1], _0lt2_ = *nodeList[0] < *nodeList[2], _1lt2_ = *nodeList[1] < *nodeList[2];
            if( (_0lt1_ && _1lt2_) || (!_0lt1_ && !_1lt2_) ){
                setEdges({nodeList[0],nodeList[1]});
                setEdges({nodeList[1],nodeList[2]});
            }
            if( (!_0lt1_ && _0lt2_) || (_0lt1_ && !_0lt2_) ){
                setEdges({nodeList[0],nodeList[1]});
                setEdges({nodeList[0],nodeList[2]});
            }
            if( (_0lt2_ && !_1lt2_) || (!_0lt2_ && _1lt2_) ){
                setEdges({nodeList[2],nodeList[0]});
                setEdges({nodeList[2],nodeList[1]});
            }
        }
    }else{
// TODO: Choose more intelligent division for conquering. E.g., divide into thirds if a base 3 #nodes, etc...
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
//        writeMesh();
//        system("./ProcScripts/MeshPlot.py Mesh.out &");
//        int a;
//        std::cin >> a;
    }
}//----------------------------------------------------------------------------------------------------

// BUILDS FACET LIST BY GRAPH TRAVERSAL AFTER TRIANGULATION HAS OCCURRED  -----------------------------
void Mesh::buildFacetList(void){
    int rootInd = findRightBaseNode();
    std::map< double, std::map<double, bool> > facetMap;
    std::queue<Node*> nodeQ;
    nodeQ.push(nodeList[rootInd]);
    while( !nodeQ.empty() ){
        Node* currNode = nodeQ.front(); nodeQ.pop();
        for(int i = 0; i < currNode->adjacent.size(); i++){
            if( !currNode->adjacent[i]->traversed ){
                currNode->adjacent[i]->traversed = true;
                nodeQ.push(currNode->adjacent[i]);
                for(int j = 0; j < currNode->adjacent[i]->adjacent.size(); j++){
                    if( currNode->isAdjacent(*currNode->adjacent[i]->adjacent[j]) ){
                        double xbar = (currNode->loc[0]+currNode->adjacent[i]->loc[0]+currNode->adjacent[i] ->adjacent[j]->loc[0])/3,
                               ybar = (currNode->loc[1]+currNode->adjacent[i]->loc[1]+currNode->adjacent[i]->adjacent[j]->loc[1])/3;
                        if( !facetMap[xbar][ybar] ){
                            facetMap[xbar][ybar] = true;
                            Facet* newFacet = new Facet({currNode,currNode->adjacent[i],currNode->adjacent[i]->adjacent[j]});
                            facetList.push_back(newFacet);
                            currNode->isVertexOf.push_back(newFacet);
                            currNode->adjacent[i]->isVertexOf.push_back(newFacet);
                            currNode->adjacent[i]->adjacent[j]->isVertexOf.push_back(newFacet);
                        }
                    }
                }
            }
        }
    }
}

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
    std::vector<int> baseNodeIndices = findBaseIndices(leftSubMesh,rightSubMesh);
    int leftBaseInd = baseNodeIndices[0], rightBaseInd = baseNodeIndices[1];
    Node* leftBase = leftSubMesh->nodeList[leftBaseInd];
    Node* rightBase = rightSubMesh->nodeList[rightBaseInd];
    leftBaseInd = leftBase->findIndByID(nodeList); rightBaseInd = rightBase->findIndByID(nodeList);
    setEdges({nodeList[leftBaseInd],nodeList[rightBaseInd]});
    while( noCandidate == false ){
        rightCand = false;
        std::vector<int> potRtCandInds = {};
        std::vector<int> potLtCandInds = {};
        potRtCandInds = rightBase->ordCandList(leftBase,"cw");
        potLtCandInds = leftBase->ordCandList(rightBase,"ccw");
        while( rightCand == false && 0 < potRtCandInds.size() ){
            if( potRtCandInds.size() == 1 ){
                rightCandidate = rightBase->adjacent[potRtCandInds[0]];
                rightCand = true;
            }else if( potRtCandInds.size() >= 2 &&
                     !rightBase->adjacent[potRtCandInds[1]]->isInCirc(*leftBase,*rightBase,*rightBase->adjacent[potRtCandInds[0]]) ){
                rightCandidate = rightBase->adjacent[potRtCandInds[0]];
                rightCand = true;
            }else{
                rmEdges({nodeList[rightBase->findIndByID(nodeList)],nodeList[ rightBase->adjacent[potRtCandInds[0]]->findIndByID(nodeList)]});
                potRtCandInds.erase(potRtCandInds.begin());
            }
        }
        leftCand = false;
        while( leftCand == false && 0 < potLtCandInds.size() ){
            if( potLtCandInds.size() == 1 ){
                leftCandidate = leftBase->adjacent[potLtCandInds[0]];
                leftCand = true;
            }else if( potLtCandInds.size() >= 2 &&
                     !leftBase->adjacent[potLtCandInds[1]]->isInCirc(*rightBase,*leftBase,*leftBase->adjacent[potLtCandInds[0]]) ){
                leftCandidate = leftBase->adjacent[potLtCandInds[0]];
                leftCand = true;
            }else{
                rmEdges({nodeList[leftBase->findIndByID(nodeList)],nodeList[ leftBase->adjacent[potLtCandInds[0]]->findIndByID(nodeList)]});
                potLtCandInds.erase(potLtCandInds.begin());
            }
        }
        if( leftCand && rightCand ){
            bool isLtCandInRtCirc = leftCandidate->isInCirc(*leftBase,*rightBase,*rightCandidate);
            bool isRtCandInLtCirc = rightCandidate->isInCirc(*leftBase,*rightBase,*leftCandidate);
            if( !isLtCandInRtCirc && !isRtCandInLtCirc  )
                output << "WARNING: Co-circular points found! (both cands outside)" << std::endl;
            if( isLtCandInRtCirc && isRtCandInLtCirc )
                output << "WARNING: Co-circular points found! (both cands inside)" << std::endl;
            if( isLtCandInRtCirc ){
                int ltCandInd = leftCandidate->findIndByID(nodeList), rtBaseInd = rightBase->findIndByID(nodeList);
                setEdges({nodeList[rtBaseInd],nodeList[ltCandInd]});
                leftBase = leftSubMesh->nodeList[leftCandidate->findIndByID(leftSubMesh->nodeList)];
                isRtCandInLtCirc = false;
            }
            if( isRtCandInLtCirc ){
                int rtCandInd = rightCandidate->findIndByID(nodeList), ltBaseInd = leftBase->findIndByID(nodeList);
                setEdges({nodeList[ltBaseInd],nodeList[rtCandInd]});
                rightBase = rightSubMesh->nodeList[rightCandidate->findIndByID(rightSubMesh->nodeList)];
            }
        }else if( leftCand ){
            int rtBaseInd = rightBase->findIndByID(nodeList), ltCandInd = leftCandidate->findIndByID(nodeList);
            setEdges({nodeList[rtBaseInd],nodeList[ltCandInd]});
            leftBase = leftSubMesh->nodeList[leftCandidate->findIndByID(leftSubMesh->nodeList)];
        }else if( rightCand ){
            int ltBaseInd = leftBase->findIndByID(nodeList), rtCandInd = rightCandidate->findIndByID(nodeList);
            setEdges({nodeList[ltBaseInd],nodeList[rtCandInd]});
            rightBase = rightSubMesh->nodeList[rightCandidate->findIndByID(rightSubMesh->nodeList)];
        }else{
            noCandidate = true;
        }
    }
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODES FOR LEFT AND RIGHT SUBMESHES  -----------------------------------------------------
std::vector<int> Mesh::findBaseIndices(const Mesh* leftMesh, const Mesh* rightMesh){
    int leftBaseInd = leftMesh->findLeftBaseNode(), rightBaseInd = rightMesh->findRightBaseNode();
    int itr = 0;
    // Check for and deal with vertically-aligned base nodes:
    if( leftMesh->nodeList[leftBaseInd]->loc[0] == rightMesh->nodeList[rightBaseInd]->loc[0] ){
        while( itr < leftMesh->nodeList[leftBaseInd]->adjacent.size() ){
            if( leftMesh->nodeList[leftBaseInd]->adjacent[itr]->loc[0] == rightMesh->nodeList[rightBaseInd]->loc[0]
               && leftMesh->nodeList[leftBaseInd]->adjacent[itr]->loc[1] > leftMesh->nodeList[leftBaseInd]->loc[1] ){
                leftBaseInd = leftMesh->nodeList[leftBaseInd]->adjacent[itr]->findIndByID(leftMesh->nodeList);
                itr = 0;
            }else{
                itr++;
            }
        }
    }
    else{ // Check that new base edge will not intersect existing edges
        bool rightBaseOK = false, leftBaseOK = false;
        while( !rightBaseOK && !leftBaseOK ){
            itr = 0;
            // if angle between base edge and any base node adjacents, reset base node to this adjacent node
            // Continue until right base node has no adjacents with positive angle with base edge, and left base node has no adjacents with negative angle with base edge.
            for(itr = 0; itr < leftMesh->nodeList[leftBaseInd]->adjacent.size(); itr++){
                Node B = *leftMesh->nodeList[leftBaseInd], P = *rightMesh->nodeList[rightBaseInd], Q = *leftMesh->nodeList[leftBaseInd]->adjacent[itr];
                std::vector<double> thisP = {P.loc[0]-B.loc[0],P.loc[1]-B.loc[1]},
                                    thisQ = {Q.loc[0]-B.loc[0],Q.loc[1]-B.loc[1]};
                double det = thisP[0]*thisQ[1]-thisP[1]*thisQ[0];
                if( det < 0 ){
                    leftBaseInd = leftMesh->nodeList[leftBaseInd]->adjacent[itr]->findIndByID(leftMesh->nodeList);
                    itr = 0;
                    rightBaseOK = false;
                }
            }
            if( itr == leftMesh->nodeList[leftBaseInd]->adjacent.size() )
                leftBaseOK = true;
            for(itr = 0; itr < rightMesh->nodeList[rightBaseInd]->adjacent.size(); itr++){
                Node B = *rightMesh->nodeList[rightBaseInd], P = *leftMesh->nodeList[leftBaseInd], Q = *rightMesh->nodeList[rightBaseInd]->adjacent[itr];
                std::vector<double> thisP = {P.loc[0]-B.loc[0],P.loc[1]-B.loc[1]},
                thisQ = {Q.loc[0]-B.loc[0],Q.loc[1]-B.loc[1]};
                double det = thisP[0]*thisQ[1]-thisP[1]*thisQ[0];
                if( det > 0 ){
                    rightBaseInd = rightMesh->nodeList[rightBaseInd]->adjacent[itr]->findIndByID(rightMesh->nodeList);
                    itr = 0;
                    leftBaseOK = false;
                }
            }
            if( itr == rightMesh->nodeList[leftBaseInd]->adjacent.size() )
                rightBaseOK = true;
        }
    }
    
    return {leftBaseInd,rightBaseInd};
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODE FOR LEFT SUBMESH  ------------------------------------------------------------------
int Mesh::findLeftBaseNode(void) const{
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
int Mesh::findRightBaseNode(void) const{
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
            output << "Setting Edge: " << nodes[i]->nodeID << " <==> " << nodes[j]->nodeID << std::endl;
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
                output << "Removing Edge: " << nodes[i]->nodeID << " <==> " << nodes[j]->nodeID << std::endl;
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

// RETURNS INDICES OF NODES a,b IF THEY ARE ADJACENT IN this MESH  ------------------------------------
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

// CHECKS IF THREE GIVEN NODES ARE CO-LINEAR  ---------------------------------------------------------
bool Mesh::areColinear(Node a, Node b, Node c){
    double tol = 1e-10;
    if( a.loc[0] == b.loc[0] ){
        if( a.loc[0] == c.loc[0] )
            return true;
        else{
            double m = (c.loc[1]-a.loc[1])/(c.loc[0]-a.loc[0]);
            if( fabs(b.loc[1] - (m*(b.loc[0]-a.loc[0])+a.loc[1])) < tol )
                return true;
        }
    }else if( a.loc[1] == b.loc[1] && a.loc[1] == c.loc[1] ){
        return true;
    }else{
        double m = (c.loc[1]-a.loc[1])/(c.loc[0]-a.loc[0]);
        if( fabs(b.loc[1] - (m*(b.loc[0]-a.loc[0])+a.loc[1])) < tol )
            return true;
    }
    return false;
}
// ========================  NODE CLASS MEMBERS  ======================================================
// CONSTRUCTOR  ---------------------------------------------------------------------------------------
Node::Node(int ID, double x, double y) : nodeID(ID), traversed(false) {
    loc.push_back(x);
    loc.push_back(y);
}//----------------------------------------------------------------------------------------------------

// SETS NODE DATA TO EXISTING NODE OBJECT  ------------------------------------------------------------
void Node::setNode(int ID, double x, double y){
//    nodeID = ID;
//    loc[0] = x;
//    loc[1] = y;
    std::cout << "ERROR: setNode() method is deprecated!" << std::endl;
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
    double tol = 1e-20;
    double y0 = (pow(A.loc[0],2) - pow(C.loc[0],2) + pow(A.loc[1],2) - pow(C.loc[1],2) -
                ((A.loc[0]-C.loc[0])*(pow(A.loc[0],2) - pow(B.loc[0],2) + pow(A.loc[1],2)
                - pow(B.loc[1],2)))/(A.loc[0]-B.loc[0]) )/(2*(A.loc[1]-C.loc[1]
                - ((A.loc[1]-B.loc[1])*(A.loc[0]-C.loc[0]))/(A.loc[0]-B.loc[0])));
    double x0 = (pow(A.loc[0],2) - pow(B.loc[0],2) + pow(A.loc[1],2) - pow(B.loc[1],2))
                /(2*(A.loc[0]-B.loc[0])) - ((A.loc[1]-B.loc[1])/(A.loc[0]-B.loc[0]))*y0;
    double r = pow(pow(A.loc[0]-x0,2)+pow(A.loc[1]-y0,2),0.5);
    if( pow(pow(loc[0]-x0,2)+pow(loc[1]-y0,2),0.5) > r-tol ){
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
        if( orientation == "cw" )
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
Facet::Facet(std::vector<Node*> nodeList) : ID(ciCounter), nodes(nodeList){
    if( nodeList.size() == 3 ){
        std::vector<double> node1Loc = nodes[0]->getLoc(), node2Loc = nodes[1]->getLoc(),
            node3Loc = nodes[2]->getLoc();
        Matrix matArray(2,2,{ node2Loc[0]-node1Loc[0], node2Loc[1]-node1Loc[1],
            node3Loc[0]-node1Loc[0], node3Loc[1]-node1Loc[1]});
        area = fabs(matArray.det())/2.0;
        centroid.push_back( (node1Loc[0]+node2Loc[0]+node3Loc[0])/3.0 );
        centroid.push_back( (node1Loc[1]+node2Loc[1]+node3Loc[1])/3.0 );
    }else{
        area = 0;
        std::cout << "WARNING IN Facet(): degenerate facet constructed." << std::endl;
    }
}//----------------------------------------------------------------------------------------------------


