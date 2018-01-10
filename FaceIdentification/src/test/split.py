import random

file = open("caisa_all.txt")

lines = file.readlines()
#for line in lines:
#	print line

a = random.sample(lines,len(lines)/10)

#for line in a:
#	print line

print len(lines),len(a)

i=0
for line in a:
	print i , line
	lines.remove(line)
	i = i+1

print len(lines)

fileo1 = open("caisa_train.txt","w")
for line in lines:
	fileo1.write(line)

fileo2 = open("caisa_val.txt","w")
for line in a:
	fileo2.write(line)