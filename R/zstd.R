#' Zstd compression
#'
#' Lossless compression of input files
#'
#' @param src Source file to be compressed or decompressed.
#' @param dest Output file name of compression or decompression. In the case of
#'   compression standard file extension is '.zstd'.
#' @param level Compression levels ranging from negative 5 to 22
#' @param cores For multithreading how many cores should be used. By default
#'   this is set using parallel::detectCores
#' @name zstd
NULL
#> NULL

#' @export
#' @rdname zstd
compression <- function(src, level) {
  compression_(src, level)
}
#' @export
#' @rdname zstd
decompression <- function(src) {
  decompression_(src)
}

#' @export
#' @rdname zstd
stream_compression <- function(src, dest, level, cores = parallel::detectCores()) {
  stream_compression_(src, dest, level, cores)
}

#' @export
#' @rdname zstd
stream_decompression <- function(src, dest) {
  stream_decompression_(src, dest)
}
