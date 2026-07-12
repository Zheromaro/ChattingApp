#ifndef CHAT_SOCKET_H
#define CHAT_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#ifndef _WIN32_LEAN_AND_MEAN
#define _WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET chatsock_fd_t;
#define CHATSOCK_INVALID_FD INVALID_SOCKET
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int chatsock_fd_t;
#define CHATSOCK_INVALID_FD (-1)
#endif

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* ---------- Low-level socket ---------- */
typedef struct {
    chatsock_fd_t fd;
    struct sockaddr_storage addr;   /* IPv4/IPv6 agnostic */
#ifdef _WIN32
    int addr_len;
#else
    socklen_t addr_len;
#endif
} chatsock_t;

static inline bool chatsock_valid(chatsock_t s) {
#ifdef _WIN32
    return s.fd != INVALID_SOCKET;
#else
    return s.fd >= 0;
#endif
}

void        chatsock_init(void);
chatsock_t  chatsock_create(const char* addr, int port);
int         chatsock_bind(chatsock_t* sock);
int         chatsock_listen(chatsock_t* sock);
int         chatsock_connect(chatsock_t* sock);
chatsock_t  chatsock_accept(chatsock_t* sock);
int         chatsock_send(chatsock_t sock, const void* buff, size_t buffsize);
int         chatsock_recv(chatsock_t sock, void* buff, size_t buffsize);
int         chatsock_recvall(chatsock_t sock, void* buff, size_t buffsize);
void        chatsock_close(chatsock_t* sock);
void        chatsock_destroy(void);

/* ---------- Client list (server mode) ---------- */
typedef struct chatsock_item {
    size_t      uid;
    chatsock_t  sock;
    pthread_t   recv_thread;
    bool        recv_started;
    bool        recv_finished;
    bool        disconnected;
} chatsock_item_t;

typedef struct chatsock_list {
    pthread_mutex_t   mutex;
    chatsock_item_t*  list;
    size_t            count;
} chatsock_list_t;

void        chatsock_list_init(chatsock_list_t* sockets);
void        chatsock_list_mutex_init(chatsock_list_t* sockets);
void        chatsock_list_mutex_lock(chatsock_list_t* sockets);
void        chatsock_list_mutex_unlock(chatsock_list_t* sockets);
void        chatsock_list_addsocket(chatsock_list_t* sockets, chatsock_t sock, size_t uid);
bool        chatsock_list_find(chatsock_list_t* sockets, size_t uid, size_t* index);
bool        chatsock_list_remove(chatsock_list_t* sockets, size_t index);
void        chatsock_list_setdisconnected_status(chatsock_list_t* sockets, size_t uid, bool status);
void        chatsock_list_setrecv_status(chatsock_list_t* sockets, size_t uid, bool status);
void        chatsock_list_closeall(chatsock_list_t* sockets);
void        chatsock_list_destroy(chatsock_list_t* sockets);

/* Server thread helpers */
typedef struct {
    chatsock_t        sock;
    chatsock_list_t*  client_list;
} chatsock_acceptarg_t;

typedef struct {
    chatsock_list_t*  client_list;
    size_t            client_index;
} chatsock_recvarg_t;

void*       chatsock_list_accept(void* arg);
pthread_t   chatsock_list_start_accept(chatsock_acceptarg_t* arg);
void        chatsock_list_join_accept(pthread_t thread);
void*       chatsock_list_recv_message(void* arg);
void        chatsock_list_start_recv(chatsock_list_t* sockets, size_t index);
char*       chatsock_list_join_recv(chatsock_list_t* sockets, size_t index);

#ifdef __cplusplus
}
#endif

#endif
