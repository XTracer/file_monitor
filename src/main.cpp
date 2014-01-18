#include <windows.h>
#include <stdio.h>
#include <time.h>

char timeStr [9];

void main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hDir = CreateFileW( L"C:\\", GENERIC_READ, 7, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

    OVERLAPPED o = {};
    o.hEvent = CreateEvent(0, FALSE, FALSE, 0);
 
    DWORD nBufferLength = 1024*60;
    BYTE* lpBuffer = new BYTE[nBufferLength];

    while( true )
    { 
        ReadDirectoryChangesW(hDir, lpBuffer, nBufferLength, TRUE, 185, NULL, &o, 0);

        WaitForSingleObject(o.hEvent, INFINITE);
		int seek = 0;

		while(seek < nBufferLength)
		{
			PFILE_NOTIFY_INFORMATION pNotify = PFILE_NOTIFY_INFORMATION(lpBuffer + seek);
			
			WCHAR szwFileName[MAX_PATH];
			int ulCount = pNotify->FileNameLength/2;
			wcsncpy(szwFileName, pNotify->FileName, ulCount);
			szwFileName[ulCount] = L'\0';

			int i;

			switch( pNotify->Action )
			{
				case FILE_ACTION_ADDED:				i = 10; break;
				case FILE_ACTION_REMOVED:			i = 12; break;
				case FILE_ACTION_MODIFIED:			i = 14; break;
				case FILE_ACTION_RENAMED_OLD_NAME:	i = 11; break;
				case FILE_ACTION_RENAMED_NEW_NAME:	i = 15; break;
			}

			_strtime( timeStr );

			SetConsoleTextAttribute(hConsole, 2);
			printf("[%s] ", timeStr);

			SetConsoleTextAttribute( hConsole, i );
			wprintf(L"C:\\%s\n", szwFileName);

			seek += pNotify->NextEntryOffset;

			if(pNotify->NextEntryOffset == 0)
				break;
		}
    }
}