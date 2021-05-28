#include <cpp11.hpp>
#include <vector>
#include <zstd.h>

using namespace cpp11;

//compression and decompression
inline size_t R_ReadConnection(SEXP con, void* buf, size_t n) {
  static auto readBin = cpp11::package("base")["readBin"];

  cpp11::raws res(
      readBin(con, cpp11::writable::raws(static_cast<R_xlen_t>(0)), n));
  memcpy(buf, RAW(res), res.size());

  return res.size();
}

inline size_t R_WriteConnection(SEXP con, void* buf, size_t n) {
  static auto writeBin = cpp11::package("base")["writeBin"];

  cpp11::writable::raws payload(n);
  memcpy(RAW(payload), buf, n);

  writeBin(payload, con);

  return n;
}

[[cpp11::register]]
cpp11::writable::raws raw_compression(raws src, int level) {

  //get the number of elements
  size_t src_size = src.size();

  //create destination
  //Size the output with the compress bounds
  //docs say best performance when dest size >= ZSTD_compressBound(src_size)
  size_t dst_cap = ZSTD_COMPRESSBOUND(src_size);
  cpp11::writable::raws dest(dst_cap);

  //RAW() to get the void * from the raws vector
  //returns size of source at dest if success else returns an error
  std::size_t new_size = ZSTD_compress(RAW(dest), dst_cap, RAW(src), src_size, level);


  //resize dest!
  dest.resize(new_size);

  //check for errors compressing
  //possibly using ZSTD_getErrorName but is a cstring
  //use stop not print!
  if (ZSTD_isError(new_size)) {
    cpp11::stop("error");
  }

  return dest;


};



[[cpp11::register]]
cpp11::writable::raws decompression(raws src) {

  //get element size
  size_t comp_size = src.size();


  //original decompressed size of `src` frame content, if known
  size_t dest_cap = ZSTD_getFrameContentSize(RAW(src), comp_size);


  //check for errors
  if (dest_cap == ZSTD_CONTENTSIZE_UNKNOWN || dest_cap == ZSTD_CONTENTSIZE_ERROR) {
    cpp11::stop("error");
  }

  //create destination
  cpp11::writable::raws dest(dest_cap);

  //returns size of source at dest
  std::size_t new_size = ZSTD_decompress(RAW(dest), dest_cap,
                                         RAW(src), comp_size);


  //resize dest
  dest.resize(new_size);

  //check for errors decompressing
  if (ZSTD_isError(new_size)) {
    cpp11::stop("error");
  }

  return dest;


};



[[cpp11::register]]
void stream(SEXP src, SEXP dest, int level) {



  //1 get your buffer memory
  //use standard vector instead of malloc SCB

  size_t const in_bufferSize = ZSTD_CStreamInSize();
  std::vector<uint8_t> in_buff(in_bufferSize);
  size_t const out_bufferSize = ZSTD_CStreamOutSize();
  std::vector<uint8_t>  out_buff(out_bufferSize);

  // create the context and check for errors
  ZSTD_CCtx* const context = ZSTD_createCCtx();

  // set level parameters
  if (ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_compressionLevel, level))) {
    cpp11:stop("error with params");
  }

  bool last_chunk = true;

  do {

    //7 set inBuffer struct
    size_t read_in = R_ReadConnection(src, in_buff.data(), in_bufferSize);

    last_chunk = (read_in < in_bufferSize);

    ZSTD_inBuffer input = {in_buff.data(), read_in, 0 };

    int finished;
    do {
      ZSTD_outBuffer output = { out_buff.data(), out_bufferSize, 0};
      size_t const remaining = ZSTD_compressStream2(context, &output, &input, ZSTD_e_continue);
      if(ZSTD_isError(remaining)) {
        cpp11::stop("compression2 error");
      }
      R_WriteConnection(dest, out_buff.data(), output.pos);

      finished = last_chunk ? (remaining == 0) : (input.pos == input.size);

    } while (!finished);

  } while (last_chunk);


};

[[cpp11::register]]
void stream_compression(SEXP src, SEXP dest, int level) {



  stream(src, dest, level);

  //close files!
  static auto close = cpp11::package("base")["close"];
  close(src);
  close(dest);


};



