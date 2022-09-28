#include "math.h"
#include "gtest/gtest.h"

namespace {


TEST(Vector, DefaultConstructor) {
  Vector<FLOAT, 3> v;

  ASSERT_FLOAT_EQ(0.0, v[0]);
  ASSERT_FLOAT_EQ(0.0, v[1]);
  ASSERT_FLOAT_EQ(0.0, v[2]);
}


TEST(Vector, ListInitializerConstructor) {
  Vector<double, 4> v = {1.0, -1.0, 0.0, 2.5};

  ASSERT_FLOAT_EQ(1.0, v[0]);
  ASSERT_FLOAT_EQ(-1.0, v[1]);
  ASSERT_FLOAT_EQ(0.0, v[2]);
  ASSERT_FLOAT_EQ(2.5, v[3]);
}


TEST(Vector, IndexOperator) {
  Vector<long double, 2> v = {999.99, -0.00888};

  ASSERT_FLOAT_EQ(999.99, v[0]);
  ASSERT_FLOAT_EQ(-0.00888, v[1]);
}

TEST(Vector, PlusOperator) {
  Vector<float, 3> v1 = {0.0, 1.5, -1000.0};
  Vector<float, 3> v2 = {0.1, 0.5,  1000.0};
  Vector<float, 3> v;

  v = v1 + v2;

  ASSERT_FLOAT_EQ(0.1, v[0]);
  ASSERT_FLOAT_EQ(2.0, v[1]);
  ASSERT_FLOAT_EQ(0.0, v[2]);
}

TEST(Vector, DifferenceOperator) {
  Vector<float, 3> v1 = {0.1, 1.5, -1000.0};
  Vector<float, 3> v2 = {0.0, 0.5,  1000.0};
  Vector<float, 3> v;

  v = v1 - v2;

  ASSERT_FLOAT_EQ(0.1, v[0]);
  ASSERT_FLOAT_EQ(1.0, v[1]);
  ASSERT_FLOAT_EQ(-2000.0, v[2]);
}

TEST(Vector, ScalarProduct1) {
  Vector<FLOAT, 3> v1 = {1.0, -1.0, 2.0};
  Vector<FLOAT, 3> v2 = {1.0, 1.0, -0.5};
  Vector<FLOAT, 3> v;

  FLOAT product = v1.scalar_product(v2);

  ASSERT_FLOAT_EQ(-1.0, product);
}

TEST(Vector, ScalarProduct2) {
  Vector<FLOAT, 3> v1 = {1.0, -10.0, 2000.0};
  Vector<FLOAT, 3> v2 = {0.0, 2.0, 0.001};
  Vector<FLOAT, 3> v;

  FLOAT product = v1.scalar_product(v2);

  ASSERT_FLOAT_EQ(-18.0, product);
}

TEST(Vector, SquareOfLength1) {
  Vector<double, 3> v1 = {0.0, 0.0, 0.0};

  double square_of_length = v1.square_of_length();

  ASSERT_FLOAT_EQ(0.0, square_of_length);
}

TEST(Vector, SquareOfLength2) {
  Vector<long double, 3> v1 = {1.0, -1.0, 1.0};

  long double square_of_length = v1.square_of_length();

  ASSERT_FLOAT_EQ(3.0, square_of_length);
}

TEST(Vector, SquareOfLength3) {
  Vector<FLOAT, 3> v1 = {1.0, 2.0, -3.0};

  FLOAT square_of_length = v1.square_of_length();

  ASSERT_FLOAT_EQ(14.0, square_of_length);
}

TEST(Vector, Length1) {
  Vector<FLOAT, 3> v1 = {1.0, 0.0, 0.0};

  FLOAT length = v1.length();

  ASSERT_FLOAT_EQ(1.0, length);
}

TEST(Vector, Length2) {
  Vector<FLOAT, 2> v1 = {-4.0, 3.0};

  FLOAT length = v1.length();

  ASSERT_FLOAT_EQ(5.0, length);
}

TEST(Vector, Normalize1) {
  Vector<float, 2> v1 = {-4.0, 3.0};

  v1.normalize();

  ASSERT_FLOAT_EQ(1.0, v1.length());
}

TEST(Vector, Normalize2) {
  Vector<float, 3> v = {6.6, 0.0, 0.0};

  v.normalize();

  ASSERT_FLOAT_EQ(1.0, v.length());
  ASSERT_FLOAT_EQ(1.0, v[0]);
  ASSERT_FLOAT_EQ(0.0, v[1]);
  ASSERT_FLOAT_EQ(0.0, v[2]);
}

TEST(Vector, Product1) {
  Vector<double, 2> v = {1.0, 2.0};

  v = 2.0 * v;

  ASSERT_FLOAT_EQ(2.0, v[0]);
  ASSERT_FLOAT_EQ(4.0, v[1]);
  
}

TEST(Vector, EqualsNear1) {
  Vector<FLOAT, 3> v1 = {1.0, 0.1, 2.0};
  Vector<FLOAT, 3> v2 = {1.0, 0.1, 2.0};

  ASSERT_TRUE( v1.equals_near(v2) ); 
}


TEST(Vector, EqualsNear2) {
  Vector<FLOAT, 3> v1 = {1.0, 0.1, 2.0};
  Vector<FLOAT, 3> v2 = {1.0, 0.1000001, 2.0};

  ASSERT_TRUE( v1.equals_near(v2, 0.00001) ); 
}

TEST(Vector, EqualsNear3) {
  Vector<double, 3> v1 = {1.0, 0.1, 2.0};
  Vector<double, 3> v2 = {1.0, 0.1000001, 2.0};

  ASSERT_TRUE( v2.equals_near(v1, 0.00001) ); 
}

TEST(Vector, EqualsNear4) {
  Vector<float, 3> v1 = {1.0, 0.1, 2.0};
  Vector<float, 3> v2 = {1.0, 0.1, 3.0};

  ASSERT_FALSE( v1.equals_near(v2) ); 
}

TEST(Vector, EqualsNear5) {
  Vector<float, 1> v1 = {0.1};
  Vector<float, 1> v2 = {0.2};

  ASSERT_FALSE( v1.equals_near(v2) ); 
}

TEST(Vector, EqualsNear6) {
  Vector<long double, 1> v1 = {10.0};
  Vector<long double, 1> v2 = {10.0000001};

  ASSERT_TRUE( v1.equals_near(v2, 0.0000001) ); 
  ASSERT_FALSE( v1.equals_near(v2, 0.0000000001) ); 
}

TEST(Vector, CrossProduct1) {
  Vector<double, 3> v1 = {1.0, 0.0, 0.0};
  Vector<double, 3> v2 = {0.0, 1.0, 0.0};

  Vector<double,3> product = cross_product(v1, v2);

  ASSERT_FLOAT_EQ(0.0, product[0]);
  ASSERT_FLOAT_EQ(0.0, product[1]);
  ASSERT_FLOAT_EQ(1.0, product[2]);
}


}
