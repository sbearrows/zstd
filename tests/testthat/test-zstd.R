

test_that("zstd contents in decompressed files are equal to original file", {
  fileInput <- "simpleTestFile"
  level <- 10
  expect_equal(test_zstd(fileInput, level),
               readChar(fileInput, file.size(fileInput)))
})

#test compression for different file types?
#test compression for different file encodings?

