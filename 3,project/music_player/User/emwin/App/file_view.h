#ifndef __FILEVIEW_H
#define __FILEVIEW_H
#include "DIALOG.h"
#include "file_process.h"
#include "stdint.h"

#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0  (GUI_ID_USER + 0x01)

#define  WM_SELECT_WINDOW            (WM_USER + 0x00)

extern WM_HWIN g_page[2];

WM_HWIN Createfile_view(void);

#endif /* __FILEVIEW_H*/
