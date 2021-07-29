#include <windows.h>
#include <iostream> 
#include <conio.h> //_getch()
#include <io.h> 
#include <fcntl.h> 
#include <string>

using namespace std;


WCHAR* calc(WCHAR* str) {
	
	WCHAR* res;
	int x1 = 0, x2 = 0, x3 = 0;
	
	int i = 0;
	while (str[i] != '\0') {
		if (str[i+1] >=L'0' && str[i+1] <= L'9') {
			if (str[i] == '+') {
				while (str[i + 1] != '+' && str[i + 1] != '-' && str[i + 1] != '*' && str[i + 1] != '/' && str[i + 1] != '\0')
				{
					i++;
					x3 = x3 * 10 + (str[i] - L'0');
				}

				x1 += x2;
				x2 = x3;
				x3 = 0;
			}
			else if (str[i] == '-') {
				while (str[i + 1] != '+' && str[i + 1] != '-' && str[i + 1] != '*' && str[i + 1] != '/' && str[i + 1] != '\0')
				{
					i++;
					x3 = x3 * 10 + (str[i] - L'0');
				}
				x1 += x2;
				x2 = -x3;
				x3 = 0;

			}
			else if (str[i] == '*') {
				while (str[i + 1] != '+' && str[i + 1] != '-' && str[i + 1] != '*' && str[i + 1] != '/' && str[i + 1] != '\0')
				{
					i++;
					x3 = x3 * 10 + (str[i] - L'0');
				}
				x2 *= x3;
				x3 = 0;
			}
			else if (str[i] == '/') {
				while (str[i + 1] != '+' && str[i + 1] != '-' && str[i + 1] != '*' && str[i + 1] != '/' && str[i + 1] != '\0')
				{
					i++;
					x3 = x3 * 10 + (str[i] - L'0');
				}
				if (x3 == 0) {
					res = new WCHAR[24];
					wchar_t zeroErr[23] = L"Ошибка деления на ноль";
					for (int i = 0; i < 23; i++)
						res[i] = zeroErr[i];

					return res;
					delete[] res;
				}
				x2 /= x3;
				x3 = 0;
			}
		}
		else {
			res = new WCHAR[1];
			res[0] = L'\0';
			return res;
			delete[] res;
		}
		i++;
	}
	
	x1 += x2;
	int len = to_string(x1).length();
	res = new WCHAR[len + 1];
	
	if (x1 < 0) {
		res[0] = L'-';
		x1 *= -1;
		for (int i = 1; i < len; i++) {
			int t = pow(10, len - i - 1);
			res[i] = L'0' + (x1 / t);
			x1 = x1 % t;
		}
	}

	else {
		for (int i = 0; i < len; i++) {
			
			int t = pow(10, len - i - 1);
			res[i] = L'0' + (x1 / t);
			x1 = x1 % t;
		}
	}

	res[len] = L'\0';
	return res;
	delete[]res;
}

int createPipe(HANDLE hNamedPipe) {

	wchar_t pipeName[] = L"\\\\.\\pipe\\NamedPipe";

	hNamedPipe = CreateNamedPipe(
		pipeName, 
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_WAIT, 
		PIPE_UNLIMITED_INSTANCES, 
		0,
		0, 
		INFINITE, 
		NULL  
	);
	
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		wcerr << L"Ошибка при создании клиента" << endl
			<< L"Код ошибки " << GetLastError() << endl;
		_getch();
		return 0;
	}

	DWORD dwBytesWritten; 
	DWORD dwBytesRead; 

	
	wcout << L"Ожидание клиента" << endl;
	if (!ConnectNamedPipe(hNamedPipe, NULL))
	{
		wcerr << L"Не удалось дождаться клиента" << endl;
		CloseHandle(hNamedPipe); 
		_getch();
		return 0;
	}
	else {
		wcerr << L"Подключился" << endl;
	}

	WCHAR* data = new WCHAR[0], * data2 = new WCHAR[1];
	data2[0] = '\0';

	while (true) {
		int len;
		if (!ReadFile(
			hNamedPipe,
			&len,
			sizeof(len),
			&dwBytesRead,
			NULL
		))
		{
			wcerr << L"Не удалось считать данные из канала" << endl;
			break;
		}

		if (!ReadFile(
			hNamedPipe,
			data,
			len * sizeof(WCHAR),
			&dwBytesRead,
			NULL
		))
		{
			wcerr << L"Не удалось считать данные из канала" << endl;
			break;
		}

		wcout << L"Получено от клиента> " << data << endl;


		bool flag = false, contin = false;
		int i = 1;
		if (data[0] == L'+' || data[0] == L'-' || data[0] == L'*' || data[0] == L'/') {
			while (data[i] != L'\0') {

				if (data[i] < L'0' || data[i] > L'9') {
					flag = false;
					break;
				}
				flag = true;
				i++;
			}
		}

		int len2;
		if (wcscmp(data, L"calc") == 0)
		{
			wcout << L"Получен запрос на вычисление> " << data << endl;
			data2 = calc(data2);
			len2 = wcslen(data2) + 1;
			contin = true;
		}

		else if (wcscmp(data, L"delete") == 0)
		{
			delete[] data2;
			data2 = new WCHAR[1];
			data2[0] = L'\0';
			len2 = 1;
		}

		else if (!flag) {
			delete[] data2;
			data2 = new WCHAR[29];
			wchar_t zeroErr[28] = L"Ошибка ввода, введите число";
			for (int i = 0; i < 28; i++)
				data2[i] = zeroErr[i];
			len2 = 29;
			contin = true;
			//	delete[] data;
		}

		else {
			int len1;
			if (data2 != 0)
				len1 = wcslen(data2);
			else len1 = 0;

			len2 = len1 + len;

			WCHAR* tmp = new WCHAR[len2];
			for (int i = 0; i < len1; ++i) {
				tmp[i] = data2[i];
			}

			for (int j = len1, k = 0; j < len2; j++) {
				tmp[j] = data[k++];
			}

			data2 = tmp;
		}

		wcout << L"Данные на клиент " << data2 << endl;

		if (!WriteFile(
			hNamedPipe,
			&len2,
			sizeof(len2),
			&dwBytesWritten,
			NULL
		))
		{
			wcerr << L"Не удалось считать данные из канала" << endl;
			break;
		}

		if (!WriteFile(
			hNamedPipe,
			data2,
			(len2) * sizeof(WCHAR),
			&dwBytesWritten,
			NULL
		))
		{
			wcerr << L"Не удалось считать данные из канала" << endl;
			break;
		}

		if (contin){
			delete[] data2;
		data2 = new WCHAR[1];
		data2[0] = L'\0';
		len2 = 1;
	}

		Sleep(1000);
	}
	delete[] data;
	delete[] data2;
}

int main()
{
	
	HANDLE hNamedPipe = NULL;

	_setmode(_fileno(stdout), _O_U16TEXT); 
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);

	createPipe(hNamedPipe);
	_getch();

	CloseHandle(hNamedPipe);

	return 0;
}