#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Core/UI.h"
#include "Controller/Chat.h"
#include "UI/SideBar.h"
#include "UI/MainPanel.h"
#include "UI/Logic/TextBox.h"
#include "UI/Widgets/TextBox.h"
#include "UI/UI_Theme.h"
#include "UI/UI_Event.h"
#include "Model/Message.h"
#include "Model/User.h"
#include "Model/Conversation.h"
#include "Model/ContactList.h"
#include "Network/P2P.h"
#include "Network/Protocol.h"

/* ---------- Static state ---------- */
static TextBox*     s_chat_tb = NULL;
static TextBox*     s_search_tb = NULL;
static User*        s_me = NULL;
static ContactList* s_contact_list = NULL;
static ChatP2P*     s_p2p = NULL;
static int          s_listen_port = 0;
static char*        s_pending_peer_id = NULL;
static bool         chatTextbox = true;
static int          previous_active_idx = 0;
static int          current_active_idx = 0;

static char s_cfg_name[64] = "zahrawi";
static int  s_cfg_port = 8080;

/* ---------- UI event callbacks ---------- */
static void SendCurrentMessage(char* text);
static void SelectSearchTexBox(void) { chatTextbox = false; }
static void SelectChatTexBox(void)   { chatTextbox = true; }

static UI_Event event = {
    .send      = SendCurrentMessage,
    .chatBox   = SelectChatTexBox,
    .searchBox = SelectSearchTexBox
};

/* ---------- Helpers ---------- */
static void AddSystemMessage(Conversation* c, const char* text)
{
    Message* m = MessageCreate("system", ConvGetID(c), text);
    if (m) ConvAddMessage(c, m);
}

static Conversation* FindConvByPeerId(const char* peer_id)
{
    for (int i = 0; i < CLGetCount(s_contact_list); i++) {
        Conversation* c = CLGetConversation(s_contact_list, i);
        if (ConvGetParticipantCount(c) > 0) {
            const User* u = ConvGetParticipant(c, 0);
            const char* addr = UserGetPeerAddr(u);
            if (addr && strcmp(addr, peer_id) == 0)
                return c;
        }
    }
    return NULL;
}

static Conversation* GetOrCreateConvForPeer(const char* peer_id, const char* name)
{
    Conversation* c = FindConvByPeerId(peer_id);
    if (c) return c;

    c = ConvCreate();
    User* u = UserCreate(name);
    UserSetPeerAddr(u, peer_id);
    ConvAddParticipant(c, u);
    CLAddConversation(s_contact_list, c);
    return c;
}

/* ---------- Config ---------- */
void ChatSetConfig(const char* username, int preferred_port)
{
    if (username) {
        strncpy(s_cfg_name, username, sizeof(s_cfg_name) - 1);
        s_cfg_name[sizeof(s_cfg_name) - 1] = '\0';
    }
    s_cfg_port = preferred_port;
}

/* ---------- Demo data ---------- */
static void CreateDemoConversations(void)
{
    static const char* names[] = {
        "Alex", "Sarah", "Mike", "Emma",
        "David", "Lisa", "Tom", "Jenny"
    };
    static const char* lastMessages[] = {
        "Typing dynamic layout rules...",
        "See you tomorrow!",
        "Can you review the PR?",
        "Thanks for the help!",
        "Meeting at 3pm",
        "Sent the files",
        "Sounds good to me",
        "Let me check..."
    };
    static const int unread[] = { 0, 2, 0, 5, 0, 1, 0, 3 };

    for (int i = 0; i < 8; i++) {
        Conversation* conv = ConvCreate();
        if (!conv) continue;

        User* contact = UserCreate(names[i]);
        if (contact) {
            ConvAddParticipant(conv, contact);
        }

        Message* msg = MessageCreate(UserGetID(contact), ConvGetID(conv), lastMessages[i]);
        if (msg) {
            ConvAddMessage(conv, msg);
        }

        ConvSetUnreadCount(conv, (size_t)unread[i]);
        CLAddConversation(s_contact_list, conv);
    }
}

/* ---------- Lifecycle ---------- */
void ChatEnter(void)
{
    s_chat_tb = TBCreate();
    s_search_tb = TBCreate();
    s_me = UserCreate(s_cfg_name);
    s_contact_list = CLCreate();

    s_p2p = P2P_Create(s_cfg_name, s_cfg_port, &s_listen_port);
    if (!s_p2p) {
        fprintf(stderr, "[Chat] P2P failed to bind\n");
    } else {
        Conversation* sys = ConvCreate();
        User* sys_u = UserCreate("System");
        ConvAddParticipant(sys, sys_u);

        char info[512];
        snprintf(info, sizeof(info),
            "P2P node '%s' listening on port %d.\n"
            "To add a friend: click search box, type host:port, press Enter.\n"
            "When a request arrives, select that chat and press F1 to accept.",
            s_cfg_name, s_listen_port);
        AddSystemMessage(sys, info);
        CLAddConversation(s_contact_list, sys);
    }

    CreateDemoConversations();
}

void ChatExit(void)
{
    P2P_Destroy(s_p2p);
    s_p2p = NULL;

    free(s_pending_peer_id);
    s_pending_peer_id = NULL;

    TBDestroy(s_chat_tb);
    TBDestroy(s_search_tb);
    s_chat_tb = NULL;
    s_search_tb = NULL;

    UserDestroy(s_me);
    s_me = NULL;

    CLDestroy(s_contact_list);
    s_contact_list = NULL;
}

/* ---------- Input ---------- */
void ChatInput(SDL_Event* event)
{
    if (!event) return;

    if (chatTextbox) {
        TBHandleEvent(s_chat_tb, event);
    } else {
        TBHandleEvent(s_search_tb, event);
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER: {
                if (chatTextbox) {
                    SendCurrentMessage(TBTakeText(s_chat_tb));
                } else {
                    char* text = TBTakeText(s_search_tb);
                    if (text) {
                        char* colon = strrchr(text, ':');
                        if (colon) {
                            *colon = '\0';
                            int port = atoi(colon + 1);
                            if (port > 0 && s_p2p) {
                                if (!P2P_ConnectToPeer(s_p2p, text, port)) {
                                    fprintf(stderr, "[Chat] Connect failed\n");
                                }
                            }
                        }
                        free(text);
                    }
                }
                break;
            }
            case SDLK_F1: {
                if (s_pending_peer_id && s_p2p) {
                    P2P_AcceptFriend(s_p2p, s_pending_peer_id);

                    Conversation* c = FindConvByPeerId(s_pending_peer_id);
                    if (c) AddSystemMessage(c, "You accepted the friend request.");

                    free(s_pending_peer_id);
                    s_pending_peer_id = NULL;
                }
                break;
            }
        }
    }
}

/* ---------- Send ---------- */
static void SendCurrentMessage(char* text)
{
    if (!text || text[0] == '\0') return;

    Conversation* active = CLGetActiveConversation(s_contact_list);
    if (!active) { free(text); return; }

    /* Local echo */
    Message* msg = MessageCreate(UserGetID(s_me), ConvGetID(active), text);
    if (msg) ConvAddMessage(active, msg);

    /* Network send */
    if (s_p2p && ConvGetParticipantCount(active) > 0) {
        const User* u = ConvGetParticipant(active, 0);
        const char* peer_id = UserGetPeerAddr(u);
        if (peer_id) {
            if (!P2P_SendMessage(s_p2p, peer_id, text)) {
                AddSystemMessage(active, "Failed to send (not connected or not accepted).");
            }
        }
    }

    free(text);
}

/* ---------- Update ---------- */
void ChatUpdate(float delta_time)
{
    TBUpdate(s_chat_tb, delta_time);
    TBUpdate(s_search_tb, delta_time);

    /* ---- Drain P2P events (main thread only) ---- */
    if (s_p2p) {
        P2PEvent* ev = NULL;
        while (P2P_PollEvent(s_p2p, &ev)) {
            switch (ev->type) {
                case P2P_EVT_FRIEND_REQUEST: {
                    Conversation* c = GetOrCreateConvForPeer(ev->peer_id, ev->peer_name);
                    ConvIncrementUnreadCount(c);

                    free(s_pending_peer_id);
                    s_pending_peer_id = strdup(ev->peer_id);

                    char buf[256];
                    snprintf(buf, sizeof(buf),
                             "Friend request from %s. Press F1 to accept.", ev->peer_name);
                    AddSystemMessage(c, buf);
                    break;
                }
                case P2P_EVT_FRIEND_ACCEPT: {
                    Conversation* c = GetOrCreateConvForPeer(ev->peer_id, ev->peer_name);
                    AddSystemMessage(c, "Friend request accepted! You can now chat.");
                    break;
                }
                case P2P_EVT_MESSAGE: {
                    Conversation* c = GetOrCreateConvForPeer(ev->peer_id, ev->peer_name);
                    Message* m = MessageCreate(ev->peer_name, ConvGetID(c), ev->text);
                    if (m) {
                        ConvAddMessage(c, m);
                        if (c != CLGetActiveConversation(s_contact_list)) {
                            ConvIncrementUnreadCount(c);
                        }
                    }
                    break;
                }
                case P2P_EVT_DISCONNECTED: {
                    Conversation* c = FindConvByPeerId(ev->peer_id);
                    if (c) AddSystemMessage(c, "Peer disconnected.");
                    break;
                }
                default: break;
            }
            P2P_FreeEvent(ev);
        }
    }
    /* --------------------------------------------- */

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        },
        .backgroundColor = C_BG_GREY
    }) {
        previous_active_idx = current_active_idx;
        current_active_idx = CLGetActiveIndex(s_contact_list);

        SideBar(s_contact_list, s_search_tb, &event);

        if (current_active_idx != previous_active_idx) {
            CLSetActiveIndex(s_contact_list, current_active_idx);

            Conversation* new_active = CLGetActiveConversation(s_contact_list);
            if (new_active) {
                ConvSetUnreadCount(new_active, 0);
            }
        }

        Conversation* active_conv = CLGetActiveConversation(s_contact_list);
        if (active_conv) {
            MainPanel(s_chat_tb, active_conv, s_me, &event);
        }
    }

    const Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);
    UI_Layout(renderCommands);
}
