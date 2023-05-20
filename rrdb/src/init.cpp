#include <stdio.h>
#include <assert.h>
#include "rrdb.h"
void init_img(char* img_addr){
    // TODO: accoring to the image given
    FILE *fp = fopen(img_addr, "rb");
    assert(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    printf("Image size = %ld", size);
    fseek(fp, 0, SEEK_SET);
    int ret = fread(guest_to_host(mem_start), size, 1, fp);
    assert(ret == 1);
    fclose(fp);
    return;
}