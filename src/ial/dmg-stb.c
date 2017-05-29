/*
** $Id: dmg-stb.c 7335 2007-08-16 03:38:27Z xgwang $
**
** dmg-stb.c: IAL Engine for Intel DMG GEN10 STB.
** 
** Copyright (C) 2003 ~ 2007, Feynman Software.
**
** Author: Wei Yongming (2003/12/14)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _DMGSTB_IAL

#include <unistd.h>
#include <fcntl.h>

#include "ial.h"
#include "dmg-stb.h"

static int ir_fd = -1;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    return 0;
}

static void mouse_getxy(int *x, int *y)
{
    *x = 0; *y = 0;
}

static int mouse_getbutton(void)
{
    return 0;
}

static unsigned int cur_key;
static unsigned char state [NR_KEYS];

static unsigned char ir2kbd_map_b1 [] = 
{
    0,              // 0
    0,              // 1
    0,              // 2
    0,              // 3
    DMG_KB_BACK,    // 4
};

static unsigned char ir2kbd_map_b2 [] = 
{
    0,              // 0
    DMG_KB_UP,      // 1
    DMG_KB_DOWN,    // 2
    0,              // 3
    DMG_KB_LEFT,    // 4
    0,              // 5
    0,              // 6
    0,              // 7
    DMG_KB_RIGHT,   // 8
};

static unsigned char ir2kbd_map_b3 [] = 
{
    0,              // 0
    DMG_KB_PLAY,    // 1
    DMG_KB_STOP,    // 2
};

static unsigned char ir2kbd_map_b4 [] = 
{
    0,              // 0
    0,              // 1
    0,              // 2
    0,              // 3
    DMG_KB_ENTER,   // 4
};

static int keyboard_update(void)
{
    if (cur_key == 0) {
        memset (state, 0, sizeof(state));
    }
    else {
        if (cur_key & 0x000F) {
            state [ir2kbd_map_b1 [cur_key & 0x000F]] = 1;
        }
        else if (cur_key & 0x00F0) {
            state [ir2kbd_map_b2 [(cur_key & 0x00F0) >> 4]] = 1;
        }
        else if (cur_key & 0x0F00) {
            state [ir2kbd_map_b3 [(cur_key & 0x0F00) >> 8]] = 1;
        }
        else if (cur_key & 0xF000) {
            state [ir2kbd_map_b4 [(cur_key & 0xF000) >> 12]] = 1;
        }
    }

    return NR_KEYS;
}

static const char * keyboard_get_state(void)
{
    return (char *)state;
}

#ifdef  _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    fd_set rfds;
    int    e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_KEYEVENT) && ir_fd >= 0) {
        FD_SET (ir_fd, in);
#ifdef _LITE_VERSION
        if (ir_fd > maxfd) maxfd = ir_fd;
#endif
    }

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e < 0) 
        return -1;

    if (ir_fd >= 0 && FD_ISSET (ir_fd, in)) {
        unsigned int key;

        FD_CLR (ir_fd, in);

        if (read (ir_fd, &key, sizeof (unsigned int)) == sizeof (unsigned int)) {
            cur_key = key;
            return IAL_KEYEVENT;
        }
        else if (cur_key) {
            cur_key = 0;
            return IAL_KEYEVENT;
        }
    }

    return 0;
}

BOOL InitDMGSTBInput (INPUT* input, const char* mdev, const char* mtype)
{
    if ((ir_fd = open( "/dev/remote", O_RDONLY|O_NDELAY )) < 0) {
        printf( "Cannot open /dev/remote (%s)", strerror(errno));
        return FALSE;
    }

    memset (state, 0, sizeof (state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;
    input->wait_event = wait_event;

    return TRUE;
}

void TermDMGSTBInput (void)
{
    if (ir_fd >= 0)
        close (ir_fd);
}

#endif /* _DMGSTB_IAL */

