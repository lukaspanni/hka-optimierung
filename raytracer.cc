// g++ -Wall -pedantic -march=native -mfpmath=sse -mavx2 -O3 raytracer.cc statistics.cc
// Ggf. -std=c++11 hinzufügen
// Für spezifischen FLOAT Wert: -D FLOAT=double oder -D FLOAT='long double' hinzufügen
// Für k-d-Baum Variante:
// g++ -Wall -pedantic -march=native -mfpmath=sse -mavx2  -O3 -D USE_KDTREE raytracer.cc statistics.cc kdtree.cc
// Für optimierten Schnittpunktalgorithmus -D OPTIMIZED_INTERSECTS  hinzufügen
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <getopt.h> // GNU getopt
#include "math.h"
#include "statistics.h"
#include "triangle.h"
#include "kdtree.h"

static int resolution_x = 256;
static int resolution_y = 256;
static std::string input_file_name = "examples/teapot.obj";
static std::string output_bmp_file_name = "output.bmp";
static bool reverse_vertice_order = false; // switch to counter clockwise orientation of the input file
static bool write_ppm_to_stdout = true;

// simple value class for the origin and direction of a ray
template <class T>
class Ray {
private:
  Vector<T, 3> origin;
  Vector<T, 3> direction;
public:
  Ray(Vector<T, 3> origin, Vector<T, 3> direction) : origin(origin), direction(direction) {
  }

  Vector<T, 3> getOrigin() const {
    return origin;
  }

  Vector<T, 3> getDirection() const {
    return direction;
  }

};


// a value class for a color in RGB space.
// the red, green, and blue values should be in the range 0.0 to 1.0
// provides methods to add and multiply two colors or a color with a scalar.
class Color {
private:
  FLOAT red, green, blue; // 0.0 - 1.0
public:
  Color(FLOAT red = 0.0, FLOAT green = 0.0, FLOAT blue = 0.0)
     : red(red), green(green), blue(blue)
  {
  }

  FLOAT getRed() const {
    return red;
  }

  FLOAT getGreen() const {
    return green;
  }

  FLOAT getBlue() const {
    return blue;
  }

  Color operator+(Color addend) const {
    return Color(red + addend.red, green + addend.green, blue + addend.blue);
  }

  Color operator*(Color factor) const {
    return Color(red * factor.red, green * factor.green, blue * factor.blue);
  }

  friend Color operator*(const FLOAT factor, Color color)  {
    return Color(factor * color.red, factor * color.green, factor * color.blue);
  }
};


// a value class for material information like the color, ambient and diffuse light factors,
// the amount of reflection and transmission light
class Material {
private:
  Color color;
  FLOAT ambient; //  percent
  FLOAT diffuse; //  percent
  FLOAT reflection; // percent
  FLOAT transmission; // percent
  // sum of all four should be <= 1.0
public:
  Material(Color color = Color(1.0, 0.7, 0.8),
           FLOAT ambient = 0.3,
           FLOAT diffuse = 0.7,
           FLOAT reflection = 0.0,
           FLOAT transmission = 0.0) 
    : color(color), ambient(ambient), diffuse(diffuse), reflection(reflection), transmission(transmission) {
  }

  Color getColor() const {
    return color;
  }

  FLOAT getAmbient() const {
    return ambient;
  }

  FLOAT getDiffuse() const {
    return diffuse;
  }

  FLOAT getReflection() const {
    return reflection;
  }

  FLOAT getTransmission() const {
    return transmission;
  }
};


// stores the rasterized screen with the color of each pixel
class Screen {
private:
  size_t width;
  size_t height;
  std::unique_ptr<Color []> buffer;
public:
  Screen(size_t width, size_t  height)
      : width(width), height(height), buffer(std::unique_ptr<Color []>( new Color[width * height] ))
  {
  }

  void setPixel(size_t x, size_t y, Color color) {
    buffer[x + y * width] = color;
  }

  void clear(Color color = Color(0.0, 0.0, 0.0) ) {
    for (size_t x = 0u; x < getWidth(); x++) {
      for (size_t y = 0u; y < getHeight(); y++) {
         setPixel(x, y, color );
      }
    }
  }

  size_t getWidth() const {
    return this->width;
  }
  size_t getHeight() const {
    return this->height;
  }

  Color getPixel(size_t x, size_t y) const {
    return buffer[x + y * width];
  }

};

// a simple camera that can be used for a central projection from an eye to a rectangular view port
// defined by a upper left point and a vector pointing downsides and to the right
// both vectors should be located in the same plane and orthogonal to  the eye->center ray (projection is distorted otherwise)
// the viewport has a defined pixel height and width 
class Camera {
  Vector<FLOAT, 3> eye,
               upper_left,
               down,
               right;
  FLOAT pixelWidth;
  FLOAT pixelHeight;
public:

  Camera(Vector<FLOAT, 3> eye, Vector<FLOAT, 3> center, Vector<FLOAT, 3>up, Vector<FLOAT, 3> right, FLOAT pixelWidth = 1.0, FLOAT pixelHeight = 1.0 )
    : eye(eye), upper_left(center - right + up), down(-2.0 * up), right(2.0 * right), pixelWidth(pixelWidth), pixelHeight(pixelHeight) {
     this->right.normalize();
     this->down.normalize();
  }

  // creates a camera from a given eye->center ray
  // this ray points to a center of a rectangle created by an up and right vector
  // the pixel height and width are calculated from the screen coordiates with the length of the up and right vector
  Camera(Vector<FLOAT, 3> eye, Vector<FLOAT, 3> center, Vector<FLOAT, 3> up, Vector<FLOAT, 3> right, Screen & screen )
    : Camera(eye, center, up, right,
             2.0 * ( right.length() / screen.getWidth() ),
             2.0 * ( up.length() / screen.getHeight() ) ) {
  }

  // returns a ray pointing to the given x,y from this camera's eye point
  Ray<FLOAT> getRay(size_t x, size_t y) const {
    Vector<FLOAT, 3> direction = (upper_left
                  + (x * pixelWidth) * right
                  + (y * pixelHeight) * down )
                  - eye;
    return Ray<FLOAT>(eye, direction);
  }
};


// puts out the image as PPM
std::ostream & operator<<(std::ostream & out, const Screen & screen) {
  out << "P3" << std::endl;
  out << screen.getWidth() << " " << screen.getHeight() << std::endl;
  out << "255" << std::endl;
  for (size_t y = 0u; y < screen.getHeight(); y++) {
    for (size_t x = 0u; x < screen.getWidth(); x++) {
      std::cout << (unsigned short) (screen.getPixel(x,y).getRed() * 255.0) << " "
                << (unsigned short) (screen.getPixel(x,y).getGreen() * 255.0) << " "
                << (unsigned short) (screen.getPixel(x,y).getBlue() * 255.0) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  return out;
}

// writes out the image as binary BMP (for windows)
void write_bmp(std::ostream & out, const Screen & screen) {
  unsigned long long size_of_bitmap_data = screen.getWidth() * screen.getHeight() * 4;
  out.put(0x42); out.put(0x4D); // "BM"
  out.put(0x00); out.put(0x00); out.put(0x00); out.put(0x00); // size of bmp file
  out.put(0x00); out.put(0x00);
  out.put(0x00); out.put(0x00);
  out.put(0x36); out.put(0x00);out.put(0x00); out.put(0x00); // offset to start of pixel array
  // Header
  out.put(0x28); out.put(0x00); out.put(0x00); out.put(0x00);  // 40 bytes Number of bytes in the DIB header (from this point)
  out.put( screen.getWidth() & 0xFF ); out.put( screen.getWidth() >> 8 & 0xFF );
  out.put(0x00); out.put(0x00); // width in pixel
  out.put( screen.getHeight() & 0xFF ); out.put( screen.getHeight() >> 8 & 0xFF );
  out.put(0x00); out.put(0x00); // height in pixel
  out.put(0x01); out.put(0x00); // 1 = number of color planes used
  out.put(0x18); out.put(0x00); // 24 bits per pixel (RGB)
  out.put(0x00); out.put(0x00); out.put(0x00); out.put(0x00); // 0 = no compression
  // size of raw bitmap data 16 bytes
  out.put(size_of_bitmap_data & 0xFF); 
  out.put(size_of_bitmap_data >> 8 & 0xFF); 
  out.put(size_of_bitmap_data >> 16 & 0xFF); 
  out.put(size_of_bitmap_data >> 32 & 0xFF); 
  out.put(0x13); out.put(0x0B); out.put(0x00); out.put(0x00); // 72 DPI resolution for printing
  out.put(0x13); out.put(0x0B); out.put(0x00); out.put(0x00); //           "
  out.put(0x00); out.put(0x00); out.put(0x00); out.put(0x00); // 0 colors in the palette
  out.put(0x00); out.put(0x00); out.put(0x00); out.put(0x00); // 0 = all colors are important
  // start of pixel map array, 4 byte alignment padding at end of line if nedded
  for (size_t y = 0u; y < screen.getHeight(); y++ ) {
    for (size_t x = 0u; x < screen.getWidth(); x++) {
      out.put( ((unsigned short) (screen.getPixel(x, screen.getHeight() - 1 - y).getRed() * 255.0) ) & 0xFF );
      out.put( ((unsigned short) (screen.getPixel(x, screen.getHeight() - 1 - y).getGreen() * 255.0) ) & 0xFF );
      out.put( ((unsigned short) (screen.getPixel(x, screen.getHeight() - 1 - y).getBlue() * 255.0) ) & 0xFF );
    }
    for (size_t padding_bytes = 0u; padding_bytes < screen.getWidth() % 4; padding_bytes++) {
      out.put( 0x00 );
    }
  } 
}

// a simple light source with a position and a light color
// the light is emitted evenly from the given position in all directions
class Light {
  Vector<FLOAT, 3> position;
  Color color;
public:
  Light(Vector<FLOAT, 3> position = Vector<FLOAT, 3>({0.0, 0.0, 1000.0}),
        Color color = Color(1.0, 1.0, 1.0) )
    : position(position), color(color) {
  }

  Color getColor() const {
     return color;
  }

  Vector<FLOAT, 3> getPosition() const {
    return position;
  }
};

// the scene with all triangles and light sources
// contains the algorithm to determine the visible triangle for a given ray
class Scene {
  std::vector< Triangle<FLOAT> > triangles;
  std::vector< Light > lights;
public:
  void add(Triangle<FLOAT> triangle) {
    triangles.push_back( triangle );
  }

  void addLight(Light light) {
    lights.push_back( light );
  }

  //  quickfix, do not alter triangles after calling this method
  std::vector< Triangle<FLOAT> * > getTriangles() {
    std::vector< Triangle<FLOAT> * > triangles_ptr;
    for (size_t i = 0; i < triangles.size(); i++) {
      triangles_ptr.push_back( &triangles[i] );      
    }    
    return triangles_ptr;
  }

  // brute force algorithme to determine the nearest (if NEAREST is true) triangle of this scence that has
  // an intersection with the given ray
  // the intersection point p is p = ray.origin + t * ray.direction
  // the u-v parameters are the barycentric coordinates of the intersection within  the triangle

bool hasNearestTriangle(const Ray<FLOAT> & ray, Triangle<FLOAT> *  & nearest_triangle, FLOAT &t, FLOAT &u, FLOAT &v, const bool NEAREST = true)  {
  FLOAT minimum_t = INFINITY;
  FLOAT minimum_u  = 0.0, minimum_v = 0.0;
  for (size_t i = 0u; i < triangles.size(); i++) {
    Triangle<FLOAT> *triangle = &triangles[i];
    stats.no_ray_triangle_intersection_tests++;
    bool intersect = triangle->intersects(ray.getOrigin(), ray.getDirection(), t, u, v, minimum_t);
    if ( intersect ) {
      stats.no_ray_triangle_intersections_found++;          
      if ( (nearest_triangle == nullptr)  || (t < minimum_t) ) {      
        minimum_t = t;
        minimum_u = u;
        minimum_v = v;
        nearest_triangle = triangle;
        if ( ! NEAREST ) {
          return true;
        }
      }
    }
  }
  t = minimum_t;
  u = minimum_u;
  v = minimum_v;
  return nearest_triangle != nullptr;
}



  // return true iff the ray is blocked by a triangle
  bool blocked(const Ray<FLOAT> & ray)  {
     Triangle<FLOAT> * triangle = nullptr;
     FLOAT t,u,v;
     return hasNearestTriangle(ray, triangle, t, u, v, false);
  }

  // shades the intersection point of the ray with the triangle with the material information
  // the normal vectors and u-v-parameter are used for interpolation
  Color shade(const Ray<FLOAT> & ray, Triangle<FLOAT> & triangle, Material & material, FLOAT &t, FLOAT &u, FLOAT &v)  {
    Color color =  material.getAmbient() * material.getColor();
    FLOAT factor = 1.0 / lights.size();
    Vector<FLOAT, 3> intersection = (ray.getOrigin() + 0.9999 * t * ray.getDirection());
    Vector<FLOAT, 3> normal = (u * triangle.n1) + (v * triangle.n2) + ( (1.0 - u - v) * triangle.n3);    
    
    normal.normalize();
    for (Light light : lights) {
      Vector<FLOAT, 3> light_direction = light.getPosition() - intersection;
      const Ray<FLOAT> ray_to_light(intersection, light_direction);
      if ( ! blocked( ray_to_light )  ) { 
        light_direction.normalize();
        FLOAT angle = std::max<FLOAT>(0.0, light_direction.scalar_product(normal));
        color = color + angle * factor * material.getDiffuse() * material.getColor() * light.getColor();
      }
    }
    return color;
  }
};

// Reads in 3d scene from Wavefront.obj
// only triangles are read in, no polygones, no normals
// only lines starting with v (vertexes) and f (faces) are read in
// other lines are ignored
void read_wavefront(std::ifstream & in, Scene & scene) {
  std::vector< Vector<FLOAT, 3> > vertices;
  std::vector< Vector<FLOAT, 3> > normals;
  size_t no_of_triangles = 0u;
  Vector<FLOAT, 3> vertice;
  Vector<FLOAT, 3> normal;
  char c;
  while (in >> c) {
    if (c == 'v' && in.peek() == ' ') {
       FLOAT x, y, z;
       in >> x;
       in >> y;
       in >> z;
       vertice = Vector<FLOAT, 3>( {x, y, z} );
       vertices.push_back( vertice ); 
    } else if (c == 'f' && in.peek() == ' ') {

       size_t xv, yv, zv;
       size_t xn = 0, yn, zn;
       in >> xv;
       if (in.peek() == '/') in.ignore(); // no texture coordinates supported
       if (in.peek() == '/') { in.ignore(); in >> xn;}
       in >> yv;
       if (in.peek() == '/') in.ignore(); // no texture coordinates supported
       if (in.peek() == '/') { in.ignore(); in >> yn;}
       in >> zv;
       if (in.peek() == '/') in.ignore(); // no texture coordinates supported
       if (in.peek() == '/') { in.ignore(); in >> zn;}
       
       if (reverse_vertice_order) {
         std::swap(yv, zv);
         std::swap(yn, zn);
       }
       if (xn == 0) { // no normals 
         scene.add(Triangle<FLOAT> ( {vertices[xv - 1], vertices[yv - 1], vertices[zv - 1]} ));
       } else {
         scene.add(Triangle<FLOAT> ( {vertices[xv - 1], vertices[yv - 1], vertices[zv - 1], normals[xn - 1], normals[yn - 1], normals[zn - 1]} ));
       }
       no_of_triangles++;
    } else if (c == 'v' && in.peek() == 'n') {
       in >> c;
       if ( in.peek() == ' ' ) {
         FLOAT nx, ny, nz;
         in >> nx;
         in >> ny;
         in >> nz;
         normal = Vector<FLOAT, 3>( {nx, ny, nz} );
         normals.push_back( normal );         
       }
    }
    // read to end of line
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  std::cerr << "no of triangles : " << no_of_triangles << std::endl;
  std::cerr << "no of vertices : " << vertices.size() << std::endl;
  std::cerr << "no of normals : " << normals.size() << std::endl;
  std::cerr << "memory used for all triangles [byte] : " << no_of_triangles * sizeof(Triangle<FLOAT>) << std::endl;   
}


// the raytrace algorithm but without refraction and reflection
void raytrace(Camera &camera, Scene & scene, Screen & screen, KDTree *tree = nullptr) {
  screen.clear();
  Material material;
  Color color;
  for (FLOAT x = 0.0; x < screen.getWidth(); x++) {
    for (FLOAT y = 0.0; y < screen.getHeight(); y++) {
      color = Color(0.0, 0.0, 0.0);
      const Ray<FLOAT> ray = camera.getRay(x,y);
      Triangle<FLOAT> *nearest_triangle = nullptr;
      FLOAT t = INFINITY, u = 0, v = 0;
#ifndef USE_KDTREE
      bool hasNearestTriangle = scene.hasNearestTriangle(ray, nearest_triangle, t, u, v);
#else
      bool hasNearestTriangle = tree->hasNearestTriangle(ray.getOrigin(), ray.getDirection(),  nearest_triangle, t, u, v);
#endif
      if ( hasNearestTriangle ) {
        // no reflection and refraction
        color = scene.shade(ray, *nearest_triangle, material, t, u, v);
      }
      screen.setPixel(x, y, color);
    }
  }
}




void parse_arguments(int argc, char *argv[]) {
  static struct option long_options[] = {
   {"file", required_argument, 0, 'f'},
   {"width", required_argument, 0, 'w'},
   {"height", required_argument, 0, 'h'},
   {"help", optional_argument, 0, 'u'},
   {"no_ppm", optional_argument, 0, 'n'},
  };
  int c;
  int option_index;

  while ( (c = getopt_long (argc, argv, "ro:", long_options, &option_index)) != -1 ) {
    switch (c) {
    case 'f': input_file_name = std::string(optarg);
              break;
    case 'r': reverse_vertice_order = true;
              break;
    case 'w': resolution_x = std::stoi(optarg);
              break;
    case 'h': resolution_y = std::stoi(optarg);
              break;
    case 'o': output_bmp_file_name = std::string(optarg);
              break;
    case 'n': write_ppm_to_stdout = false;
              break;
    case 'u': std::cout << "Aufruf: " << argv[0] << " [options]" << std::endl;
              std::cout << "  --file FILE          wavefront obj input file ('example/teapot.obj')" << std::endl;
              std::cout << "  --width WIDTH        horizontal resolution (256)" << std::endl;
              std::cout << "  --height HEIGHT      vertical resolution (256)" << std::endl;
              std::cout << "  --help               this help text" << std::endl;
              std::cout << "  --no_ppm             no PPM output is generated" << std::endl;
              std::cout << "  -o BMP               BMP output file ('output.bmp')" << std::endl;
              std::cout << "  -r                   reverse vertice order of triangles in input file" << std::endl;
              exit(0);
    } 
  }

  std::cerr << "reading from       : " << input_file_name << std::endl;
  std::cerr << "writing bmp to     : " << output_bmp_file_name << std::endl;
  std::cerr << "resolution x set to: " << resolution_x << std::endl;
  std::cerr << "resolution y set to: " << resolution_y << std::endl;
  std::cerr << "vertices reversed  : " << reverse_vertice_order << std::endl;
  std::cerr << "write ppm          : " << write_ppm_to_stdout << std::endl;
}

int main(int argc, char *argv[]) {
  parse_arguments(argc, argv);
  Scene scene;
  scene.addLight( Light( Vector<FLOAT,3>{500.0, 500.0, 1000.0}) );
  std::ifstream input(input_file_name);
  std::ofstream output(output_bmp_file_name, std::ofstream::binary); // for windows
  read_wavefront(input, scene);
  std::vector<Triangle<FLOAT> *> triangles = scene.getTriangles();
  Screen screen(resolution_x, resolution_y);


  // camera for teapot_large.obj
  Camera camera( Vector<FLOAT, 3>( {  0.05, 1.0, 200.0} ),
                 Vector<FLOAT, 3>( {  0.05, 1.0, 100.0} ),
                 Vector<FLOAT, 3>( {  0.0, 1.6, 0.0} ),
                 Vector<FLOAT, 3>( {  1.6, 0.0,  0.0} ),
                 screen );

  stats.time_start();
#ifndef USE_KDTREE
  raytrace(camera, scene, screen);
#else
  std::unique_ptr<KDTree> tree =  std::unique_ptr<KDTree>( KDTree::buildTree( triangles ) );
  raytrace(camera, scene, screen, tree.get());
#endif
  stats.time_stop();
  if (write_ppm_to_stdout) {
    std::cout << screen; // write image in PPM format to the standard output
  }
  write_bmp(output, screen);
  output.close();
  stats.print();

  return 0;
}

