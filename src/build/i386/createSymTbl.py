import os
data = []
count = 0

with open("rtos.nmdump", "r") as f:
    data = f.readlines()

root_dir = "../../../include"
file_set = set()

for dir_,_, files in os.walk(root_dir):
    for file_name in files:
        rel_dir = os.path.relpath(dir_, root_dir)
        rel_file = os.path.join(rel_dir, file_name)
        file_set.add(rel_file)

for f in file_set:
    if f[0] == '.' and f[1] == '/':
        f = f[2:]
    print "#include <%s>" % f


#for line in data:
#    if line.split()[1] == 'T':
#        print "extern int %s();" % line.split()[2]

print "SYMBOL symTableAll[] = {"
for line in data:
    if line.split()[1] == 'T':
        count = count + 1
        print "    {NULL, \"_%s\", %s, 0, N_TEXT | N_EXT}," % (line.split()[2], line.split()[2])
#        print line
#symTableAdd(sysSymTable, &symTableDemo[i]);
print "};"
print "int symTableAllCount = %d;" % count

