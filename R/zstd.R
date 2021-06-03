#' Zstd compression
#'
#' Lossless compression of input files
#'
#' @param src Raw source vector to be compressed or decompressed.
#' @param s_file Source file path of compression or decompression. Standard
#'   file extension for compressed file is '.zst'.
#' @param d_file Destination file path of compression or decompression. Standard
#'   file extension for compressed file is '.zst'.
#' @param level Compression levels ranging from negative 5 to 22
#' @param cores For multithreading how many cores should be used. By default
#'   this is set using parallel::detectCores().
#' @return If compressing or decompressing from a raw vector the output will also
#'   be a raw vector. If compressing or decompressing from a file path or file
#'   connection, the output will be placed in the file pointed to by d_file.
#' @name zstd
NULL
#> NULL

#' @export
#' @rdname zstd
compress_raw <- function(src, level) {
  compress_raw_(src, level)
}
#' @export
#' @rdname zstd
decompress_raw <- function(src) {
  decompress_raw_(src)
}

#' @export
#' @rdname zstd
compress_file <- function(s_file, d_file, level, cores = parallel::detectCores()) {
  if (!inherits(s_file, "connection")) {
    input <- file(s_file, "rb")
  }
  if (!inherits(d_file, "connection")) {
    output <- file(d_file, "wb")
  }
  compress_connection_(input, output, level, cores)
}

#' @export
#' @rdname zstd
decompress_file <- function(s_file, d_file) {
  if (!inherits(s_file, "connection")) {
    input <- file(s_file, "rb")
  }
  if (!inherits(d_file, "connection")) {
    output <- file(d_file, "wb")
  }
  decompress_connection_(input, output)
}
