test_zstd <- function(src, level) {
  t1 <- readBin(src, what = 'raw', n = file.size(src))

  t2 <- raw_compression(t1, level = level)

  t3 <- decompression(t2)

  return(rawToChar(t3))

}
