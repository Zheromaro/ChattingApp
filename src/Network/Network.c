#include "Network/Network.h"
#include "Network/Socket.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

typedef struct NetMessage {
    char* text;
    struct NetMessage* next;
} NetMessage;

struct ChatNetwork {
    chatsock_t      sock;
    pthread_t       recv_thread;
    atomic_bool     running;
    atomic_bool     connected;
    bool            recv_thread_created;  /* ensures we always join */

    pthread_mutex_t queue_mutex;
    NetMessage*     queue_head;
    NetMessage*     queue_tail;
};

static void* recv_loop(void* arg) {
    ChatNetwork* cn = arg;
    while (atomic_load(&cn->running) && atomic_load(&cn->connected)) {
        size_t sz = 0;
        int r = chatsock_recv(cn->sock, &sz, sizeof(sz));
        if (r <= 0) { atomic_store(&cn->connected, false); break; }

        if (sz == 0 || sz > 1024 * 1024) { atomic_store(&cn->connected, false); break; }

        char* buf = calloc(sz + 1, sizeof(char));
        if (!buf) { atomic_store(&cn->connected, false); break; }

        r = chatsock_recvall(cn->sock, buf, sz);
        if (r <= 0) { free(buf); atomic_store(&cn->connected, false); break; }

        NetMessage* node = malloc(sizeof(NetMessage));
        node->text = buf;
        node->next = NULL;

        pthread_mutex_lock(&cn->queue_mutex);
        if (cn->queue_tail) cn->queue_tail->next = node;
        else                cn->queue_head = node;
        cn->queue_tail = node;
        pthread_mutex_unlock(&cn->queue_mutex);
    }
    return NULL;
}

ChatNetwork* CN_Create(const char* addr, int port) {
    if (port <= 0 || port > 65535) return NULL;

    ChatNetwork* cn = calloc(1, sizeof(ChatNetwork));
    if (!cn) return NULL;
    chatsock_init();
    cn->sock = chatsock_create(addr, port);
    if (!chatsock_valid(cn->sock)) {
        free(cn);
        return NULL;
    }
    pthread_mutex_init(&cn->queue_mutex, NULL);
    atomic_store(&cn->running, true);
    atomic_store(&cn->connected, false);
    return cn;
}

void CN_Destroy(ChatNetwork* cn) {
    if (!cn) return;
    atomic_store(&cn->running, false);

    /* If still connected, close socket to unblock recv thread */
    if (atomic_load(&cn->connected)) {
        chatsock_close(&cn->sock);
    }

    /* Always join if the thread was ever created */
    if (cn->recv_thread_created) {
        pthread_join(cn->recv_thread, NULL);
    }

    pthread_mutex_lock(&cn->queue_mutex);
    NetMessage* n = cn->queue_head;
    while (n) {
        NetMessage* next = n->next;
        free(n->text); free(n);
        n = next;
    }
    pthread_mutex_unlock(&cn->queue_mutex);

    pthread_mutex_destroy(&cn->queue_mutex);
    chatsock_close(&cn->sock);
    chatsock_destroy();
    free(cn);
}

bool CN_Connect(ChatNetwork* cn) {
    if (!cn || atomic_load(&cn->connected)) return false;
    if (chatsock_connect(&cn->sock) != 0) return false;
    atomic_store(&cn->connected, true);
    atomic_store(&cn->running, true);
    pthread_create(&cn->recv_thread, NULL, recv_loop, cn);
    cn->recv_thread_created = true;
    return true;
}

void CN_Disconnect(ChatNetwork* cn) {
    if (!cn || !atomic_load(&cn->connected)) return;
    atomic_store(&cn->running, false);
    atomic_store(&cn->connected, false);
    chatsock_close(&cn->sock);
    if (cn->recv_thread_created) {
        pthread_join(cn->recv_thread, NULL);
        cn->recv_thread_created = false;
    }
}

bool CN_IsConnected(const ChatNetwork* cn) {
    return cn && atomic_load(&cn->connected);
}

bool CN_SendMessage(ChatNetwork* cn, const char* text) {
    if (!cn || !atomic_load(&cn->connected) || !text) return false;
    size_t len = strlen(text);
    if (len == 0) return false;

    if (chatsock_send(cn->sock, &len, sizeof(len)) <= 0) goto fail;
    if (chatsock_send(cn->sock, text, len) <= 0) goto fail;
    return true;

fail:
    atomic_store(&cn->connected, false);
    return false;
}

bool CN_PollReceive(ChatNetwork* cn, char** out_text) {
    if (!cn || !out_text) return false;
    pthread_mutex_lock(&cn->queue_mutex);
    NetMessage* node = cn->queue_head;
    if (node) {
        cn->queue_head = node->next;
        if (!cn->queue_head) cn->queue_tail = NULL;
        *out_text = node->text;
        free(node);
    }
    pthread_mutex_unlock(&cn->queue_mutex);
    return node != NULL;
}
