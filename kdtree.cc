#include "kdtree.h"
#include <algorithm>
#include <iostream>

BoundingBox::BoundingBox() {}

BoundingBox::BoundingBox(Vector<FLOAT, 3> min, Vector<FLOAT, 3> max)
    : min(min), max(max) {}

void BoundingBox::split(BoundingBox &left, BoundingBox &right)
{
  float lengthX = std::abs(max[0] - min[0]);
  float lengthY = std::abs(max[1] - min[1]);
  float lengthZ = std::abs(max[2] - min[2]);

  // min/max points are always the same, only set the missing point during split
  left.min = min;
  right.max = max;

  if (lengthX >= lengthY && lengthX >= lengthZ)
  {
    float newWidth = lengthX / 2;
    left.max = Vector<float, 3>{min[0] + newWidth, max[1], max[2]};
    right.min = Vector<float, 3>{min[0] + newWidth, min[1], min[2]};
    return;
  }

  if (lengthY >= lengthX && lengthY >= lengthZ)
  {
    float newWidth = lengthY / 2;
    left.max = Vector<float, 3>{max[0], min[1] + newWidth, max[2]};
    right.min = Vector<float, 3>{min[0], min[1] + newWidth, min[2]};
    return;
  }

  float newWidth = lengthZ / 2;
  left.max = Vector<float, 3>{max[0], max[1], min[2] + newWidth};
  right.min = Vector<float, 3>{min[0], min[1], min[2] + newWidth};
}

bool BoundingBox::contains(Vector<FLOAT, 3> v)
{
  return v[0] >= min[0] && v[1] >= min[1] && v[2] >= min[2] &&
         v[0] <= max[0] && v[1] <= max[1] && v[2] <= max[2];
}

bool BoundingBox::contains(Triangle<FLOAT> *triangle)
{
  // one point in box
  return contains(triangle->p1) || contains(triangle->p2) || contains(triangle->p3);
}

bool BoundingBox::intersects(Vector<FLOAT, 3> eye, Vector<FLOAT, 3> direction)
{
  // slab test implementation
  FLOAT tmin[3] = {(min[0] - eye[0]) / direction[0],
                   (min[1] - eye[1]) / direction[1],
                   (min[2] - eye[2]) / direction[2]};
  FLOAT tmax[3] = {(max[0] - eye[0]) / direction[0],
                   (max[1] - eye[1]) / direction[1],
                   (max[2] - eye[2]) / direction[2]};
  FLOAT tminimum = std::min(tmin[0], tmax[0]);
  FLOAT tmaximum = std::max(tmin[0], tmax[0]);
  tminimum = std::max(tminimum, std::min(tmin[1], tmax[1]));
  tmaximum = std::min(tmaximum, std::max(tmin[1], tmax[1]));
  tminimum = std::max(tminimum, std::min(tmin[2], tmax[2]));
  tmaximum = std::min(tmaximum, std::max(tmin[2], tmax[2]));

  return tmaximum >= tminimum;
}

std::string BoundingBox::toString()
{
  return "min: (" + std::to_string(min[0]) + ", " + std::to_string(min[1]) + ", " + std::to_string(min[2]) + ") max: (" + std::to_string(max[0]) + ", " + std::to_string(max[1]) + ", " + std::to_string(max[2]) + ")";
}

KDTree::~KDTree()
{
  delete left;
  delete right;
}

KDTree *KDTree::buildTree(KDTree *tree, std::vector<Triangle<FLOAT> *> &triangles, int depth = 0)
{

#ifdef DEBUG
  std::cout << "Recursion Depth: " << depth << " Building kd-tree with " << triangles.size() << " triangles for base-tree " << tree << std::endl;
  std::cout << "Current bounding box " << box.toString() << std::endl;
#endif

  // stop recursion
  if (triangles.size() <= MAX_TRIANGLES_PER_LEAF)
  {
#ifdef DEBUG
    std::cout << indent << "stop recursion in tree generation " << triangles.size() << std::endl;
#endif
    // copy triangles to this node
    tree->triangles.insert(std::end(tree->triangles), std::begin(triangles), std::end(triangles));
    return tree;
  }

  left = new KDTree();
  right = new KDTree();
  // split bounding box
  box.split(left->box, right->box);

  auto leftTriangles = std::vector<Triangle<float> *>();
  auto rightTriangles = std::vector<Triangle<float> *>();

  // assign triangles to left/right children
  for (auto const &triangle : triangles)
  {
    bool leftContains = tree->left->box.contains(triangle);
    bool rightContains = tree->right->box.contains(triangle);

    if (leftContains && rightContains)
    {
      tree->triangles.push_back(triangle);
      continue;
    }

    if (leftContains)
    {
      leftTriangles.push_back(triangle);
    }

    if (rightContains)
    {
      rightTriangles.push_back(triangle);
    }
  }

#ifdef DEBUG
  std::cout << "left triangles: " << leftTriangles.size() << " right triangles: " << rightTriangles.size() << " tree triangles " << tree->triangles.size() << std::endl;
  depth++;
#endif

  depth++;

  left = left->buildTree(left, leftTriangles, depth);

  right = right->buildTree(right, rightTriangles, depth);
  return tree;
}

KDTree *KDTree::buildTree(std::vector<Triangle<FLOAT> *> &triangles)
{
  KDTree *root = new KDTree();
  // find min and max coordinates
  auto min = Vector<float, 3>{triangles[0]->p1[0], triangles[0]->p1[0], triangles[0]->p1[0]};
  auto max = Vector<float, 3>{triangles[0]->p1[0], triangles[0]->p1[0], triangles[0]->p1[0]};

  for (auto iterator = std::next(triangles.begin()); iterator != triangles.end(); ++iterator)
  {
    Triangle<float> *triangle = *iterator;
    min[0] = std::min({min[0], triangle->p1[0], triangle->p2[0], triangle->p3[0]});
    min[1] = std::min({min[1], triangle->p1[1], triangle->p2[1], triangle->p3[1]});
    min[2] = std::min({min[2], triangle->p1[2], triangle->p2[2], triangle->p3[2]});

    max[0] = std::max({max[0], triangle->p1[0], triangle->p2[0], triangle->p3[0]});
    max[1] = std::max({max[1], triangle->p1[1], triangle->p2[1], triangle->p3[1]});
    max[2] = std::max({max[2], triangle->p1[2], triangle->p2[2], triangle->p3[2]});
  }

  // create bounding box
  root->box = BoundingBox(min, max);
  // use private constructor to build tree
  root->buildTree(root, triangles);
  return root;
}

bool KDTree::hasNearestTriangle(Vector<FLOAT, 3> eye, Vector<FLOAT, 3> direction, Triangle<FLOAT> *&nearest_triangle, FLOAT &t, FLOAT &u, FLOAT &v, FLOAT minimum_t)
{
  // check if ray intersects bounding box
  if (!box.intersects(eye, direction))
  {
    return false;
  }

  // check if ray intersects triangles in children
  if (this->left != nullptr)
  {
    if (this->left->hasNearestTriangle(eye, direction, nearest_triangle, t, u, v, minimum_t))
      minimum_t = t;
  }
  if (this->right != nullptr)
  {
    if (this->right->hasNearestTriangle(eye, direction, nearest_triangle, t, u, v, minimum_t))
      minimum_t = t;
  }

  // check if ray intersects triangles in this node
  for (auto triangle : this->triangles)
  {
    stats.no_ray_triangle_intersection_tests++;
    // every call to triangle-> intersects will change the value of t, u, v but not minimum_t
    if (triangle->intersects(eye, direction, t, u, v, minimum_t) && t < minimum_t)
    {
      stats.no_ray_triangle_intersections_found++;
      nearest_triangle = triangle;
      minimum_t = t;
    }
  }

  // set t to the found minimum (t could have changed since the minimum was found!)
  t = minimum_t;
  return nearest_triangle != nullptr;
}
