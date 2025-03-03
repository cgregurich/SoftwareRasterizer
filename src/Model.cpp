/*
A class to handle parsing model data from an .obj file.
Constructor takes a file path and will parse and store
the data from that file 
*/

#include <vector>
#include <string>
#include <iostream>

class Face {
    what is a face?
    For now, its a thing that has indices to vertices

};

class Model {
    public:
    std::string filepath;
    CoordinateType coordType;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Model(std::string filepath, CoordinateType coordType) {
        std::ifstream file(filepath);
        if (!file) {
            std::cerr << "Error opening file '" << filepath << "'" << std::endl;
            return;
        }
    }


};
