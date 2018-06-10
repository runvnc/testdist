/* nuklear - 1.32.0 - public domain */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "rs232/rs232.h"

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 700

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDIP_IMPLEMENTATION
#include "../../nuklear.h"
#include "nuklear_gdip.h"

  static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  if (nk_gdip_handle_event(wnd, msg, wparam, lparam))
    return 0;
  return DefWindowProcW(wnd, msg, wparam, lparam);
}

struct nk_context *ctx;
HWND wnd; 
int running = 1;
int needs_refresh = 1;
GdipFont* font;
WNDCLASSW wc;

void setupWin32() {
  RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
  DWORD style = WS_OVERLAPPEDWINDOW;
  DWORD exstyle = WS_EX_APPWINDOW;
  /* Win32 */
  memset(&wc, 0, sizeof(wc));
  wc.style = CS_DBLCLKS;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = GetModuleHandleW(0);
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = L"NuklearWindowClass";
  RegisterClassW(&wc);

  AdjustWindowRectEx(&rect, style, FALSE, exstyle);

  wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Distributor Tester",
      style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
      rect.right - rect.left, rect.bottom - rect.top,
      NULL, NULL, wc.hInstance, NULL);

  /* GUI */
  ctx = nk_gdip_init(wnd, WINDOW_WIDTH, WINDOW_HEIGHT);
  font = nk_gdipfont_create("Arial", 12);
  nk_gdip_set_font(font);
}

int com = 0;

void comTest() {
  int numPorts = comEnumerate();
  numPorts = comGetNoPorts();
  printf("Found %d COM ports.\n", numPorts);
  for (int i = 0; i<numPorts; i++) {
    printf("Checking port %d\n", i);
    const char * name = comGetPortName(i);
    printf("Found COM port: %s\n", name);
  } 
  printf("COM check done.\n");
  int ok = comOpen(com, 115200);
  printf("COM port open result: %d\n");
}


char* lines[5000];

int splitLines(char *str) {
  int n = 0;
  char* line = strtok(str, "\r\n");
  while (line != NULL) {
    lines[n++] = line;
    line = strtok(NULL, "\r\n");
  }
  return n;
} 

FILE *datafile = NULL;
struct nk_image graphimg;

void checkCOM() {
  char msg[999024];
  int bytes = comRead(com, msg, 999024); 
  if (bytes > 0) {
    int numLines = splitLines(msg);
    for (int i = 0; i < numLines; i++) {
      char* line = lines[i];
      char* str = strtok(line, " \r\n");
      if (strcmp(str, "UPLOAD")==0) {
        datafile = fopen("test.dat", "w");
        remove("img1.png");
      } else if (strcmp(str, "DATA")==0) {
        str = strtok(NULL, " \r\n");
        float x = atof(str);
        str = strtok(NULL, " \r\n");
        float y = atof(str);
        if (!(x==0 && y==0)) fprintf(datafile,"%f %f\n",x,y);
      } else if (strcmp(str,"END")==0) {
        fclose(datafile);
        nk_gdip_image_free(graphimg);
        int ok = remove("img1.png");
        if (ok == 0) printf("Delete OK\n"); else printf("Delete failed.\n");

        system("DEL img1.png");
        system("gnuplot\\gnuplot.exe showresults.gpi");
        graphimg = nk_gdip_load_image_from_file(L"img1.png");    
      }
    }
  }
}


void writeCOM(const char* msg) {
  comWrite(com, msg, strlen(msg));
}

int main(void)
{
  setvbuf(stdout, (char*)NULL, _IONBF, 0);

  setupWin32();

  comTest();

  graphimg = nk_gdip_load_image_from_file(L"img1.png");

  while (running)
  {
    checkCOM();

    /* Input */
    MSG msg;
    nk_input_begin(ctx);

    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        running = 0;
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
      needs_refresh = 1;
    }
    nk_input_end(ctx);

    /* GUI */
    if (nk_begin(ctx, "Analysis", nk_rect(20, 20, 900, 500),
          NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
          NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
      nk_layout_row_static(ctx, 420, 900, 1);
      nk_layout_row_static(ctx, 30, 80, 2);
      if (nk_button_label(ctx, "START")) {
        fprintf(stdout, "start pressed\n");
        writeCOM("START\n");
      }
      if (nk_button_label(ctx, "STOP")) {
        fprintf(stdout, "stop pressed\n");
        writeCOM("STOP\n");
      }

      struct nk_rect bounds = nk_window_get_bounds(ctx);
      bounds.y = 40;
      bounds.w = 800;
      bounds.h = 400;

      nk_draw_image(nk_window_get_canvas(ctx), bounds, &graphimg, nk_white);
    }

    nk_end(ctx);


    /* Draw */  
    nk_gdip_render(NK_ANTI_ALIASING_ON, nk_rgb(30,30,30));

  }

  nk_gdipfont_del(font);
  nk_gdip_shutdown();
  UnregisterClassW(wc.lpszClassName, wc.hInstance);
  return 0;
}
