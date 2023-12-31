#ifndef WAVELET_H
#define WAVELET_H

#include <array>
#include <vector>
#include <stdexcept>


//=============================================================================
// WaveledSubbdand
// Type defining the 2D subbands of wavelet decompostion as enum.
//=============================================================================

enum WaveledSubbdand {
  kDiagonalSubband = 0,
  kVerticalSubband,
  kHorizontalSubband
};

//=============================================================================
// Decomposition1D and Decomposition1D2D
// Contain and manage the result of 1D or 2D wavelet decomposition and provide
// accessors to set/get a specific subband.
//=============================================================================

template<typename T>
class Decomposition1D : public std::vector<std::vector<T>> {
public:
  explicit Decomposition1D(size_t num_levels);
  ~Decomposition1D();

  size_t NumLevels() const;

  const std::vector<T>& GetAppcoef() const;
  const std::vector<T>& GetDetcoef(size_t level) const;

  void SetDetcoef(const std::vector<T>& d, size_t level);
  void SetAppcoef(const std::vector<T>& a);
};

//=============================================================================
// Wavelet
// Implementation of 1D and 2D single-level and multi-level Wavelet transform.
//=============================================================================

template<typename T>
class Wavelet {
public:

  // Constructor requires the decomposition/reconstruction filters
  explicit Wavelet(const std::vector<T>& Lo_D, const std::vector<T>& Hi_D,
          const std::vector<T>& Lo_R, const std::vector<T>& Hi_R);
  ~Wavelet();

public:

  //===========================================================================
  // 1D Discrete Wavelet Transform (DWT)
  //===========================================================================

  // Multi-level wavelet decomposition of 1D input vector x
  Decomposition1D<T> Wavedec(const std::vector<T>& x, size_t num_levels) const;

  // Multi-level wavelet reconstruction of 1D vector having length_rec
  std::vector<T> Waverec(const Decomposition1D<T>& decomposition, size_t length_rec) const;

  // Single-level DWT transform of 1D vector x. The output vectors contain the
  // approximation coefficients a and the detail coefficients d
  void Dwt(const std::vector<T>& x, std::vector<T>& a, std::vector<T>& d) const;

  // Single-level inverse DWT transform which computes a 1D vector of length l
  // from the given approximation and detail coefficients
  std::vector<T> Idwt(const std::vector<T>& a, const std::vector<T>& d, size_t l) const;

  //===========================================================================
  // Utils
  //===========================================================================

  // Linearly reconstruct 1D vector assuming the given input a represents the
  // approximation coefficients of a multi-level wavelet decomposition with
  // num_levels. Thus, if vector a has length l, the output will have length
  // equal to l*(2^num_levels)
   
   std::vector<T> Linrec(const std::vector<T>& a, size_t num_levels) const;
private:

  //===========================================================================
  // Utils for 1D DWT
  //===========================================================================

  // Reconstruct 1D approximation of a vector at given level n from the input
  // decomposition. Using level=0 means full reconstruction, level = 1 means
  // reconstruction at the second-most coarse level, and so on. The length of
  // the reconstructed vector is inferred by the input Decomposition1D and it
  // is equal to the length of the detail coefficients at the next higher
  // level. If level=0 (and thus there is no higher level), the length will be
  // defined as double the length of level zero.
  std::vector<T> Appcoef(const Decomposition1D<T>& decomposition, size_t level) const;

  // Reconstruct the vector approximation at given level and specify the length
  // of the reconstruction using the third argument
  std::vector<T> Appcoef(const Decomposition1D<T>& decomposition, size_t level, size_t length_rec) const;

  // Convolution and decimation
  std::vector<T> Convdown(const std::vector<T>& x, const std::vector<T>& f) const;

  // Upsampling with zero-padding, dyadup, and convolution
  std::vector<T> Upconv(const std::vector<T>& x, const std::vector<T>& f, size_t s) const;

  // Utils
  //===========================================================================

  // Double the size of x interleaving zeros as  {x1, 0, x2, 0, x3, ... xn, 0}
  std::vector<T> Dyadup(const std::vector<T>& x) const;

  // Periodized extension of x
  std::vector<T> Wextend(const std::vector<T>& x, size_t lenEXT) const;
  
  
  std::vector<T> plus( std::vector<T> const & vector1, std::vector<T> const & vector2) const {
    if(vector1.size() != vector2.size()) {
      throw std::invalid_argument("std::vector operator+");
    }
    std::vector<T> plus;
    for(int i(0); i<vector1.size(); i++){
         plus.push_back(vector1[i] + vector2[i]);
    }
    // This is actually implemented as vector1 += vector2, so the first
    // parameter is passed by copy and then overwritten with the result
    return plus;
  }
  
private:

  //===========================================================================
  // Member variables
  //===========================================================================

  std::vector<T> Lo_D_; // Decomposition low-pass filter
  std::vector<T> Hi_D_; // Decomposition high-pass filter
  std::vector<T> Lo_R_; // Reconstruction low-pass filter
  std::vector<T> Hi_R_; // Reconstruction high-pass filter

  size_t half_length_filter_;

}; // class Wavelet<T>

//=============================================================================
// Decomposition1D
//=============================================================================

template<typename T>
Decomposition1D<T>::Decomposition1D(size_t num_levels) {
  // Internal representation is structured as
  // {d1, d2, ... dn, an}
  this->resize(num_levels + 1);
}

template<typename T>
Decomposition1D<T>::~Decomposition1D() {}

template<typename T>
size_t Decomposition1D<T>::NumLevels() const {
  return this->size() - 1;
}

template<typename T>
const std::vector<T>& Decomposition1D<T>::GetAppcoef() const {
  return this->back();
}

template<typename T>
const std::vector<T>& Decomposition1D<T>::GetDetcoef(size_t level) const {
  if(level >= NumLevels()) {
    throw std::invalid_argument("Decomposition1D::GetDetcoef");
  }
  return this->at(level);
}

template<typename T>
void Decomposition1D<T>::SetDetcoef(const std::vector<T>& d, size_t level) {
  if(level >= NumLevels()) {
    throw std::invalid_argument("Decomposition1D::SetDetcoef");
  }
  this->operator[](level) = d;
}

template<typename T>
void Decomposition1D<T>::SetAppcoef(const std::vector<T>& a) {
  this->back() = a;
}


//=============================================================================
// Wavelet c-tor and d-tor
//=============================================================================

template<typename T>
Wavelet<T>::Wavelet(const std::vector<T>& Lo_D, const std::vector<T>& Hi_D,
                    const std::vector<T>& Lo_R, const std::vector<T>& Hi_R) {
  Lo_D_ = Lo_D;
  Hi_D_ = Hi_D;
  Lo_R_ = Lo_R;
  Hi_R_ = Hi_R;
  half_length_filter_ = Lo_D_.size() / 2;
}

template<typename T>
Wavelet<T>::~Wavelet()
{ }

//=============================================================================
// 1D Discrete Wavelet Transform (DWT)
//=============================================================================

template<typename T>
Decomposition1D<T> Wavelet<T>::Wavedec(const std::vector<T>& x, size_t num_levels) const {
  std::cout << "this is inside   " << num_levels << std::endl;
 
 
  if (x.size() < (1 << num_levels)) {
    std::cout << "this is the error " << x.size() << "  " << (1 << num_levels) << std::endl;
    throw std::invalid_argument("Wavelet::Wavedec");
  }
 
  Decomposition1D<T> decomposition(num_levels);
  std::vector<T> a = x;
  for (size_t k = 0; k < num_levels; k++) {
    std::vector<T> ak, dk;
    Dwt(a, ak, dk);
    // Save approximation coefficients for next iteration
    a = ak;
    // Store detail coefficients of current level k to Decomposition object
    decomposition.SetDetcoef(dk, k);
  }
  // Store final approximation coefficients
  decomposition.SetAppcoef(a);
  return decomposition;
}

template<typename T>
std::vector<T> Wavelet<T>::Waverec(const Decomposition1D<T>& decomposition, size_t length_rec) const {
  return Appcoef(decomposition, 0, length_rec);
}

template<typename T>
void Wavelet<T>::Dwt(const std::vector<T>& x, std::vector<T>& a, std::vector<T>& d) const {
  a = Convdown(x, Lo_D_);
  d = Convdown(x, Hi_D_);
}

template<typename T>
std::vector<T> Wavelet<T>::Idwt(const std::vector<T>& a, const std::vector<T>& d, size_t l) const {
  return (Upconv(a, Lo_R_, l) , Upconv(d, Hi_R_, l));
}

//=============================================================================
// Utils
//=============================================================================

template<typename T>
std::vector<T> Wavelet<T>::Linrec(const std::vector<T>& a, size_t num_levels) const {
  Decomposition1D<T> c(num_levels);
  const size_t length = a.size();
  for (size_t k = 0; k < num_levels; k++) {
    // Set k-th level detail coefficients to zero
    std::vector<T> dk(length * (1 << (num_levels - k - 1)), static_cast<T>(0));
    c.SetDetcoef(dk, k);
  }
  // Set approx coefficients equal to the given input
  c.SetAppcoef(a);
  // Call Appcoef with length of reconstruction equal to N*2^n
  return Appcoef(c, 0, length * (1 << num_levels));
}


//===========================================================================
// Utils for 1D DWT
//===========================================================================

template<typename T>
std::vector<T> Wavelet<T>::Appcoef(const Decomposition1D<T>& decomposition, size_t level) const {
  if (level >= decomposition.NumLevels()) {
    throw std::invalid_argument("Wavelet::Appcoef");
  }
  const size_t length_rec = (level == 0) ? 2 * decomposition.GetDetcoef(0).size() : decomposition.GetDetcoef(level - 1).size();
  return Appcoef(decomposition, level, length_rec);
}

template<typename T>
std::vector<T> Wavelet<T>::Appcoef(const Decomposition1D<T>& decomposition, size_t level, size_t length_rec) const {
  std::vector<T> a = decomposition.GetAppcoef();
  for (size_t k = decomposition.NumLevels(); k != level; k--) {
    // Get size from next upper level, if current level is the final then
    // resort to the final lenght argument sz
    const size_t length_rec_k = k != 1 ? decomposition.GetDetcoef(k - 2).size() : length_rec;
    // Reconstruct signal of size s from kth level approx and detail
    a = Idwt(a, decomposition.GetDetcoef(k - 1), length_rec_k);
  }
 
  return a;
}

template<typename T>
std::vector<T> Wavelet<T>::Convdown(const std::vector<T>& x, const std::vector<T>& f) const {
  const size_t last = static_cast<size_t>(std::ceil(static_cast<double>(x.size()) / 2.0) * 2.0);
  //std::cout << last <<"  " << half_length_filter_ << std::endl;
  std::vector<T> z = Wextend(x, half_length_filter_);
  z = Conv(z, f, Vector::kValidConvolution);
  std::vector<T> y(last / 2);
  for (size_t k = 1; k <= last; k += 2) {
    // Since k is always an odd integer, k/2 === floor(k/2)
    y[k / 2] = z[k];
  }
  
  return y;
}

template<typename T>
std::vector<T> Wavelet<T>::Upconv(const std::vector<T>& x, const std::vector<T>& f, size_t s) const {
  const size_t lf = f.size();
  std::vector<T> y = Dyadup(x);
  y = Wextend(y, lf / 2);
  y = Conv(y, f, Vector::kFullConvolution);
  y.erase(y.begin(), y.begin() + lf - 1);
  y.erase(y.begin() + s, y.end());
  return y;
}


//=============================================================================
// Utils
//=============================================================================

template<typename T>
std::vector<T> Wavelet<T>::Dyadup(const std::vector<T>& x) const {
  // Return a vector having double length
  std::vector<T> y;
  y.reserve(2 * x.size());
  // The output vector interleaves the elements of x with zero
  for (size_t k = 0; k < x.size(); k++) {
    y.push_back(x[k]);
    y.push_back(0);
  }
  return y;
}

template<typename T>
std::vector<T> Wavelet<T>::Wextend(const std::vector<T>& x, size_t lenEXT) const {
  std::vector<T> temp = x;
  // Add extra sample if signal is odd
  if (x.size() % 2 == 1) {
    temp.push_back(temp.back());
  }
  // Handle cases when x is shorter than lenEXT
  const size_t rep = lenEXT / temp.size(); // (size_t)std::floor(lenEXT/temp.size());
  const size_t len = lenEXT % temp.size();
  std::vector<T> y;
  y.reserve(2 * (len + rep * temp.size()) + temp.size());
  // Copy last len elements at the beginning
  y.insert(y.begin(), temp.end() - len, temp.end());
  for (size_t k = 0; k < 2 * rep + 1; k++) {
    y.insert(y.end(), temp.begin(), temp.end());
  }
  // Copy first len elements at the end
  y.insert(y.end(), temp.begin(), temp.begin() + len);
  return y;
}






#endif // WAVELET_H
