//
//  Mesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//

#ifndef Mesh_h
#define Mesh_h

#include "Includes.hpp"
#include "VectOps.hpp"
#include "Matrix.hpp"

//! classInstancesCounter class tracks number of instatiated objects of type T
template<class T>
class classInstanceCounter{
public:
	//! The constructor increments the global variable ciCounter<T>
    classInstanceCounter(){
        ciCounter++;
    }
	//! The destructor decrements the global variable ciCounter<T>
	~classInstanceCounter() {
		ciCounter--;
	}
protected:
    static int ciCounter;
};

template<class T>
int classInstanceCounter<T>::ciCounter = 0;

//Forward declarations
class Node;
class Facet;

//! Mesh class generates and stores a 2D mesh from an input grid points file
/*!
  This class generates and stores a 2D mesh from an input grid points file. A divide-and-conquer Delaunay Triangulation algorithm is implemented and requires as input a 2D grid point file. The class generates the list of nodes/adjacencies with the facet objects that are used in the finite element solver classes.
*/
class Mesh{
public:
	//! Public Constructor
	/*!
	This public constructor parses the provided grid point file and performs the triangulation, constructing the node and facet list required
		\param meshDataFilename (std::string) The grid point data file name to be parsed for grid point locations
		\param rotFlag (bool) whether to linearly rotate the grid points prior to meshing (and rotate back after meshing)
	*/
    Mesh(std::string meshDataFilename, bool rotFlag = false);

	//! Destructor
	/*!
	Performs necessary gargbage clean-up (frees node/facet list members)
	*/
    ~Mesh();

	//! Writes relevant mesh data to the provided output file name
	/*!
	This function writes all data to the provided output file necessary to exactly reconstruct the computed mesh.
		\param meshOutFile (std::string) file name of output file in which to write the computed mesh data
	*/
    void writeMesh(std::string meshOutFile = "Mesh.out");

	//! Query the size of the mesh (equivalently, number of nodes in mesh/grid)
	/*!
	Note: this size may significantly differ from the number of facet in the Mesh object
		\return size of the Mesh object (number of nodes in the Mesh)
	*/
    unsigned size(void) const {return (unsigned int)nodeList.size();};

	//! Assignment operator
	/*!
	This operator performs a copy assignment to the left-hand side Mesh object
		\param rhs (Mesh) the Mesh object to be copied left
		\return (Mesh) the left-hand side Mesh object, with data members set to rhs'
	*/
    Mesh operator=(const Mesh& rhs);

	//! Returns (as read-only) the i-th node in the Mesh objects node list
	/*!
	Note: this function performs a nodelist.size() to guard against invalid accesses
		\param i (size_t) index of node in node list to access
	*/
    const Node* nodelist(size_t i) const;
private:
    Mesh(){};
    Mesh(std::vector<Node*> nodes, std::vector<Facet*> facets);
    Mesh(std::vector<Node*> nodes);
    void cleanUp();
    void parseMeshData(std::string meshDataFilename);
    void preRotate(void);
    void postRotate(void);
    void triangulate(void);
    void buildFacetList(void);
    void setSubmeshAdjacency(const Mesh* submesh);
    void mergeMeshes(const Mesh* leftSubMesh, const Mesh* rightSubMesh);
    void setBoundaryNodes(void);
    void findBaseIndices(std::vector<int>& baseInds, const Mesh* leftMesh, const Mesh* rightMesh);
    int findLeftBaseNode(void) const;
    int findRightBaseNode(void) const;
    void setEdges(std::vector<Node*> nodes);
    void rmEdges(std::vector<Node*> nodes);
    void sortNodeList(void);
    std::vector<int> isAdjacent(Node a, Node b);
    bool areColinear(Node a, Node b, Node c);
    
    std::vector<Node*> nodeList;
    std::vector<Facet*> facetList;
    std::vector<Node*> boundaryNodes;
    std::stringstream output;
    bool rotateFlag;
    double theta;
};

//! Node object 
class Node : public classInstanceCounter<Node>{
public:
    Node(int ID, double x, double y);
    std::vector<double> getLoc(void){return loc;};
    unsigned short getDegree(void){return (unsigned short)adjacent.size();};
    std::vector<Facet> getAdjFacets(void);
    void setNode(int ID, double x, double y);
    double calcAngle(Node P, Node Q);
    bool isInCirc(Node A, Node B, Node C);
    bool isCoCirc(Node A, Node B, Node C);
    std::vector<int> ordCandList(Node* node, std::string orientation = "ccw");
    std::vector<int> ordAdjByAng(void);
    bool isAdjacent(Node node);
    int findIndByID(std::vector<Node*> nodes);
    double x(void){return loc[0];};
    double y(void){return loc[1];};
    bool operator<(Node& rhs) const;
private:
    int nodeID;
    std::vector<double> loc;
    std::vector<Node*> adjacent;
    std::vector<Facet*> isVertexOf;
    bool traversed;
    bool isBoundaryNode;
    friend class Mesh;
};

class Facet : public classInstanceCounter<Facet>{
public:
    Facet(double a): ID(ciCounter-1) ,area(a){};
    Facet(std::vector<Node*> nodeList);
    ~Facet(){ciCounter--;};
    double getArea(void){return area;};
    
private:
    void sortVerticesByAngle(void);
    
    const int ID;
    double area;
    std::vector<double> centroid;
    std::vector<Node*> nodes;
    std::vector<double> angles;
    
    friend class Mesh;
};

#endif /* Mesh_h */
