

test_that("zstd contents in decompressed files are equal to original file", {
  fileInput <- "simpleTestFile"
  level <- 10
  expect_equal(test_zstd(fileInput, level),
               readChar(fileInput, file.size(fileInput)))
})


test_that("zstd contents in streaming compressed file is smaller than original file", {
  fileInput <- "simpleTestFile"
  level <- 10
  in_con <- file(fileInput, "rb")
  out_con <- file("output.zstd", "wb")
  stream_compression(in_con, out_con, level)
  expect_lt(file.size("output.zstd"), file.size(fileInput))
})


#test compression for different file types?
#test compression for different file encodings?

