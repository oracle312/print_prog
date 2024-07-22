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