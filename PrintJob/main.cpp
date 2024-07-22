#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <iostream>

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;
using namespace std;

enum {
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKYBLUE,
	DARK_RED,
	DARK_VOILET,
	DAKR_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
};

void SetColor(unsigned short text)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text);
}

void PrintJpeg(const wstring& printerName, const wstring& filePath);

int main()
{
	SetConsoleTitle(L"디오시스템");
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << "                 디오시스템                    " << endl;
	SetColor(GREEN);
	cout << "                          Code By. SIYUL PARK" << endl;
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << endl;
	SetColor(RED);
	cout << "[알림] 잉크막힘을 방지하기위한 정기적 인쇄입니다." << endl;
	cout << "[알림] 확인을 눌러 인쇄해주세요 !!" << endl;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	std::wstring printerName = L"YourPrinterName";
	std::wstring filePath = L"C:\\Path\\print.jpg";

	PrintJpeg(printerName, filePath);

	GdiplusShutdown(gdiplusToken);

	return 0;
}

void PrintJpeg(const std::wstring& printerName, const std::wstring& filePath)
{
	PRINTDLG pd = { 0 };
	pd.lStructSize = sizeof(pd);
	pd.Flags = PD_RETURNDC;
	pd.hwndOwner = NULL;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;

	if (PrintDlg(&pd) == FALSE)
	{
		wprintf(L"프린터 선택 취소 또는 오류 발생\n");
		return;
	}

	HDC hdcPrinter = pd.hDC;
	if (!hdcPrinter)
	{
		wprintf(L"프린터 DC 생성 실패\n");
		return;
	}

	// GDI+를 사용하여 이미지 로드
	Image image(filePath.c_str());
	if (image.GetLastStatus() != Ok)
	{
		wprintf(L"이미지 로드 실패\n");
		DeleteDC(hdcPrinter);
		return;
	}

	// 프린트 작업 시작
	DOCINFO di = { 0 };
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = L"Print JPEG Document";

	if (StartDoc(hdcPrinter, &di) > 0)
	{
		if (StartPage(hdcPrinter) > 0)
		{
			// 이미지 인쇄
			Graphics graphics(hdcPrinter);
			if (graphics.DrawImage(&image, 0, 0, image.GetWidth(), image.GetHeight()) == Ok)
			{
				SetColor(GREEN);
				cout << "[성공] 이미지 인쇄 성공 !!";
				//wprintf(L"이미지 인쇄 성공\n");
			}
			else
			{
				SetColor(RED);
				cout << "[실패] 이미지 인쇄 실패 !!";
			}
			EndPage(hdcPrinter);
		}
		EndDoc(hdcPrinter);
	}
	else
	{
		cout << "[실패] 프린트 작업 시작 실패 !!";
	}

	// 프린터 DC 삭제
	DeleteDC(hdcPrinter);
}