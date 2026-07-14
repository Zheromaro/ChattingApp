#include "Network/P2P.h"
#include "Network/Socket.h"
#include "Network/Protocol.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

/* ---------- Internal peer list ---------- */
typedef struct Peer {
    chatsock_t      sock;
    char*           id;
    char*           display_name;
    bool            accepted;
    bool            we_accepted;
    bool            they_accepted;
    bool            connected;
    struct ChatP2P* owner;
    pthread_t       recv_thread;
    volatile bool   recv_running;
    struct Peer*    next;
} Peer;

struct ChatP2P {
    char*           my_name;
    chatsock_t      listener;
    int             listen_port;
    pthread_t       accept_thread;
    volatile bool   running;

    pthread_mutex_t peers_mutex;
    Peer*           peers;

    pthread_mutex_t events_mutex;
    P2PEvent*       event_head;
    P2PEvent*       event_tail;
};

/* ---------- Event queue ---------- */
static void push_event(ChatP2P* p2p, P2PEventType type,
                       const char* peer_id, const char* peer_name, const char* text) {
    P2PEvent* ev = calloc(1, sizeof(P2PEvent));
    ev->type      = type;
    ev->peer_id   = peer_id   ? strdup(peer_id)   : NULL;
    ev->peer_name = peer_name ? strdup(peer_name) : NULL;
    ev->text      = text      ? strdup(text)      : NULL;

    pthread_mutex_lock(&p2p->events_mutex);
    if (p2p->event_tail) p2p->event_tail->next = ev;
    else                 p2p->event_head = ev;
    p2p->event_tail = ev;
    pthread_mutex_unlock(&p2p->events_mutex);
}

/* ---------- Peer helpers ---------- */
static Peer* find_peer(Peer* list, const char* id) {
    while (list) {
        if (strcmp(list->id, id) == 0) return list;
        list = list->next;
    }
    return NULL;
}

static void peer_destroy(Peer* p) {
    if (!p) return;
    p->recv_running = false;
    chatsock_close(&p->sock);
    pthread_join(p->recv_thread, NULL);
    free(p->id);
    free(p->display_name);
    free(p);
}

/* ---------- Send helper: uses ChatProtocol ---------- */
static bool peer_send_packet(Peer* peer, const char* json_payload) {
    if (!peer || !json_payload) return false;
    size_t len = strlen(json_payload);
    return chatsock_send(peer->sock, &len, sizeof(len)) > 0 &&
           chatsock_send(peer->sock, json_payload, len) > 0;
}

/* ---------- Per-peer receive thread ---------- */
static void* peer_recv_thread(void* arg) {
    Peer* peer = (Peer*)arg;
    ChatP2P* p2p = peer->owner;

    while (peer->recv_running) {
        size_t sz = 0;
        int r = chatsock_recv(peer->sock, &sz, sizeof(sz));
        if (r <= 0) break;
        if (sz == 0 || sz > 1024 * 1024) break;

        char* buf = calloc(sz + 1, 1);
        if (!buf) break;

        r = chatsock_recvall(peer->sock, buf, sz);
        if (r <= 0) { free(buf); break; }

        /* === DELEGATE TO ChatProtocol === */
        NetPacket* pkt = ChatProtocol_Deserialize(buf);
        free(buf);

        if (!pkt) continue;

        switch (pkt->type) {
            case PKT_FRIEND_REQ: {
                free(peer->display_name);
                peer->display_name = pkt->sender_id ? strdup(pkt->sender_id) : strdup("Unknown");
                push_event(p2p, P2P_EVT_FRIEND_REQUEST, peer->id, peer->display_name, NULL);
                break;
            }
            case PKT_FRIEND_ACC: {
                free(peer->display_name);
                peer->display_name = pkt->sender_id ? strdup(pkt->sender_id) : strdup("Unknown");
                pthread_mutex_lock(&p2p->peers_mutex);
                peer->they_accepted = true;
                peer->accepted = true;
                pthread_mutex_unlock(&p2p->peers_mutex);
                push_event(p2p, P2P_EVT_FRIEND_ACCEPT, peer->id, peer->display_name, NULL);
                break;
            }
            case PKT_MESSAGE: {
                if (pkt->text) {
                    push_event(p2p, P2P_EVT_MESSAGE, peer->id, peer->display_name, pkt->text);
                }
                break;
            }
            default: break;
        }
        ChatProtocol_FreePacket(pkt);
    }

    peer->connected = false;
    chatsock_close(&peer->sock);
    push_event(p2p, P2P_EVT_DISCONNECTED, peer->id, peer->display_name, NULL);
    return NULL;
}

/* ---------- Accept thread ---------- */
static void* accept_thread(void* arg) {
    ChatP2P* p2p = arg;
    while (p2p->running) {
        chatsock_t c = chatsock_accept(&p2p->listener);
        if (!chatsock_valid(c)) break;

        char id[64];
        struct sockaddr_in* peer_addr = (struct sockaddr_in*)&c.addr;
                snprintf(id, sizeof(id), "%s:%d",
                         inet_ntoa(peer_addr->sin_addr), ntohs(peer_addr->sin_port));

        Peer* peer = calloc(1, sizeof(Peer));
        peer->sock      = c;
        peer->id        = strdup(id);
        peer->connected = true;
        peer->owner     = p2p;
        peer->recv_running = true;

        pthread_mutex_lock(&p2p->peers_mutex);
        peer->next = p2p->peers;
        p2p->peers = peer;
        pthread_mutex_unlock(&p2p->peers_mutex);

        pthread_create(&peer->recv_thread, NULL, peer_recv_thread, peer);
        printf("[P2P] Accepted connection from %s\n", id);
    }
    return NULL;
}

/* ---------- Public API ---------- */
ChatP2P* P2P_Create(const char* my_name, int preferred_port, int* actual_port) {
    ChatP2P* p2p = calloc(1, sizeof(ChatP2P));
    if (!p2p) return NULL;

    p2p->my_name = strdup(my_name);
    p2p->running = true;
    pthread_mutex_init(&p2p->peers_mutex, NULL);
    pthread_mutex_init(&p2p->events_mutex, NULL);
    chatsock_init();

    for (int i = 0; i < 100; i++) {
        p2p->listener = chatsock_create("0.0.0.0", preferred_port + i);
        if (chatsock_bind(&p2p->listener) == 0 && chatsock_listen(&p2p->listener) == 0) {
            p2p->listen_port = preferred_port + i;
            if (actual_port) *actual_port = p2p->listen_port;
            printf("[P2P] Listening on port %d\n", p2p->listen_port);
            break;
        }
        chatsock_close(&p2p->listener);
    }

    if (p2p->listen_port == 0) {
        free(p2p->my_name); free(p2p); return NULL;
    }

    pthread_create(&p2p->accept_thread, NULL, accept_thread, p2p);
    return p2p;
}

void P2P_Destroy(ChatP2P* p2p) {
    if (!p2p) return;
    p2p->running = false;

    /* Unblock accept_thread */
    chatsock_close(&p2p->listener);
    pthread_join(p2p->accept_thread, NULL);

    /* Steal list under lock, then destroy outside the lock */
    pthread_mutex_lock(&p2p->peers_mutex);
    Peer* to_kill = p2p->peers;
    p2p->peers = NULL;
    pthread_mutex_unlock(&p2p->peers_mutex);

    while (to_kill) {
        Peer* next = to_kill->next;   // save next BEFORE free
        peer_destroy(to_kill);
        to_kill = next;
    }

    /* Free events */
    P2PEvent* e = p2p->event_head;
    while (e) {
        P2PEvent* n = e->next;
        P2P_FreeEvent(e);
        e = n;
    }

    pthread_mutex_destroy(&p2p->peers_mutex);
    pthread_mutex_destroy(&p2p->events_mutex);
    free(p2p->my_name);
    chatsock_destroy();
    free(p2p);
}

bool P2P_ConnectToPeer(ChatP2P* p2p, const char* addr, int port) {
    if (!p2p) return false;
    chatsock_t s = chatsock_create(addr, port);
    if (chatsock_connect(&s) != 0) { chatsock_close(&s); return false; }

    char id[64];
    snprintf(id, sizeof(id), "%s:%d", addr, port);

    Peer* peer = calloc(1, sizeof(Peer));
    peer->sock = s; peer->id = strdup(id);
    peer->connected = true; peer->owner = p2p; peer->recv_running = true;

    pthread_mutex_lock(&p2p->peers_mutex);
    peer->next = p2p->peers; p2p->peers = peer;
    pthread_mutex_unlock(&p2p->peers_mutex);

    /* === USE ChatProtocol === */
    char* payload = ChatProtocol_SerializeFriendReq(p2p->my_name);
    bool ok = peer_send_packet(peer, payload);
    free(payload);

    pthread_create(&peer->recv_thread, NULL, peer_recv_thread, peer);
    printf("[P2P] Dialed %s, friend request sent\n", id);
    return ok;
}

bool P2P_AcceptFriend(ChatP2P* p2p, const char* peer_id) {
    if (!p2p || !peer_id) return false;
    pthread_mutex_lock(&p2p->peers_mutex);
    Peer* peer = find_peer(p2p->peers, peer_id);
    if (!peer) { pthread_mutex_unlock(&p2p->peers_mutex); return false; }

    peer->we_accepted = true;
    peer->accepted = true;

    /* === USE ChatProtocol === */
    char* payload = ChatProtocol_SerializeFriendAcc(p2p->my_name);
    bool ok = peer_send_packet(peer, payload);
    free(payload);

    pthread_mutex_unlock(&p2p->peers_mutex);
    printf("[P2P] Accepted friend %s\n", peer_id);
    return ok;
}

bool P2P_RemovePeer(ChatP2P* p2p, const char* peer_id) {
    if (!p2p || !peer_id) return false;
    pthread_mutex_lock(&p2p->peers_mutex);
    Peer* prev = NULL, *cur = p2p->peers;
    while (cur) {
        if (strcmp(cur->id, peer_id) == 0) {
            if (prev) prev->next = cur->next;
            else      p2p->peers = cur->next;
            peer_destroy(cur);
            break;
        }
        prev = cur; cur = cur->next;
    }
    pthread_mutex_unlock(&p2p->peers_mutex);
    return true;
}

bool P2P_SendMessage(ChatP2P* p2p, const char* peer_id, const char* text) {
    if (!p2p || !peer_id || !text) return false;

    pthread_mutex_lock(&p2p->peers_mutex);
    Peer* peer = find_peer(p2p->peers, peer_id);
    if (!peer || !peer->accepted || !peer->connected) {
        pthread_mutex_unlock(&p2p->peers_mutex);
        return false;
    }

    /* === USE ChatProtocol === */
    char* payload = ChatProtocol_SerializeText(text);
    bool ok = peer_send_packet(peer, payload);
    free(payload);

    pthread_mutex_unlock(&p2p->peers_mutex);
    return ok;
}

bool P2P_PollEvent(ChatP2P* p2p, P2PEvent** out) {
    if (!p2p || !out) return false;
    pthread_mutex_lock(&p2p->events_mutex);
    P2PEvent* ev = p2p->event_head;
    if (ev) {
        p2p->event_head = ev->next;
        if (!p2p->event_head) p2p->event_tail = NULL;
        ev->next = NULL;
        *out = ev;
    }
    pthread_mutex_unlock(&p2p->events_mutex);
    return ev != NULL;
}

void P2P_FreeEvent(P2PEvent* ev) {
    if (!ev) return;
    free(ev->peer_id); free(ev->peer_name); free(ev->text); free(ev);
}

int P2P_GetListenPort(const ChatP2P* p2p) {
    return p2p ? p2p->listen_port : 0;
}
