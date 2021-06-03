

test_that("zstd contents in decompressed files are equal to original file", {
  fileInput <- "simple_testFile"
  level <- 10

  t1 <- readBin(fileInput, what = 'raw', n = file.size(fileInput))

  t2 <- compress_raw(t1, level = level)

  t3 <- decompress_raw(t2)

  expect_equal(rawToChar(t3),
               readChar(fileInput, file.size(fileInput)))
})


test_that("zstd contents in streaming compressed file is smaller than original file", {
  fileInput <- "simple_testFile"
  level <- 10
  compress_file(fileInput, "output.zst", level)
  expect_lt(file.size("output.zst"), file.size(fileInput))
})

test_that("zstd contents in streaming decompressed file is the same as the input file", {
  fileInput <- "simple_testFile"
  level <- 10
  compress_file(fileInput, "output.zst", level)


  decompress_file("output.zst", "final_output")
  expect_equal(readChar(fileInput, file.size(fileInput)), readChar("final_output", file.size("final_output")))
})





