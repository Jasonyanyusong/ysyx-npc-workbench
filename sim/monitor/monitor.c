#include "paddr.h"
#include <getopt.h>
#include "sdb.h"

//void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb(char* trace_ftrace_elf, char* trace_ftrace_diasm);
//void init_diasm(const char *triple);

static void welcome(){
    printf("Welcome to riscv64-NSIM\n");
    printf("For help, type \"help\"\n");
}

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static char *elf_file = NULL;
static char *das_file = NULL;

static long load_img(){
    if (img_size == NULL){
        printf("No image is given. Use the default build-in image.\n");
        return 4096; // built-in image size;
    }

    FILE *fp = fopen(img_file, "rb");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    printf("The image is %s, size = %ld\n", img_file, size);

    fseek(fp, 0, SEEK_SET);
    int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
    assert(ret == 1);

    fclose(fp);
    return size;
}

static int parse_args(int argc, char *argv[]){
    const struct option table[] = {
        {"batch"    , no_argument      , NULL, 'b'},
        {"log"      , required_argument, NULL, 'l'},
        {"diff"     , required_argument, NULL, 'd'},
        {"help"     , no_argument      , NULL, 'h'},
        {"readelf"  , required_argument, NULL, 'r'},
        {"readdiasm", required_argument, NULL, 'a'},
        {0          , 0                , NULL,  0 }
    };
    int o;
    while( (o = getopt_long(argc, argv, "-bhl:d:r:a:", table, NULL)) != -1){
        switch (o){
            case 'b': sdb_set_batch_mode(); break;
            case 'l': log_file     = optarg; printf("log_file = \"%s\"\n", log_file);         break;
            case 'd': diff_so_file = optarg; printf("diff_so_file = \"%s\"\n", diff_so_file); break;
            case 'r': elf_file     = optarg; printf("elf_file = \"%s\"\n", elf_file);         break;
            case 'a': das_file     = optarg; printf("das_file = \"%s\"\n", das_file);         break;
            case 1  : img_file     = optarg; printf("img_file = \"%s\"\n", img_file);         return 0;
            default:
                printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
                printf("\t-b,--batch              run with batch mode\n");
                printf("\t-l,--log=FILE           output log to FILE\n");
                printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
                printf("\t-r,--readelf=ELF_FILE   run FTRACE with ELF_FILE\n");
                printf("\t-r,--readdiasm=DAS_FILE run FTRACE with DAS_FILE\n");
                printf("\n");
                exit(0);
        }
    }
    return 0;
}

void init_monitor(int argc, char *argv[]){
    parse_args(argc, argv);
    //init_rand();
    init_log(log_file);
    init_mem();
    IFDEF(CONFIG_DEVICE, init_device());
    //init_rtl();
    long image_size = load_img();
    init_difftest(diff_so_file, img_size);
    init_sdb(char* trace_ftrace_elf, char* trace_ftrace_diasm);
    //init_diasm("riscv64", "bad", "-pc-linux-gnu");
    welcome();
}
