
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow) {
MSG			msg;
HWND		hWnd;
HMENU		hMenu;
WNDCLASS	wc;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="LAB4";

if (!RegisterClass(&wc))
  return(FALSE);

hWnd=CreateWindow("LAB4","Region Grow",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

strcpy(filename,"");
OriginalImage=NULL;
ROWS=COLS=0;

InvalidateRect(hWnd,NULL,TRUE);
UpdateWindow(hWnd);

hMenu = GetMenu(MainWnd);
switch (RegionMode) {
case 1:
	CheckMenuItem(hMenu, ID_REGIONGROWMODE_PLAY, MF_CHECKED);
	CheckMenuItem(hMenu, ID_REGIONGROWMODE_STEP, MF_UNCHECKED);
	break;
case 2:
	CheckMenuItem(hMenu, ID_REGIONGROWMODE_STEP, MF_CHECKED);
	CheckMenuItem(hMenu, ID_REGIONGROWMODE_PLAY, MF_UNCHECKED);
	break;
}
switch (Delay) {
case 0:
	CheckMenuItem(hMenu, ID_DELAY_NONE, MF_CHECKED);
	CheckMenuItem(hMenu, ID_DELAY_1MS, MF_UNCHECKED);
	break;
case 1:
	CheckMenuItem(hMenu, ID_DELAY_1MS, MF_CHECKED);
	CheckMenuItem(hMenu, ID_DELAY_NONE, MF_UNCHECKED);
	break;
}
switch (RegionColor) {
case 1: // red
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 2: // orange
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 3: // yellow
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 4: // green
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 5: // blue
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 6: // purple
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_CHECKED);
	break;
}
DrawMenuBar(hWnd);

while (GetMessage(&msg,NULL,0,0)) {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}




LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam) {
HMENU				hMenu;
OPENFILENAME		ofn;
FILE				*fpt;
HDC					hDC;
int					ret;
char				header[320], text[320];
int					BYTES, i, x, y;
double				avg, var;

switch (uMsg) {
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
	  case ID_REGIONGROWMODE_PLAY:
			RegionMode = 1; // play
			break;
	  case ID_REGIONGROWMODE_STEP:
			RegionMode = 2; // step
			break;
	  case ID_DELAY_NONE:
		  Delay = 0; // no delay
		  break;
	  case ID_DELAY_1MS:
		  Delay = 1; // 1 ms delay
		  break;
	  case ID_SELECTREGIONCOLOR_RED:
			RegionColor = 1; // red
			PaintColor = RGB(255, 0, 0);
			break;
	  case ID_SELECTREGIONCOLOR_ORANGE:
			RegionColor = 2; // orange
			PaintColor = RGB(246, 103, 51);
			break;
	  case ID_SELECTREGIONCOLOR_YELLOW:
			RegionColor = 3; // yellow
			PaintColor = RGB(255, 215, 0);
			break;
	  case ID_SELECTREGIONCOLOR_GREEN:
			RegionColor = 4; // green
			PaintColor = RGB(0, 255, 0);
			break;
	  case ID_SELECTREGIONCOLOR_BLUE:
			RegionColor = 5; // blue
			PaintColor = RGB(0, 0, 255);
			break;
	  case ID_SELECTREGIONCOLOR_PURPLE:
		  RegionColor = 6; // purple
		  PaintColor = RGB(82, 45, 128);
		  break;
	  case ID_DISPLAY_CLEARREGIONS:
		  labels = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
		  RegionSize, TotalRegions = 0;
		  PaintImage();
		  break;
	  case ID_EDIT_PREDICATEVALUES:
		  ret = DialogBox(GetModuleHandle(NULL),
			  MAKEINTRESOURCE(IDD_DIALOG), hWnd, DlgProc);
		  if (ret == IDOK) {
			  /*MessageBox(hWnd, "Values updated.", "Success",
				  MB_OK | MB_ICONINFORMATION);*/
		  }
		  else if (ret == -1) {
			  MessageBox(hWnd, "Dialog failed!", "Error",
				  MB_OK | MB_ICONINFORMATION);
		  }
		  break;
	  case ID_FILE_LOAD:
		if (OriginalImage != NULL) {
		  free(OriginalImage);
		  OriginalImage=NULL;
		  }
		memset(&(ofn),0,sizeof(ofn));
		ofn.lStructSize=sizeof(ofn);
		ofn.lpstrFile=filename;
		filename[0]=0;
		ofn.nMaxFile=MAX_FILENAME_CHARS;
		ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
		ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
		if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
		  break;		/* user cancelled load */
		if ((fpt=fopen(filename,"rb")) == NULL) {
		  MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
		  break;
		  }
		fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
		if (strcmp(header,"P5") != 0  ||  BYTES != 255) {
		  MessageBox(NULL,"Not a PPM (P5 greyscale) image",filename,MB_OK | MB_APPLMODAL);
		  fclose(fpt);
		  break;
		  }
		OriginalImage=(unsigned char *)calloc(ROWS*COLS,1);
		header[0]=fgetc(fpt);	/* whitespace character after header */
		fread(OriginalImage,1,ROWS*COLS,fpt);
		fclose(fpt);
		SetWindowText(hWnd,filename);
		PaintImage();
		break;

      case ID_FILE_QUIT:
        DestroyWindow(hWnd);
        break;
      }
    break;
  case WM_SIZE:		  /* could be used to detect when window size changes */
	InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd);
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_PAINT:
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
	  if (OriginalImage != NULL) {
		  xPos = LOWORD(lParam);
		  yPos = HIWORD(lParam);

			  avg = var = 0.0;	/* compute average and variance in 7x7 window */
			  for (x = -3; x <= 3; x++)
				  for (y = -3; y <= 3; y++)
					  avg += (double)(OriginalImage[(xPos + x) * COLS + (yPos + y)]);
			  avg /= 49.0;
			  for (x = -3; x <= 3; x++)
				  for (y = -3; y <= 3; y++)
					  var += SQR(avg - (double)(OriginalImage[(xPos + x) * COLS + (yPos + y)]));
			  var = sqrt(var) / 49.0;
			  if (var < 1.0) {	/* condition for seeding a new region is low var */
				  TotalRegions++;
				  if (TotalRegions == 255) {
					  MessageBox(hWnd, "Segmentation incomplete.  Ran out of labels.", "Segmentation incomplete", MB_OK);
					  break;
				  }
				  _beginthread(RegionGrow, 0, NULL);	/* start up a child thread to do other work while this thread continues GUI */
			  }
			  else {
				  MessageBox(hWnd, "Region could not be grown at this location.", "Warning", MB_OK);
			  }
		  }
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
  case WM_MOUSEMOVE:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_KEYDOWN:
	  if (wParam == 'j' || wParam == 'J') {
		RunAnimation = 1; // trigger step mode for animation
	  }
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	break;
  case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
	InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd);
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
	InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd);
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
    break;
  }

hMenu=GetMenu(MainWnd);
switch (RegionMode) {
	case 1:
		CheckMenuItem(hMenu, ID_REGIONGROWMODE_PLAY, MF_CHECKED);
		CheckMenuItem(hMenu, ID_REGIONGROWMODE_STEP, MF_UNCHECKED);
		break;
	case 2:
		CheckMenuItem(hMenu, ID_REGIONGROWMODE_STEP, MF_CHECKED);
		CheckMenuItem(hMenu, ID_REGIONGROWMODE_PLAY, MF_UNCHECKED);
		break;
}
switch (Delay) {
case 0:
	CheckMenuItem(hMenu, ID_DELAY_NONE, MF_CHECKED);
	CheckMenuItem(hMenu, ID_DELAY_1MS, MF_UNCHECKED);
	break;
case 1:
	CheckMenuItem(hMenu, ID_DELAY_1MS, MF_CHECKED);
	CheckMenuItem(hMenu, ID_DELAY_NONE, MF_UNCHECKED);
	break;
}
switch (RegionColor) {
case 1: // red
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 2: // orange
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 3: // yellow
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 4: // green
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 5: // blue
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_CHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_UNCHECKED);
	break;
case 6: // purple
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_RED, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_ORANGE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_YELLOW, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_GREEN, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_BLUE, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SELECTREGIONCOLOR_PURPLE, MF_CHECKED);
	break;
}
DrawMenuBar(hWnd);

return(0L);
}



BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_INITDIALOG:
		// set radio buttons according to user choice
		switch (PixDiff) {
			case 5:
				CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO4, IDC_RADIO1);
				break;
			case 10:
				CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO4, IDC_RADIO2);
				break;
			case 25:
				CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO4, IDC_RADIO3);
				break;
			case 50:
				CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO4, IDC_RADIO4);
				break;
		}
		switch (CentroidDist) {
			case 25:
				CheckRadioButton(hwnd, IDC_RADIO5, IDC_RADIO8, IDC_RADIO5);
				break;
			case 100:
				CheckRadioButton(hwnd, IDC_RADIO5, IDC_RADIO8, IDC_RADIO6);
				break;
			case 250:
				CheckRadioButton(hwnd, IDC_RADIO5, IDC_RADIO8, IDC_RADIO7);
				break;
			case 1000:
				CheckRadioButton(hwnd, IDC_RADIO5, IDC_RADIO8, IDC_RADIO8);
				break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO1:
			PixDiff = 5;
			break;
		case IDC_RADIO2:
			PixDiff = 10;
			break;
		case IDC_RADIO3:
			PixDiff = 25;
			break;
		case IDC_RADIO4:
			PixDiff = 50;
			break;
		case IDC_RADIO5:
			CentroidDist = 25;
			break;
		case IDC_RADIO6:
			CentroidDist = 100;
			break;
		case IDC_RADIO7:
			CentroidDist = 250;
			break;
		case IDC_RADIO8:
			CentroidDist = 1000;
			break;
		case IDOK:
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}




void PaintImage() {
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			*bm_info;
int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
unsigned char		*DisplayImage;

if (OriginalImage == NULL)
  return;		/* no image to draw */

		/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
DISPLAY_ROWS=ROWS;
DISPLAY_COLS=COLS;
if (DISPLAY_ROWS % 4 != 0)
  DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
if (DISPLAY_COLS % 4 != 0)
  DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
	DisplayImage[r*DISPLAY_COLS+c]=OriginalImage[r*COLS+c];

BeginPaint(MainWnd,&Painter);
hDC=GetDC(MainWnd);
bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
bm_info_header.biWidth=DISPLAY_COLS;
bm_info_header.biHeight=-DISPLAY_ROWS; 
bm_info_header.biPlanes=1;
bm_info_header.biBitCount=8; 
bm_info_header.biCompression=BI_RGB; 
bm_info_header.biSizeImage=0; 
bm_info_header.biXPelsPerMeter=0; 
bm_info_header.biYPelsPerMeter=0;
bm_info_header.biClrUsed=256;
bm_info_header.biClrImportant=256;
bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
bm_info->bmiHeader=bm_info_header;
for (i=0; i<256; i++) {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  } 

SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,
			  0, /* first scan line */
			  DISPLAY_ROWS, /* number of scan lines */
			  DisplayImage,bm_info,DIB_RGB_COLORS);
ReleaseDC(MainWnd,hDC);
EndPaint(MainWnd,&Painter);

free(DisplayImage);
free(bm_info);
}


/*
** Given an image, a starting point, and a label, this routine
** paint-fills (8-connected) the area with the given new label
** according to the given criteria (pixels close to the average
** intensity of the growing region are allowed to join).
*/

void RegionGrow(void) {
	HDC	hDC;
	int	r2, c2;
	int	queue[MAX_QUEUE], qh, qt;
	int	average, total;	/* average and total intensity in growing region */
	int centroid_row, centroid_col, c_sum, r_sum;
	char	text[300];

	int r = yPos;
	int c = xPos;
	int count = 0;
	int paint_over_label = 0;

	/* segmentation image = labels; calloc initializes all labels to 0 */
	labels = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	/* used to quickly erase small grown regions */
	indices = (int*)calloc(ROWS * COLS, sizeof(int));

	count = 0;
	if (labels[r * COLS + c] != paint_over_label)
		return;
	labels[r * COLS + c] = TotalRegions;
	r_sum = r;
	c_sum = c;
	average = total = (int)OriginalImage[r * COLS + c];
	if (indices != NULL)
		indices[0] = r * COLS + c;
	queue[0] = r * COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	count = 1;

	hDC = GetDC(MainWnd);
	SetPixel(hDC, xPos, yPos, PaintColor);
	ReleaseDC(MainWnd, hDC);

		while (qt != qh) {
			if (RegionMode == 1) RunAnimation = 1;
			while (RunAnimation == 0) { // pause animation with in step mode
				if (RegionMode == 1) RunAnimation = 1; // enable run mode to continue after being in step mode
			}

			if (count % 50 == 0)  /* recalculate average after each 50 pixels join */
				average = total / count;
			/* recalculate centroid of region every time */
			centroid_row = r_sum / count;
			centroid_col = c_sum / count;
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++) {
					if (r2 == 0 && c2 == 0)
						continue;
					if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS ||
						(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS)
						continue;
					if (labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] != paint_over_label)
						continue;
					/* test criteria to join region */
					// pixel intensity difference
					if (abs((int)(OriginalImage[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2])
						- average) > PixDiff)
						continue;
					// distance from centroid of region
					if (sqrt(SQR((queue[qt] / COLS + r2) - centroid_row) + SQR((queue[qt] % COLS + c2) - centroid_col)) > CentroidDist)
						continue;
					labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] = TotalRegions;

					// paint pixels on image in window
					hDC = GetDC(MainWnd);
					SetPixel(hDC, queue[qt] % COLS + c2, (queue[qt] / COLS + r2), PaintColor);
					ReleaseDC(MainWnd, hDC);

					if (indices != NULL)
						indices[count] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
					total += OriginalImage[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2];
					r_sum += queue[qt] / COLS + r2;
					c_sum += queue[qt] % COLS + c2;
					count++;
					queue[qh] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
					qh = (qh + 1) % MAX_QUEUE;
					if (qh == qt) {
						MessageBox(NULL, "Max queue size exceeded.", "Warning", MB_OK);
						exit(0);
					}
					if (RegionMode == 2) RunAnimation = 0;

					if (Delay) Sleep(1);		/* pause 1 ms */
				}
			qt = (qt + 1) % MAX_QUEUE;
		}
}
