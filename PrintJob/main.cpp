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
	// PDFium �ʱ�ȭ
	FPDF_InitLibrary();

	string pdfPathStr = WideToMultiByte(pdfPath);
	const char* pdfPathCStr = pdfPathStr.c_str();

	// PDF ���� ����
	FPDF_DOCUMENT document = FPDF_LoadDocument(pdfPathCStr, nullptr);
	if (!document) {
		std::cerr << "Could not open PDF document." << std::endl;
		FPDF_DestroyLibrary();
		return;
	}

	// �⺻ ������ ��������
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

	// ������ DC ��������
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

	// ���� ����
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

		// ������ ����
		if (StartPage(hdc) <= 0) {
			std::cerr << "Could not start page." << std::endl;
			FPDF_ClosePage(page);
			continue;
		}

		// ������ ũ�� ��������
		double width = FPDF_GetPageWidth(page);
		double height = FPDF_GetPageHeight(page);

		// ��Ʈ�� ����
		FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
		FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);
		FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, 0);

		// ��Ʈ���� HBITMAP���� ��ȯ
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

		// HBITMAP�� HDC�� ����
		HDC hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);
		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);

		// ������ ��
		EndPage(hdc);
		FPDFBitmap_Destroy(bitmap);
		FPDF_ClosePage(page);
	}

	// ���� ��
	EndDoc(hdc);
	DeleteDC(hdc);

	// PDF ���� �ݱ�
	FPDF_CloseDocument(document);
	FPDF_DestroyLibrary();
	delete[] printerName;
}

int main()
{
	SetConsoleTitle(L"����ý���");
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << "                 ����ý���                    " << endl;
	SetColor(GREEN);
	cout << "                          Code By. SIYUL PARK" << endl;
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << endl;
	SetColor(RED);
	cout << "[�˸�] ��ũ������ �����ϱ����� ������ �μ��Դϴ�." << endl;
	cout << "[�˸�] Ȯ���� ���� �μ����ּ��� !!" << endl;
	//const wchar_t* filename = L"print.jpg";
	//PrintImage(filename);
	//return 0;
	const wchar_t* pdfPath = L"print.pdf";
	PrintPDF(pdfPath);
	return 0;
}

/*int main()
{
	SetConsoleTitle(L"����ý���");
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << "                 ����ý���                    " << endl;
	SetColor(GREEN);
	cout << "                          Code By. SIYUL PARK" << endl;
	SetColor(SKYBLUE);
	cout << "______________________________________________" << endl;
	cout << endl;
	SetColor(RED);
	cout << "[�˸�] ��ũ������ �����ϱ����� ������ �μ��Դϴ�." << endl;
	cout << "[�˸�] Ȯ���� ���� �μ����ּ��� !!" << endl;

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
		wprintf(L"������ ���� ��� �Ǵ� ���� �߻�\n");
		return;
	}

	HDC hdcPrinter = pd.hDC;
	if (!hdcPrinter)
	{
		wprintf(L"������ DC ���� ����\n");
		return;
	}

	// GDI+�� ����Ͽ� �̹��� �ε�
	Image image(filePath.c_str());
	if (image.GetLastStatus() != Ok)
	{
		wprintf(L"�̹��� �ε� ����\n");
		DeleteDC(hdcPrinter);
		return;
	}

	// ����Ʈ �۾� ����
	DOCINFO di = { 0 };
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = L"Print JPEG Document";

	if (StartDoc(hdcPrinter, &di) > 0)
	{
		if (StartPage(hdcPrinter) > 0)
		{
			// �̹��� �μ�
			Graphics graphics(hdcPrinter);
			if (graphics.DrawImage(&image, 0, 0, image.GetWidth(), image.GetHeight()) == Ok)
			{
				SetColor(GREEN);
				cout << "[����] �̹��� �μ� ���� !!";
				//wprintf(L"�̹��� �μ� ����\n");
			}
			else
			{
				SetColor(RED);
				cout << "[����] �̹��� �μ� ���� !!";
			}
			EndPage(hdcPrinter);
		}
		EndDoc(hdcPrinter);
	}
	else
	{
		cout << "[����] ����Ʈ �۾� ���� ���� !!";
	}

	// ������ DC ����
	DeleteDC(hdcPrinter);
}

void PrintImage(const wchar_t* filename)
{
	// GDI+ �ʱ�ȭ
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// ������ ����
	PRINTDLG pd;
	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.Flags = PD_RETURNDC;
	PrintDlg(&pd);

	if (pd.hDC)
	{
		// �̹��� �ε�
		Image image(filename);

		// DC�� �̹����� ���
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

	// GDI+ ����
	GdiplusShutdown(gdiplusToken);
}
