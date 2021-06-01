stream_compression <- function(src, dest, level, cores = parallel::detectCores()) {


  stream_compression_(src, dest, level, cores)
}
