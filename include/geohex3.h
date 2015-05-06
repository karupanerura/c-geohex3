#ifndef GEOHEX_H
#define GEOHEX_H

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define GEOHEX_MIN_LEVEL           0
#define GEOHEX_MAX_LEVEL           15
#define GEOHEX_GLOBAL_CODE_BUFSIZE 4
#define GEOHEX_DEC9_BUFSIZE        32
#define GEOHEX_DEC3_BUFSIZE        64

typedef struct _geohex_location_s {
  long double lat;
  long double lng;
} geohex_location_t;

typedef struct _geohex_coordinate_s {
  long x;
  long y;
} geohex_coordinate_t;

struct _geohex_location_lrpair_s {
  geohex_location_t right;
  geohex_location_t left;
};

typedef struct _geohex_polygon_s {
  struct _geohex_location_lrpair_s top;
  struct _geohex_location_lrpair_s middle;
  struct _geohex_location_lrpair_s bottom;
} geohex_polygon_t;

typedef struct _geohex_s {
  geohex_location_t   location;
  geohex_coordinate_t coordinate;
  char                code[GEOHEX_MAX_LEVEL + 3];
  size_t              level;
  long double         size;
} geohex_t;

typedef enum _geohex_verify_result_enum {
  GEOHEX_VERIFY_RESULT_SUCCESS,
  GEOHEX_VERIFY_RESULT_INVALID_CODE,
  GEOHEX_VERIFY_RESULT_INVALID_LEVEL
} geohex_verify_result_t;

typedef size_t geohex_level_t;

inline geohex_coordinate_t geohex_coordinate (const long x, const long y) {
  const geohex_coordinate_t coordinate = { .x = x, .y = y };
  return coordinate;
}

inline geohex_location_t geohex_location (const double lat, const double lng) {
  const geohex_location_t location = { .lat = lat, .lng = lng };
  return location;
}

inline geohex_level_t geohex_calc_level_by_code(const char *code) {
  return strlen(code) - 2;
}

extern geohex_verify_result_t geohex_verify_code(const char *code);
extern geohex_coordinate_t geohex_location2coordinate(const geohex_location_t location);
extern geohex_location_t   geohex_coordinate2location(const geohex_coordinate_t coordinate);
extern geohex_t            geohex_get_zone_by_location(const geohex_location_t location, geohex_level_t level);
extern geohex_t            geohex_get_zone_by_coordinate(const geohex_coordinate_t coordinate, geohex_level_t level);
extern geohex_t            geohex_get_zone_by_code(const char *code);
extern geohex_polygon_t    geohex_get_hex_polygon (const geohex_t *geohex);

// XXX: for test
extern geohex_coordinate_t geohex_get_coordinate_by_location(const geohex_location_t location, geohex_level_t level);

#endif
