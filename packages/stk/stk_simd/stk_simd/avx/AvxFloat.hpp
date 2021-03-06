// Copyright 2013 Sandia Corporation, Albuquerque, NM.

namespace stk {
namespace simd {

struct Float {

  STK_MATH_FORCE_INLINE Float() {}

  STK_MATH_FORCE_INLINE Float(const float* x) 
    : _data(_mm256_loadu_ps(x)) {
  }

  STK_MATH_FORCE_INLINE Float(const float* x, const int offset) 
    :_data(_mm256_setr_ps(x[0],       x[offset],  x[2*offset],x[3*offset],
                          x[4*offset],x[5*offset],x[6*offset],x[7*offset])) {
  }

  STK_MATH_FORCE_INLINE Float(const float x)
    : _data(_mm256_set1_ps(x)) {
  }

  STK_MATH_FORCE_INLINE Float(const __m256& x)
    : _data(x) {
  }

  STK_MATH_FORCE_INLINE Float(const Float& x)
    : _data(x._data) {
  }

  STK_MATH_FORCE_INLINE Float& operator= (const Float& x) {
    _data = x._data;
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator= (const float x) {
    _data = _mm256_set1_ps(x);
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator+= (const Float& a) {
    _data = _mm256_add_ps(_data,a._data);
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator-= (const Float& a) {
    _data = _mm256_sub_ps(_data,a._data);
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator*= (const Float& a) {
    _data = _mm256_mul_ps(_data,a._data);
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator/= (const Float& a) {
    _data = _mm256_div_ps(_data,a._data);
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator+= (const float a) {
    _data = _mm256_add_ps(_data,_mm256_set1_ps(a));
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator-= (const float a) {
    _data = _mm256_sub_ps(_data,_mm256_set1_ps(a));
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator*= (const float a) {
    _data = _mm256_mul_ps(_data,_mm256_set1_ps(a));
    return *this;
  }

  STK_MATH_FORCE_INLINE Float& operator/= (const float a) {
    _data = _mm256_div_ps(_data,_mm256_set1_ps(a));
    return *this;
  }

  STK_MATH_FORCE_INLINE Float operator-() const {
    return Float( _mm256_sub_ps(Float(0.0)._data, _data) );
  }

  STK_MATH_FORCE_INLINE float& operator[](int i) {return (reinterpret_cast<float*>(&_data))[i];}
  STK_MATH_FORCE_INLINE const float& operator[](int i) const {return (reinterpret_cast<const float*>(&_data))[i];}
    
  STK_MATH_FORCE_INLINE int32_t& Int(int i) {return (reinterpret_cast<int32_t*>(&_data))[i];}
  STK_MATH_FORCE_INLINE const int32_t& Int(int i) const {return (reinterpret_cast<const int32_t*>(&_data))[i];}

  STK_MATH_FORCE_INLINE uint32_t& UInt(int i) {return (reinterpret_cast<uint32_t*>(&_data))[i];}
  STK_MATH_FORCE_INLINE const uint32_t& UInt(int i) const {return (reinterpret_cast<const uint32_t*>(&_data))[i];}

  __m256 _data; // the "_" means you should try not to use this directly
  // it is made public to avoid function call overhead 
  // and/or so the compiler doesn't have to use up one of
  // inlining depths (usually max inlining depth ~5)

};

} // namespace simd
} // namespace stk

