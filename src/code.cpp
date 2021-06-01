#include <cpp11.hpp>
#include <vector>
#include <zstd.h>

using namespace cpp11;


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
  size_t dst_cap = ZSTD_COMPRESSBOUND(src_size);
  cpp11::writable::raws dest(dst_cap);

  std::size_t new_size = ZSTD_compress(RAW(dest), dst_cap, RAW(src), src_size, level);

  //check for errors compressing
  if (ZSTD_isError(new_size)) {
    cpp11::stop("error");
  }

  dest.resize(new_size);

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

  //check for errors decompressing
  if (ZSTD_isError(new_size)) {
    cpp11::stop("error");
  }

  //resize dest
  dest.resize(new_size);

  return dest;


};



[[cpp11::register]]
void stream_compression(SEXP src, SEXP dest, int level, int cores) {



  //create buffer memory

  size_t const in_bufferSize = ZSTD_CStreamInSize();
  std::vector<uint8_t> in_buff(in_bufferSize);
  size_t const out_bufferSize = ZSTD_CStreamOutSize();
  std::vector<uint8_t>  out_buff(out_bufferSize);

  // create the context and check for errors
  ZSTD_CCtx* const context = ZSTD_createCCtx();


  // set level parameters
  if (ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_compressionLevel, level))) {
    //close files first
    static auto close = cpp11::package("base")["close"];
    close(src);
    close(dest);
    //error
    cpp11::stop("error with setting compression level");
  }

  if (ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_nbWorkers, cores))) {
    //close files first
    static auto close = cpp11::package("base")["close"];
    close(src);
    close(dest);
    //error
    cpp11::stop("error with setting multithreading");
  }



  bool last_chunk = true;

  do {


    size_t read_in = R_ReadConnection(src, in_buff.data(), in_bufferSize);

    //checks there is still data to add
    last_chunk = (in_bufferSize > read_in);

    // set mode
    ZSTD_EndDirective const mode = last_chunk ? ZSTD_e_end : ZSTD_e_continue;

    //set inBuffer struct
    ZSTD_inBuffer input = {in_buff.data(), read_in, 0};

    bool finished = false;

    do {
      //set out buffer struct
      ZSTD_outBuffer output = {out_buff.data(), out_bufferSize, 0};

      size_t const remaining = ZSTD_compressStream2(context, &output, &input, mode);

      if(ZSTD_isError(remaining)) {
        //close files first
        static auto close = cpp11::package("base")["close"];
        close(src);
        close(dest);
        //error
        cpp11::stop("compression2 error");
      }
      //write output
      R_WriteConnection(dest, out_buff.data(), output.pos);

      //check if we are done and need more space (remaining == 0)
      //or if we are done and all data is in the output
      finished = last_chunk ? (remaining == 0) : (input.pos == input.size);

    } while (!finished);

  } while (!last_chunk);

  //close files
  static auto close = cpp11::package("base")["close"];
  close(src);
  close(dest);

};

[[cpp11::register]]
void stream_decompression(SEXP src, SEXP dest) {

  size_t const in_bufferSize = ZSTD_DStreamInSize();
  std::vector<uint8_t> in_buff(in_bufferSize);
  size_t const out_bufferSize = ZSTD_DStreamOutSize();
  std::vector<uint8_t>  out_buff(out_bufferSize);

  ZSTD_DCtx* const dctx = ZSTD_createDCtx();

  if(dctx == NULL) {
    //close files first
    static auto close = cpp11::package("base")["close"];
    close(src);
    close(dest);
    //error
    cpp11::stop("dictionary error in decompression");
  }

  size_t read;
  size_t lastRet = 0;
  int isEmpty = 1;

  //while not 0
  while ((read = R_ReadConnection(src, in_buff.data(), in_bufferSize))) {
    isEmpty = 0;

    ZSTD_inBuffer input = { in_buff.data(), read, 0 };

    //while input pos != input size
    while(input.pos < input.size) {
      ZSTD_outBuffer output = { out_buff.data(), out_bufferSize, 0 };

      //returns size of buffer or error
      size_t const ret = ZSTD_decompressStream(dctx, &output, &input);

      if (ZSTD_isError(ret)) {
        //close files first
        static auto close = cpp11::package("base")["close"];
        close(src);
        close(dest);
        //error
        cpp11::stop("streaming decompression error");
      }

      R_WriteConnection(dest, out_buff.data(), output.pos);

      lastRet = ret;
    }
  }

  //isEmpty should update to != 0 unless file is empty
  if (isEmpty) {
    //close files first
    static auto close = cpp11::package("base")["close"];
    close(src);
    close(dest);
    //error
    cpp11::stop("input is empty");
  }


  if(lastRet != 0) {
    //close files first
    static auto close = cpp11::package("base")["close"];
    close(src);
    close(dest);
    //error
    cpp11::stop("EOF before end of stream");
  }


  //close files
  static auto close = cpp11::package("base")["close"];
  close(src);
  close(dest);



};





