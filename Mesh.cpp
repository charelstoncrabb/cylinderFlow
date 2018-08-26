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
Mesh::Mesh(std::string meshDataFilename, bool rotFlag, Mesh* constraintMesh) : rotateFlag(rotFlag), theta(0.1) {
    std::cout << "Parsing input data...";
    parseMeshData(meshDataFilename);
    std::cout << " ... Done!" << std::endl;
    std::cout << "Triangulating nodes...";
    preRotate();
    triangulate();
    std::cout << " ... Done!" << std::endl;
    std::cout << "Building facets from triangulation...";
    postRotate();
    buildFacetList();
    std::cout << " ... Done!" << std::endl;
	if (constraintMesh != NULL)
	{
		std::cout << "Applying constraint mesh to triangulation...";
		constrainMesh(constraintMesh);
		std::cout << "... Done!" << std::endl;
	}
    std::cout << "Identifying boundary nodes...";
    setBoundaryNodes();
    std::cout << " ... Done!" << std::endl;
    std::cout << "Mesh successfully constructed!" << std::endl;
}//----------------------------------------------------------------------------------------------------

// WRITES MESH DATA TO OUTPUT FILE  -------------------------------------------------------------------
void Mesh::writeMesh(std::string meshOutFile){
    std::fstream outfile(meshOutFile,std::fstream::out | std::fstream::trunc);
    std::fstream mainOut("Meshing.out",std::fstream::out | std::fstream::trunc);
    mainOut << output.str();
    outfile << "NODE DATA:" << std::endl << "ID ISBORD X Y ADJ" << std::endl;
    for(size_t i = 0; i < nodeList.size(); i++){
        if( nodeList[i]->isBoundaryNode )
            outfile << nodeList[i]->nodeID << " " << 1 << " " << nodeList[i]->loc[0] << " " << nodeList[i]->loc[1] << " ";
        else
            outfile << nodeList[i]->nodeID << " " << 0 << " " << nodeList[i]->loc[0] << " " << nodeList[i]->loc[1] << " ";
        for(size_t j = 0; j < nodeList[i]->adjacent.size(); j++){
            outfile << nodeList[i]->adjacent[j]->nodeID << " ";
        }
        outfile << std::endl;
    }
    outfile << std::endl << "BOUNDARY NODES: " << std::endl;
    for(size_t i = 0; i < boundaryNodes.size(); i++){
        outfile << boundaryNodes[i]->nodeID << " ";
    }
    outfile << std::endl << std::endl << "FACET DATA:" << std::endl << "ID AREA CENTROID VERTICES" << std::endl;
    for(size_t i = 0; i < facetList.size(); i++){
        outfile << facetList[i]->facetID << " " << facetList[i]->area << " (" << facetList[i]->centroid[0] << "," << facetList[i]->centroid[1] << ") " << facetList[i]->nodes[0]->nodeID << " " << facetList[i]->nodes[1]->nodeID << " "<< facetList[i]->nodes[2]->nodeID << std::endl;
    }
    outfile << std::endl << "NODE VERTEX DATA:" << std::endl << "NODE_ID FACET_IDS" << std::endl;
    for(size_t i = 0; i < nodeList.size(); i++){
        outfile << nodeList[i]->nodeID << " ";
        for(size_t j = 0; j < nodeList[i]->isVertexOf.size(); j++){
            outfile << nodeList[i]->isVertexOf[j]->facetID << " ";
        }
        outfile << std::endl;
    }
    outfile.close();
    mainOut.close();
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE/FACET LISTS (ALREADY TRIANGULATED) ------------------
Mesh::Mesh(std::vector<Node*> nodes, std::vector<Facet*> facets){
    for(size_t i = 0; i < nodes.size(); i++){
        Node *newNode = new Node(nodes[i]->nodeID,nodes[i]->loc[0],nodes[i]->loc[1]);
        nodeList.push_back(newNode);
    }
    for(size_t i = 0; i < nodes.size(); i++){
        Facet *newFacet = new Facet(facets[i]->area);
        facetList.push_back(newFacet);
    }
}//----------------------------------------------------------------------------------------------------

// PRIVATE CONSTRUCTOR CONSTRUCTS MESH GIVEN NODE LIST (CREATES TRIANGULATION)  -----------------------
Mesh::Mesh(std::vector<Node*> nodes){
    for(size_t i = 0; i < nodes.size(); i++){
        Node *newNode = new Node(nodes[i]->nodeID,nodes[i]->loc[0],nodes[i]->loc[1]);
        nodeList.push_back(newNode);
    }
    triangulate();
}//----------------------------------------------------------------------------------------------------

// DESTRUCTOR - DELETES ALLOCATED NODES/FACETS  -------------------------------------------------------
Mesh::~Mesh(){
    for(size_t i = 0; i < nodeList.size(); i++){
        if( nodeList[i] != NULL )
            delete nodeList[i];
    }
    for(size_t i = 0; i < facetList.size(); i++){
        if( facetList[i] != NULL )
            delete facetList[i];
    }
}//----------------------------------------------------------------------------------------------------

// PUBLIC CONST NODELIST ACCESS  ----------------------------------------------------------------------
const Node* Mesh::nodelist(size_t i) const{
    if( i < nodeList.size() )
        return nodeList[i];
    else{
        std::cout << "ERROR in Mesh::nodelist(int): attempting access outside of nodeList!" << std::endl;
        return NULL;
    }
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
        output << "ERROR IN parseMeshData(): cannot open file "
        << meshDataFilename << "!" << std::endl;
    }
    sortNodeList();
}//----------------------------------------------------------------------------------------------------

// ROTATES THE GRID POINTS PRIOR TO MESHING  ----------------------------------------------------------
void Mesh::preRotate(void){
	double x, y;
    if( this->rotateFlag ){
        output << "Pre-rotating grid..." << std::endl;
        for(size_t i = 0; i < nodeList.size(); i++){
			x = nodeList[i]->loc[0];
			y = nodeList[i]->loc[1];
			nodeList[i]->loc[0] = cos(theta)*x - sin(theta)*y;
			nodeList[i]->loc[1] = sin(theta)*x + cos(theta)*y;
        }
    }
}//----------------------------------------------------------------------------------------------------

// ROTATES THE GRID POINTS AFTER MESHING  ----------------------------------------------------------
void Mesh::postRotate(void){
	double x, y;
    if( this->rotateFlag ){
		output << "Post-rotating grid to original orientation..." << std::endl;
        for(size_t i = 0; i < nodeList.size(); i++){
			x = nodeList[i]->loc[0];
			y = nodeList[i]->loc[1];
			nodeList[i]->loc[0] = cos(theta)*x + sin(theta)*y;
			nodeList[i]->loc[1] = cos(theta)*y - sin(theta)*x;
        }
    }
}//----------------------------------------------------------------------------------------------------

// BUILDS DELAUNAY TRIANGULATION OF NODES IN MESH USING D&C ALGORITHM  --------------------------------
void Mesh::triangulate(void){
    output << "Triangulating on " << nodeList.size() << " nodes..." << std::endl;
    if( nodeList.size() <= 1 ){
        output << "ERROR IN triangulate(): error subdividing mesh!" << std::endl;
        return;
    }else if( nodeList.size() == 2 ){
        output << "Setting edge on two vertices..." << std::endl;
        setEdges(nodeList);
    }
    else if( nodeList.size() == 3 ){
        if( !areColinear(*(nodeList[0]),*(nodeList[1]),*(nodeList[2]) ) )
        {
            setEdges(nodeList);
        }else{
            bool _0lt1_ = *nodeList[0] < *nodeList[1], _0lt2_ = *nodeList[0] < *nodeList[2], _1lt2_ = *nodeList[1] < *nodeList[2];
			std::vector<Node*> subList1(2,NULL), subList2(2,NULL);
            if( (_0lt1_ && _1lt2_) || (!_0lt1_ && !_1lt2_) ){
				subList1[0] = nodeList[0]; subList1[1] = nodeList[1];
				subList2[0] = nodeList[1]; subList2[1] = nodeList[2];
            }
            if( (!_0lt1_ && _0lt2_) || (_0lt1_ && !_0lt2_) ){
				subList1[0] = nodeList[0]; subList1[1] = nodeList[1];
				subList2[0] = nodeList[0]; subList2[1] = nodeList[2];
            }
            if( (_0lt2_ && !_1lt2_) || (!_0lt2_ && _1lt2_) ){
				subList1[0] = nodeList[2]; subList1[1] = nodeList[0];
				subList2[0] = nodeList[2]; subList2[1] = nodeList[1];
            }
			setEdges(subList1);
			setEdges(subList2);
        }
    }else{
// TODO: Choose more intelligent division for conquering. E.g., divide into thirds if a base 3 #nodes, etc...
        size_t halfInd = nodeList.size()/2-1;
        std::vector<Node*> leftNodeList = range<Node>(nodeList,0,halfInd);
        std::vector<Node*> rightNodeList = range<Node>(nodeList,halfInd+1,nodeList.size()-1);
        Mesh *leftSubmesh = new Mesh(leftNodeList);
        Mesh *rightSubmesh = new Mesh(rightNodeList);
        setSubmeshAdjacency(leftSubmesh);
        setSubmeshAdjacency(rightSubmesh);
        output << "Merging meshes--leftSubmesh.size() = " << leftSubmesh->size() << ", rightSubmesh.size() = " << rightSubmesh->size() << std::endl;
        mergeMeshes(leftSubmesh,rightSubmesh);
        delete leftSubmesh;
        delete rightSubmesh;
    }
}//----------------------------------------------------------------------------------------------------

// BUILDS FACET LIST BY GRAPH TRAVERSAL AFTER TRIANGULATION HAS OCCURRED  -----------------------------
void Mesh::buildFacetList(void)
{
	double tol = 1.e-3;
	std::map<std::pair<double,double>, std::set<Node*>> facetsFound;

	for (size_t i = 0; i < nodeList.size(); i++)
	{
		nodeList[i]->ordAdjByAng();
		for (size_t j = 0; j < nodeList[i]->adjacent.size(); j++)
		{
			int k = (j + 1) % nodeList[i]->adjacent.size();
			if (nodeList[i]->adjacent[j]->isAdjacent(*nodeList[i]->adjacent[k]))
			{
				// We have found a triangle, and need to check if we've already found it
				// in some sort of efficient way.
				// We do this by mapping the set of 3 Nodes, keyed by centroid x value, 
				// and iterate over an interval of tolerance on x centroids, checking 
				// if the same set has already been found; if it hasn't construct a facet.
				std::set<Node*> currentSet;
				currentSet.insert(nodeList[i]);
				currentSet.insert(nodeList[i]->adjacent[j]);
				currentSet.insert(nodeList[i]->adjacent[k]);

				double centroidX = (nodeList[i]->x() + nodeList[i]->adjacent[j]->x()
					+ nodeList[i]->adjacent[k]->x()) / 3.0,
					centroidY = (nodeList[i]->y() + nodeList[i]->adjacent[j]->y()
						+ nodeList[i]->adjacent[k]->y()) / 3.0;

				std::map<std::pair<double, double>, std::set<Node*>>::iterator 
					lbound = facetsFound.lower_bound(std::pair<double, double>(centroidX - tol, centroidY - tol)),
					ubound = facetsFound.upper_bound(std::pair<double, double>(centroidX + tol, centroidY + tol));
				bool foundCurrentSet = false;
				while (lbound != ubound)
				{
					if (lbound->second == currentSet)
					{
						foundCurrentSet = true;
						break;
					}
					lbound++;
				}
				if (!foundCurrentSet)
				{
					facetsFound[std::pair<double, double>(centroidX, centroidY)] = currentSet;
					Facet* newFacet = new Facet({ nodeList[i],nodeList[i]->adjacent[j],nodeList[i]->adjacent[k] });
					facetList.push_back(newFacet);
					nodeList[i]->isVertexOf.push_back(newFacet);
					nodeList[i]->adjacent[j]->isVertexOf.push_back(newFacet);
					nodeList[i]->adjacent[k]->isVertexOf.push_back(newFacet);
				}
			}
		}
	}
}

// CONSTRAINS THE CONSTRUCTED MESH BASED ON PROVIDED CONSTRAINT MESH  ---------------------------------
void Mesh::constrainMesh(Mesh* constraintMesh)
{
	std::vector<std::vector<Node*>::iterator> toRemove;

	// A node in this mesh is inside the convhull of the constraint mesh iff all 
	// consecutive pairs of constraint boundary nodes form a positive angle with
	// the given node in this mesh
	for (std::vector<Node*>::iterator i = nodeList.begin(); i != nodeList.end(); ++i)
	{
		size_t j = 0,
			jp1 = (j + 1) % constraintMesh->boundaryNodes.size();

		Eigen::Vector2d u(constraintMesh->boundaryNodes[j]->x() - (*i)->x(),
			constraintMesh->boundaryNodes[j]->y() - (*i)->y()),
			v(constraintMesh->boundaryNodes[jp1]->x() - (*i)->x(),
				constraintMesh->boundaryNodes[jp1]->y() - (*i)->y());

		double totBoundaryAng = 0.,
			det = u(0)*v(1) - u(1)*v(0);

		while( det > 0 && j < constraintMesh->boundaryNodes.size() )
		{
			jp1 = (j + 1) % constraintMesh->boundaryNodes.size();

			u(0) = constraintMesh->boundaryNodes[j]->x() - (*i)->x();
			u(1) = constraintMesh->boundaryNodes[j]->y() - (*i)->y();
			v(0) = constraintMesh->boundaryNodes[jp1]->x() - (*i)->x();
			v(1) = constraintMesh->boundaryNodes[jp1]->y() - (*i)->y();

			det = det = u(0)*v(1) - u(1)*v(0);
			j++;

			totBoundaryAng += (det >= 0 ? acos(u.dot(v) / pow(u.dot(u)*v.dot(v),0.5)) : -acos(u.dot(v) / pow(u.dot(u)*v.dot(v), 0.5)));
		}
		
		if (j == constraintMesh->boundaryNodes.size() && fabs(totBoundaryAng - 2.0*acos(-1)) < 1.e-6)
			toRemove.push_back(i);
	}

	// Order of operations MUST occur as follows:
	// 1. Remove all facets to delete from this->facetList
	// 2. Delete the facets to delete (this will release all nodes' references to the respective facets)
	// 3. Remove node to delete from this->nodeList
	// 4. Delete the node to delete (this will release all adjacent nodes' references to itself)
	



	// Find all facets that need deleting:
	std::map<int,Facet*> facetsToDelete;
	for (size_t i = toRemove.size()-1; i < UINT_MAX; i--)
		for (size_t j = 0; j < (*(toRemove[i]))->isVertexOf.size(); j++)
			facetsToDelete[(*(toRemove[i]))->isVertexOf[j]->facetID] = (*(toRemove[i]))->isVertexOf[j];

	// Erase all facets to delete from this->facetList and delete:
	std::map<int,Facet*>::iterator ftditr = facetsToDelete.end();
	size_t firstFacetID = facetList[0]->facetID; // Need to figure out why this isn't starting at 0...
	do
	{
		ftditr--;
		facetList.erase( facetList.begin() + (ftditr->first - firstFacetID) );
		ftditr->second->freeReferences();
		delete ftditr->second;
	} while (ftditr != facetsToDelete.begin());

	std::vector<std::vector<Node*>::iterator>::iterator ntditr = toRemove.end();
	do
	{
		ntditr--;
		Node* nodeToDelete = **ntditr;
		nodeList.erase(*ntditr);
		nodeToDelete->freeReferences();
		delete nodeToDelete;
	} while (ntditr != toRemove.begin());
}//----------------------------------------------------------------------------------------------------

// SETS SUBMESH ADJACENCY IN COMBINED MESH WHEN MERGING SUBMESHES  ------------------------------------
void Mesh::setSubmeshAdjacency(const Mesh* submesh){
    for(size_t i = 0; i < submesh->nodeList.size(); i++){
        for(size_t j = 0; j < submesh->nodeList[i]->adjacent.size(); j++){
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
    std::vector<int> baseNodeIndices = {};
    findBaseIndices(baseNodeIndices,leftSubMesh,rightSubMesh);
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
                     ( !rightBase->adjacent[potRtCandInds[1]]->isInCirc(*leftBase,*rightBase,*rightBase->adjacent[potRtCandInds[0]]) ) ){
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
                     ( !leftBase->adjacent[potLtCandInds[1]]->isInCirc(*rightBase,*leftBase,*leftBase->adjacent[potLtCandInds[0]]) ) ){
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

// SETS BOUNDARY NODE LIST AFTER TRIANGULATION  -------------------------------------------------------
void Mesh::setBoundaryNodes(void)
{
    for(size_t i = 0; i < nodeList.size(); i++)
	{
		if (nodeList[i]->adjacent.size() == nodeList[i]->isVertexOf.size())
			nodeList[i]->isBoundaryNode = false;
		else if (nodeList[i]->adjacent.size() == nodeList[i]->isVertexOf.size() + 1)
		{
			nodeList[i]->isBoundaryNode = true;
			boundaryNodes.push_back(nodeList[i]);
		}
		else
			throw "node/facet adjacency error!";
    }
	sortNodesByAngle(boundaryNodes);
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODES FOR LEFT AND RIGHT SUBMESHES  -----------------------------------------------------
void Mesh::findBaseIndices(std::vector<int>& baseInds, const Mesh* leftMesh, const Mesh* rightMesh){
    baseInds.resize(2);
    int leftBaseInd = leftMesh->findLeftBaseNode(), rightBaseInd = rightMesh->findRightBaseNode();
	size_t itr = 0;
    bool rightBaseOK = false, leftBaseOK = false;
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
        while( !rightBaseOK && !leftBaseOK ){
            // if angle between base edge and any base node adjacents, reset base node to this adjacent node
            // Continue until right base node has no adjacents with positive angle with base edge, and left base node has no adjacents with negative angle with base edge.
            for(itr = 0; itr < leftMesh->nodeList[leftBaseInd]->adjacent.size(); itr++){
                Node B = *(leftMesh->nodeList[leftBaseInd]), P = *(rightMesh->nodeList[rightBaseInd]), Q = *(leftMesh->nodeList[leftBaseInd]->adjacent[itr]);
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
                Node B = *(rightMesh->nodeList[rightBaseInd]), P = *(leftMesh->nodeList[leftBaseInd]), Q = *(rightMesh->nodeList[rightBaseInd]->adjacent[itr]);
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
    baseInds[0] = leftBaseInd;
    baseInds[1] = rightBaseInd;
}//----------------------------------------------------------------------------------------------------

// FINDS BASE NODE FOR LEFT SUBMESH  ------------------------------------------------------------------
int Mesh::findLeftBaseNode(void) const{
    int baseInd = 0;
    for(size_t i = 0; i < nodeList.size(); i++){
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
    for(size_t i = 0; i < nodeList.size(); i++){
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
	size_t N = nodes.size();
    for(size_t i = 0; i < N-1; i++){
        for(size_t j = i+1; j < N; j++){
            nodes[i]->adjacent.push_back(nodes[j]);
            nodes[j]->adjacent.push_back(nodes[i]);
            output << "Setting Edge: " << nodes[i]->nodeID << " <==> " << nodes[j]->nodeID << std::endl;
        }
    }
}//----------------------------------------------------------------------------------------------------

// REMOVES ALL EDGES BETWEEN GIVEN NODES  -------------------------------------------------------------
void Mesh::rmEdges(std::vector<Node*> nodes){
	size_t N = nodes.size();
    for(size_t i = 0; i < N; i++){
        for(size_t j = i+1; j < N; j++){
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
	size_t N = nodeList.size();
    Node* temp;
    for(size_t i = N; i > 0; i--){
        for(size_t j = 0; j < i-1; j++){
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
	size_t aN = a.adjacent.size(), bN = b.adjacent.size();
    for(size_t i = 0; i < aN; i++){
        for(size_t j = 0; j < bN; j++){
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
Node::Node(int ID, double x, double y) : nodeID(ID), traversed(false), isBoundaryNode(false) {
    loc.push_back(x);
    loc.push_back(y);
}//----------------------------------------------------------------------------------------------------

Node::~Node()
{

}

void Node::freeReferences(void)
{
	// Remove self from adjacent list of all adjacent nodes
	for (size_t i = 0; i < adjacent.size(); i++)
	{
		int indToErase = findIndByID(adjacent[i]->adjacent);
		if (indToErase > -1 && indToErase < (int)adjacent[i]->adjacent.size())
			adjacent[i]->adjacent.erase(adjacent[i]->adjacent.begin() + indToErase);
	}
}

// SETS NODE DATA TO EXISTING NODE OBJECT  ------------------------------------------------------------
void Node::setNode(int ID, double x, double y){
//    nodeID = ID;
//    loc[0] = x;
//    loc[1] = y;
    std::cout << "ERROR: setNode() method is deprecated!" << std::endl;
}//----------------------------------------------------------------------------------------------------

// PUBLIC ACCESS TO ISVERTEXOF  -----------------------------------------------------------------------
std::vector<Facet> Node::getAdjFacets(void){
    std::vector<Facet> pubFacets;
    for(size_t i = 0; i < isVertexOf.size(); i++)
        pubFacets.push_back(*isVertexOf[i]);
    return pubFacets;
}//----------------------------------------------------------------------------------------------------

// RETURNS ANGLE BETWEEN LINE SEGMENTS this-P AND this-Q  ---------------------------------------------
double Node::calcAngle(Node P, Node Q){
    double angle = 0.0;
    std::vector<double> thisP = {P.loc[0]-loc[0],P.loc[1]-loc[1]},
                        thisQ = {Q.loc[0]-loc[0],Q.loc[1]-loc[1]};
    
    double det = thisP[0]*thisQ[1]-thisP[1]*thisQ[0];
    
    if( det >= 0.0 )
        angle = acos(dot(thisP,thisQ)/(norm(thisP)*norm(thisQ)));
    else
        angle = 2.0*acos(-1.0) - acos(dot(thisP,thisQ)/(norm(thisP)*norm(thisQ)));
    return angle;
}//----------------------------------------------------------------------------------------------------

// DECIDES IF this IS INSIDE CIRCUMCIRCLE OF TRIANGLE ABC  --------------------------------------------
bool Node::isInCirc(Node A, Node B, Node C){
    double tol = 0.0, r = 1e99;

	Eigen::Matrix2d A1, A2, A3;
	A1 << B.loc[0] - A.loc[0], B.loc[1] - A.loc[1], C.loc[0] - A.loc[0], C.loc[1] - A.loc[1];
	A2 << B.loc[0] - A.loc[0], B.loc[1] - A.loc[1], C.loc[0] - B.loc[0], C.loc[1] - B.loc[1];
	A3 << C.loc[0] - A.loc[0], C.loc[1] - A.loc[1], C.loc[0] - B.loc[0], C.loc[1] - B.loc[1];

	Eigen::Vector2d center, b1, b2, b3;
	b1 << 0.5*(pow(B.loc[0], 2.0) + pow(B.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0)), 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0));
	b2 << 0.5*(pow(B.loc[0], 2.0) + pow(B.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0)), 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(B.loc[0], 2.0) - pow(B.loc[1], 2.0));
	b3 << 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0)), 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(B.loc[0], 2.0) - pow(B.loc[1], 2.0));

	if( A1.determinant() ){
        center = A1.inverse()*b1;
    }else if( A2.determinant() ){
        center = A2.inverse()*b2;
    }else if( A3.determinant() ){
        center = A3.inverse()*b3;   
    }
	r = pow(pow(A.loc[0]-center(0),2)+pow(A.loc[1]-center(1),2),0.5);
	double dist2CircCenter = pow(pow(loc[0] - center(0), 2) + pow(loc[1] - center(1), 2), 0.5);
    if( dist2CircCenter > r+tol ){
        return false;
    }else{
        return true;
    }
}//----------------------------------------------------------------------------------------------------

// DECIDES IF this IS COCIRCULAR WITH CIRCUMCIRCLE OF TRIANGLE ABC  --------------------------------------------
bool Node::isCoCirc(Node A, Node B, Node C){
    double tol = 1e-5, r = 1e99;
    
	Eigen::Matrix2d A1, A2, A3;
	A1 << B.loc[0] - A.loc[0], B.loc[1] - A.loc[1], C.loc[0] - A.loc[0], C.loc[1] - A.loc[1];
	A2 << B.loc[0] - A.loc[0], B.loc[1] - A.loc[1], C.loc[0] - B.loc[0], C.loc[1] - B.loc[1];
	A3 << C.loc[0] - A.loc[0], C.loc[1] - A.loc[1], C.loc[0] - B.loc[0], C.loc[1] - B.loc[1];

	Eigen::Vector2d center(0, 0), b1, b2, b3;
	b1 << 0.5*(pow(B.loc[0], 2.0) + pow(B.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0)), 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0));
	b2 << 0.5*(pow(B.loc[0], 2.0) + pow(B.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0)), 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(B.loc[0], 2.0) - pow(B.loc[1], 2.0));
	b3 << 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(A.loc[0], 2.0) - pow(A.loc[1], 2.0)), 0.5*(pow(C.loc[0], 2.0) + pow(C.loc[1], 2.0) - pow(B.loc[0], 2.0) - pow(B.loc[1], 2.0));

	if (A1.determinant()) {
		center = A1.inverse()*b1;
	}
	else if (A2.determinant()) {
		center = A2.inverse()*b2;
	}
	else if (A3.determinant()) {
		center = A3.inverse()*b3;
	}
	r = pow(pow(A.loc[0] - center(0), 2) + pow(A.loc[1] - center(1), 2), 0.5);
	double dist2CircCenter = pow(pow(loc[0] - center(0), 2) + pow(loc[1] - center(1), 2), 0.5);
    
    if(dist2CircCenter > r-tol && dist2CircCenter < r+tol ){
        return true;
    }else{
        return false;
    }
}//----------------------------------------------------------------------------------------------------

// SORTS ADJACENT NODES BY ANGLE WITH GIVEN NODE  -----------------------------------------------------
std::vector<int> Node::ordCandList(Node* node, std::string orientation){
	size_t N = adjacent.size(), minInd;
    bool minIndSet = false;
    std::vector<double> angles(N);
    std::vector<int> ordered;
    for(size_t i = 0; i < N; i++){
        if( orientation == "cw" )
            angles[i] = calcAngle(*adjacent[i],*node);
        if( orientation == "ccw" )
            angles[i] = calcAngle(*node,*adjacent[i]);
    }
    double theta1 = acos(-1), theta2 = -1, tol = 1e-5;
    for(size_t i = 0; i < N; i++){
        for(size_t j = 0; j < N; j++){
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

// ORDERS ADJACENCT NODES BY ANGLE WITH +X AXIS  ------------------------------------------------------
void Node::ordAdjByAng(void){
	std::vector<Node*> adjCopy = adjacent;
	std::map<double, int> theta;

	std::vector<Eigen::Vector2d> cent2Nodes_normal;
	for (size_t i = 0; i < adjacent.size(); i++)
	{
		Eigen::Vector2d u(adjacent[i]->loc[0] - loc[0], adjacent[i]->loc[1] - loc[1]);
		u /= u.norm();
		cent2Nodes_normal.push_back(u);
	}
	size_t i = 0;
	double vDotE;
	for (i = 0; i < adjacent.size(); i++) {
		vDotE = acos(cent2Nodes_normal[i](0));
		if (cent2Nodes_normal[i](1) >= 0)
			theta[vDotE] = i;
		else
			theta[2 * acos(-1) - vDotE] = i;
	}
	i = 0;
	for (std::map<double, int>::iterator itr = theta.begin(); itr != theta.end() && i < adjacent.size(); ++itr) {
		adjacent[i] = adjCopy[itr->second];
		i++;
	}
}//----------------------------------------------------------------------------------------------------

// DECIDES IF GIVEN NODE IS ADJACENT TO this
bool Node::isAdjacent(Node node){
    for(size_t i = 0; i < adjacent.size(); i++)
        if( node.nodeID == adjacent[i]->nodeID )
            return true;
    return false;
}//----------------------------------------------------------------------------------------------------

// RETURNS INDEX OF this IN LIST nodes - RETURNS -1 IF this IS NOT IN LIST  ---------------------------
int Node::findIndByID(std::vector<Node*> nodes){
    int ind = -1;
    for(size_t i = 0; i < nodes.size(); i++){
        if( nodes[i]->nodeID == nodeID ){
            return i;
        }
    }
    return ind;
}//----------------------------------------------------------------------------------------------------

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
Facet::Facet(std::vector<Node*> nodeList) : facetID(ciCounter-1), nodes(nodeList){
    double tol = 1e-10;

    if( nodeList.size() == 3 ){
        std::vector<double> node1Loc = nodes[0]->getLoc(), node2Loc = nodes[1]->getLoc(),
            node3Loc = nodes[2]->getLoc();

        area = fabs((node2Loc[0] - node1Loc[0])*(node3Loc[1] - node1Loc[1]) 
			- (node2Loc[1] - node1Loc[1])*(node3Loc[0] - node1Loc[0]))/2.0;

        centroid.push_back( (node1Loc[0]+node2Loc[0]+node3Loc[0])/3.0 );
        centroid.push_back( (node1Loc[1]+node2Loc[1]+node3Loc[1])/3.0 );
        double ang1 = fmin(nodes[0]->calcAngle(*nodes[1],*nodes[2]),nodes[0]->calcAngle(*nodes[2],*nodes[1])),
               ang2 = fmin(nodes[1]->calcAngle(*nodes[0],*nodes[2]),nodes[1]->calcAngle(*nodes[2],*nodes[0])),
               ang3 = fmin(nodes[2]->calcAngle(*nodes[0],*nodes[1]),nodes[2]->calcAngle(*nodes[1],*nodes[0]));
        if( fabs(ang1+ang2+ang3 - acos(-1)) < tol ){
            angles.push_back(ang1);
            angles.push_back(ang2);
            angles.push_back(ang3);
        }
        else
            std::cout << "ERROR IN Facet(): inconsistent vertex angles." << std::endl;
    }else{
        area = 0;
        std::cout << "WARNING IN Facet(): degenerate facet constructed." << std::endl;
    }
    sortVerticesByAngle();
}//----------------------------------------------------------------------------------------------------

Facet::~Facet(void)
{

}

void Facet::freeReferences(void)
{
	// Remove self from isVertexOf list of all nodes
	for (size_t i = 0; i < nodes.size(); i++)
	{
		int indToErase = -1;
		for (int j = 0; j < (int)nodes[i]->isVertexOf.size() && indToErase == -1; j++)
			indToErase = this == nodes[i]->isVertexOf[j] ? j : -1;
		if(indToErase > -1 && indToErase < nodes[i]->isVertexOf.size())
			nodes[i]->isVertexOf.erase(nodes[i]->isVertexOf.begin() + indToErase);
	}
}
// SORTS THE FACET'S VERTICES BY CCW ORIENTATION  -----------------------------------------------------
void Facet::sortVerticesByAngle(void){
    std::vector<Node*> nlCopy = nodes;
    std::vector<double> angCopy = angles;
    std::map<double,int> theta;
	
	std::vector<Eigen::Vector2d> cent2Nodes_normal;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		Eigen::Vector2d u(nodes[i]->loc[0] - centroid[0], nodes[i]->loc[1] - centroid[1]);
		u /= u.norm();
		cent2Nodes_normal.push_back(u);
	}
	size_t i = 0;
    double vDotE;
    for( i = 0; i < nodes.size(); i++ ){
        vDotE = acos( cent2Nodes_normal[i](0) );
        if(cent2Nodes_normal[i](1) >= 0 )
            theta[vDotE] = i;
        else
            theta[2*acos(-1)-vDotE] = i;
    }
    i = 0;
    for( std::map<double,int>::iterator itr = theta.begin(); itr != theta.end() && i < nodes.size(); ++itr){
        nodes[i] = nlCopy[itr->second];
        angles[i] = angCopy[itr->second];
        i++;
    }
}//----------------------------------------------------------------------------------------------------

void sortNodesByAngle(std::vector<Node*>& nodeList)
{
	if (nodeList.size() == 0)
		return;

	double centroidX = nodeList[0]->x(), centroidY = nodeList[0]->y();

	for (size_t i = 1; i < nodeList.size(); i++)
	{
		centroidX += nodeList[i]->x();
		centroidY += nodeList[i]->y();
	}
	centroidX /= (double)nodeList.size();
	centroidY /= (double)nodeList.size();

	std::vector<Node*> nlCopy = nodeList;
	std::map<double, int> theta;

	std::vector<Eigen::Vector2d> cent2Nodes_normal;
	for (size_t i = 0; i < nodeList.size(); i++)
	{
		Eigen::Vector2d u(nodeList[i]->x() - centroidX, nodeList[i]->y() - centroidY);
		u /= u.norm();
		cent2Nodes_normal.push_back(u);
	}
	size_t i = 0;
	double vDotE;
	for (; i < nodeList.size(); i++) {
		vDotE = acos(cent2Nodes_normal[i](0));
		if (cent2Nodes_normal[i](1) >= 0)
			theta[vDotE] = i;
		else
			theta[2 * acos(-1) - vDotE] = i;
	}
	i = 0;
	for (std::map<double, int>::iterator itr = theta.begin(); itr != theta.end() && i < nodeList.size(); ++itr) {
		nodeList[i] = nlCopy[itr->second];
		i++;
	}
}