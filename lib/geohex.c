#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "macro.h"
#include "geohex.h"

static const char*  GEOHEX_HASH_KEY  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const double GEOHEX_HASH_BASE = 20037508.34;
static const double GEOHEX_HASH_DEG  = M_PI * 30 / 180;

// for performance :)
static const double GEOHEX_CALCED_HEX_SIZE[] = {
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  3), //  0
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  4), //  1
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  5), //  2
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  6), //  3
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  7), //  4
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  8), //  5
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0,  9), //  6
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 10), //  7
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 11), //  8
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 12), //  9
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 13), // 10
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 14), // 11
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 15), // 12
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 16), // 13
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 17), // 14
  GEOHEX_HASH_BASE / GEOHEX_MACRO_POW(3.0, 18)  // 15
};

// for performance :)
static const double GEOHEX_CALCED_POW3[] = {
  GEOHEX_MACRO_POW(3.0,  0),
  GEOHEX_MACRO_POW(3.0,  1),
  GEOHEX_MACRO_POW(3.0,  2),
  GEOHEX_MACRO_POW(3.0,  3),
  GEOHEX_MACRO_POW(3.0,  4),
  GEOHEX_MACRO_POW(3.0,  5),
  GEOHEX_MACRO_POW(3.0,  6),
  GEOHEX_MACRO_POW(3.0,  7),
  GEOHEX_MACRO_POW(3.0,  8),
  GEOHEX_MACRO_POW(3.0,  9),
  GEOHEX_MACRO_POW(3.0, 10),
  GEOHEX_MACRO_POW(3.0, 11),
  GEOHEX_MACRO_POW(3.0, 12),
  GEOHEX_MACRO_POW(3.0, 13),
  GEOHEX_MACRO_POW(3.0, 14),
  GEOHEX_MACRO_POW(3.0, 15),
  GEOHEX_MACRO_POW(3.0, 16),
  GEOHEX_MACRO_POW(3.0, 17),
  GEOHEX_MACRO_POW(3.0, 18)
};

// for performance :)
static const double GEOHEX_CALCED_POW10[] = {
  GEOHEX_MACRO_POW(10.0,  0),
  GEOHEX_MACRO_POW(10.0,  1),
  GEOHEX_MACRO_POW(10.0,  2),
  GEOHEX_MACRO_POW(10.0,  3),
  GEOHEX_MACRO_POW(10.0,  4),
  GEOHEX_MACRO_POW(10.0,  5),
  GEOHEX_MACRO_POW(10.0,  6),
  GEOHEX_MACRO_POW(10.0,  7),
  GEOHEX_MACRO_POW(10.0,  8),
  GEOHEX_MACRO_POW(10.0,  9),
  GEOHEX_MACRO_POW(10.0, 10),
  GEOHEX_MACRO_POW(10.0, 11),
  GEOHEX_MACRO_POW(10.0, 12),
  GEOHEX_MACRO_POW(10.0, 13),
  GEOHEX_MACRO_POW(10.0, 14),
  GEOHEX_MACRO_POW(10.0, 15),
  GEOHEX_MACRO_POW(10.0, 16),
  GEOHEX_MACRO_POW(10.0, 17)
};

inline double geohex_pow3(int y) {
  if (0 <= y && y <= GEOHEX_MAX_LEVEL+3) {
    return GEOHEX_CALCED_POW3[y];
  }
  else {
    return pow(3, y);
  }
}

inline double geohex_pow10(int y) {
  if (0 <= y && y <= GEOHEX_MAX_LEVEL+2) {
    return GEOHEX_CALCED_POW10[y];
  }
  else {
    return pow(10, y);
  }
}

inline double geohex_hexsize(const size_t level) {
  if (level > GEOHEX_MAX_LEVEL) {
    return GEOHEX_HASH_BASE / geohex_pow3(level + 3);
  }
  else {
    return GEOHEX_CALCED_HEX_SIZE[level];
  }
}

static double _deg() {
  static double deg = 0;
  if (deg == 0) deg = tan(GEOHEX_HASH_DEG);
  return deg;
}

int _indexOfKey(int chr) {
  return (strchr(GEOHEX_HASH_KEY, chr) - GEOHEX_HASH_KEY);
}

double geohex_get_hex_size (const geohex_t *geohex) {
  return geohex_hexsize(geohex->level);
}

geohex_coordinate_t geohex_location2coordinate(const geohex_location_t location) {
  geohex_coordinate_t coordinate;
  coordinate.x = location.lng * GEOHEX_HASH_BASE / 180;
  coordinate.y = GEOHEX_HASH_BASE * log(tan((90 + location.lat) * M_PI / 360)) / M_PI;
  return coordinate;
}

geohex_location_t geohex_coordinate2location(const geohex_coordinate_t coordinate) {
  geohex_location_t location;
  location.lng = 180 * coordinate.x / GEOHEX_HASH_BASE;
  location.lat = 180 * coordinate.y / GEOHEX_HASH_BASE;
  location.lat = 180 * (2 * atan(exp(location.lat * M_PI / 180)) - M_PI / 2) / M_PI;
  return location;
}

geohex_polygon_t geohex_get_hex_polygon (const geohex_t *geohex) {
  const double deg = _deg();

  const double h_x    = geohex->coordinate.x;
  const double h_y    = geohex->coordinate.y;
  const double h_size = geohex->size;

  const double top_lat        = geohex_coordinate2location(geohex_coordinate(h_x,              h_y + deg * h_size)).lat;
  const double center_lat     = geohex_coordinate2location(geohex_coordinate(h_x,              h_y               )).lat;
  const double bottom_lat     = geohex_coordinate2location(geohex_coordinate(h_x,              h_y - deg * h_size)).lat;
  const double left_lng       = geohex_coordinate2location(geohex_coordinate(h_x - 2 * h_size, h_y               )).lng;
  const double right_lng      = geohex_coordinate2location(geohex_coordinate(h_x + 2 * h_size, h_y               )).lng;
  const double base_left_lng  = geohex_coordinate2location(geohex_coordinate(h_x - 1 * h_size, h_y               )).lng;
  const double base_right_lng = geohex_coordinate2location(geohex_coordinate(h_x + 1 * h_size, h_y               )).lng;

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
  const double               size       = geohex_hexsize(level);
  const geohex_coordinate_t  coordinate = geohex_location2coordinate(location);

  const double deg      = _deg();
  const double lon_grid = coordinate.x;
  const double lat_grid = coordinate.y;
  const double unit_x   = 6 * size;
  const double unit_y   = 6 * size * deg;
  const double h_pos_x  = (lon_grid + lat_grid / deg) / unit_x;
  const double h_pos_y  = (lat_grid - deg * lon_grid) / unit_y;
  const double h_x_0    = floor(h_pos_x);
  const double h_y_0    = floor(h_pos_y);
  const double h_x_q    = h_pos_x - h_x_0;
  const double h_y_q    = h_pos_y - h_y_0;

  double h_x = round(h_pos_x);
  double h_y = round(h_pos_y);
  if (h_y_q > -h_x_q + 1) {
    if ((h_y_q < 2 * h_x_q) && (h_y_q > 0.5 * h_x_q)) {
      h_x = h_x_0 + 1;
      h_y = h_y_0 + 1;
    }
  } else if (h_y_q < -h_x_q + 1) {
    if ((h_y_q > (2 * h_x_q) - 1) && (h_y_q < (0.5 * h_x_q) + 0.5)) {
      h_x = h_x_0;
      h_y = h_y_0;
    }
  }

  return geohex_adjust_coordinate(geohex_coordinate(h_x, h_y), level);
}

bool geohex_is_special_area(const long area_num) {
  if (area_num < 10) {
    return area_num == 1 || area_num == 5;
  }
  else if (area_num < 1000) {
    const long digit1x = area_num % 10;
    return geohex_is_special_area(area_num / 10) && (digit1x == 0 || digit1x == 3 || digit1x == 4 || (5 < digit1x && digit1x < 10));
  }
  else {
    return geohex_is_special_area(area_num / 10);
  }
}

long geohex_adjusted_area_num(const long area_num) {
  long offset = 0;
  if (geohex_is_special_area(area_num)) {
    offset = (area_num <   1000) ?    200 :
             (area_num <  10000) ?   2000 :
             (area_num < 100000) ?  20000 :
                                   200000 ;
  }
  return area_num + offset;
}

long geohex_area_num_by_code(const char *code) {
  const size_t level           = geohex_calc_level_by_code(code);
  const long   global_area_num = _indexOfKey(code[0]) * 30 + _indexOfKey(code[1]);
  const long   area_num        = global_area_num * geohex_pow10(level) + atol(code+2);
  return global_area_num > 0 ? geohex_adjusted_area_num(area_num) : area_num;
}

static char *geohex_itoa_with_base(register char *p, const size_t len, const int base, const long n) {
  if (p == NULL || len < 2) return NULL;
  if (n < base) {
    const static char chr[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    *p++ = chr[n];
    *p   = '\0';
    return p;
  }
  else {
    return geohex_itoa_with_base(geohex_itoa_with_base(p, len - 1, base, n / base), 2, base, n % base);
  }
}

static char *_geohex_generate_hdec3(char *p, size_t len, const long n) {
  if (p == NULL || len < 3) return NULL;
  if (n < 10) {
    if (n < 3) {
      *p++ = '0';
      len--;
    }
    return geohex_itoa_with_base(p, len, 3, n);
  }
  else {
    return _geohex_generate_hdec3(_geohex_generate_hdec3(p, len - 2, n / 10), 4, n % 10);
  }
}

static char *geohex_generate_hdec3(char *p, size_t len, const long area_num, const int level) {
  // zero padding
  const int zero = level + 3 - (int)floor(log10(area_num)) - 1;
  for (int i = 0; i < zero; i += 2) {
    *p++ = '0';
    if (--len == 0) return NULL;
    *p++ = '0';
    if (--len == 0) return NULL;
  }

  return _geohex_generate_hdec3(p, len, area_num);
}

geohex_coordinate_t geohex_get_coordinate_by_code(const char *code) {
  const size_t level    = geohex_calc_level_by_code(code);
  const long   area_num = geohex_area_num_by_code(code);

  char   h_dec3_buf[GEOHEX_DEC3_BUFSIZE];
  size_t h_dec3_len = geohex_generate_hdec3(h_dec3_buf, GEOHEX_DEC3_BUFSIZE, area_num, level) - h_dec3_buf;

  char h_decx[GEOHEX_DEC3_BUFSIZE/2];
  char h_decy[GEOHEX_DEC3_BUFSIZE/2];
  for (unsigned int i = 0; i < h_dec3_len / 2; i++) {
    h_decx[i] = h_dec3_buf[i*2]   - '0';
    h_decy[i] = h_dec3_buf[i*2+1] - '0';
  }

  double h_x = 0;
  double h_y = 0;
  for (unsigned int i = 0; i <= level+2; i++) {
    const double h_pow = geohex_pow3(level + 2 - i);
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
  const double h_k    = _deg();
  const double h_size = geohex_hexsize(level);

  double h_x = coordinate.x;
  double h_y = coordinate.y;

  const double unit_x = 6 * h_size;
  const double unit_y = 6 * h_size * h_k;

  const double            h_lat = (h_k * h_x * unit_x + h_y * unit_y) / 2;
  const double            h_lon = (h_lat - h_y * unit_y) / h_k;
  const geohex_location_t z_loc = geohex_coordinate2location(geohex_coordinate(h_lon, h_lat));

  double z_loc_x = z_loc.lng;
  double z_loc_y = z_loc.lat;

  const double max_hsteps = geohex_pow3(level + 2);
  const double hsteps     = abs(h_x - h_y);
  if (hsteps == max_hsteps) {
    if (h_x > h_y) {
      double tmp = h_x;
      h_x = h_y;
      h_y = tmp;
    }
    z_loc_x = -180;
  }

  char code3_x[GEOHEX_DEC3_BUFSIZE/2];
  char code3_y[GEOHEX_DEC3_BUFSIZE/2];

  double mod_x = h_x;
  double mod_y = h_y;
  for (int i = 0; i <= level + 2; i++) {
    const double h_pow = geohex_pow3(level + 2 - i);

    if (mod_x >= ceil(h_pow / 2)) {
      code3_x[i] = 2;
      mod_x -= h_pow;
    }
    else if (mod_x <= -ceil(h_pow/2)) {
      code3_x[i] = 0;
      mod_x += h_pow;
    }
    else {
      code3_x[i] = 1;
    }

    if (mod_y >= ceil(h_pow/2)) {
      code3_y[i] = 2;
      mod_y -= h_pow;
    }
    else if (mod_y <= -ceil(h_pow/2)) {
      code3_y[i] = 0;
      mod_y += h_pow;
    }
    else {
      code3_y[i] = 1;
    }

    if (i==2 && (z_loc_x == -180 || z_loc_x >= 0)) {
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
  double x = coordinate.x;
  double y = coordinate.y;
  double max_hsteps = geohex_pow3(level + 2);
  double hsteps     = abs(x - y);
  if (hsteps == max_hsteps && x > y) {
    const double tmp = x;
    x = y;
    y = tmp;
  }
  else if (hsteps > max_hsteps) {
    const double dif   = hsteps - max_hsteps;
    const double dif_x = floor(dif / 2);
    const double dif_y = dif - dif_x;

    double edge_x;
    double edge_y;
    if (x > y) {
      edge_x = x - dif_x;
      edge_y = y + dif_y;

      const double h_coordinate = edge_x;
      edge_x = edge_y;
      edge_y = h_coordinate;

      x = edge_x + dif_x;
      y = edge_y - dif_y;
    }
    else if (y > x) {
      edge_x = x + dif_x;
      edge_y = y - dif_y;

      const double h_coordinate = edge_x;
      edge_x = edge_y;
      edge_y = h_coordinate;

      x = edge_x - dif_x;
      y = edge_y + dif_y;
    }
  }

  return geohex_coordinate(x, y);
}

