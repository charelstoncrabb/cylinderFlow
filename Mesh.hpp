//
//  Mesh.hpp
//  cylinderFlow
//
//  Created by Nicholas Crabb on 3/30/17.
//  Copyright © 2017 Nicholas Crabb. All rights reserved.
//
#include "Includes.hpp"
#ifndef Mesh_h
#define Mesh_h

class Node; //Forward declarations
class Facet;

class Mesh{
public:
    Mesh(std::string meshDataFilename);
private:
    void parseMeshData(std::string meshDataFilename);
    void triangulate(void);
    std::vector<Node> nodeList;
    std::vector<Facet> facetGraph;
};

class Node{
public:
    Node(int ID, double x, double y);
    double* getLoc(void){return loc;};
    void setNode(int ID, double x, double y);
private:
    int nodeID;
    double loc[2];
    std::vector<int> adjacent;
};

class Facet{
public:
    Facet();
    double getArea(void){return area;};
private:
    double area;
    std::vector<int> nodes;
    std::vector<Facet> adjacent;
};
#endif /* Mesh_h */
