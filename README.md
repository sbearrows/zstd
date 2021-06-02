
# zstd

<!-- badges: start -->
[![R-CMD-check](https://github.com/sbearrows/zstd/workflows/R-CMD-check/badge.svg)](https://github.com/sbearrows/zstd/actions)
<!-- badges: end -->

Fast, lossless compression using the Zstandard algorithm

## Installation

You can install the released version of zstd from [CRAN](https://CRAN.R-project.org) with:

``` r
install.packages("zstd")
```

## Example

Simple data compression example:

``` r
library(zstd)
# simple compression and decompression
input <- readBin("input", what = 'raw', n = file.size("input"))
output <- compression(input, level = 10)
original <- decompression(output)

# streamable compression
# open input and output connections
input <- file("input", "rb")
output <- file("output.zstd", "wb")

# output written to file
stream_compression(input, output, level = 10)

# streamable decompression
# open input and output connections
input <- file("output.zstd", "rb")
output <- file("original", "wb")

# output written to file
stream_decompression(input, output)


```

