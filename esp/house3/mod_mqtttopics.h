#ifndef MOD_TOPICS_H
#define MOD_TOPICS_H

// Haupttopic-Basis
#define TOPIC_BASE "resort/house3"

// Commands
#define TOPIC_CMD_PARTY        TOPIC_BASE "/party/cmd"
#define TOPIC_CMD_STORM        TOPIC_BASE "/storm/cmd"
#define TOPIC_CMD_SONG         TOPIC_BASE "/song/cmd"
#define TOPIC_CMD_NEXT         TOPIC_BASE "/party/next"

// Broadcasts
#define TOPIC_BC_STORM         "resort/broadcast/storm"
#define TOPIC_BC_PARTY         "resort/broadcast/party"

// Status
#define TOPIC_CURRENT_SONG     TOPIC_BASE "/song/current"
#define TOPIC_STATUS_STORM     TOPIC_BASE "/storm/status"
#define TOPIC_STATUS_HOUSE3    TOPIC_BASE "/party/status"

#endif
