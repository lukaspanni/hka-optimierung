#include "triangle.h"
#include "gtest/gtest.h"

namespace {


TEST(Triangle, ConstructorWithoutNormals1) {
  Triangle<FLOAT> t( {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0});


  ASSERT_TRUE( t.p1.equals_near({0.0, 0.0, 0.0}));
  ASSERT_TRUE( t.p2.equals_near({1.0, 0.0, 0.0}));
  ASSERT_TRUE( t.p3.equals_near({0.0, 1.0, 0.0}));
  ASSERT_TRUE( t.n1.equals_near({0.0, 0.0, 1.0}) );
  ASSERT_TRUE( t.n2.equals_near({0.0, 0.0, 1.0}) );
  ASSERT_TRUE( t.n3.equals_near({0.0, 0.0, 1.0}) );
}

TEST(Triangle, ConstructorWithoutNormals2) {
  Triangle<double> t( {-1.0, -1.0, 0.0}, {2.0, -1.0, 0.0}, {-1.0, 2.0, 0.0});

  ASSERT_TRUE( t.p1.equals_near({-1.0, -1.0, 0.0}));
  ASSERT_TRUE( t.p2.equals_near({2.0, -1.0, 0.0}));
  ASSERT_TRUE( t.p3.equals_near( {-1.0, 2.0, 0.0}));
  ASSERT_TRUE( t.n1.equals_near({0.0, 0.0, 1.0}) );
  ASSERT_TRUE( t.n2.equals_near({0.0, 0.0, 1.0}) );
  ASSERT_TRUE( t.n3.equals_near({0.0, 0.0, 1.0}) );

}


TEST(Triangle, ConstructorWithoutNormals3) {
  Triangle<FLOAT> t( {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0});


  ASSERT_TRUE( t.p1.equals_near({0.0, 0.0, 0.0}));
  ASSERT_TRUE( t.p2.equals_near({0.0, 0.0, 1.0}));
  ASSERT_TRUE( t.p3.equals_near({0.0, 1.0, 0.0}));
  ASSERT_TRUE( t.n1.equals_near({-1.0, 0.0, 0.0}) );
}

// TODO: tests for triangles with normals

// intersection is (0,0,0)
TEST(Triangle, Intersects1) {
  Triangle<FLOAT> triangle( {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0});
  FLOAT t, u, v;

  bool intersects = triangle.intersects( {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}, t, u, v);

  ASSERT_TRUE(intersects);
  ASSERT_FLOAT_EQ(1.0, t);
  ASSERT_FLOAT_EQ(1.0, u);
  ASSERT_FLOAT_EQ(0.0, v);
}

// same test as above, but point of triangle are ordered in reverse direction
TEST(Triangle, Intersects2) {
  Triangle<FLOAT> triangle( {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0} );
  FLOAT t, u, v;

  bool intersects = triangle.intersects( {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}, t, u, v);

  ASSERT_TRUE(intersects);
  ASSERT_FLOAT_EQ(1.0, t);
  ASSERT_FLOAT_EQ(1.0, u);
  ASSERT_FLOAT_EQ(0.0, v);
}

// intersection with plane is (2,2,0) outside triangle
TEST(Triangle, Intersects3) {
  Triangle<FLOAT> triangle( {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0});
  FLOAT t, u, v;

  bool intersects = triangle.intersects( {2.0, 2.0, 1.0}, {0.0, 0.0, -1.0}, t, u, v);

  ASSERT_FALSE(intersects);
}

// intersection is (1.0, 1.0, 0.0)
TEST(Triangle, Intersects4) {
  Triangle<FLOAT> triangle( {0.0, 0.0, 0.0}, {0.0, 2.0, 0.0}, {2.0, 0.0, 0.0});
  FLOAT t, u, v;
  bool intersects = triangle.intersects( {1.0, 1.0, 1.0}, {0.0, 0.0, -1.0}, t, u, v);

  ASSERT_TRUE(intersects);
  ASSERT_FLOAT_EQ(1.0, t);
  ASSERT_FLOAT_EQ(0.0, u);
  ASSERT_FLOAT_EQ(0.5, v);

}

// intersection is (0.5, 0.5, 0.0)
TEST(Triangle, Intersects5) {
  Triangle<FLOAT> triangle( {0.0, 0.0, 0.0}, {0.0, 2.0, 0.0}, {2.0, 0.0, 0.0});
  FLOAT t, u, v;
  bool intersects = triangle.intersects( {0.5, 0.5, 10.0}, {0.0, 0.0, -1.0}, t, u, v);

  ASSERT_TRUE(intersects);
  ASSERT_FLOAT_EQ(10.0, t);
  ASSERT_FLOAT_EQ(0.5, u);
  ASSERT_FLOAT_EQ(0.25, v);

}

}
