// #include "Obj.hpp"

// bool parseVertexFromObjLine(std::string rawLine, Vertex &vertex) {
//     /*
//     Currently working with simple .obj files with lines like:
//     v 0.5 0.12 0.654

//     So we're parsing them as:
//     <string> <float> <float> <float>

//     And passing these three floats into the given Vertex's x, y, and z
//     */
//     std::stringstream ss(rawLine);

//     /* Check if line can is a vertex line */
//     std::string keyword;
//     ss >> keyword;
//     if (keyword != "v") return false;

//     /* Parse x, y, and z values from line */
//     float x, y, z;
//     ss >> x >> y >> z;

//     vertex.x = x;
//     vertex.y = y;
//     vertex.z = z;

//     return true;
// }

// bool parseFaceFromObjLine(const std::string rawLine, std::vector<int> &vertexIndices) {
//     std::stringstream ss(rawLine);
//     std::string keyword;
//     ss >> keyword;
//     if (keyword != "f") return false;

//     /* Parse vertex indices from line */
//     /* Line example:
//     f 24/1/25 25/2/25 26/3/26
//     */
//     int a, b, c;
//     ss >> a >> b >> c;

//     vertexIndices[0] = a;
//     vertexIndices[1] = b;
//     vertexIndices[2] = c;

//     // std::cout << "a=" << a << "  b=" << b << "  c=" << c << std::endl;

//     return true;
// }

// /*
// Todo:
// Will likely need to make an Object class, or perhaps a Model class
// that contains the data parsed from a .obj file, which can then be passed to Canvas::drawObj() (a method that doesn't currently exist for the Canvas class).
// */
// void drawObj(const std::string filepath, const CoordinateType coordType ) {
//     // Model model(filepath, coordType);

//     ///////////////
//     std::ifstream file(filepath);
//     if (!file) {
//         std::cerr << "Error opening file" << std::endl;
//         return;
//     }

//     std::string line; // Line read from file
//     std::vector<Vertex> vertices; // Vertices parsed from file
//     Vertex vertex;

//     /* Parse all the vertex position lines and turn them into a vector of vertices
//     which will be used to index into based off the faces */
//     while (std::getline(file, line)) {
//         if (parseVertexFromObjLine(line, vertex)) {
//             vertices.push_back(vertex);
//         }
//     }
//     std::cout << "Done parsing vertices" << std::endl;


//     /* Parse the face elements */
//     // todo: can optimize by reading file once
//     file.clear(); 
//     file.seekg(0);
//     std::vector<int> vertexIndices(3);
//     int faceCounter = 0;
//     while (std::getline(file, line)) {
//         if (parseFaceFromObjLine(line, vertexIndices)) {
//             // todo this is just for debugging/seeing progress
//             faceCounter++;
//             if (faceCounter % 100 == 0) std::cout << "faceCounter=" << faceCounter << std::endl;
//             // todo end

//             int v1Index, v2Index, v3Index;
//             v1Index = vertexIndices[0];
//             v2Index = vertexIndices[1];
//             v3Index = vertexIndices[2];

//             Vertex v1, v2, v3;
//             v1 = vertices[v1Index-1];
//             v2 = vertices[v2Index-1];
//             v3 = vertices[v3Index-1];

//             /* Need to convert to screen coords if the input is NDC */
//             if (coordType == CoordinateType::NDC) {
//                 v1 = NDCToNormalizedScreen(v1, WIDTH, HEIGHT);
//                 v2 = NDCToNormalizedScreen(v2, WIDTH, HEIGHT);
//                 v3 = NDCToNormalizedScreen(v3, WIDTH, HEIGHT);
//             }

//             Triangle t(v1, v2, v3);
//             drawTriangle(t, CoordinateType::NormalizedScreen, true, true, false);
//         }
//     }

//     file.close();
// }