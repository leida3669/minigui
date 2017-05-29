/*
** $Id: sscanf.c 8944 2007-12-29 08:29:16Z xwyan $
**
** scanf.c: implementation of own_scanf function.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-03 by Wei yongming
*/

#include <stddef.h>
#include <limits.h>

#include "common.h"

#if defined (_MGUSE_OWN_STDIO) && defined (_MGUSE_OWN_SSCANF)

int own_sscanf( const char *s, const char *format, ... )
{
    int rc;
    va_list ap;

    va_start(ap, format);

    rc = own_vsscanf (s, format, ap);

    va_end(ap);

    return rc;
}

#endif /* _MGUSE_OWN_STDIO */

