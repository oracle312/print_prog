#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <winspool.h>
#include <fpdfview.h>
#include <fpdf_doc.h>
#include <fpdf_text.h>
#include <vector>

#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "pdfium.lib")
#pragma comment(lib, "winspool.lib")

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
void PrintImage(const wchar_t* filename);

string WideToMultiByte(const std::wstring& wideStr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], (int)wideStr.size(), NULL, 0, NULL, NULL);
	std::string multiByteStr(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], (int)wideStr.size(), &multiByteStr[0], size_needed, NULL, NULL);
	return multiByteStr;
}


void PrintPDF(const wchar_t* pdfPath) {
	// PDFium 초기화
	FPDF_InitLibrary();

	string pdfPathStr = WideToMultiByte(pdfPath);
	const char* pdfPathCStr = pdfPathStr.c_str();

	// PDF 파일 열기
	FPDF_DOCUMENT document = FPDF_LoadDocument(pdfPathCStr, nullptr);
	if (!document) {
		std::cerr << "Could not open PDF document." << std::endl;
		FPDF_DestroyLibrary();
		return;
	}

	// 기본 프린터 가져오기
	DWORD size = 0;
	GetDefaultPrinter(nullptr, &size);
	wchar_t* printerName = new wchar_t[size];
	if (!GetDefaultPrinter(printerName, &size)) {
		std::cerr << "Could not get default printer." << std::endl;
		FPDF_CloseDocument(document);
		FPDF_DestroyLibrary();
		delete[] printerName;
		return;
	}

	// 프린터 DC 가져오기
	HDC hdc = CreateDC(nullptr, printerName, nullptr, nullptr);
	if (!hdc) {
		std::cerr << "Could not create printer DC." << std::endl;
		FPDF_CloseDocument(document);
		FPDF_DestroyLibrary();
		delete[] printerName;
		return;
	}

	DOCINFO di;
	ZeroMemory(&di, sizeof(di));
	di.cbSize = sizeof(di);
	di.lpszDocName = L"PDF Print";

	// 문서 시작
	if (StartDoc(hdc, &di) <= 0) {
		std::cerr << "Could not start print job." << std::endl;
		DeleteDC(hdc);
		FPDF_CloseDocument(document);
		FPDF_DestroyLibrary();
		delete[] printerName;
		return;
	}

	int pageCount = FPDF_GetPageCount(document);
	for (int i = 0; i < pageCount; ++i) {
		FPDF_PAGE page = FPDF_LoadPage(document, i);
		if (!page) {
			std::cerr << "Could not load page " << i << std::endl;
			continue;
		}

		// 페이지 시작
		if (StartPage(hdc) <= 0) {
			std::cerr << "Could not start page." << std::endl;
			FPDF_ClosePage(page);
			continue;
		}

		// 페이지 크기 가져오기
		double width = FPDF_GetPageWidth(page);
		double height = FPDF_GetPageHeight(page);

		// 비트맵 생성
		FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
		FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);
		FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, 0);

		// 비트맵을 HBITMAP으로 변환
		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height; // top-down DIB
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* bits;
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
		memcpy(bits, FPDFBitmap_GetBuffer(bitmap), width * height * 4);

		// HBITMAP을 HDC로 복사
		HDC hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);
		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);

		// 페이지 끝
		EndPage(hdc);
		FPDFBitmap_Destroy(bitmap);
		FPDF_ClosePage(page);
	}

	// 문서 끝
	EndDoc(hdc);
	DeleteDC(hdc);

	// PDF 문서 닫기
	FPDF_CloseDocument(document);
	FPDF_DestroyLibrary();
	delete[] printerName;
}

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
	//const wchar_t* filename = L"print.jpg";
	//PrintImage(filename);
	//return 0;
	const wchar_t* pdfPath = L"print.pdf";
	PrintPDF(pdfPath);
	return 0;
}

/*int main()
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
}*/

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

void PrintImage(const wchar_t* filename)
{
	// GDI+ 초기화
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 프린터 설정
	PRINTDLG pd;
	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.Flags = PD_RETURNDC;
	PrintDlg(&pd);

	if (pd.hDC)
	{
		// 이미지 로드
		Image image(filename);

		// DC에 이미지를 출력
		HDC hdc = pd.hDC;
		DOCINFO di;
		ZeroMemory(&di, sizeof(di));
		di.cbSize = sizeof(di);
		di.lpszDocName = L"Image Print";

		StartDoc(hdc, &di);
		StartPage(hdc);

		Graphics graphics(hdc);
		graphics.DrawImage(&image, 0, 0, image.GetWidth(), image.GetHeight());

		EndPage(hdc);
		EndDoc(hdc);
		DeleteDC(hdc);
	}
	else
	{
		std::cerr << "Could not get printer DC." << std::endl;
	}

	// GDI+ 종료
	GdiplusShutdown(gdiplusToken);
}
