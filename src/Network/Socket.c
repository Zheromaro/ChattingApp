#include "Network/Socket.h"
#include <stdio.h>
#include <limits.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

/* ---------- Basic socket ---------- */
void chatsock_init(void) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

chatsock_t chatsock_create(const char* addr, int port) {
    chatsock_t s = {0};
    s.fd = CHATSOCK_INVALID_FD;

    if (port <= 0 || port > 65535) {
        return s;
    }

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);

    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;      /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */

    const char* host = addr;
    if (!addr || strcmp(addr, "0.0.0.0") == 0) {
        hints.ai_flags = AI_PASSIVE;  /* Bind to all interfaces */
        host = NULL;
    }

    struct addrinfo* result = NULL;
    int status = getaddrinfo(host, port_str, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return s;
    }

    for (struct addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
        s.fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
#ifdef _WIN32
        if (s.fd == INVALID_SOCKET) continue;
#else
        if (s.fd == -1) continue;
#endif
        memcpy(&s.addr, rp->ai_addr, rp->ai_addrlen);
        s.addr_len = rp->ai_addrlen;
        break;
    }

    freeaddrinfo(result);
    return s;
}

int chatsock_bind(chatsock_t* s) {
    int opt = 1;
#ifdef _WIN32
    if (setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR,
                   (const char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        perror("setsockopt");
    }
#else
    if (setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
    }
#endif
    return bind(s->fd, (struct sockaddr*)&s->addr, s->addr_len);
}

int chatsock_listen(chatsock_t* s) {
    return listen(s->fd, SOMAXCONN);
}

int chatsock_connect(chatsock_t* s) {
    return connect(s->fd, (struct sockaddr*)&s->addr, s->addr_len);
}

chatsock_t chatsock_accept(chatsock_t* s) {
    chatsock_t c = {0};
    c.fd = CHATSOCK_INVALID_FD;
    c.addr_len = sizeof(c.addr);
    c.fd = accept(s->fd, (struct sockaddr*)&c.addr, &c.addr_len);
    return c;
}

int chatsock_send(chatsock_t s, const void* buff, size_t sz) {
    if (sz > INT_MAX) sz = INT_MAX;
    return (int)send(s.fd, buff, (int)sz, 0);
}

int chatsock_recv(chatsock_t s, void* buff, size_t sz) {
    if (sz > INT_MAX) sz = INT_MAX;
    return (int)recv(s.fd, buff, (int)sz, 0);
}

int chatsock_recvall(chatsock_t s, void* buff, size_t sz) {
    size_t total = 0;
    char* b = (char*)buff;
    while (total < sz) {
        int r = chatsock_recv(s, b + total, sz - total);
        if (r <= 0) return r;
        total += r;
    }
    return (int)total;
}

void chatsock_close(chatsock_t* s) {
    if (!chatsock_valid(*s)) return;
#ifdef _WIN32
    closesocket(s->fd);
#else
    close(s->fd);
#endif
    s->fd = CHATSOCK_INVALID_FD;
}

void chatsock_destroy(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

/* ---------- List ---------- */
void chatsock_list_init(chatsock_list_t* s) {
    s->list = NULL;
    s->count = 0;
}

void chatsock_list_mutex_init(chatsock_list_t* s) {
    pthread_mutex_init(&s->mutex, NULL);
}

void chatsock_list_mutex_lock(chatsock_list_t* s) {
    pthread_mutex_lock(&s->mutex);
}

void chatsock_list_mutex_unlock(chatsock_list_t* s) {
    pthread_mutex_unlock(&s->mutex);
}

void chatsock_list_addsocket(chatsock_list_t* s, chatsock_t sock, size_t uid) {
    size_t n = s->count + 1;
    chatsock_item_t* nl = realloc(s->list, n * sizeof(chatsock_item_t));
    if (!nl) return;
    memset(&nl[s->count], 0, sizeof(chatsock_item_t));
    nl[s->count].sock = sock;
    nl[s->count].uid  = uid;
    s->list = nl;
    s->count = n;
}

bool chatsock_list_find(chatsock_list_t* s, size_t uid, size_t* idx) {
    for (size_t i = 0; i < s->count; i++) {
        if (s->list[i].uid == uid) { *idx = i; return true; }
    }
    return false;
}

bool chatsock_list_remove(chatsock_list_t* s, size_t idx) {
    if (idx >= s->count) return false;
    for (size_t i = idx; i < s->count - 1; i++)
        s->list[i] = s->list[i + 1];
    s->count--;
    if (s->count == 0) {
        free(s->list); s->list = NULL;
    } else {
        chatsock_item_t* nl = realloc(s->list, s->count * sizeof(chatsock_item_t));
        if (nl) s->list = nl;
    }
    return true;
}

void chatsock_list_setdisconnected_status(chatsock_list_t* s, size_t uid, bool st) {
    size_t i; if (chatsock_list_find(s, uid, &i)) s->list[i].disconnected = st;
}

void chatsock_list_setrecv_status(chatsock_list_t* s, size_t uid, bool st) {
    size_t i; if (chatsock_list_find(s, uid, &i)) s->list[i].recv_finished = st;
}

void chatsock_list_closeall(chatsock_list_t* s) {
    for (size_t i = 0; i < s->count; i++) chatsock_close(&s->list[i].sock);
}

void chatsock_list_destroy(chatsock_list_t* s) {
    pthread_mutex_destroy(&s->mutex);
    chatsock_list_closeall(s);
    free(s->list);
    chatsock_list_init(s);
}

/* ---------- Server threads ---------- */
static void* accept_thread(void* arg) {
    chatsock_acceptarg_t* a = (chatsock_acceptarg_t*)arg;
    size_t uid = 0;
    while (1) {
        chatsock_t c = chatsock_accept(&a->sock);
        if (!chatsock_valid(c)) break;

        if (c.addr.ss_family == AF_INET) {
            struct sockaddr_in* sin = (struct sockaddr_in*)&c.addr;
            printf("Connection from %s:%d\n",
                   inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
        }

        chatsock_list_mutex_lock(a->client_list);
        chatsock_list_addsocket(a->client_list, c, uid);
        chatsock_list_start_recv(a->client_list, a->client_list->count - 1);
        chatsock_list_mutex_unlock(a->client_list);
        uid++;
    }
    free(a);
    return NULL;
}

pthread_t chatsock_list_start_accept(chatsock_acceptarg_t* arg) {
    pthread_t t;
    pthread_create(&t, NULL, accept_thread, arg);
    return t;
}

void chatsock_list_join_accept(pthread_t t) {
    pthread_join(t, NULL);
}

static void* recv_thread(void* arg) {
    chatsock_recvarg_t* r = (chatsock_recvarg_t*)arg;
    chatsock_list_t* list = r->client_list;
    size_t idx = r->client_index;
    size_t uid = list->list[idx].uid;
    chatsock_t client = list->list[idx].sock;

    size_t msg_size = 0;
    if (chatsock_recv(client, &msg_size, sizeof(size_t)) <= 0) goto fail;

    char* msg = calloc(msg_size + 1, sizeof(char));
    if (!msg) goto fail;

    if (chatsock_recvall(client, msg, msg_size) <= 0) {
        free(msg); goto fail;
    }

    printf("Received: %s\n", msg);
    chatsock_list_mutex_lock(list);
    chatsock_list_setrecv_status(list, uid, true);
    chatsock_list_mutex_unlock(list);
    free(r);
    return msg;

fail:
    chatsock_list_mutex_lock(list);
    chatsock_list_setdisconnected_status(list, uid, true);
    chatsock_close(&list->list[idx].sock);  /* close canonical fd in the list */
    chatsock_list_mutex_unlock(list);
    printf("Client %zu disconnected\n", idx);
    free(r);
    return NULL;
}

void chatsock_list_start_recv(chatsock_list_t* s, size_t idx) {
    chatsock_recvarg_t* a = malloc(sizeof(chatsock_recvarg_t));
    a->client_list = s; a->client_index = idx;
    pthread_create(&s->list[idx].recv_thread, NULL, recv_thread, a);
    s->list[idx].recv_started = true;
}

char* chatsock_list_join_recv(chatsock_list_t* s, size_t idx) {
    void* res = NULL;
    pthread_join(s->list[idx].recv_thread, &res);
    s->list[idx].recv_started = false;
    s->list[idx].recv_finished = false;
    return (char*)res;
}
