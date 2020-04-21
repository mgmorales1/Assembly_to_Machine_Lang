/* Maria Morales
 * Project shows functions for encoding from binary pattern to
 * ARM mnemonic. Is based on the real ARM ISA. It only covers
 * a subset of the ISA, and it's been simplified for this project
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//for branching instruction:
void branchInstruction(uint instr) {
    //Look at 4 condition bits
    //Use these to determine which type of branching
    uint condition = instr & 0xf0000000;
    // Use lower 24 bits to formulate target address
    uint offset = instr & 0x00ffffff;
    if (condition == 0x0) {
        printf("beq <PC + 0x%x>\n", offset);
    } else if (condition == 0x10000000) {
        printf("bne <PC + 0x%x>\n", offset);
    } else if (condition == 0xa0000000) {
        printf("bge <PC + 0x%x>\n", offset);
    } else if (condition == 0xb0000000) {
        printf("blt <PC + 0x%x>\n", offset);
    } else if (condition == 0xc0000000) {
        printf("bgt <PC + 0x%x>\n", offset);
    } else if (condition == 0xd0000000) {
        printf("ble <PC + 0x%x>\n", offset);
    } else if (condition == 0xe0000000) {
        printf("b <PC + 0x%x>\n", offset);
    } else {
        printf("Incorrect condition\n");
    }
}

void dataProcessingInstruction(uint instr) {
    //Isolate the 4 bit opcode
    uint opcode = instr & 0x01e00000;
    // Extract and decode Op1, Op2, and Destination
    uint operand1 = instr & 0x000f0000;
    operand1 = operand1 >> 16;

    //Extract immediate bit.
    uint operand2 = instr & 0x02000000;
    uint operand2_s;

    uint destR = instr & 0x0000f000;
    destR = destR >> 12;

    //Decode Op2 based on either immediate or not interpretation
    if (operand2 == 0x0) { // register
        // isolate last four bits
        operand2_s = instr & 0x0000000f;
    } else if (operand2 == 0x02000000) { // immediate value
        // isolate last 8 bits
        operand2_s = instr & 0x000000ff;
    } else {
        printf("something wrong with operand2");
    }
    // Use opcode chart to determine which instruction
    // Format assembly code string based on opcode and then the needed pieces of op1, op2, and dest.
    if (opcode == 0x00400000) { // sub
        if (operand2 == 0x0) { // register
            printf("sub r%d, r%d, r%d\n", destR, operand1, operand2_s);
        } else { // immediate value
            printf("sub r%d, r%d, #%d\n", destR, operand1, operand2_s);
        }
    } else if (opcode == 0x00800000) { // add
        if (operand2 == 0x0) { // register
            printf("add r%d, r%d, r%d\n", destR, operand1, operand2_s);
        } else { // immediate value
            printf("add r%d, r%d, #%d\n", destR, operand1, operand2_s);
        }
    } else if (opcode == 0x01400000) { // cmp
        if (operand2 == 0x0) { // register
            printf("cmp r%d, r%d\n", operand1, operand2_s);
        } else { // immediate value
            printf("cmp r%d, #%d\n", operand1, operand2_s);
        }
    } else if (opcode == 0x01a00000) {// mov
        if (operand2 == 0x0) { // register
            printf("mov r%d, r%d\n", destR, operand2_s);
        } else { // immediate value
            printf("mov r%d, #%d\n", destR, operand2_s);
        }
    } else {
        printf("Incorrect opcode\n");
    }
}

//for loads and stores
void dataTransferInstruction (uint instr) {
    // Extract Store bit: 0 for stores, 1 for loads
    uint type = instr & 0x00100000;
    // Extract next 4 bits: base register
    uint baseR = instr & 0x000f0000;
    baseR = baseR >> 16;
    //Extract next 4 bits: destination register
    uint destR = instr & 0x0000f000;
    destR = destR >> 12;

    uint offset = instr & 0x02000000;
    uint offset_s;

    // Extract immediate bit - remember this is backwards!
    if (offset == 0x0) { // immediate value
        // isolate last 12 bits to get value
        offset_s = instr & 0x00000fff;
    } else if (offset == 0x02000000) { // register
        // isolate last four bits
        offset_s = instr & 0x0000000f;
    } else {
        printf("something wrong with offset");
    }

    if (type == 0x00100000) { // load instr
        if (offset == 0x0 && offset_s != 0x0) {
            // immediate value not zero, list
            printf("ldr r%d, [r%d, #%d]\n", destR, baseR, offset_s);
        } else if (offset == 0x0 && offset_s == 0x0) {
            //immediate value is zero, don't list
            printf ("ldr r%d, [r%d]\n", destR, baseR);
        } else if (offset == 0x02000000) { // register
            printf("ldr r%d, [r%d, r%d]\n", destR, baseR, offset_s);
        } else {
            printf("Incorrect offset for ldr\n");
        }
    } else if (type == 0x0) {
        if (offset == 0x0 && offset_s != 0x0) {
            // immediate value not zero, list
            printf("str r%d, [r%d, #%d]\n", destR, baseR, offset_s);
        } else if (offset == 0x0 && offset_s == 0x0) {
            //immediate value is zero, don't list
            printf ("str r%d, [r%d]\n", destR, baseR);
        } else if (offset == 0x02000000) { // register
            printf("str r%d, [r%d, r%d]\n", destR, baseR, offset_s);
        } else {
            printf("Incorrect offset for str\n");
        }
    } else {
        printf("Incorrect type\n");
    }
}

int main(int argc, char* argv[]) {
  //check number of command line arguments

  //try to open file from argument
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Incorrect usage\nCorrect usage: decode input_file.txt\n");
        return 1;
    }
    //determine category of instruction
    uint instr;
    uint andresult;
    int shifted;
    char * hex;
    // to save lines that will be read
    char line [100];
    //read lines from file
    while (fgets(line, sizeof(line), fp) != NULL) {
        // line is empty, read next line
        if (line[0] == '\n'){
            continue;
        } else {
            //if line isn't empty (only contains \n char)
            //extract hex value in line (remove comments)
             hex = strtok(line, " ");
             //and store hex in a uint
             sscanf(hex, "%x", &instr);
             // isolate bits 26 and 27
             andresult = instr & 0x0c000000;
             // shift to far right end
             shifted = andresult >> 26;
        }
        // determine category of instruction
        if (shifted == 2) {
            branchInstruction(instr);
        } else if (shifted == 0) {
            dataProcessingInstruction(instr);
        } else if (shifted == 1) {
            dataTransferInstruction(instr);
        } else {
            printf("Undefined instruction\n");
        }
    }

  return 1;

}
