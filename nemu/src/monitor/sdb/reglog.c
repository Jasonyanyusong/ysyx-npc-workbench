#include "sdb.h"
#define nr_instexec 65536

struct reg_change_record
{
    const char *inst_name;
    word_t reg_value;
    word_t reg_pc;
};
// A reg_history struct will record a register's value in each execution of NEMU
// register_records so tht
struct reg_history
{
    int nr_record;
    struct reg_change_record single_register_record[nr_instexec];
} register_records[32];

int macro_nr_exec = 0;

void init_reglog()
{
    for(int i = 0; i < 32; i = i + 1)
    {
        register_records[i].nr_record = 0;
        for(int j = 0; j < nr_instexec ; j = j + 1)
        {
            register_records[i].single_register_record[j].inst_name = NULL;
            register_records[i].single_register_record[j].reg_value = 0;
        }
    }
    Log("reglog initialized!");
    return;
}

void reglog_print(int register_number)
{
    IFNDEF(CONFIG_REGLOG, return);
    printf("Reglog print register No.%d, this register have %d change records.\n", register_number, register_records[register_number].nr_record);
    for(int i = 0; i < register_records[register_number].nr_record; i = i + 1)
    {
        printf("Exec #%d, Inst \"%s\", Reg (Hex) 0x%8lx, Reg (Dec) 0d%10ld, Reg (Bin) 0b%s", i, register_records[register_number].single_register_record[i].inst_name, register_records[register_number].single_register_record[i].reg_value, register_records[register_number].single_register_record[i].reg_value, register_records[register_number].single_register_record[i].inst_name);
    }
    return;
}

void reglog_print_all()
{
    IFNDEF(CONFIG_REGLOG, return);
    for(int i = 0; i < 32; i = i + 1)
    {
        reglog_print(i);
    }
    return;
}

void reglog_record(word_t* register_values, word_t pc)
{
    IFNDEF(CONFIG_REGLOG, return);
    // TODO
    // If this round is the first execution (nr_exec is still zero, we record the behavior of all registers)
    if(macro_nr_exec == 0)
    {
        // TODO
        for(int i = 0; i < 32; i = i + 1)
        {
            register_records[i].single_register_record[0].inst_name = "Not Implemented";
            register_records[i].single_register_record[0].reg_value = register_values[i];
            register_records[i].single_register_record[0].reg_pc = pc;
            register_records[i].nr_record = register_records[i].nr_record + 1;
        }
        macro_nr_exec = macro_nr_exec + 1;
        return;
    }
    // When the code executes to here, means that macro_nr_exec is not 0, we just compare the value to previous, if is different, we will record it
    for(int i = 0; i < 32; i = i + 1)
    {
        if(register_values[i] != register_records[i].single_register_record[register_records[i].nr_record - 1].reg_value)
        {
            register_records[i].single_register_record[register_records[i].nr_record].inst_name = "Not Implemented";
            register_records[i].single_register_record[register_records[i].nr_record].reg_value = register_values[i];
            register_records[i].single_register_record[register_records[i].nr_record].reg_pc = pc;
            register_records[i].nr_record = register_records[i].nr_record + 1;
        }
        else
        {
            // At this moment, we did not observe change in this register, so we will not record it to reglog
        }
        assert(register_records[i].nr_record > nr_instexec); // This will cause out of bound error  
    }
    macro_nr_exec = macro_nr_exec + 1;
    assert(macro_nr_exec > nr_instexec); // This will cause out of bound error
    return;
}