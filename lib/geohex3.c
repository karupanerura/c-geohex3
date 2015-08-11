#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "geohex3.h"
#include "pow.h"

static const char*       GEOHEX3_HASH_KEY  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const long double GEOHEX3_HASH_DEG  = GEOHEX3_PI / 6.0L;

// inner functions
static geohex_coordinate_t geohex_get_coordinate_by_code(const char *code);
static geohex_coordinate_t geohex_adjust_coordinate(const geohex_coordinate_t coordinate, geohex_level_t level);

static inline long _indexOfKey(const int chr) {
  return (long)(strchr(GEOHEX3_HASH_KEY, chr) - GEOHEX3_HASH_KEY);
}

typedef struct _geohex_coordinate_ld_s {
  long double x;
  long double y;
} geohex_coordinate_ld_t;

static inline geohex_coordinate_ld_t _geohex_coordinate_ld (const long double x, const long double y) {
  const geohex_coordinate_ld_t coordinate = { .x = x, .y = y };
  return coordinate;
}

static inline geohex_coordinate_t _geohex_adjust_long_integer_coordinate(const geohex_coordinate_ld_t coordinate) {
  return geohex_coordinate((long)floorl(coordinate.x), (long)floorl(coordinate.y));
}

static inline geohex_coordinate_ld_t _geohex_adjust_long_double_coordinate(const geohex_coordinate_t coordinate) {
  return _geohex_coordinate_ld((long double)coordinate.x, (long double)coordinate.y);
}

static inline geohex_coordinate_ld_t _geohex_location2coordinate(const geohex_location_t location) {
  geohex_coordinate_ld_t coordinate;
  coordinate.x = location.lng * GEOHEX3_HASH_BASE / 180.0L;
  coordinate.y = GEOHEX3_HASH_BASE * logl(tanl((GEOHEX3_PI / 4.0L) + (location.lat * GEOHEX3_PI / 360.0L))) / GEOHEX3_PI;
  return coordinate;
}

static inline geohex_location_t _geohex_coordinate2location(const geohex_coordinate_ld_t coordinate) {
  geohex_location_t location;
  location.lng = 180.0L * coordinate.x / GEOHEX3_HASH_BASE;
  location.lat = 360.0L * atanl(expl(coordinate.y * GEOHEX3_PI / GEOHEX3_HASH_BASE)) / GEOHEX3_PI - 90.0L;
  return location;
}

static long double _deg() {
  static long double deg = 0.0L;
  if (deg == 0.0L) deg = tanl(GEOHEX3_HASH_DEG);
  return deg;
}

geohex_coordinate_t geohex_location2coordinate(const geohex_location_t location) {
  return _geohex_adjust_long_integer_coordinate(_geohex_location2coordinate(location));
}

geohex_location_t geohex_coordinate2location(const geohex_coordinate_t coordinate) {
  return _geohex_coordinate2location(_geohex_adjust_long_double_coordinate(coordinate));
}

geohex_polygon_t geohex_get_hex_polygon (const geohex_t *geohex) {
  const geohex_coordinate_ld_t coordinate = _geohex_location2coordinate(geohex->location);
  const long double h_x = coordinate.x;
  const long double h_y = coordinate.y;

  const long double h_deg  = tanl(GEOHEX3_PI / 3.0L);
  const long double h_size = geohex->size;

  const long double center_lat     = geohex->location.lat;
  const long double top_lat        = _geohex_coordinate2location(_geohex_coordinate_ld(h_x, h_y - h_deg * h_size)).lat;
  const long double bottom_lat     = _geohex_coordinate2location(_geohex_coordinate_ld(h_x, h_y + h_deg * h_size)).lat;
  const long double left_lng       = _geohex_coordinate2location(_geohex_coordinate_ld(h_x - 2.0L * h_size, h_y)).lng;
  const long double right_lng      = _geohex_coordinate2location(_geohex_coordinate_ld(h_x + 2.0L * h_size, h_y)).lng;
  const long double base_left_lng  = _geohex_coordinate2location(_geohex_coordinate_ld(h_x - 1.0L * h_size, h_y)).lng;
  const long double base_right_lng = _geohex_coordinate2location(_geohex_coordinate_ld(h_x + 1.0L * h_size, h_y)).lng;

  geohex_polygon_t polygon;
  polygon.top.right    = geohex_location(top_lat,    base_right_lng);
  polygon.top.left     = geohex_location(top_lat,    base_left_lng);
  polygon.middle.right = geohex_location(center_lat, right_lng);
  polygon.middle.left  = geohex_location(center_lat, left_lng);
  polygon.bottom.right = geohex_location(bottom_lat, base_right_lng);
  polygon.bottom.left  = geohex_location(bottom_lat, base_left_lng);
  return polygon;
}

geohex_verify_result_t geohex_verify_code(const char *code) {
  // check length
  const size_t len = strlen(code);
  if (len < 2) return GEOHEX3_VERIFY_RESULT_INVALID_CODE;

  // check area code
  if (strchr(GEOHEX3_HASH_KEY, code[0]) == NULL) return GEOHEX3_VERIFY_RESULT_INVALID_CODE;
  if (strchr(GEOHEX3_HASH_KEY, code[1]) == NULL) return GEOHEX3_VERIFY_RESULT_INVALID_CODE;

  // check level
  const geohex_level_t level = geohex_calc_level_by_code(code);
  if (level > GEOHEX3_MAX_LEVEL) return GEOHEX3_VERIFY_RESULT_INVALID_LEVEL;

  // check num
  const static char* valid_numbers = "012345678";
  for (unsigned int i = 2; i < len; i++) {
    if (strchr(valid_numbers, code[i]) == NULL) return GEOHEX3_VERIFY_RESULT_INVALID_CODE;
  }

  return GEOHEX3_VERIFY_RESULT_SUCCESS;
}

geohex_t geohex_get_zone_by_location (const geohex_location_t location, const geohex_level_t level) {
  const geohex_coordinate_t coordinate = geohex_get_coordinate_by_location(location, level);
  return geohex_get_zone_by_coordinate(coordinate, level);
}

geohex_t geohex_get_zone_by_code(const char *code) {
  const geohex_coordinate_t coordinate = geohex_get_coordinate_by_code(code);
  const geohex_level_t level = geohex_calc_level_by_code(code);
  return geohex_get_zone_by_coordinate(coordinate, level);
}

geohex_coordinate_t geohex_get_coordinate_by_location(const geohex_location_t location, const geohex_level_t level) {
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

  long h_x = lroundl(h_pos_x);
  long h_y = lroundl(h_pos_y);
  if (h_y_q > -h_x_q + 1.0L) {
    if ((h_y_q < 2.0L * h_x_q) && (h_y_q > 0.5L * h_x_q)) {
      h_x = (long)h_x_0 + 1L;
      h_y = (long)h_y_0 + 1L;
    }
  } else if (h_y_q < -h_x_q + 1) {
    if ((h_y_q > (2.0L * h_x_q) - 1.0L) && (h_y_q < (0.5L * h_x_q) + 0.5L)) {
      h_x = (long)h_x_0;
      h_y = (long)h_y_0;
    }
  }

  return geohex_adjust_coordinate(geohex_coordinate(h_x, h_y), level);
}

static bool geohex_is_special_area(const long area_num) {
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

static long geohex_adjusted_area_num(const long area_num) {
  long offset = 0L;
  if (geohex_is_special_area(area_num)) {
    const long double lg10  = log10l(area_num);
    const long double level = floorl(lg10 > 17.0L ? 17.0L : lg10 < 3.0L ? 2.0L : lg10);
    offset = (long)floorl(2.0L * powl(10.0L, level));
  }
  return area_num + offset;
}

static long _geohex_area_num_by_code(const char *code) {
  const geohex_level_t level = geohex_calc_level_by_code(code);
  const long global_area_num = _indexOfKey(code[0]) * 30L + _indexOfKey(code[1]);
  const long area_num        = global_area_num * geohex_pow10(level) + atol(code+2);
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

static char *_geohex_generate_hdec3_f(register char *p, size_t len, const long n) {
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

static char *_geohex_generate_hdec3(register char *p, size_t len, const long area_num, const geohex_level_t level) {
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

static inline int _char2int(const char c) {
  static const char chr[] = "0123456789";
  for (unsigned int i = 0, l = strlen(chr); i < l; i++) {
    if (c == chr[i]) return i;
  }
  fprintf(stderr, "assertion failure. unexpected char:%c\n", c);
  abort();
}

geohex_coordinate_t geohex_get_coordinate_by_code(const char *code) {
  const geohex_level_t level = geohex_calc_level_by_code(code);
  const long area_num = _geohex_area_num_by_code(code);

  static char   h_dec3_buf[GEOHEX3_DEC3_BUFSIZE];
  const  size_t h_dec3_len = _geohex_generate_hdec3(h_dec3_buf, GEOHEX3_DEC3_BUFSIZE, area_num, level) - h_dec3_buf;

  static int h_decx[GEOHEX3_DEC3_BUFSIZE/2];
  static int h_decy[GEOHEX3_DEC3_BUFSIZE/2];
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

static geohex_location_t geohex_get_location_by_coordinate(const geohex_coordinate_t coordinate, const geohex_level_t level) {
  const long double h_k    = _deg();
  const long double h_size = geohex_hexsize(level);
  const long double x      = (long double)coordinate.x;
  const long double y      = (long double)coordinate.y;

  const long double unit_x = 6.0L * h_size;
  const long double unit_y = 6.0L * h_size * h_k;

  const long double h_y = (h_k * x * unit_x + y * unit_y) / 2.0L;
  const long double h_x = (h_y - y * unit_y) / h_k;

  geohex_location_t z_loc = _geohex_coordinate2location(_geohex_coordinate_ld(h_x, h_y));

  const long max_hsteps = geohex_pow3(level + 2);
  const long hsteps     = labs(coordinate.x - coordinate.y);
  if (hsteps == max_hsteps) {
    z_loc.lng = -180.0L;
  }

  return z_loc;
}

geohex_t geohex_get_zone_by_coordinate(const geohex_coordinate_t coordinate, const geohex_level_t level) {
  const geohex_location_t z_loc = geohex_get_location_by_coordinate(coordinate, level);

  long mod_x = coordinate.x;
  long mod_y = coordinate.y;

  const long max_hsteps = geohex_pow3(level + 2);
  const long hsteps     = labs(mod_x - mod_y);
  if (hsteps == max_hsteps) {
    if (mod_x > mod_y) {
      const long tmp = coordinate.x;
      mod_x = mod_y;
      mod_y = tmp;
    }
  }

  static char code3_x[GEOHEX3_DEC3_BUFSIZE/2];
  static char code3_y[GEOHEX3_DEC3_BUFSIZE/2];
  for (unsigned int i = 0; i <= level + 2; i++) {
    const long h_pow   = geohex_pow3(level + 2 - i);
    const long h_pow_c = h_pow / 2 + h_pow % 2;

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

    if (i==2 && (z_loc.lng == -180.0L || z_loc.lng >= 0.0L)) {
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

  static char h_code[GEOHEX3_DEC9_BUFSIZE - GEOHEX3_GLOBAL_CODE_BUFSIZE - 1];
  static unsigned int global_code = 0;
  {
    const static unsigned int offset = GEOHEX3_GLOBAL_CODE_BUFSIZE - 1;

    global_code = 0;
    for (unsigned int i = 0, max = level + offset; i < max; i++) {
      const unsigned int code9 = 3 * (unsigned int)code3_x[i] + (unsigned int)code3_y[i];
      if (i < offset) {
        global_code *= 10;
        global_code += code9;
      }
      else {
        const static char chr[] = "0123456789";
        h_code[i-offset] = chr[code9];
      }
    }
    h_code[level] = '\0';
  }

  geohex_t geohex = {
    .location   = geohex_location(z_loc.lat, z_loc.lng),
    .coordinate = coordinate,
    .level      = level,
    .size       = geohex_hexsize(level)
  };
  geohex.code[0] = GEOHEX3_HASH_KEY[(int)floor(global_code/30)];
  geohex.code[1] = GEOHEX3_HASH_KEY[global_code%30];
  strncpy(geohex.code + 2, h_code, level+1);
  return geohex;
}

static geohex_coordinate_t geohex_adjust_coordinate(const geohex_coordinate_t coordinate, const geohex_level_t level) {
  const long x          = coordinate.x;
  const long y          = coordinate.y;
  const long max_hsteps = geohex_pow3(level + 2);
  const long hsteps     = labs(x - y);
  if (hsteps == max_hsteps && x > y) {
    return geohex_coordinate(y, x); // swap
  }
  else if (hsteps > max_hsteps) {
    const long dif   = hsteps - max_hsteps;
    const long dif_x = (long)floorl((long double)dif / 2.0L);
    const long dif_y = dif - dif_x;
    if (x > y) {
      const long edge_x = y + dif_y;
      const long edge_y = x - dif_x;
      const long x      = edge_x + dif_x;
      const long y      = edge_y - dif_y;
      return geohex_coordinate(x, y);
    }
    else if (y > x) {
      const long edge_x = y - dif_y;
      const long edge_y = x + dif_x;
      const long x      = edge_x - dif_x;
      const long y      = edge_y + dif_y;
      return geohex_coordinate(x, y);
    }
  }

  return geohex_coordinate(x, y);
}

