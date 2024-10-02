Apparently a software rasterizer needs four things:
* Matrix math (to project triangles/vertices in 3D space onto the 2D screen)
* A clipping algorithm which rejects triangles that are offscreen and clips the onscreen ones against the screen boundaries
* A line-drawing algorithm eg. DDA or Bresenham to draw the edges of these triangles
* A rasterization algorithm that will fill in the middle of the triangles

Another summary of steps to be done:
Step 1: Learn line rasterization. Implement Xiaolin Wu's line rasterizer to draw a line with some simple anti aliasing https://en.m.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
Step 2: Draw a triangle in wireframe. It's just step 1 three times
Step 3: Fill the triangle with a single color by doing three cross products to see if a point is inside the triangle or not. At the simplest and slowest, you can just do this for every point on the screen. You cross product from each vertex to the next vertex clockwise with the vector from the same starting vertex to the point. If you get negative results for each point, it's isnide. Draw the color there.
Step 4: Interpolate colors. Learn barycentric coordiinates (which is basically how close each point in a triangle is to each vertex) and interpolate the pixel color based on the colors of the three vertices.
Step 5: Put the triangle in world space rather than screen space. Start with an orthogonal camera staring down the z-axis and it's only a couple extra lines of code to clip triangles behind the camera. Try animating the triangle.
Step 6: Switch the orthogonal camera for a regular perspective camera with a view frustum with a near clipping plane, a far clipping plane, etc. Try animating the camera.
Step 7: Add z-buffering and multiple triangles animating around.
Step 8: Add lighting. A light contributes its light to a surface based on the dot product of the triangle normal with a vector pointing from the triangle to the light. The color of the light multiplies each element with the color of the surface.
Step 9: Add texturing. Load a picture from disk using CImg or something and UV map it to the triangle, basically saying this point here in the image is this point on the triangle.
Step 10: Load models off disk and render.

<hr>

## A useful response by ChatGPT when asked "What is a checklist for functionality of a successful rasterizer?"
A successful software rasterizer implements core rendering techniques while producing correct and visually accurate results. Here's a checklist of key features and functionalities that a well-built software rasterizer should have, progressing from the basics to more advanced features:

### 1. Basic Setup and Rendering Pipeline

* **Frame Buffer**: A pixel buffer to store RGB color values for each pixel in the image.
* **Coordinate System**: Correct handling of screen space (2D) and world space (3D) coordinates.
* **Vertex Processing**: Projecting 3D vertex positions to 2D screen space using perspective projection.
* **Triangle Setup**: Ability to rasterize triangles by filling the correct pixels between the triangle vertices.
* **Color Handling**: Support for basic RGB color output to represent the object or triangle colors.

### 2. Basic Rendering Features

* **Triangle Rasterization**: Efficient rasterization of triangles, with correct edge filling and pixel coverage.

* **Backface Culling**: Optional feature to avoid rendering triangles facing away from the camera, improving performance.

* **Z-buffer (Depth Buffering)**: Implement depth testing to ensure correct rendering of overlapping triangles (closer objects should cover farther ones).

* **Wireframe Mode (Optional)**: Ability to toggle between solid-fill and wireframe triangle rendering for debugging purposes.

### 3. Transformations

* **Model Transformation**: Support for translating, rotating, and scaling objects in 3D space.

* **View (Camera) Transformation**: Implement camera positioning and orientation (moving the camera through 3D space).

* **Projection (Perspective and Orthographic)**: Handle perspective projection (for 3D effects) and optionally orthographic projection.

### 4. Shading and Lighting
* **Flat Shading**: Color the entire triangle using a single color, typically based on the triangle's normal and a light source.
* **Gouraud Shading**: Interpolate colors across the triangle vertices to produce a smoother gradient based on vertex lighting.
* **Phong Shading**: Per-pixel shading using normals interpolated across the triangle for more accurate lighting results.
* **Basic Lighting Models**: Implement basic lighting techniques like:
	* **Diffuse Lighting (Lambertian Reflection)**: Light intensity based on the angle between the surface normal and light direction.
	* **Specular Highlighting (Phong Lighting)**: Simulate shiny surfaces with highlights where light reflects directly into the camera.

### 5. Advanced Rasterization Features
 Barycentric Coordinates: Use barycentric coordinates for accurate interpolation of attributes (like depth, color, or texture coordinates) across the surface of a triangle.
 Clipping: Correctly clip triangles that are partially outside the view frustum (the camera's visible area).
 Viewport Mapping: Map normalized device coordinates (after projection) to screen space coordinates based on screen dimensions.

### 6. Texture Mapping
 UV Mapping: Support UV coordinates for mapping 2D textures onto 3D surfaces.
 Perspective-Correct Interpolation: Handle correct texture mapping to avoid distortion caused by perspective projection (requires perspective-correct interpolation of texture coordinates).
 Bilinear Filtering: Optionally apply bilinear filtering to smooth textures when sampling between pixels.

### 7. Anti-Aliasing and Smoothing
 Edge Anti-Aliasing: Implement techniques like supersampling or multisampling to smooth jagged edges of triangles.
 Subpixel Accuracy: Improve rasterization by calculating triangle coverage more accurately at the subpixel level.

### 8. Optimization Techniques
 Scanline Rasterization: Implement efficient rasterization by filling horizontal spans of pixels for each triangle row.
 Backface Culling: Prevent rendering triangles facing away from the camera to save performance.
 Hierarchical Z-buffer: Optional optimization for depth tests, allowing early rejection of hidden objects.

### 9. Rendering Output and Debugging Tools
 Output Image: Ability to generate images as files (e.g., PPM, BMP, PNG) or display directly on the screen using a windowing library.
 Wireframe Mode: Option to visualize the model in wireframe mode for debugging geometry and rasterization issues.
 Depth Buffer Visualization: Render the depth buffer to an image to visualize the depth information.
 Performance Metrics (Optional): Output timing or performance stats, like frames-per-second (FPS), triangle count, or pixel fill rate.

### 10. Optional Advanced Features
 Normal Mapping: Use normal maps to add surface detail without adding extra geometry.
 Shadow Mapping: Implement basic shadow mapping to simulate shadows cast by objects onto others.
 Reflections and Refractions (Optional): Add reflective and refractive surfaces to simulate light bouncing or passing through transparent materials.
A Complete Checklist for a Successful Software Rasterizer:
Basic Setup: Framebuffer, screen-to-world projection, and vertex processing.
Basic Rendering: Triangle rasterization, backface culling, depth buffer, and color handling.
Transformations: Model, view, and projection transformations (perspective and orthographic).
Shading: Flat, Gouraud, and Phong shading; basic lighting models.
Textures: UV mapping, perspective-correct interpolation, and texture filtering.
Advanced Features: Anti-aliasing, clipping, and subpixel accuracy.
Optimizations: Efficient rasterization techniques and culling.
Output and Debugging: Image output, wireframe mode, and performance stats.
This checklist provides a roadmap for ensuring a successful rasterizer implementation, from basic rasterization techniques to advanced graphical effects. Achieving all these milestones will give you a full-featured rasterizer that rivals early 3D engines in both functionality and accuracy.

### Things to research:
* DDA
* Bresenham
* Minimum required matrix math/operations to be implemented

<hr>

### Questions:
* Why when viewing the image (at least in SumatraPDF) the red pixel looks like a blurry dot? Some sort of built in anti-aliasing? Is this part of the viewer or the image file itself? Likely not the image itself, right?

<hr>

