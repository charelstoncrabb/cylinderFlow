//
//  Mesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//
#include "Includes.hpp"
#include "VectOps.hpp"


#ifndef Mesh_h
#define Mesh_h

//Forward declaration of Node class -- Needed for use in Mesh class
class Node;

// Forward declaration of Facet class -- Needed for use in Mesh class
class Facet;

//! Mesh class generates and stores a 2D mesh from an input grid points file
/*!
  This class generates and stores a 2D mesh from an input grid points file. A divide-and-conquer <B><A HREF="https://en.wikipedia.org/wiki/Delaunay_triangulation">Delaunay Triangulation</A></B> algorithm is implemented and requires as input a 2D grid point file. The class generates the list of nodes/adjacencies with the facet objects that are used in the finite element solver classes.
  \todo Allow for constrained DT
*/
class Mesh{
public:
	//! Public Constructor
	/*!
	This public constructor parses the provided grid point file and performs the triangulation, constructing the node and facet list required
		\param meshDataFilename  <B>(std::string)</B> The grid point data file name to be parsed for grid point locations
		\param rotFlag <B>(bool)</B> whether to linearly rotate the grid points prior to meshing (and rotate back after meshing)
		\return Constructed Mesh object
	*/
    Mesh(std::string meshDataFilename, bool rotFlag = false);

	//! Destructor
	/*!
	Performs necessary gargbage clean-up (frees node/facet list members)
		\param None
		\return None
	*/
    ~Mesh();

	//! Writes relevant mesh data to the provided output file name
	/*!
	This function writes all data to the provided output file necessary to exactly reconstruct the computed mesh.
		\param meshOutFile <B>(std::string)</B> file name of output file in which to write the computed mesh data
		\return None
	*/
    void writeMesh(std::string meshOutFile = "Mesh.out");

	//! Query the size of the mesh (equivalently, number of nodes in mesh/grid)
	/*!
	Note: this size may significantly differ from the number of facet in the Mesh object
		\param None
		\return <B>(unsigned)</B> size of the Mesh object (number of nodes in the Mesh)
	*/
    unsigned size(void) const {return (unsigned int)nodeList.size();};

	//! Assignment operator
	/*!
	This operator performs a copy assignment to the left-hand side Mesh object
		\param rhs <B>(Mesh)</B> the Mesh object to be copied left
		\return <B>(Mesh)</B> the left-hand side Mesh object, with data members set to rhs'
	*/
    Mesh operator=(const Mesh& rhs);

	//! Returns (as read-only) the i-th node in the Mesh objects node list
	/*!
	Note: this function performs an i < **this**->nodelist.size() ? check to guard against invalid accesses. A runtime error is thrown if i >= **this**->nodeList.size()
		\param i <B>(size_t)</B> index of node in node list to access
		\return <B>(const Node*)</B> Node object at index i in **this**->nodeList
	*/
    const Node* nodelist(size_t i) const;

private:
    Mesh(){};
    Mesh(std::vector<Node*> nodes, std::vector<Facet*> facets);
    Mesh(std::vector<Node*> nodes);
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

//! Node class provides API for a vertex object in a Mesh
/*!
	This class stores the (2D) location and adjacency information for each point (i.e., Node) in the Mesh object. It also provides (through a private API to it's friend class Mesh) the necessary computations (relative to **this**) for computing the Delaunay triangulation performed in the Mesh constructor.
*/
class Node : public classInstanceCounter<Node>{
public:
	//! Public Constructor
	/*!
	This public constructor initialized the Node object's location and ID (used for bookkeeping)
		\param ID <B>(int)</B> identification number used for bookkeeping in Mesh class
		\param x <B>(double)</B> abscissa value of Node object in 2D coordinate system
		\param y <B>(double)</B> ordinate value of Node object in 2D coordinate system
		\return Constructed Node object
	*/
    Node(int ID, double x, double y);

	//! Public destructor
	/*!
	Note: the destructor is virtual to ensure proper decrementing of the inherited classInstanceCounter member ciCounter
		\param None
		\return None
	*/
	virtual ~Node() {};

	//! Less than operator
	/*!
	This less than operator implements a dictionary order on the 2D coordinate location of **this** relative to rhs
		\param rhs <B>(Node)</B> right-hand side of less-than operator (Node to be compared with **this**)
		\return <B>(bool)</B> The dictionary order of (**this**->x,**this**->y) with (rhs->x,rhs->y)
	*/
	bool operator<(Node& rhs) const;

	//! Query 2D location of Node object
	/*!
		\param None
		\return <B>(std::vector<int>)</B> 2D coordinate system location of Node object
	*/
	std::vector<double> getLoc(void) const{return loc;};

	//! Query abscissa value location of Node object
	/*!
		\param None
		\return <B>(double)</B> **this** abscissa value
	*/
	double x(void) const {return loc[0];};

	//! Query ordinate value location of Node object
	/*!
		\param None
		\return <B>(double)</B> **this** ordinate value
	*/
    double y(void) const {return loc[1];};

private:
	// Private helper methods required for triangulation computation:
	unsigned short getDegree(void){return (unsigned short)adjacent.size();};
    std::vector<Facet> getAdjFacets(void);
    void setNode(int ID, double x, double y);
    double calcAngle(Node P, Node Q);
    bool isInCirc(Node A, Node B, Node C);
    bool isCoCirc(Node A, Node B, Node C);
    std::vector<int> ordCandList(Node* node, std::string orientation = "ccw");
    void ordAdjByAng(void);
    bool isAdjacent(Node node);
    int findIndByID(std::vector<Node*> nodes);
	bool sharesFacet(Node* node);

	// Data members:
    int nodeID;
    std::vector<double> loc;
    std::vector<Node*> adjacent;
    std::vector<Facet*> isVertexOf;
    bool traversed;
    bool isBoundaryNode;

	// Allow Mesh and Facet classes private access
    friend class Mesh;
	friend class Facet;
};

//! Facet object is a triangle defined by three Node objects
/*!
This class stores the position, defining nodes, centroid, area, other essential information, etc. for a facet element of the Mesh object.
*/
class Facet : public classInstanceCounter<Facet>{
public:
	//! Public Constructor
	/*!
	This constructs a Facet object with area a, under the assumption that the defining Nodes will be subsequently added. 
		\todo Need to restructure some Mesh architecture to prevent the need for this constructor.
		\param a <B>(double)</B> prescribed area of the Facet to be constructed
		\return Constructed Facet object
	*/
    Facet(double a): ID(ciCounter-1) ,area(a){};

	//! Public Constructor
	/*!
	This constructs a Facet object with defining vertices given by argument nodeList
		\todo provide checks within this constructor to either block nodeList.size() != 3 or generalize the Facet code to allow for arbitrary polygons as Facet objects.
		\param nodeList <B>(std::vector<Node*>)</B> List of Node objects to use as the defining vertices for the Facet object.  It is implicitly assumed here that nodeList.size() == 3 
		\return Constructed Facet object
	*/
    Facet(std::vector<Node*> nodeList);

	//! Public destructor
	/*!
	Note: the destructor is virtual to ensure proper decrementing of the inherited classInstanceCounter member ciCounter
		\param None
		\return None
	*/
    virtual ~Facet(){};

	//! Query Facet object's area
	/*!
	This function allows for public access to the Facet object's area
		\param None
		\return <B>(double)</B> **this** area property.
	*/
    double getArea(void) const{return area;};
    
private:
	// Private methods:
    void sortVerticesByAngle(void);
    
	// Private data members:
    const int ID;
    double area;
    std::vector<double> centroid;
    std::vector<Node*> nodes;
    std::vector<double> angles;
    
	// Allow Mesh class private access
    friend class Mesh;
};

void sortNodesByAngle(std::vector<Node*>& nodeList);

#endif /* Mesh_h */
