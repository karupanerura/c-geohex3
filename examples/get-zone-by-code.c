#include "geohex.h"
#include <stdio.h>

int main (int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    printf("/********* geohex:%s **********/\n", argv[i]);
    geohex_t geohex = geohex_get_zone_by_code(argv[i]);
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
}
