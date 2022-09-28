// A Triangle in 3-dimensional euclidian space with 
// three edges and normal vectors.
// The normal vectors have to point away from the surface of the Triangle.
// The orientation is clockwise order

template <class T>
class Triangle {
public:
  Vector<T, 3> p1, p2, p3; // edges
  Vector<T, 3> n1, n2, n3; // normals

  Triangle(Vector<T, 3> p1, Vector<T, 3>p2, Vector<T, 3>p3 ) 
    : p1(p1), p2(p2), p3(p3) {
/*
 clockwise order
   p2
  /  \
 p1---p3

(face points away)
*/
    n1 = cross_product(p2  - p1, p3 - p1); 
    n1.normalize();
    n2 = n1;
    n3 = n1;
  }

  Triangle(Vector<T, 3> p1, Vector<T, 3>p2, Vector<T, 3>p3,
           Vector<T, 3> n1, Vector<T, 3>n2, Vector<T, 3>n3) 
    : p1(p1), p2(p2), p3(p3), n1(n1), n2(n2), n3(n3) {
    n1.normalize();
    n2.normalize();
    n3.normalize();
 }

#ifndef OPTIMIZED_INTERSECTS
// returns true if this Triangle intersects with a ray given by  a origin and the direction of the ray. 
// it also calculates:
// the parameter value t for the intersection = origin + t * direction.
// the u-v-Parameters for the barycentric coordinates (u,v,w) of the 
// intersection point with w = 1 - u - v.
//  minimum_t contains the parametic value s for the current nearest triangle (not used in this unoptimized version)
  bool intersects(Vector<T,3> origin, Vector<T,3> direction,
                   FLOAT &t, FLOAT &u, FLOAT &v, FLOAT minimum_t = INFINITY) {
    Vector<T, 3> normal =  cross_product(p2 - p1, p3  - p1);
    
    T normalRayProduct = normal.scalar_product( direction );
    T area = normal.length(); // used for u-v-parameter calculation

    if ( fabs(normalRayProduct) < EPSILON ) {
      return false;
    }

    T d = normal.scalar_product( p1 );
    t = (d - normal.scalar_product( origin ) ) / normalRayProduct;

    if ( t < 0.0 ) {
      return false;
    }
   
    Vector<T, 3> intersection = origin + t * direction;
   
    Vector<T, 3> vector = cross_product(p2 - p1,  intersection - p1 );
    if ( normal.scalar_product(vector) < 0.0 ) { 
      return false;
    }

    
    vector = cross_product(p3 - p2,  intersection - p2 );
    if ( normal.scalar_product(vector) < 0.0 ) { 
      return false;
    }

    u = vector.length()  / area;


    vector = cross_product(p1 - p3, intersection - p3 );
    if (normal.scalar_product(vector) < 0.0 ) {
      return false;
    }

    v = vector.length() / area;

    return true;
  }
#else

// optimized version
bool intersects(Vector<T,3> origin, Vector<T,3> direction,
                   FLOAT &t, FLOAT &u, FLOAT &v, FLOAT minimum_t) {
    // from here
    // TODO: your code
    // to here
    return true;
  }
#endif

};

