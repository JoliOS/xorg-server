/*
 * Copyright 2007-2008 Peter Hutterer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Author: Peter Hutterer, University of South Australia, NICTA
 */

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <X11/X.h>	/* for inputstr.h    */
#include <X11/Xproto.h>	/* Request macro     */
#include "inputstr.h"	/* DeviceIntPtr      */
#include "windowstr.h"	/* window structure  */
#include "scrnintstr.h"	/* screen structure  */
#include <X11/extensions/XI.h>
#include <X11/extensions/XIproto.h>
#include "extnsionst.h"
#include "extinit.h"	/* LookupDeviceIntRec */
#include "exevents.h"
#include "exglobals.h"

#include "getcptr.h"

/***********************************************************************
 * This procedure allows a client to query another client's client pointer
 * setting.
 */

int
SProcXGetClientPointer(ClientPtr client)
{
    char n;
    REQUEST(xGetClientPointerReq);

    swaps(&stuff->length, n);
    swapl(&stuff->win, n);
    return ProcXGetClientPointer(client);
}

int ProcXGetClientPointer(ClientPtr client)
{
    int err;
    WindowPtr win;
    ClientPtr winclient;
    xGetClientPointerReply rep;
    REQUEST(xGetClientPointerReq);
    REQUEST_SIZE_MATCH(xGetClientPointerReq);

    err = dixLookupWindow(&win, stuff->win, client, DixReadAccess);
    if (err != Success)
    {
        SendErrorToClient(client, IReqCode, X_GetClientPointer,
                stuff->win, err);
        return Success;
    }

    winclient = wClient(win);

    rep.repType = X_Reply;
    rep.RepType = X_GetClientPointer;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.set = (winclient->clientPtr != NULL);
    rep.deviceid = (winclient->clientPtr) ? winclient->clientPtr->id : 0;

    WriteReplyToClient(client, sizeof(xGetClientPointerReply), &rep);
    return Success;
}

/***********************************************************************
 *
 * This procedure writes the reply for the XGetClientPointer function,
 * if the client and server have a different byte ordering.
 *
 */

void
SRepXGetClientPointer(ClientPtr client, int size,
        xGetClientPointerReply* rep)
{
    char n;
    swaps(&rep->sequenceNumber, n);
    swapl(&rep->length, n);
    WriteToClient(client, size, (char *)rep);
}
