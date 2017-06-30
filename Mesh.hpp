//
//  Mesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//
#include "Includes.hpp"
#include "VectOps.hpp"
#include "Matrix.hpp"
#include "Solver.hpp"

#ifndef Mesh_h
#define Mesh_h
template<class T>
class classInstanceCounter{
public:
    classInstanceCounter(){
        ciCounter++;
    };
protected:
    static int ciCounter;
};

template<class T>
int classInstanceCounter<T>::ciCounter = 0;

class Node; //Forward declarations
class Facet;

class Mesh{
public:
    Mesh(const char* meshDataFilename, bool rotFlag = false);
    ~Mesh();
    void writeMesh(const char* meshOutFile = "Mesh.out");
    unsigned size(void) const {return (unsigned int)nodeList.size();};
    Mesh operator=(const Mesh& rhs);
    const Node* nodelist(int i) const;
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
    
    // Data members
    std::vector<Node*> nodeList;
    std::vector<Facet*> facetList;
    std::vector<Node*> boundaryNodes;
    std::stringstream output;
    bool rotateFlag;
    double theta;
};

class Node : public classInstanceCounter<Node>{
public:
//    Node(){};
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
    friend class SolverBase;
};

class Facet : public classInstanceCounter<Facet>{
public:
    Facet(double a): ID(ciCounter-1) ,area(a){};
    Facet(std::vector<Node*> nodeList);
    ~Facet(){ciCounter--;};
    double getArea(void){return area;};
    
//    Facet operator=(Facet& rhs);
private:
    void sortVerticesByAngle(void);
    
    const int ID;
    double area;
    std::vector<double> centroid;
    std::vector<Node*> nodes;
    std::vector<double> angles;
//    std::vector<Facet*> adjacent;
    
    friend class Mesh;
    friend class SolverBase;
};

#endif /* Mesh_h */
