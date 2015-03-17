#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "macro.h"
//#include "geohex.h"
#include "../include/geohex.h"

static const char*       GEOHEX_HASH_KEY  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const long double GEOHEX_HASH_BASE = 20037508.34L;
static const long double GEOHEX_HASH_DEG  = (long double)M_PI / 6.0L;

inline long _indexOfKey(const int chr) {
  return (long)(strchr(GEOHEX_HASH_KEY, chr) - GEOHEX_HASH_KEY);
}

typedef struct _geohex_coordinate_ld_s {
  long double x;
  long double y;
} geohex_coordinate_ld_t;

inline geohex_coordinate_ld_t _geohex_coordinate_ld (const long double x, const long double y) {
  const geohex_coordinate_ld_t coordinate = { .x = x, .y = y };
  return coordinate;
}

inline geohex_coordinate_t _geohex_adjust_long_integer_coordinate(const geohex_coordinate_ld_t coordinate) {
  return geohex_coordinate((long)floorl(coordinate.x), (long)floorl(coordinate.y));
}

inline geohex_coordinate_ld_t _geohex_adjust_long_double_coordinate(const geohex_coordinate_t coordinate) {
  return _geohex_coordinate_ld((long double)coordinate.x, (long double)coordinate.y);
}

// for performance :)
static const long double GEOHEX_CALCED_HEX_SIZE[] = {
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  3), //  0
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  4), //  1
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  5), //  2
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  6), //  3
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  7), //  4
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  8), //  5
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L,  9), //  6
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 10), //  7
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 11), //  8
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 12), //  9
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 13), // 10
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 14), // 11
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 15), // 12
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 16), // 13
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 17), // 14
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0L, 18)  // 15
};

// for performance :)
static const long GEOHEX_CALCED_POW3[] = {
  GEOHEX_MACRO_POW(3L,  0),
  GEOHEX_MACRO_POW(3L,  1),
  GEOHEX_MACRO_POW(3L,  2),
  GEOHEX_MACRO_POW(3L,  3),
  GEOHEX_MACRO_POW(3L,  4),
  GEOHEX_MACRO_POW(3L,  5),
  GEOHEX_MACRO_POW(3L,  6),
  GEOHEX_MACRO_POW(3L,  7),
  GEOHEX_MACRO_POW(3L,  8),
  GEOHEX_MACRO_POW(3L,  9),
  GEOHEX_MACRO_POW(3L, 10),
  GEOHEX_MACRO_POW(3L, 11),
  GEOHEX_MACRO_POW(3L, 12),
  GEOHEX_MACRO_POW(3L, 13),
  GEOHEX_MACRO_POW(3L, 14),
  GEOHEX_MACRO_POW(3L, 15),
  GEOHEX_MACRO_POW(3L, 16),
  GEOHEX_MACRO_POW(3L, 17),
  GEOHEX_MACRO_POW(3L, 18)
};

// for performance :)
static const long GEOHEX_CALCED_POW10[] = {
  GEOHEX_MACRO_POW(10L,  0),
  GEOHEX_MACRO_POW(10L,  1),
  GEOHEX_MACRO_POW(10L,  2),
  GEOHEX_MACRO_POW(10L,  3),
  GEOHEX_MACRO_POW(10L,  4),
  GEOHEX_MACRO_POW(10L,  5),
  GEOHEX_MACRO_POW(10L,  6),
  GEOHEX_MACRO_POW(10L,  7),
  GEOHEX_MACRO_POW(10L,  8),
  GEOHEX_MACRO_POW(10L,  9),
  GEOHEX_MACRO_POW(10L, 10),
  GEOHEX_MACRO_POW(10L, 11),
  GEOHEX_MACRO_POW(10L, 12),
  GEOHEX_MACRO_POW(10L, 13),
  GEOHEX_MACRO_POW(10L, 14),
  GEOHEX_MACRO_POW(10L, 15),
  GEOHEX_MACRO_POW(10L, 16),
  GEOHEX_MACRO_POW(10L, 17)
};

inline long geohex_pow3(int y) {
  if (y > GEOHEX_MAX_LEVEL+3) {
    return (long)pow(3, y);
  }
  else {
    return GEOHEX_CALCED_POW3[y];
  }
}

inline long geohex_pow10(int y) {
  if (y > GEOHEX_MAX_LEVEL+2) {
    return (long)pow(10, y);
  }
  else {
    return GEOHEX_CALCED_POW10[y];
  }
}

inline long double geohex_hexsize(const size_t level) {
  if (level > GEOHEX_MAX_LEVEL) {
    return GEOHEX_HASH_BASE / (long double)geohex_pow3(level + 3);
  }
  else {
    return GEOHEX_CALCED_HEX_SIZE[level];
  }
}

static long double _deg() {
  static long double deg = 0.0L;
  if (deg == 0.0L) deg = tanl(GEOHEX_HASH_DEG);
  return deg;
}

geohex_coordinate_ld_t _geohex_location2coordinate(const geohex_location_t location) {
  geohex_coordinate_ld_t coordinate;
  coordinate.x = location.lng * GEOHEX_HASH_BASE / 180.0L;
  coordinate.y = GEOHEX_HASH_BASE * logl(tanl((90.0L + location.lat) * (long double)M_PI / 360.0L)) / (long double)M_PI;
  return coordinate;
}

geohex_coordinate_t geohex_location2coordinate(const geohex_location_t location) {
  return _geohex_adjust_long_integer_coordinate(_geohex_location2coordinate(location));
}

geohex_location_t _geohex_coordinate2location(const geohex_coordinate_ld_t coordinate) {
  geohex_location_t location;
  location.lng = 180.0L * coordinate.x / GEOHEX_HASH_BASE;
  location.lat = 180.0L * coordinate.y / GEOHEX_HASH_BASE;
  location.lat = 180.0L * (2.0L * atanl(expl(location.lat * (long double)M_PI / 180.0L)) - (long double)M_PI / 2.0L) / (long double)M_PI;
  return location;
}

geohex_location_t geohex_coordinate2location(const geohex_coordinate_t coordinate) {
  return _geohex_coordinate2location(_geohex_adjust_long_double_coordinate(coordinate));
}

geohex_t _geohex_get_zone_by_coordinate(const geohex_coordinate_ld_t coordinate, const size_t level);
geohex_coordinate_ld_t _geohex_get_coordinate_by_code(const char *code);
geohex_coordinate_ld_t _geohex_get_coordinate_by_location(const geohex_location_t location, const size_t level);

geohex_polygon_t geohex_get_hex_polygon (const geohex_t *geohex) {
  const long double deg = _deg();

  const long double h_x    = (long double)geohex->coordinate.x;
  const long double h_y    = (long double)geohex->coordinate.y;
  const long double h_size = geohex->size;

  const long double top_lat        = _geohex_coordinate2location(_geohex_coordinate_ld(h_x,                 h_y + deg * h_size)).lat;
  const long double center_lat     = _geohex_coordinate2location(_geohex_coordinate_ld(h_x,                 h_y               )).lat;
  const long double bottom_lat     = _geohex_coordinate2location(_geohex_coordinate_ld(h_x,                 h_y - deg * h_size)).lat;
  const long double left_lng       = _geohex_coordinate2location(_geohex_coordinate_ld(h_x - 2.0L * h_size, h_y               )).lng;
  const long double right_lng      = _geohex_coordinate2location(_geohex_coordinate_ld(h_x + 2.0L * h_size, h_y               )).lng;
  const long double base_left_lng  = _geohex_coordinate2location(_geohex_coordinate_ld(h_x - 1.0L * h_size, h_y               )).lng;
  const long double base_right_lng = _geohex_coordinate2location(_geohex_coordinate_ld(h_x + 1.0L * h_size, h_y               )).lng;

  geohex_polygon_t polygon;
  polygon.top.right    = geohex_location(top_lat,    base_right_lng);
  polygon.top.left     = geohex_location(top_lat,    base_left_lng);
  polygon.middle.right = geohex_location(center_lat, right_lng);
  polygon.middle.left  = geohex_location(center_lat, left_lng);
  polygon.bottom.right = geohex_location(bottom_lat, base_right_lng);
  polygon.bottom.left  = geohex_location(bottom_lat, base_left_lng);
  return polygon;
}

geohex_t geohex_get_zone_by_location (const geohex_location_t location, const size_t level) {
  const geohex_coordinate_t coordinate = geohex_get_coordinate_by_location(location, level);
  return geohex_get_zone_by_coordinate(coordinate, level);
}

geohex_t geohex_get_zone_by_code(const char *code) {
  const geohex_coordinate_t coordinate = geohex_get_coordinate_by_code(code);
  const size_t level = geohex_calc_level_by_code(code);
  return geohex_get_zone_by_coordinate(coordinate, level);
}

geohex_coordinate_t geohex_get_coordinate_by_location(const geohex_location_t location, const size_t level) {
  const geohex_coordinate_ld_t coordinate = _geohex_location2coordinate(location);

  const long double size     = geohex_hexsize(level);
  const long double deg      = _deg();
  const long double lon_grid = coordinate.x;
  const long double lat_grid = coordinate.y;
  const long double unit_x   = 6.0L * size;
  const long double unit_y   = 6.0L * size * deg;
  const long double h_pos_x  = (lon_grid + lat_grid / deg) / unit_x;
  const long double h_pos_y  = (lat_grid - deg * lon_grid) / unit_y;
  const long double h_x_0    = floorl(h_pos_x);
  const long double h_y_0    = floorl(h_pos_y);
  const long double h_x_q    = h_pos_x - h_x_0;
  const long double h_y_q    = h_pos_y - h_y_0;

  long h_x = (long)roundl(h_pos_x);
  long h_y = (long)roundl(h_pos_y);
  if (h_y_q > -h_x_q + 1.0L) {
    if ((h_y_q < 2.0L * h_x_q) && (h_y_q > 0.5L * h_x_q)) {
      h_x = (long)h_x_0 + 1L;
      h_y = (long)h_y_0 + 1L;
    }
  } else if (h_y_q < -h_x_q + 1) {
    if ((h_y_q > (2.0L * h_x_q) - 1) && (h_y_q < (0.5L * h_x_q) + 0.5L)) {
      h_x = (long)h_x_0;
      h_y = (long)h_y_0;
    }
  }

  return geohex_adjust_coordinate(geohex_coordinate(h_x, h_y), level);
}

bool geohex_is_special_area(const long area_num) {
  if (area_num < 10L) {
    return area_num == 1L || area_num == 5L;
  }
  else if (area_num < 1000L) {
    const long digit1x = area_num % 10L;
    return geohex_is_special_area(area_num / 10L) && (digit1x == 0L || digit1x == 3L || digit1x == 4L || (5L < digit1x && digit1x < 10L));
  }
  else {
    return geohex_is_special_area(area_num / 10L);
  }
}

long geohex_adjusted_area_num(const long area_num) {
  long offset = 0L;
  if (geohex_is_special_area(area_num)) {
    const long double lg10  = log10l(area_num);
    const long double level = floorl(lg10 > 17.0L ? 17.0L : lg10 < 3.0L ? 2.0L : lg10);
    offset = (long)floorl(2.0L * powl(10.0L, level));
  }
  return area_num + offset;
}

long _geohex_area_num_by_code(const char *code) {
  const size_t level           = geohex_calc_level_by_code(code);
  const long   global_area_num = _indexOfKey(code[0]) * 30L + _indexOfKey(code[1]);
  const long   area_num        = global_area_num * geohex_pow10(level) + atol(code+2);
  return global_area_num > 0L ? geohex_adjusted_area_num(area_num) : area_num;
}

static char *_geohex_itoa_with_base(register char *p, const size_t len, const int base, const long n) {
  if (p == NULL || len < 2L) return NULL;
  if (n < base) {
    const static char chr[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    *p++ = chr[n];
    *p   = '\0';
    return p;
  }
  else {
    return _geohex_itoa_with_base(_geohex_itoa_with_base(p, len - 1L, base, n / base), 2L, base, n % base);
  }
}

static char *_geohex_generate_hdec3_f(char *p, size_t len, const long n) {
  if (p == NULL || len < 3L) return NULL;
  if (n < 10L) {
    if (n < 3L) {
      *p++ = '0';
      len--;
    }
    return _geohex_itoa_with_base(p, len, 3L, n);
  }
  else {
    return _geohex_generate_hdec3_f(_geohex_generate_hdec3_f(p, len - 2L, n / 10L), 4L, n % 10L);
  }
}

static char *_geohex_generate_hdec3(char *p, size_t len, const long area_num, const size_t level) {
  // zero padding
  const size_t zero = level + 3 - (size_t)floor(log10((double)area_num)) - 1;
  for (unsigned int i = 0; i < zero; i += 2) {
    *p++ = '0';
    if (--len == 0) return NULL;
    *p++ = '0';
    if (--len == 0) return NULL;
  }

  return _geohex_generate_hdec3_f(p, len, area_num);
}

inline int _char2int(const char c) {
  static const char chr[] = "0123456789";
  for (unsigned int i = 0, l = strlen(chr); i < l; i++) {
    if (c == chr[i]) return i;
  }
  fprintf(stderr, "assertion failure. unexpected char:%c\n", c);
  abort();
}

geohex_coordinate_t geohex_get_coordinate_by_code(const char *code) {
  const size_t level    = geohex_calc_level_by_code(code);
  const long   area_num = _geohex_area_num_by_code(code);

  char   h_dec3_buf[GEOHEX_DEC3_BUFSIZE];
  size_t h_dec3_len = _geohex_generate_hdec3(h_dec3_buf, GEOHEX_DEC3_BUFSIZE, area_num, level) - h_dec3_buf;

  int h_decx[GEOHEX_DEC3_BUFSIZE/2];
  int h_decy[GEOHEX_DEC3_BUFSIZE/2];
  for (unsigned int i = 0; i < h_dec3_len / 2; i++) {
    h_decx[i] = _char2int(h_dec3_buf[i*2]);
    h_decy[i] = _char2int(h_dec3_buf[i*2+1]);
  }

  long h_x = 0;
  long h_y = 0;
  for (unsigned int i = 0; i <= level+2; i++) {
    const long h_pow = geohex_pow3(level + 2 - i);
    if (h_decx[i] == 0) {
      h_x -= h_pow;
    }
    else if(h_decx[i] == 2){
      h_x += h_pow;
    }

    if (h_decy[i] == 0) {
      h_y -= h_pow;
    }
    else if(h_decy[i] == 2){
      h_y += h_pow;
    }
  }

  return geohex_adjust_coordinate(geohex_coordinate(h_x, h_y), level);
}

geohex_t geohex_get_zone_by_coordinate(const geohex_coordinate_t coordinate, const size_t level) {
  const long double h_k    = _deg();
  const long double h_size = geohex_hexsize(level);

  long double h_x = (long double)coordinate.x;
  long double h_y = (long double)coordinate.y;

  const long double unit_x = 6.0L * h_size;
  const long double unit_y = 6.0L * h_size * h_k;

  const long double       h_lat = (h_k * h_x * unit_x + h_y * unit_y) / 2.0L;
  const long double       h_lon = (h_lat - h_y * unit_y) / h_k;
  const geohex_location_t z_loc = _geohex_coordinate2location(_geohex_coordinate_ld(h_lon, h_lat));

  long double z_loc_x = z_loc.lng;
  long double z_loc_y = z_loc.lat;

  const long max_hsteps = geohex_pow3(level + 2);
  const long hsteps     = (long)floorl(fabsl(h_x - h_y));
  if (hsteps == max_hsteps) {
    if (h_x > h_y) {
      long double tmp = h_x;
      h_x = h_y;
      h_y = tmp;
    }
    z_loc_x = -180.0L;
  }

  char code3_x[GEOHEX_DEC3_BUFSIZE/2];
  char code3_y[GEOHEX_DEC3_BUFSIZE/2];

  long mod_x = h_x;
  long mod_y = h_y;
  for (unsigned int i = 0; i <= level + 2; i++) {
    const long h_pow   = geohex_pow3(level + 2 - i);
    const long h_pow_c = (long)ceill((long double)h_pow / 2.0L);

    if (mod_x >= h_pow_c) {
      code3_x[i] = 2;
      mod_x -= h_pow;
    }
    else if (mod_x <= -h_pow_c) {
      code3_x[i] = 0;
      mod_x += h_pow;
    }
    else {
      code3_x[i] = 1;
    }

    if (mod_y >= h_pow_c) {
      code3_y[i] = 2;
      mod_y -= h_pow;
    }
    else if (mod_y <= -h_pow_c) {
      code3_y[i] = 0;
      mod_y += h_pow;
    }
    else {
      code3_y[i] = 1;
    }

    if (i==2 && (z_loc_x == -180.0L || z_loc_x >= 0.0L)) {
      if (code3_x[0] == 2 && code3_y[0] == 1 && code3_x[1] == code3_y[1] && code3_x[2] == code3_y[2]) {
        code3_x[0] = 1;
        code3_y[0] = 2;
      }
      else if (code3_x[0] == 1 && code3_y[0] == 0 && code3_x[1] == code3_y[1] && code3_x[2] == code3_y[2]) {
        code3_x[0] = 0;
        code3_y[0] = 1;
      }
    }
  }

  char h_code[GEOHEX_DEC9_BUFSIZE];
  for (unsigned int i = 0; i <= level + 2; i++) {
    const static char chr[] = "0123456789";

    char code3[GEOHEX_DEC3_BUFSIZE];
    code3[0] = chr[(unsigned)code3_x[i]];
    code3[1] = chr[(unsigned)code3_y[i]];
    code3[2] = '\0';

    int code9 = (int)strtol(code3, NULL, 3);
    h_code[i] = chr[code9];
  }
  h_code[level + 3] = '\0';

  char global_code_buf[GEOHEX_GLOBAL_CODE_BUFSIZE];
  strncpy(global_code_buf, h_code, GEOHEX_GLOBAL_CODE_BUFSIZE);
  global_code_buf[GEOHEX_GLOBAL_CODE_BUFSIZE-1] = '\0';
  const int global_code = atoi(global_code_buf);

  geohex_t geohex = {
    .location   = geohex_location(z_loc_y, z_loc_x),
    .coordinate = coordinate,
    .level      = level,
    .size       = h_size
  };
  geohex.code[0] = GEOHEX_HASH_KEY[(int)floor(global_code/30)];
  geohex.code[1] = GEOHEX_HASH_KEY[global_code%30];
  strncpy(geohex.code + 2, h_code + 3, GEOHEX_MAX_LEVEL);
  geohex.code[strlen(h_code)] = '\0';
  return geohex;
}

geohex_coordinate_t geohex_adjust_coordinate(const geohex_coordinate_t coordinate, const size_t level) {
  long x          = coordinate.x;
  long y          = coordinate.y;
  long max_hsteps = geohex_pow3(level + 2);
  long hsteps     = labs(x - y);
  if (hsteps == max_hsteps && x > y) {
    const long double tmp = x;
    x = y;
    y = tmp;
  }
  else if (hsteps > max_hsteps) {
    const long dif   = hsteps - max_hsteps;
    const long dif_x = (long)floorl((long double)dif / 2.0L);
    const long dif_y = dif - dif_x;

    long edge_x;
    long edge_y;
    if (x > y) {
      edge_x = x - dif_x;
      edge_y = y + dif_y;

      const long h_coordinate = edge_x;
      edge_x = edge_y;
      edge_y = h_coordinate;

      x = edge_x + dif_x;
      y = edge_y - dif_y;
    }
    else if (y > x) {
      edge_x = x + dif_x;
      edge_y = y - dif_y;

      const long h_coordinate = edge_x;
      edge_x = edge_y;
      edge_y = h_coordinate;

      x = edge_x - dif_x;
      y = edge_y + dif_y;
    }
  }

  return geohex_coordinate(x, y);
}

