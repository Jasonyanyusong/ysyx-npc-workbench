#include <stdio.h>
#include "fixedptc.h"

fixedpt fixedptTestPositiveRealFrac = fixedpt_rconst(0.7);
fixedpt fixedptTestNegativeRealFrac = fixedpt_rconst(-0.7);
fixedpt fixedptTestPositiveFakeFrac = fixedpt_rconst(1.7);
fixedpt fixedptTestNegativeFakeFrac = fixedpt_rconst(-1.7);

void test_fixedpt(fixedpt fixedptTest);

int main(){
    printf("[fixedptc-test] running float test\n");

    printf("[fixedptc-test] test I: Zero-Test\n");
    test_fixedpt(fixedpt_rconst(0));

    printf("[fixedptc-test] test II: Positive-Integer-Test\n");
    test_fixedpt(fixedpt_rconst(10));

    printf("[fixedptc-test] test III: Negative-Integer-Test\n");
    test_fixedpt(fixedpt_rconst(-10));

    printf("[fixedptc-test] test IV: Positive-Fraction-Test\n");
    test_fixedpt(fixedpt_rconst(10.3));

    printf("[fixedptc-test] test V: Negative-Fraction-Test\n");
    test_fixedpt(fixedpt_rconst(-10.3));

    return 0;
}

void test_fixedpt(fixedpt fixedptTest){
    char printBuffer[16] = {0};

    fixedpt_str(fixedptTest, &printBuffer, -2);
    printf("\tsection  1: display itself -> %s\n", printBuffer);
    fixedpt_str(fixedpt_abs(fixedptTest), &printBuffer, -2);
    printf("\tsection  2: abstract value -> %s\n", printBuffer);

    fixedpt_str(fixedpt_floor(fixedptTest), &printBuffer, -2);
    printf("\tsection  3: floor itself   -> %s\n", printBuffer);
    fixedpt_str(fixedpt_ceil(fixedptTest), &printBuffer, -2);
    printf("\tsection  4: ceil itself    -> %s\n", printBuffer);


    fixedpt_str(fixedpt_muli(fixedptTest, 2), &printBuffer, -2);
    printf("\tsection  5: muli int 2     -> %s\n", printBuffer);
    fixedpt_str(fixedpt_divi(fixedptTest, 2), &printBuffer, -2);
    printf("\tsection  6: divi int 2     -> %s\n", printBuffer);

    fixedpt_str(fixedpt_muli(fixedptTest, 0), &printBuffer, -2);
    printf("\tsection  7: muli int 0     -> %s\n", printBuffer);
    printf("\tsection  8: divi int 0     -> SKIPPED (UB!)\n");

    fixedpt_str(fixedpt_muli(fixedptTest, -2), &printBuffer, -2);
    printf("\tsection  9: muli int -2    -> %s\n", printBuffer);
    fixedpt_str(fixedpt_divi(fixedptTest, -2), &printBuffer, -2);
    printf("\tsection 10: divi int -2    -> %s\n", printBuffer);

    fixedpt_str(fixedpt_mul(fixedptTest, fixedptTestPositiveRealFrac), &printBuffer, -2);
    printf("\tsection 11: mul Pos_R_Fact -> %s\n", printBuffer);
    fixedpt_str(fixedpt_div(fixedptTest, fixedptTestPositiveRealFrac), &printBuffer, -2);
    printf("\tsection 12: div Pos_R_Fact -> %s\n", printBuffer);

    fixedpt_str(fixedpt_mul(fixedptTest, fixedptTestNegativeRealFrac), &printBuffer, -2);
    printf("\tsection 13: mul Neg_R_Fact -> %s\n", printBuffer);
    fixedpt_str(fixedpt_div(fixedptTest, fixedptTestNegativeRealFrac), &printBuffer, -2);
    printf("\tsection 14: div Neg_R_Fact -> %s\n", printBuffer);

    fixedpt_str(fixedpt_mul(fixedptTest, fixedptTestPositiveFakeFrac), &printBuffer, -2);
    printf("\tsection 15: mul Pos_F_Fact -> %s\n", printBuffer);
    fixedpt_str(fixedpt_div(fixedptTest, fixedptTestPositiveFakeFrac), &printBuffer, -2);
    printf("\tsection 16: div Pos_F_Fact -> %s\n", printBuffer);

    fixedpt_str(fixedpt_mul(fixedptTest, fixedptTestNegativeFakeFrac), &printBuffer, -2);
    printf("\tsection 17: mul Neg_F_Fact -> %s\n", printBuffer);
    fixedpt_str(fixedpt_div(fixedptTest, fixedptTestNegativeFakeFrac), &printBuffer, -2);
    printf("\tsection 18: div Neg_F_Fact -> %s\n", printBuffer);
}