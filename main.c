#include <windows.h>

char szClassName[ ] = "BmpToPng";

#define YES       1
#define NO        0

HINSTANCE ins;
HWND hWnd;
HWND hBitmap;
UCHAR DropFile[MAX_PATH];
COLORREF nColor=RGB(255,0,0);
POINT cursor;
HBRUSH hBackBrush;
RECT back={0,0,420,20};

UCHAR ret=NO;
UCHAR ReadDataFile(UCHAR *);
void SavePNG(UCHAR *);
void SetColor(UCHAR red,UCHAR green,UCHAR blue);

void CenterOnScreen(HWND hnd)
{
     RECT rC, rD;
     SystemParametersInfo(SPI_GETWORKAREA, 0, &rD, 0);
     GetWindowRect(hnd, &rC);
     int nX=((rD.right - rD.left) / 2) -((rC.right - rC.left) / 2);
     int nY=((rD.bottom - rD.top) / 2) -((rC.bottom - rC.top) / 2);
     SetWindowPos(hnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
return;
}

void ReadDataFromFile()
{

HBITMAP hp = LoadImage(ins, DropFile,IMAGE_BITMAP,0, 0,LR_DEFAULTCOLOR | LR_LOADFROMFILE);
			SendMessage(hBitmap, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP,(LPARAM)(HANDLE) hp);
ret=ReadDataFile(DropFile);
if(ret==NO) SetWindowText(hWnd,"Error: Drop a BMP(24Bit) File For Windows");
}

void SaveDataToFile()
{
     strcat(DropFile,".png\0");
     if(ret==YES)
      SavePNG(DropFile);
  PostMessage(hWnd,WM_DESTROY,0,0);
  _exit(0);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)             
    {
       	case WM_CONTEXTMENU:{
            CreateThread(0,0,(LPTHREAD_START_ROUTINE)SaveDataToFile,0,0,0);
	}break;           
        case WM_CREATE:{
            hWnd = hwnd;
            hBitmap= CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_NOTIFY,
				0, 20, 400, 400, hwnd, (HMENU)1234, ins, NULL );    
            CenterOnScreen(hwnd); 
        }break; 
        case WM_ERASEBKGND:{
            hBackBrush = CreateSolidBrush(nColor);
            FillRect((HDC)wParam, &back, hBackBrush);
            DeleteObject(hBackBrush);
        }
        return 1;
        case WM_COMMAND:{ switch(LOWORD(wParam)){
            case 1234:
            {    
                HDC hScreenDC = GetDC(hwnd);
                GetCursorPos(&cursor);
                ScreenToClient(hwnd, &cursor);
                nColor = GetPixel(hScreenDC, cursor.x, cursor.y);
                ReleaseDC(hwnd,hScreenDC);
                SendMessage(hWnd, WM_ERASEBKGND, (WPARAM)GetDC(hWnd), 0);
                SetColor(GetRValue(nColor),GetGValue(nColor),GetBValue(nColor));
                SetForegroundWindow(hwnd);
            } break;
          }//switch
        }//WM_COMMAND
        break;   
        case WM_DROPFILES:{
            SetWindowText(hwnd,"Bmp To Png");
            memset(DropFile,0,MAX_PATH);; 
	    DragQueryFile((HDROP)wParam, 0, DropFile, MAX_PATH);
	    DragFinish((HDROP) wParam);
	    CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadDataFromFile,0,0,0);
        } break; 
        case WM_DESTROY:
            PostQuitMessage (0);       
            break;
        default: 
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
  return 0;
}



int WINAPI WinMain (HINSTANCE _A_, HINSTANCE _B_, LPSTR _C_,  int _D_)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
    ins = _A_;

    wincl.hInstance = _A_;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon(ins, "A");
    wincl.hIconSm = LoadIcon(ins, "A");
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; 
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    
    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx(WS_EX_ACCEPTFILES|WS_EX_TOPMOST|WS_EX_PALETTEWINDOW,szClassName,"Bmp To Png",WS_SYSMENU,
    0, 0, 405, 444,
    HWND_DESKTOP, NULL, ins, NULL);

    ShowWindow (hwnd, _D_);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}

