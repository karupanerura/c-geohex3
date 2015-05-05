[![Build Status](https://travis-ci.org/karupanerura/c-geohex3.svg?branch=master)](https://travis-ci.org/karupanerura/c-geohex3)

# c-geohex3

GeoHex v3 implementation in C99.

# What is GeoHex?

https://sites.google.com/site/geohexdocs/
http://geohex.net/

# Synopsis

```c
#include <geohex3.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    printf("/********* geohex:%s **********/\n", argv[i]);
    const geohex_verify_result_t result = geohex_verify_code(argv[i]);
    switch (result) {
      case GEOHEX_VERIFY_RESULT_SUCCESS:
        {
          const geohex_t geohex = geohex_get_zone_by_code(argv[i]);
          printf("code  = %s\n", geohex.code);
          printf("level = %zu\n", geohex.level);
          printf("size  = %Lf\n", geohex.size);
          printf("[location]\n");
          printf("lat = %Lf\n", geohex.location.lat);
          printf("lng = %Lf\n", geohex.location.lng);
          printf("[coordinate]\n");
          printf("x = %ld\n", geohex.coordinate.x);
          printf("y = %ld\n", geohex.coordinate.y);
        }
        break;
      case GEOHEX_VERIFY_RESULT_INVALID_CODE:
        printf("code:%s is invalid.\n", argv[i]);
        break;
      case GEOHEX_VERIFY_RESULT_INVALID_LEVEL:
        printf("code:%s is invalid level. MAX_LEVEL:%d\n", argv[i], GEOHEX_MAX_LEVEL);
        break;
    }
  }
}

```

# Dependencies

* picotest
* cmake
* prove

# Cheat Sheet

## SETUP

```bash
git clone https://github.com/karupanerura/c-geohex3.git
cd c-geohex3
git submodule update --init
```

## BUILD

```bash
cmake .
make
```

## TEST

```bash
make test
```

## INSTALL

```bash
sudo -H make install
```
