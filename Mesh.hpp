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

class Node; //Forward declarations
class Facet;

class Mesh{
public:
    Mesh(std::string meshDataFilename);
    void writeMesh(std::string meshOutFile = "Mesh.out");
    Mesh operator=(const Mesh& rhs);
private:
    Mesh(){};
    Mesh(std::vector<Node> nodes, std::vector<Facet> facets);
    Mesh(std::vector<Node> nodes);
    void parseMeshData(std::string meshDataFilename);
    void triangulate(void);
    Mesh mergeMeshes(Mesh leftMesh, Mesh rightMesh);
    int findLeftBaseNode(void);
    int findRightBaseNode(void);
    void setEdges(std::vector<Node*> nodes);
    void rmEdges(std::vector<Node*> nodes);
    // Data members
    std::vector<Node> nodeList;
    std::vector<Facet> facetList;
};

class Node{
public:
    Node(){};
    Node(int ID, double x, double y);
    std::vector<double> getLoc(void){return loc;};
    void setNode(int ID, double x, double y);
    double calcAngle(Node P, Node Q);
    bool isInCirc(Node A, Node B, Node C);
    std::vector<int> ordCandList(Node node);
    bool isAdjacent(Node node);
    int findIndByID(std::vector<Node> nodes);
    Node operator=(Node& rhs);
    
private:
    
    
    int nodeID;
    std::vector<double> loc;
    std::vector<Node> adjacent;
    
    friend class Mesh;
};

class Facet{
public:
    Facet(){};
    Facet(std::vector<Node> nodeList);
    double getArea(void){return area;};
    Facet operator=(Facet& rhs);
private:
    double area;
    std::vector<Node> nodes;
    std::vector<Facet> adjacent;
    
    friend class Facet;
};
#endif /* Mesh_h */
