/*
** $Id: accelkey.c 12332 2009-12-14 07:28:03Z weiym $
**
** accelkey.c: The Accelerator module.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.04.19
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "blockheap.h"
#include "accelkey.h"

static BLOCKHEAP ACHeap;

/* acceltable construction */
inline static void InitFreeACList (void)
{
    InitBlockDataHeap (&ACHeap, sizeof (ACCELTABLE), SIZE_AC_HEAP);
}

inline static PACCELTABLE AccelTableAlloc (void)
{
    return (PACCELTABLE) BlockDataAlloc (&ACHeap);
}

inline static void FreeAccelTable (PACCELTABLE pac)
{
    BlockDataFree (&ACHeap, pac);
}

inline static void DestroyFreeACList (void)
{
    DestroyBlockDataHeap (&ACHeap);
}

/* accelkey item routeline */
static BLOCKHEAP AIHeap;
inline static void InitFreeAIList (void)
{
    InitBlockDataHeap (&AIHeap, sizeof (ACCELITEM), SIZE_AI_HEAP);
}

inline static PACCELITEM AccelItemAlloc (void)
{
    return (PACCELITEM) BlockDataAlloc (&AIHeap);
}

inline static void FreeAccelItem (ACCELITEM* pai)
{
    BlockDataFree (&AIHeap, pai);
}

inline static void DestroyFreeAIList (void)
{
    DestroyBlockDataHeap (&AIHeap);
}

BOOL mg_InitAccel (void)
{
    InitFreeACList ();
    InitFreeAIList ();

    return TRUE;
}

void mg_TerminateAccel (void)
{
    DestroyFreeACList ();
    DestroyFreeAIList ();
}

/*********  Interfaces  ************************************/
HACCEL GUIAPI LoadAccelerators (const char* filename, int id)
{
    return 0;
}

HACCEL GUIAPI CreateAcceleratorTable (HWND hWnd)
{
    PACCELTABLE pac;

    if (!(pac = AccelTableAlloc ()))
        return 0;
    
    pac->category = TYPE_HACCEL;
    pac->hwnd = hWnd;
    pac->head = NULL;

    return (HACCEL)pac;
}

int GUIAPI IsAccel (HACCEL hac)
{
    PACCELTABLE pAcc = (PACCELTABLE)hac;

    if (pAcc->category != TYPE_HACCEL) return 0;

    return TRUE;
}

int GUIAPI DestroyAcceleratorTable (HACCEL hacc)
{
    PACCELTABLE pac;
    PACCELITEM pai, ptmpai;

    pac = (PACCELTABLE) hacc;

    if (pac->category != TYPE_HACCEL)
        return ERR_INVALID_HANDLE;
    
    pai = pac->head;
    while (pai) {
        ptmpai = pai->next;
        FreeAccelItem (pai);
        pai = ptmpai;
    }

    FreeAccelTable (pac);
    return 0;
}

static BOOL does_mask_match (DWORD mask1, DWORD mask2)
{
    BOOL ctrl = FALSE, alt = FALSE, shift = FALSE;

    if ((mask1 & KS_CTRL) == KS_CTRL) {
        if ((mask2 & KS_LEFTCTRL) || (mask2 & KS_RIGHTCTRL))
            ctrl = TRUE;
    }
    else if ((mask1 & KS_CTRL) == (mask2 & KS_CTRL))
        ctrl = TRUE;

    if ((mask1 & KS_ALT) == KS_ALT) {
        if ((mask2 & KS_LEFTALT) || (mask2 & KS_RIGHTALT))
            alt = TRUE;
    }
    else if ((mask1 & KS_ALT) == (mask2 & KS_ALT))
        alt = TRUE;

    if ((mask1 & KS_SHIFT) == KS_SHIFT) {
        if ((mask2 & KS_LEFTSHIFT) || (mask2 & KS_RIGHTSHIFT))
            shift = TRUE;
    }
    else if ((mask1 & KS_SHIFT) == (mask2 & KS_SHIFT))
        shift = TRUE;

    return ctrl && alt && shift;
}

static PACCELITEM *accFindMatchAccelKeys (HACCEL hacc, int key, DWORD keymask)
{
    PACCELITEM  pai;
    PACCELTABLE pacc = (PACCELTABLE) hacc;

    pai = pacc->head;
    if (pai) {
        if (pai->key == key && does_mask_match (pai->keymask, keymask))
            return &(pacc->head);
        else {
            while (pai->next) {
                if (pai->next->key == key 
                        && does_mask_match (pai->next->keymask, keymask))
                    return &(pai->next);
                pai = pai ->next;
            }
        }
    }

    return NULL;
}
 
int GUIAPI AddAccelerators (HACCEL hacc, int key, DWORD keymask, 
                WPARAM wParam , LPARAM lParam)
{
    PACCELTABLE pac;
    PACCELITEM pnewai;

    pac = (PACCELTABLE) hacc;

    if (pac->category != TYPE_HACCEL)
        return ERR_INVALID_HANDLE; 
    if (accFindMatchAccelKeys (hacc, key, keymask))
       return  ERR_ALREADY_EXIST;
    if (!(pnewai = AccelItemAlloc ())) 
        return ERR_RES_ALLOCATION;

    pnewai->key     = key;
    pnewai->keymask = keymask;
    pnewai->wParam  = wParam;  
    pnewai->lParam  = lParam;
    pnewai->next    = pac->head ;
    pac->head       = pnewai ;
    return 0;
}

int GUIAPI DeleteAccelerators (HACCEL hacc,int key, DWORD keymask)
{
    PACCELTABLE pac;
    PACCELITEM *ptempac;
    PACCELITEM ptempac1;

    pac = (PACCELTABLE) hacc;
    if (pac -> category != TYPE_HACCEL)
        return ERR_INVALID_HANDLE;
    if (!(ptempac=accFindMatchAccelKeys(hacc,key, keymask)))
        return ERR_NO_MATCH;

    ptempac1= ((*ptempac)->next);
    FreeAccelItem (*ptempac);
    (*ptempac) = ptempac1; 
    return 0;  
}

HACCEL GUIAPI CopyAcceleratorTable (HACCEL hacc)
{
    HACCEL hac;
    PACCELTABLE pac = (PACCELTABLE)hacc;
    PACCELITEM pai;

    if ((hac = (HACCEL) CreateAcceleratorTable (pac->hwnd))) {
        pai = pac -> head;
        while (pai) {
            if (AddAccelerators (hac, pai->key, pai->keymask, 
                                    pai->wParam, pai->lParam)) {
                DestroyAcceleratorTable(hac);
                return (HACCEL) NULL;
            }
            pai = pai -> next;
                     
        }

        return hac;
    }
    else
        return (HACCEL) NULL; 
}

int GUIAPI TranslateAccelerator (HACCEL hAccel, PMSG pMsg)
{
    PACCELTABLE pac;
    PACCELITEM  *pai;
    int key;

    pac = (PACCELTABLE) hAccel;

    if (pac->hwnd != pMsg->hwnd)
        return ERR_BAD_OWNER;

    if (pMsg->message == MSG_KEYDOWN || pMsg->message == MSG_SYSKEYDOWN)
        key = pMsg->wParam | 256;
    else if (pMsg->message == MSG_CHAR || pMsg->message == MSG_SYSCHAR)
        key = pMsg->wParam & 255;
    else 
        return ERR_NO_MATCH;

    if ((pai = accFindMatchAccelKeys (hAccel, key, pMsg->lParam))) {
        if (pMsg->message == MSG_SYSKEYDOWN || pMsg->message == MSG_SYSCHAR)
            return SendNotifyMessage (pMsg->hwnd, 
                            MSG_SYSCOMMAND, (*pai)->wParam, (*pai)->lParam);
        else 
            return SendNotifyMessage (pMsg->hwnd, 
                           MSG_COMMAND, (*pai)->wParam , (*pai)->lParam);
    }
    else 
        return ERR_NO_MATCH;
}

