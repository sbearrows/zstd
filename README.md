
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
stream_compression("input", "output.zst", level = 10)

# streamable decompression
stream_decompression("output.zst", "original")


```

