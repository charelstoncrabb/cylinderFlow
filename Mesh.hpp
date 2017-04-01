//
//  Mesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//
#include "Includes.hpp"
#include "Matrix.hpp"
#ifndef Mesh_h
#define Mesh_h

class Node; //Forward declarations
class Facet;

class Mesh{
public:
    Mesh(std::string meshDataFilename);
private:
    Mesh();
    Mesh(std::vector<Node> nodes);
    void parseMeshData(std::string meshDataFilename);
    void triangulate(void);
    Mesh mergeMeshes(Mesh leftMesh, Mesh rightMesh);
    std::vector<Node> nodeList;
    std::vector<Facet> facetGraph;
};

class Node{
public:
    Node(int ID, double x, double y);
    std::vector<double> getLoc(void){return loc;};
    void setNode(int ID, double x, double y);
private:
    int nodeID;
    std::vector<double> loc;
    std::vector<Node> adjacent;
    
    friend class Mesh;
};

class Facet{
public:
    Facet(std::vector<Node> nodeList);
    double getArea(void){return area;};
private:
    double area;
    std::vector<Node> nodes;
    std::vector<Facet> adjacent;
    
    friend class Facet;
};
#endif /* Mesh_h */
