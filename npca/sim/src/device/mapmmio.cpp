/***************************************************************************************
* Copyright (c) 2023 Yusong Yan, Beijing No.101 High School
* Copyright (c) 2023 Yusong Yan, University of Washington - Seattle
*
* NSIM is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include<device.h>

static bool device_map_inside(IOMap *map, uint64_t addr){
    //printf("At pc = 0x%8lx, addr: 0x%8x, Map \"%s\" [0x%8x,0x%8x]\n", cpu.pc, addr, map -> name, map -> low, map -> high);
    return (addr >= map->low && addr <= map->high);
}

int device_find_mapID_by_addr(IOMap *maps, int size, uint64_t addr){
    int i;
    //printf("[find_mapid_by_addr] paddr is 0x%8x\n", addr);
    for(i = 0; i < size; i = i + 1){
        if(device_map_inside(maps + i, addr)){
            diff_skip_ref_exec = true;
            //difftest_skip_ref(); //As we use NEMU to difftest, the behavior of decices are same with NSIM
            return i;
        }
    }
    // should not reach here!
    assert(0);
    return -1;
}

static IOMap* device_mmio_fetch_mmio_map(uint64_t addr){
    //printf("[fetch_mmio_map] paddr is 0x%lx\n", addr);
    //printf("device_nr_map = %d\n", device_nr_map);
    int mapid = device_find_mapID_by_addr(device_maps, device_nr_map, addr);
    //printf("[device-mmio-fetch-mmio-map]\n");
    //printf("[fetch_mmio_map] mapid = %d\n", mapid);
    return (mapid == -1 ? NULL : &device_maps[mapid]);
}

void device_report_mmio_overlap(const char* name1, uint64_t left1, uint64_t right1, const char* name2, uint64_t left2, uint64_t right2){
    printf("[device] error: MMIO region \"%s\"@[0x%lx,0x%lx] is overlapped with \"%s\"@[0x%lx,0x%lx]\n", name1, left1, right1, name2, left2, right2);
    // should not reach here!
    assert(0);
    return;
}

void device_add_mmio_map(const char *name, uint64_t addr, void *space, uint32_t len, io_callback_t callback){
    assert(device_nr_map < DEVICE_NR_MAP); // so that we have space to add another device
    uint64_t left = addr;
    uint64_t right = addr + len - 1;
    //printf("[device] name \"%s\", left 0x%lx, right 0x%lx check before add to mmio map\n", name, left, right);
    if(mem_addr_in_bound(left) || mem_addr_in_bound(right)){
        // should not reach here!
        device_report_mmio_overlap(name, left, right, "pmem", mem_start_addr, mem_end_addr);
        assert(0);
        return;
    }
    for(int i = 0; i < device_nr_map; i = i + 1){
        if(left <= device_maps[i].high && right >= device_maps[i].low){
            // should not reach here!
            device_report_mmio_overlap(name, left, right, device_maps[i].name, device_maps[i].low, device_maps[i].high);
            assert(0);
            return;
        }
    }
    //printf("[device] name \"%s\", left 0x%lx, right 0x%lx check passed, adding to mmio map\n", name, left, right);
    device_maps[device_nr_map] = (IOMap){ .name = name, .low = addr, .high = addr + len - 1, .space = space, .callback = callback};
    device_nr_map = device_nr_map + 1;
    printf("[device] name \"%s\", left 0x%lx, right 0x%lx, added to mmio map\n", name, left, right);
    return;
}

void device_mmio_check_bound(IOMap *map, uint64_t addr){
    if(map == NULL){
        // should not reach here!
        printf("[device] error: IOMap is NULL\n");
        assert(0);
        return;
    }
    else{
        if(addr > map -> high || addr < map -> low){
            // should not reach here!
            printf("[device] error: address 0x%lx is out of bound @\"%s\"[0x%lx,0x%lx]\n", addr, map -> name, map -> low, map -> high);
            assert(0);
            return;
        }
    }
}

void device_mmio_invoke_callback(io_callback_t c, uint64_t offset, int len, bool is_write){
    if (c != NULL) { c(offset, len, is_write); }
    return;
}

uint64_t device_map_read(uint64_t addr, int len, IOMap *map){
    assert(len >= 1 && len <= 8);
    device_mmio_check_bound(map, addr);
    uint64_t offset = addr - map -> low;
    device_mmio_invoke_callback(map -> callback, offset, len, false);
    uint64_t ret = mem_host_read((uint8_t *)map -> space + offset, len);
    //printf("[device-map-read] read map \"%s\", at addr = 0x%lx, with len = %d, ret = 0x%lx\n", map -> name, addr, len, ret);
    return ret;
}

void device_map_write(uint64_t addr, int len, uint64_t data, IOMap *map){
    assert(len >= 1 && len <= 8);
    //if(strcmp(map -> name, "vmem") != 0) {printf("[device-map-write] write map \"%s\", at addr = 0x%lx, with len = %d, data = 0x%lx\n", map -> name, addr, len, data);}
    device_mmio_check_bound(map, addr);
    uint64_t offset = addr - map -> low;
    //if(strcmp(map -> name, "vmem") != 0) {printf("[device-map-write] offset = 0x%lx\n", offset);}
    mem_host_write(map -> space + offset, len, data);
    device_mmio_invoke_callback(map -> callback, offset, len, true);
    return;
}

uint64_t device_mmio_read(uint64_t addr, int len){
    if(print_debug_informations) {printf("[mmio_read] paddr is 0x%lx\n", addr);}
    return device_map_read(addr, len, device_mmio_fetch_mmio_map(addr));
}

void device_mmio_write(uint64_t addr, int len, uint64_t data){
    if(print_debug_informations) {printf("[mmio_write] paddr is 0x%lx\n", addr);}
    //if(addr == 0xa0000100 || addr == 0xa0000104){printf("[device-mmio-write] catch vgactl write, addr = 0x%lx, len = %d, data = 0x%lx\n", addr, len, data);}
    device_map_write(addr, len, data, device_mmio_fetch_mmio_map(addr));
    //if(addr == 0xa0000100 || addr == 0xa0000104) {printf("device_vga_ctl_port_base[0] = 0x%x, device_vga_ctl_port_base[1] = 0x%x\n", device_vga_ctl_port_base[0], device_vga_ctl_port_base[1]);}
    return;
}

uint8_t* device_map_new_space(int size){
    uint8_t *p = device_map_p_space;
    size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;
    device_map_p_space += size;
    assert(device_map_p_space - device_map_io_space < DEVICE_MAP_IO_SPACE_MAX);
    return p;
}
