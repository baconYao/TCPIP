/*
** selectserver.c -- 一個 cheezy 的多人聊天室 server
*/

// ref: 
//      http://beej-zhtw.netdpi.net/09-man-manual/9-19-select
//      http://beej-zhtw.netdpi.net/07-advanced-technology/7-2-select

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "5555" // 我們正在 listen 的 port

// 取得 sockaddr，IPv4 或 IPv6：
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master; // master file descriptor 清單
    fd_set read_fds; // 給 select() 用的暫時 file descriptor 清單
    int fdmax; // 最大的 file descriptor 數目

    int listener; // listening socket descriptor
    int newfd; // 新接受的 accept() socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256]; // 儲存 client 資料的緩衝區
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1; // 供底下的 setsockopt() 設定 SO_REUSEADDR
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master); // 清除 master 與 temp sets
    FD_ZERO(&read_fds);

    // 給我們一個 socket，並且 bind 它
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      //不論IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) 
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) 
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) 
        {
            continue;
        }

        // 避開這個錯誤訊息："address already in use"
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) 
        {
            close(listener);
            continue;
        }
        break;
    }

    // 若我們進入這個判斷式，則表示我們 bind() 失敗
    if (p == NULL) 
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }
    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) 
    {
        perror("listen");
        exit(3);
    }

    // 將 listener 新增到 master set
    FD_SET(listener, &master);

    // 持續追蹤最大的 file descriptor
    fdmax = listener; // 到此為止，就是它了

    // 主要迴圈
    for( ; ; ) 
    {
        read_fds = master; // 複製 master
        // 不用管 writefds 、 exceptfds 、 timeout
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
        {
            perror("select");
            exit(4);
        }

        // 在現存的連線中尋找需要讀取的資料
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // 我們找到一個！！
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) 
                    {
                        perror("accept");
                    } 
                    else 
                    {
                        FD_SET(newfd, &master); // 新增到 master set
                        if (newfd > fdmax) 
                        { // 持續追蹤最大的 fd
                            fdmax = newfd;
                        }
                    printf("selectserver: new connection from %s on "
                      "socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                    }

                } 
                else 
                {
                    // 處理來自 client 的資料
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) 
                    {
                        // got error or connection closed by client
                        if (nbytes == 0) 
                        {
                            // 關閉連線
                            printf("selectserver: socket %d hung up\n", i);
                        } 
                        else 
                        {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // 從 master set 中移除

                    } 
                    else 
                    {
                        // 我們從 client 收到一些資料
                        for(j = 0; j <= fdmax; j++) 
                        {
                            // 送給大家！
                            if (FD_ISSET(j, &master)) 
                            {
                                // 不用送給 listener 跟我們自己
                                if (j != listener && j != i) 
                                {
                                    if (send(j, buf, nbytes, 0) == -1) 
                                    {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for( ; ; )--and you thought it would never end!

    return 0;
}
