import os
import sys

if len(sys.argv) < 3:
    print "createSymTbl <binary file> <output file>"
    sys.exit()

binaryFile = sys.argv[1]
outputFile = open(sys.argv[2], "w+")

#os.system("rm -f temp.sym; nm -g %s > temp.sym" % binaryFile)

"""
Symbols:
    A: The symbols value is abosulute and wont change with futher linking.
    B: The symbol is in the uninitialized data section (BSS).
    C: The symbol is common. (uninitialized).
    D: The symbol is in the initialized data section.
    R: The symbol is in the read only section.
    T: The symbol is in the text (code) section.
    V: The symbols is a weak object.
    W: The symbol is a weak object that was specifically designated as a weak.
"""

data = []
symTblAllSize = 0

# open the nm symbol dump.
with open(binaryFile, "r") as f:
    data = f.readlines()

# print the macros for symbols.
outputFile.write("""
#define NULL ((void*)0)
#define N_UNDF 0x00 /* Undefined */
#define N_ABS  0x02 /* Absolute */
#define N_TEXT 0x04 /* Text */
#define N_DATA 0x06 /* Data */
#define N_BSS  0x08 /* Bss */
#define N_EXT  0x01 /* External or bit (global) */
#define N_TYPE 0x1e /* Mask for all types */\n
typedef unsigned char SYM_TYPE;
typedef struct sl_node
{
    struct sl_node *next;
} SL_NODE;
typedef struct 
{
    SL_NODE nameHashNode;
    char *name;
    void *value;
    unsigned short group;
    SYM_TYPE type;
} SYMBOL;
\n""")
# Add the extern lines.
for line in data:
    if (line.split()[1] == 'T' or line.split()[1] == 'W' or line.split()[1] == 'V' ) and "x86." not in line:
        outputFile.write("extern int %s();\n" % line.split()[2])
        symTblAllSize = symTblAllSize + 1
    elif (line.split()[1] == 'D' or line.split()[1] == 'R'):
        outputFile.write("extern int %s;\n" % line.split()[2])
        symTblAllSize = symTblAllSize + 1
    elif (line.split()[1] == 'B' or line.split()[1] == 'C'):
        outputFile.write("extern int %s;\n" % line.split()[2])
        symTblAllSize = symTblAllSize + 1

# Add the array definition.
outputFile.write("""
SYMBOL symTblAll[%d] =
{
""" % symTblAllSize)

# Add the array initializers.
for line in data:
    if (line.split()[1] == 'T' or line.split()[1] == 'W' or line.split()[1] == 'V' ) and "x86." not in line:
        outputFile.write("    {NULL, \"_%s\", (char*)%s, 0, (N_TEXT | N_EXT)},\n" % (line.split()[2], line.split()[2]))
    elif (line.split()[1] == 'D' or line.split()[1] == 'R'):
        outputFile.write("    {NULL, \"_%s\", (char*)&%s, 0, (N_DATA | N_EXT)},\n" % (line.split()[2], line.split()[2]))
    elif (line.split()[1] == 'B' or line.split()[1] == 'C'):
        outputFile.write("    {NULL, \"_%s\", (char*)&%s, 0, (N_BSS | N_EXT)},\n" % (line.split()[2], line.split()[2]))
#    'A' Reqires geting the address of the absolte symbol, i dont have something to test this on
#    so its commented out. aka 'unsigned char buf[128]@0x2000;'
#    elif (line.split()[1] == 'A'):
#        print "    {NULL, \"_%s\", %s, 0, N_ABS | N_EXT},\n" % (line.split()[2], line.split()[2])
# End the array definition.
outputFile.write("};\n")

outputFile.write("int symTblAllSize = %d;\n" % symTblAllSize)
