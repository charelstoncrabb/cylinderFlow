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
    Mesh operator=(const Mesh& rhs);
private:
    Mesh(){};
    Mesh(std::vector<Node*> nodes, std::vector<Facet*> facets);
    Mesh(std::vector<Node*> nodes);
    void cleanUp();
    void parseMeshData(std::string meshDataFilename);
    void triangulate(void);
    void buildFacetList(void);
    void setSubmeshAdjacency(const Mesh* submesh);
    void mergeMeshes(const Mesh* leftSubMesh, const Mesh* rightSubMesh);
    std::vector<int> findBaseIndices(const Mesh* leftMesh, const Mesh* rightMesh);
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
    Matrix rotation;
};

class Node : public classInstanceCounter<Node>{
public:
//    Node(){};
    Node(int ID, double x, double y);
    std::vector<double> getLoc(void){return loc;};
    void setNode(int ID, double x, double y);
    double calcAngle(Node P, Node Q, std::string orientation = "ccw");
    bool isInCirc(Node A, Node B, Node C);
    std::vector<int> ordCandList(Node* node, std::string orientation = "ccw");
    bool isAdjacent(Node node);
    int findIndByID(std::vector<Node*> nodes);
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
//    Facet operator=(Facet& rhs);
private:
    const int ID;
    double area;
    std::vector<double> centroid;
    std::vector<Node*> nodes;
    std::vector<Facet*> adjacent;
    
    friend class Mesh;
};

#endif /* Mesh_h */
