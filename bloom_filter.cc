#include <string>
#include <iomanip>
#include <iostream>
#include <stdint.h>

// simple bloom filter
// this bit map is saved in LE order
#define ALIGN(n, p2) ((n)+(p2-1))&~((p2)-1)

struct bitmap {
  explicit bitmap(int numbits) {
    mBits = numbits;
    size_t allocSize = ALIGN(numbits, 8);
    mAllocSize = allocSize/8;
    mBitMask = new unsigned char [mAllocSize];
    for (size_t i=0; i<mAllocSize; ++i) {
      mBitMask[i] = 0;
    }
  }

  bitmap(const std::string& init_str) {
    // allocate a bitmap with the bit pattern as in the string
  }

  ~bitmap() {
    mBits = 0;
    delete [] mBitMask;
  }

  // overload to allow access as bitmap[]
  bool operator[](const size_t bit) {
    return read_bit(bit);
  }

  // note that this code works for representation
  // in LE, since the return of __builtin_ffsll
  // is the first bit from LSB, so as we go
  // from lower bit in the mask to higher,
  // we go from lower byte to higher bytes in the
  // underlying bitmask array
  uint64_t operator&(uint64_t mask) {
    uint64_t res=0;
    while (mask) {
      int idx = __builtin_ffsll(mask) - 1;
      if (read_bit(idx)) {
          res &= 1<<idx;
      }
      mask &= ~(1<<idx);
    }
    return res;
  }

  // overload to allow set as bitmap = bitmap & mask;
  // see operator& comment for why this works for LE
  // underlying representation
  bitmap& operator&=(uint64_t mask) {
    int bit=0;
    while (mask) {
      bool set = mask&1;
      // std::cout << "bit " << bit << " " << set << "\n";
      set_bit(bit, set ? 1 : 0);
      mask>>=1;
      ++bit;
    }
    return *this;
  }

  std::string operator&(const std::string& str)
  {
    std::string s;
    for (size_t i=0; i<str.length(); i++) s.push_back('0');
    return s;
  }

  bitmap& operator&=(const std::string& mask) {
    return *this;
  }

 private:
  size_t mBits;
  size_t mAllocSize;
  unsigned char * mBitMask;

  void set_bit(const size_t bit, bool enable) {
    // first find which byte does this correspond to.
    if (bit >= mBits) {
      return; // undefined.
    }

    size_t byte = bit/8;
    size_t bit_within_byte = bit%8;
    if (enable) {
      mBitMask[byte] |= (1<<bit_within_byte);
    } else {
      mBitMask[byte] &= ~(1<<bit_within_byte);
    }
  }

  bool read_bit(const size_t bit) {
    // first find which byte does this correspond to.
    if (bit >= mBits) {
      return false; // undefined.
    }

    size_t byte = bit/8;
    size_t bit_within_byte = bit%8;
    return mBitMask[byte]&(1<<bit_within_byte);
  }

  friend std::ostream& operator<<(std::ostream& out, const bitmap& b);
};

// start from MSB
// since the underlying representation is in LE
// but printing is prettier in BE
std::ostream& operator<<(std::ostream& out, const bitmap& b)
{
  for (size_t i=b.mAllocSize; i>0; i--) {
    char v[9];
    snprintf(v, sizeof(v), "%x", b.mBitMask[i-1]);
    out << v << ((i-1) % 2 ? "" : " ");
  }
  return out;
}

//A thought .. each key is unique. so in theory
//it should give rise to a uniform setting.
//how about using the key as seed and generating
//a random permutation using Floyd's method?
template<typename T>
struct hasher
{
  hasher(int num_hash, int capacity) : mHash(num_hash),
                                       mCapacity(capacity) {}
  uint64_t hash(const T& t) {
    uint64_t r=0;
    srand(hash<T>(t));
    for (size_t i=0; i < mHash; i++) {
      r |= 1<<(rand()%mCapacity);
    }
    return r;
  }
}

template<typename T>
struct bloom_filter
{
  bloom_filter(size_t m, int k) : bitmap_ (new bitmap(m)),
                                  hasher_ (new hasher<T>(k, m)) {}
  add(const T &t) {
    uint64_t bits = hasher_.hash(t);
    bitmap_ &= bits;
  }

  int check(const T &t) {
    uint64_t bits = hasher_.hash(t);
    uint64_t map = bitmap_ & bits;
    if (popcount(map) != k) {
      return 0; // definitely not set as one of the hash fns was not set
    } else {
      return 1; // maybe found
    }
  }
};

int main() {
  bitmap map(24);
  map &= (1<<23);
  std::cout << map << std::endl;
}
