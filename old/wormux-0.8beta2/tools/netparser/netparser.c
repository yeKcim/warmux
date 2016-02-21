#include <stdio.h>
#include <stdlib.h>
#include <SDL_net.h>
#include <inttypes.h>

#undef   main
#define  BUF_SIZE  4096

inline int32_t readInt(const char* buf)
{
    int32_t val;
    *((uint32_t*)&val) = SDLNet_Read32(buf);
    return val;
}

inline double readDouble(const char *buf)
{
    double   val;
    uint32_t *tmp = (uint32_t*)(&val);

#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
    tmp[0] = SDLNet_Read32(buf);
    tmp[1] = SDLNet_Read32(buf);
#else
    tmp[1] = SDLNet_Read32(buf);
    tmp[0] = SDLNet_Read32(buf);
#endif

    return val;
}

char *readString(const char *in, int *len)
{
    uint32_t length = SDLNet_Read32(in);
    char     *out, *ptr;
    int      i;
    
    if (length > 1024*64 || length==0)
    {
        printf("Suspicious string length (%08X), aborting\n", length);
        len = 0;
        return NULL;
    }

    in += 4;
    *len = (length+3)&(-4);
    out = malloc(1+*len);
    ptr = out;
    for (i=0; i<*len/4; i++)
    {
        *((uint32_t*)ptr) = SDLNet_Read32(in);
        ptr += 4;
        in  += 4;
    }

    // Make sure it is NULL-terminated
    ptr[0] = 0;
    *len += 4;
    return out;
}

typedef struct
{
    const char *name;
    const char *param;
} action_t;

static void parse_action(const char *buf, int size, const char *param)
{
    char       *out, name[256] = { 0, };
    static const char *(network_states[]) =
        { "No network", "Menu OK", "Loading data", "Ready to play", "Playing" };

    //printf("Parsing %s\n", param);

    while (param[0] && size>0)
    {
        int len = 0;

        switch (param[0])
        {
        case '[':
            len = 0;
            param++;
            while (param[0] && param[0] != ']' && len<255)
                name[len++] = *(param++);
            name[len] = 0;
            len = 0;
            //printf("Got name='%s'\n", name);
            break;
        case ']':
            //printf("Attempt to resynchronize?\n");
            break;
        case 'N': // Network state
            printf("\t. %s: %s\n", name, network_states[SDLNet_Read32(buf)]);
            len = 4;
            break;
        case 'P': // Point2i
            printf("\t. %s: (%i, %i)\n", name, readInt(buf), readInt(buf));
            len = 8;
            break;
        case 'D': // Point2d
            printf("\t. %s: (%.3f, %.3f)\n", name, readDouble(buf), readDouble(buf));
            len = 16;
            break;
        case 'F': // Double using scientific representation
            printf("\t. %s: %.3f\n", name, readDouble(buf));
            len = 8;
            break;
        case 'E': // Double using scientific representation
            printf("\t. %s: %.3e\n", name, readDouble(buf));
            len = 8;
            break;
        case '?': // int conditionning whether rest is skipped
            if (!SDLNet_Read32(buf))
            {
                 if (size>0)
                 {
                      uint32_t *dw = (uint32_t*)buf;
                      printf("<<< %u bytes not consumed, first are: %08X!\n", size, dw[0]);
                 }
                 return;
            }
            break;
        case 'I': // int32_t
            printf("\t. %s: %i\n", name, readInt(buf));
            len = 4;
            break;
        case 'U': // uint32_t
            printf("\t. %s: %u\n", name, SDLNet_Read32(buf));
            len = 4;
            break;
        case 'X': // uint32_t as hexadecimal
            printf("\t. %s: %X\n", name, SDLNet_Read32(buf));
            len = 4;
            break;
        case 'S': // String
            out = readString(buf, &len);
            if (!out || len<1)
            {
                uint32_t *dw = (uint32_t*)buf;
                printf("Dwords: %08X %08X\n", dw[0], dw[1]);
                return;
            }
            printf("\t. %s: '%s'\n", name, out);
            free(out);
            break;
        default:
            printf("<<<< Unknow format %c!\n", param[0]);
            break;
        case 0:
            return;
        }

        param++;
        buf += len;
        size -= len;
    }

    if (size>0)
    {
        uint32_t *dw = (uint32_t*)buf;
        printf("<<< %u bytes not consumed, first are: %08X!\n", size, dw[0]);
    }
}

const char StoreCharacter[] =
    "[Team no.]I[Character no.]I[Position]P[Direction]I[Firing angle]F"
    "[Energy]I[Alive?]I[Disease damage]I[Disease duration]I"
    "[Speed vector]D[Strength]D[Rope angle]F[Rope length]F"
    "?[Clothe]S[Movement]S[Animation frame]I";

action_t actions[] = {
    { "ACTION_NICKNAME", "[Nickname]S" },
    { "ACTION_NETWORK_CHANGE_STATE", "[State]N" },

    // ########################################################
    { "ACTION_PLAYER_CHANGE_WEAPON", "[Weapon]S" },
    { "ACTION_PLAYER_NEXT_CHARACTER", NULL },
    { "ACTION_PLAYER_PREVIOUS_CHARACTER", NULL },
    { "ACTION_GAMELOOP_CHANGE_CHARACTER", StoreCharacter },
    { "ACTION_GAMELOOP_NEXT_TEAM", "[Next team]S" },
    { "ACTION_GAMELOOP_SET_STATE", "[State]U" },

    // ########################################################
    // To be sure that rules will be the same on each computer
    { "ACTION_RULES_ASK_VERSION", NULL },
    { "ACTION_RULES_SEND_VERSION", "[Version]S" },
    { "ACTION_RULES_SET_GAME_MODE", "[Name]S[Mode]S[Objects]S" },

    // ########################################################
    // Chat message
    { "ACTION_CHAT_MESSAGE", "[Message]S" },

    // Map selection in network menu
    { "ACTION_MENU_SET_MAP", "[Map]S" },

    // Teams selection in network menu
    { "ACTION_MENU_ADD_TEAM", "[Team]S[Nickname]S[Players]U" },
    { "ACTION_MENU_DEL_TEAM", "[Team]S" },
    { "ACTION_MENU_UPDATE_TEAM", "[Team]S[Nickname]S[Players]U" },

    // ########################################################
    // Character's move
    { "ACTION_CHARACTER_JUMP", NULL },
    { "ACTION_CHARACTER_HIGH_JUMP", NULL },
    { "ACTION_CHARACTER_BACK_JUMP", NULL },
    { "ACTION_CHARACTER_SET_PHYSICS", StoreCharacter },

    // ########################################################
    // Using Weapon
    { "ACTION_WEAPON_SHOOT", "[Strength]F[Angle]F" },
    { "ACTION_WEAPON_STOP_USE", NULL },

    // Quite standard weapon options
    { "ACTION_WEAPON_SET_TIMEOUT", "[Timeout]F" },
    { "ACTION_WEAPON_SET_TARGET", "[World position]P" },

    // Special weapon options
    { "ACTION_WEAPON_SUPERTUX", "[Visible when active]I" },
    { "ACTION_WEAPON_CONSTRUCTION", "[New angle]F" },
    { "ACTION_WEAPON_GRAPPLE", "[Attach rope]I[Contact point]P" },

    // ########################################################
    { "ACTION_NETWORK_SYNC_BEGIN", NULL },
    { "ACTION_NETWORK_SYNC_END", NULL },
    { "ACTION_EXPLOSION",
      "[Position]P[Explosion range]I[Particle range]I[Damage]I"
      "[Blast range]U[Blast force]U[Sound]S"
    },
    { "ACTION_WIND", "[Vector]I" },
    { "ACTION_NETWORK_PING", NULL },
    { "ACTION_NETWORK_RANDOM_CLEAR", NULL },
    { "ACTION_NETWORK_RANDOM_ADD", "[Added]E" },
    { "ACTION_NETWORK_CONNECT", "[Connecting to]S" },
    { "ACTION_NETWORK_DISCONNECT", "[Disconnected from]S" },

    { "UNDEF", NULL }
};
#define  UNDEF     (sizeof(actions)/sizeof(action_t))-1

void printhelp() { printf("Syntax: netparser <network dump file>\n"); exit(0); }

int main(int argc, char *argv[])
{
    int         count     = 0;
    FILE        *rec;
    int         size      = 1024;
    char        *buffer   = malloc(size);

    if (argc != 2)
    {
         fprintf(stderr, "Invalid command-line\n");
         printhelp();
    }

    if ((rec = fopen(argv[1], "rb")) == NULL)
    {
         fprintf(stderr, "Couldn't open for reading '%s'\n", argv[1]);
         printhelp();
    }


    while (!feof(rec))
    {
        uint32_t psize, type, ts, len;
        action_t *a;

        if (fread(&psize, 1, 4, rec) != 4)
            break;

        if (psize > size)
        {
            if (psize > 1024*1024)
            {
                printf("Suspicious packet size %u at %li\n", psize, ftell(rec));
                break;
            }

            free(buffer);
            buffer = malloc(psize);
        }

        // Read whole packet
        if (fread(buffer, 1, psize, rec) != psize)
        {
            printf("EOF at %li met abruptly while reading action\n", ftell(rec));
            break;
        }

        // Parse action header
        type  = SDLNet_Read32(buffer+0);
        a     = &(actions[(type>=UNDEF) ? UNDEF : type]);
        ts    = SDLNet_Read32(buffer+4);
        len   = SDLNet_Read32(buffer+8)*4;
        printf("Packet %i, %uB at %li: type=%s (%08X) ts=%u len=%u\n",
               count, psize, ftell(rec), a->name, type, ts, len);

        // Parse action body
        if (len>0 && a->param)
            parse_action(buffer+12, psize-12, a->param);

        if (len != psize-12)
        {
            printf("Corrupt packet? Action and packet size disagree: %u vs %u\n",
                   len, psize-12);
        }

        // Header check
        if (fread(&psize, 1, 4, rec) != 4)
        {
            printf("EOF at %li met abruptly while reading action header\n", ftell(rec));
            break;
        }
        if (psize != 0xFFFFFFFF)
        {
            printf("Sync lost at %li: %08X\n", ftell(rec), psize);
            break;
        }

        count++;
    }

    free(buffer);
    fclose(rec);
    return 0;
}
