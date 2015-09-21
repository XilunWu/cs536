# cs536
gcc -o clg clg.c -lpthread
export PATH=$PATH:`pwd`
clg xinu00.cs.purdue.edu 65000 1.1 20
gcc -o myhttpd myhttpd.c -lpthread
myhttpd 1.1 65000 20

# HTTP request
GET / HTTP/1.1
Host: 127.0.0.1:10000
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:33.0) Gecko/20100101 Firefox/33.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
