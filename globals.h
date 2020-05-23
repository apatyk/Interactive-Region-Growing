
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320
#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

#define TIMER_SECOND	1			/* ID of timer used for animation */

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

// UI variables
int			RegionColor = 1;
int			RegionMode = 1;
COLORREF	PaintColor = RGB(255, 0, 0);
int			Delay = 0;
int			PixDiff = 10;
int			CentroidDist = 250;

// Image data
unsigned char	*OriginalImage;
int				ROWS,COLS;
unsigned char*	labels;
int*			indices;
int				xPos, yPos, RegionSize, TotalRegions;

// Drawing flags
int		RunAnimation = 0;

// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void PaintImage();
void RegionGrow();