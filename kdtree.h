#ifndef KDTREE_H
#define KDTREE_H
#include "math.h"
#include "triangle.h"
#include "statistics.h"

#include <vector>

// a iso oriented bounding box (axis-aligned bounding-box, AABB)
// defined by points with minimum and maximum coordinates
class BoundingBox {
  Vector<FLOAT,3> min,
                  max;
private:
  // returns true iff v is contained in this bounding box
  bool contains(Vector<FLOAT, 3> v);
public:
  // the default constructor creates an uninitialized bounding box
  BoundingBox();

  // creates a bounding box with the given minimum and maximum coordinates
  // the coordinates of min has to be smaller than of  max
  BoundingBox(Vector<FLOAT,3> min, Vector<FLOAT,3> max);

  // splits this bounding box into two adjacent boxes 
  // in the mid along the longest axis
  // the two resulting bounding boxes are written to left and right
  void split(BoundingBox & left, BoundingBox & right);

  // returnes true iff one of the three edges of the given triangle
  // is contained in this bounding box
  bool contains(Triangle<FLOAT> *triangle);

  // returns true iff ray defined by eye + t*direction
  // intersects this bounding box
  bool intersects(Vector<FLOAT,3> eye, Vector<FLOAT, 3> direction);
};

// A k-d-tree used for a spatial subdivision of a vector of triangles
class KDTree {
  static const int MAX_TRIANGLES_PER_LEAF = 16;

  // the bounding box for this KDTree
  BoundingBox box;

  // the triangles that are fully contained in the bonding box of this KDTree
  // and not fully contained in the left and right subtree
  std::vector< Triangle<FLOAT> *> triangles;
  KDTree *left = nullptr,
         *right = nullptr;
private:
  // creates a KDTree for the given triangles with the root tree
  // the tree's  bounding box has to be properly initialized for the triangles
  // returns the root of the tree
  KDTree *buildTree(KDTree * tree, std::vector< Triangle<FLOAT> *> & triangles);

public:
  // frees all resources of this KDTree
  ~KDTree();

  // creates an new KDTree for the given triangles and returns the root
  // of this tree
  static KDTree * buildTree(std::vector< Triangle<FLOAT> *> & triangles);

  // returns true iff this KDTree contains a triangle that intersects with the
  // ray defined by eye + t*direction
  // the point to the intersecting triangle nearest to the eye is written to nearest_triangle
  // the value t for the intersection point is set
  // the barycentric coordinates of the intersection are set to u and v 
  // minimum_t is the parametic value for the current nearst intersection
  bool hasNearestTriangle(Vector<FLOAT,3> eye, Vector<FLOAT,3> direction, Triangle<FLOAT> *  & nearest_triangle, FLOAT &t, FLOAT &u, FLOAT &v, FLOAT minimum_t = INFINITY);
};

#endif

