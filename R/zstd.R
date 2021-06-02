#' Lossless compression of input files
#'
#' @param src Source file to be compressed or decompressed.
#' @param dest Output file name of compression or decompression. In the case of
#' compression standard file extension is '.zstd'.
#' @param level Compression levels ranging from negative 5 to 22
#' @param cores For multithreading how many cores should be used. By default
#' this is set using parallel::detectCores()

compression <- function(src, level) {
  compression_(src, level)
}

decompression <- function(src) {
  decompression_(src)
}


stream_compression <- function(src, dest, level, cores = parallel::detectCores()) {
  stream_compression_(src, dest, level, cores)
}

stream_decompression <- function(src, dest) {
  stream_decompression_(src, dest)
}
