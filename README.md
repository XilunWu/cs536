# cs536
gcc -o clg clg.c -lpthread
export PATH=$PATH:`pwd`
clg xinu00.cs.purdue.edu 65000 1.1 20
gcc -o myhttpd myhttpd.c -lpthread
myhttpd 1.1 65000 20
