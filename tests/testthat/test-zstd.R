

test_that("zstd contents in decompressed files are equal to original file", {
  fileInput <- "simpleTestFile"
  level <- 10

  t1 <- readBin(fileInput, what = 'raw', n = file.size(fileInput))

  t2 <- compression(t1, level = level)

  t3 <- decompression(t2)

  expect_equal(rawToChar(t3),
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

test_that("zstd contents in streaming decompressed file is the same as the input file", {
  fileInput <- "simpleTestFile"
  level <- 10
  in_con <- file(fileInput, "rb")
  out_con <- file("output.zstd", "wb")
  stream_compression(in_con, out_con, level)

  in_con <- file("output.zstd", "rb")
  out_con <- file("final_output", "wb")
  stream_decompression_(in_con, out_con)
  expect_equal(readChar(fileInput, file.size(fileInput)), readChar("final_output", file.size("final_output")))
})





