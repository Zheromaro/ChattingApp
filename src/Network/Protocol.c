#include "Network/Protocol.h"
#include "Model/Message.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// TODO: replace with CJson
static char* json_obj(const char* pairs[], int count) {
    size_t len = 3; /* {} + null */
    for (int i = 0; i < count; i += 2) {
        len += strlen(pairs[i]) + strlen(pairs[i+1]) + 6; /* "k":"v", */
    }
    char* out = malloc(len);
    if (!out) return NULL;
    strcpy(out, "{");
    size_t off = 1;
    for (int i = 0; i < count; i += 2) {
        off += snprintf(out + off, len - off, "\"%s\":\"%s\"%s",
                        pairs[i], pairs[i+1], i + 2 < count ? "," : "");
    }
    strcat(out, "}");
    return out;
}

static char* escape_quotes(const char* src) {
    if (!src) return strdup("");
    size_t n = 0;
    for (const char* p = src; *p; p++) if (*p == '\"' || *p == '\\') n++;
    char* out = malloc(strlen(src) + n + 1);
    if (!out) return NULL;
    size_t j = 0;
    for (const char* p = src; *p; p++) {
        if (*p == '\"' || *p == '\\') out[j++] = '\\';
        out[j++] = *p;
    }
    out[j] = '\0';
    return out;
}

/* ---------- Serialization ---------- */
char* ChatProtocol_SerializeMessage(const struct Message* msg, const char *conversationID) {
    if (!msg) return NULL;
    const char* senderID = MessageGetAuthorID(msg);
    const char* txt = MessageGetText(msg);

    char* esc = escape_quotes(txt ? txt : "");
    const char* pairs[] = {"t","msg","s",senderID?senderID:"","c",conversationID?conversationID:"","txt",esc};
    char* out = json_obj(pairs, 8);
    free(esc);
    return out;
}

char* ChatProtocol_SerializeFriendReq(const char* my_name) {
    char* esc = escape_quotes(my_name);
    const char* pairs[] = {"t","friend_req","n",esc};
    char* out = json_obj(pairs, 4);
    free(esc);
    return out;
}

char* ChatProtocol_SerializeFriendAcc(const char* my_name) {
    char* esc = escape_quotes(my_name);
    const char* pairs[] = {"t","friend_acc","n",esc};
    char* out = json_obj(pairs, 4);
    free(esc);
    return out;
}

char* ChatProtocol_SerializeText(const char* text) {
    char* esc = escape_quotes(text);
    const char* pairs[] = {"t","msg","txt",esc};
    char* out = json_obj(pairs, 4);
    free(esc);
    return out;
}

/* ---------- Deserialization ---------- */
static char* json_extract(const char* src, const char* key) {
    char pat[32];
    snprintf(pat, sizeof(pat), "\"%s\":\"", key);
    const char* s = strstr(src, pat);
    if (!s) return NULL;
    s += strlen(pat);
    const char* e = strchr(s, '\"');
    if (!e) return NULL;
    size_t n = e - s;
    /* Handle escaped quotes */
    char* out = malloc(n + 1);
    if (!out) return NULL;
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        if (s[i] == '\\' && i + 1 < n) i++;
        out[j++] = s[i];
    }
    out[j] = '\0';
    return out;
}

PacketType ChatProtocol_ParseType(const char* data) {
    if (!data) return PKT_UNKNOWN;
    char* t = json_extract(data, "t");
    if (!t) return PKT_UNKNOWN;
    PacketType pt = PKT_UNKNOWN;
    if      (strcmp(t, "friend_req") == 0) pt = PKT_FRIEND_REQ;
    else if (strcmp(t, "friend_acc") == 0) pt = PKT_FRIEND_ACC;
    else if (strcmp(t, "msg") == 0)        pt = PKT_MESSAGE;
    else if (strcmp(t, "ping") == 0)       pt = PKT_PING;
    free(t);
    return pt;
}

NetPacket* ChatProtocol_Deserialize(const char* data) {
    if (!data) return NULL;
    NetPacket* p = calloc(1, sizeof(NetPacket));
    if (!p) return NULL;

    p->raw = strdup(data);
    p->type = ChatProtocol_ParseType(data);
    p->sender_id       = json_extract(data, "s");
    p->conversation_id = json_extract(data, "c");
    p->text            = json_extract(data, "txt");

    /* "n" field is used for name in friend_req/acc */
    if (!p->sender_id) p->sender_id = json_extract(data, "n");

    return p;
}

void ChatProtocol_FreePacket(NetPacket* pkt) {
    if (!pkt) return;
    free(pkt->sender_id);
    free(pkt->conversation_id);
    free(pkt->text);
    free(pkt->raw);
    free(pkt);
}
