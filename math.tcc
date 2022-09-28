// A Vector with SIZE elements of type T.
//
// Provides basic mathematical behaviour to add, subtract, normalize,
// calculating the length, building scalar product of Vectors.
// Provides cross product and determinant or Vectors of SIZE = 3.
//
// T has to be a type with defined 0 constant, the
// binary +, -, * and / operators. The sqrt function have to be overloaded for T.
template <class T, size_t SIZE>
class Vector {
private:
  T x[SIZE];
public:
  Vector<T, SIZE>() {
    for (size_t i = 0u; i < SIZE; i++) {
      this->x[i] = 0;
    }
  }

  Vector<T, SIZE>(T x[SIZE]) {
    for (size_t i = 0u; i < SIZE; i++) {
      this->x[i] = x[i];
    }
  }

  // undefined behaviour if list.size() > SIZE
  Vector<T, SIZE>(std::initializer_list<T> list) {
    size_t i = 0u;
    for (T t : list) {
      this->x[i++] = t;
    }
  }

  T & operator[](size_t index) {
    return x[index];
  }

  Vector<T, SIZE> operator+(const Vector<T, SIZE> addend) const {
    Vector<T, SIZE> sum;
 
    for (size_t i = 0u; i < SIZE; i++) {
      sum.x[i] = this->x[i] + addend.x[i];
    }

    return sum;
  }

  Vector<T, SIZE> operator-(const Vector<T, SIZE> subtract) const {
    Vector<T, SIZE> difference;
 
    for (size_t i = 0u; i < SIZE; i++) {
      difference.x[i] = this->x[i] - subtract.x[i];
    }

    return difference;
  }

  

  T scalar_product(Vector<T, SIZE>  factor) {
    T product = 0;

    for (size_t i = 0u; i < SIZE; i++) {
      product += this->x[i] * factor.x[i];
    }

    return product;
  }

  
  // returns the square of the euclidian length of this vector
  T square_of_length(void) const {
    T square_of_length = 0;

    for (size_t i = 0u; i < SIZE; i++) {
      square_of_length += ( this->x[i] * this->x[i] );
    }

    return square_of_length;
  }

  // returns the euclidian length of this vector
  // needs T sqrt(T) to be defined for T 
  T length(void) const {
    return sqrt( square_of_length() );
  }

  // needs binary / to be defined for T
  void normalize(void) {
    const T length = this->length();

    for (size_t i = 0u; i < SIZE; i++) {
       this->x[i] /= length;
    }       
  }


  friend Vector<T, SIZE> operator*(const T factor, Vector<T, SIZE>  v)  {
    Vector<T, SIZE> product;

    for (size_t i = 0u; i < SIZE; i++) {
      product[i] = factor * v[i];
    }

    return product;
  }

  bool equals_near(Vector<T, SIZE> v, const FLOAT EPSILON = 0.00001) {
    for (size_t i = 0u; i < SIZE; i++) {
      if ( fabs(this->x[i] - v.x[i]) > EPSILON ) {
        return false;
      }
    }
    return true;
  }
};

template <class T>
std::ostream & operator<<(std::ostream & out, Vector<T, 3> & v) {
  out << "( " << v[0] << ", "<< v[1] << ", " << v[2] << ")";
  return out;
}


template <class T>
Vector<T, 3> cross_product(Vector<T, 3> v1, Vector<T, 3> v2) {
  Vector<T, 3> cross;

  cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
  cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
  cross[2] = v1[0] * v2[1] - v1[1] * v2[0];

  return cross;
}




