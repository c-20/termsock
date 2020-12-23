#if 0
gcc=gcc
src=hoof.c
out=hoof.e
libs=-lWS2_32
$gcc -o $out $src $libs
exit 0
#else
#define HOSTNODE   "127.0.0.1"
#define HOSTPORT   6660

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef WIN32
#define WIN32
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

// #define msg(txt)  printf(txt "\n")

#define MESSAGE(txt) puts(txt "\n"); fflush(stdout)
#define MSG(txt)     MESSAGE(txt)
#define PROMPT(txt)  puts(txt); fflush(stdout)
#define ANSWER(txt)  puts(txt); fflush(stdout)
// todo: colour markup


#define WSAreturnonfail(errnum, vernum) \
  { if (errnum != 0 || LOBYTE(vernum) != 2 || HIBYTE(vernum) != 2) \
      { MSG("WSA startup/version fail"); return 1; }          }
#define WSAreturn(msg, code) \
  { MSG(msg); WSACleanup(); return code; }
#define WSAreturn0() \
  { WSACleanup(); return 0; }

#define SOCKP    struct sockaddr *
//#ifdef WIN32
//#define SOCKIN   sockaddr_in
//#else
#define SOCKIN   struct sockaddr_in
//#endif

int main(int argc, char **argv) {
  WORD versionrequested = MAKEWORD(2, 2);
  WSADATA wsadata;
  int wsa = WSAStartup(versionrequested, &wsadata);
  WSAreturnonfail(wsa, wsadata.wVersion);
  
  int sockdesc = socket(AF_INET, SOCK_STREAM, 0);
  if (sockdesc == -1) { WSAreturn("socket fail", 2); }

  SOCKIN socke;
  socke.sin_family = AF_INET;
  //socke.sin_addr.s_addr = INADDR_ANY;
  socke.sin_addr.s_addr = inet_addr(HOSTNODE);
  socke.sin_port = htons(HOSTPORT);

  if (connect(sockdesc, (SOCKP)&socke, sizeof(socke)) < 0)
    { WSAreturn("connect fail", 3); }

  while (1) {
    char sendmsg[1000];
    PROMPT("SEND: ");
    fflush(stdout);

    scanf(" %[^\n]", sendmsg);
    if (send(sockdesc, sendmsg, strlen(sendmsg), 0) < 0)
      { WSAreturn("send fail", 4); }
    
    MSG("waiting for reply...\n");

    int readsize = -1;
    char recvmsg[2000];
    if ((readsize = recv(sockdesc, recvmsg, 2000, 0)) < 0)
      { WSAreturn("recv fail", 5); }
    else {
      recvmsg[readsize] = '\0';
      ANSWER("RECV: ");
      puts(recvmsg);
      fflush(stdout);
    }
    // reply includes linebreak
    // todo: close_sock on recv fail or send fail?
  }
  closesocket(sockdesc);
  //close(sockdesc);
  WSAreturn0();
}


#endif

