#include <cpp11.hpp>
#include <zstd.h>
using namespace cpp11;

//compile using devtools::load_all()
// read in raw using readBin("text", what = 'raw', n = 816) -> t1

[[cpp11::register]]
cpp11::writable::raws raw_compression(raws src, int level) {

  //get the number of elements
  size_t srcSize = src.size();

  //create destination
  //Size the output with the compress bounds
  //docs say best performance when dest size >= ZSTD_compressBound(srcSize)
  size_t dstCap = ZSTD_COMPRESSBOUND(srcSize);
  cpp11::writable::raws dest(dstCap);

  //RAW() to get the void * from the raws vector
  //returns size of source at dest if success else returns an error
  std::size_t newSize = ZSTD_compress(RAW(dest), srcSize, RAW(src), dstCap, level);


  //resize dest!
  dest.resize(newSize);

  //check for errors compressing
  //possibly using ZSTD_getErrorName but is a cstring
  //use stop not print!
  if (ZSTD_isError(newSize)) {
    cpp11::stop("error");
  }

  return dest;


};



[[cpp11::register]]
cpp11::writable::raws decompression(writable::raws src) {

  //get the number of elements
  size_t compSize = src.size();


  //decompressed size of `src` frame content, if known
  size_t dstCap = ZSTD_getFrameContentSize(RAW(src), compSize);


  //check for errors of ZSTD_getFrameContentSize!!
  //if decompressed frame size can not be determined suggested to use streaming
  //however if
  //mode or we could also hardcode an upper limit but if we already want to
  //implement streaming mode??
  if (dstCap == ZSTD_CONTENTSIZE_UNKNOWN) {

    cpp11::stop("error");
  }
  if(dstCap == ZSTD_CONTENTSIZE_ERROR) {
    cpp11::stop("error");
  }


  //create destination
  cpp11::writable::raws dest(dstCap);

  //RAW() to get the void * from the raws vector
  //returns size of source at dest if success else returns an error
  std::size_t newSize = ZSTD_decompress(RAW(dest), dstCap,
                                         RAW(src), compSize);


  //resize dest!
  dest.resize(newSize);

  //check for errors compressing
  //possibly using ZSTD_getErrorName but is a cstring
  //use stop not print!
  if (ZSTD_isError(newSize)) {
    cpp11::stop("error");
  }

  return dest;


};

