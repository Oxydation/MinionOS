/*
 * main.c
 */

//void process(void);

int main(void) {
    volatile unsigned long i = 0;
    int baseAddress = 0;
    int base = 149 / 32;
    if (base == 0) {
        baseAddress = 0x48310000;
    } else if (base == 1) {
        baseAddress = 0x49050000;
    } else if (base == 2) {
        baseAddress = 0x49052000;
    } else if (base == 3) {
        baseAddress = 0x49054000;
    } else if (base == 4) {
        baseAddress = 0x49056000;
    } else if (base == 5) {
        baseAddress = 0x49058000;
    }
    int* out = (int*) (baseAddress + 0x003C);
    int pinPos = 149 % 32;

    while (1) {
        ((*out) &= ~(1UL << (pinPos)));
    }
}

/*
#pragma CODE_SECTION(process,".process")
void process(void) {
    volatile unsigned long i = 0;
    int baseAddress = 0;
    int base = 149 / 32;
    if (base == 0) {
        baseAddress = 0x48310000;
    } else if (base == 1) {
        baseAddress = 0x49050000;
    } else if (base == 2) {
        baseAddress = 0x49052000;
    } else if (base == 3) {
        baseAddress = 0x49054000;
    } else if (base == 4) {
        baseAddress = 0x49056000;
    } else if (base == 5) {
        baseAddress = 0x49058000;
    }
    int* out = (int*) (baseAddress + 0x003C);
    int pinPos = 149 % 32;

    while (1) {
        ((*out) &= ~(1UL << (pinPos)));
    }
}
*/
