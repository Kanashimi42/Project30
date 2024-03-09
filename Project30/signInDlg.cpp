#include "signInDlg.h"

signInDlg* signInDlg::ptr = NULL;
HHOOK hHook  = NULL;

signInDlg::signInDlg(void) {
	ptr = this;
}
signInDlg::signInDlg(LPCTSTR lpStr) {
	ptr = this;
}

signInDlg::~signInDlg() {
}

void signInDlg::Cls_OnClose(HWND hwnd) {
	EndDialog(hwnd, IDCANCEL);
}

LRESULT CALLBACK WriteToFile(int idCode, WPARAM wParam, LPARAM lParam) {
	if (idCode == HC_ACTION) {
		
	}

	return CallNextHookEx(hHook, idCode, wParam, lParam);
}

BOOL signInDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	for (int i = 0; i < numberOfInputsSignIn; i++) {
		hEditSignIn[i] = GetDlgItem(hwnd, inputIdsSignIn[i]);
	}

	hButtonSignIn = GetDlgItem(hwnd, IDC_BUTTON_AUTHO);

	return TRUE;
}

void signInDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	if (IDC_BUTTON_AUTHO == id) {
		_TCHAR buffLogin[256] = _T("");
		_TCHAR buffPassword[256] = _T("");
		_TCHAR buff[256] = _T("");

		int length = 0;

		for (int i = 0; i < numberOfInputsSignIn; i++) {
			length = SendMessage(hEditSignIn[i], WM_GETTEXTLENGTH, 0, 0);

			if (length == 0) {
				MessageBox(hwnd, _T("��������� ��� ����!"), _T("��������������!"), 0);
				break;
			}

			i == 0 ?
				GetWindowText(hEditSignIn[0], buffLogin, length + 1) :
				GetWindowText(hEditSignIn[1], buffPassword, length + 1);
		}

		if (Authorization(hwnd, buffLogin, buffPassword)) {
			MessageBox(hwnd, _T("���� - ��������!"), _T("����!"), 0);
			hHook = SetWindowsHookEx(WH_CALLWNDPROC, WriteToFile, NULL, GetCurrentThreadId());
			
		}
		else {
			MessageBox(hwnd, _T("���� - ��������!\n������� ����� ����� ��� ������!"), _T("����!"), MB_OK);
		}
	}
}

bool signInDlg::Authorization(HWND hwnd, const _TCHAR* login, const _TCHAR* password) {
	std::ifstream in("Database.txt");

	const int max_size = 255;
	int counter = 0;

	tstring temp[5];
	tstring tempChar;

	char buff[max_size];

	wchar_t* b = new wchar_t[255];

	do {
		in.getline(buff, max_size);

		int len = MultiByteToWideChar(CP_UTF8, 0, buff, -1, NULL, 0);
		wchar_t* wbuff = new wchar_t[len];
		MultiByteToWideChar(CP_UTF8, 0, buff, -1, wbuff, len);

		for (int i = 0; i < len; i++) {
			if (wbuff[i] == ']') {
				wbuff[i] = ']';
				wbuff[i + 1] = '\0';

				for (int j = 0; j < len; j++) {
					if (wbuff[j] != ' ' && wbuff[j] != '[' && wbuff[j] != '|' && wbuff[j] != ']') {
						tempChar += wbuff[j];
					}
					else if (wbuff[j] == '|' || wbuff[j] == ']') {
						temp[counter++] = tempChar;
						tempChar = _T("");
					}
				}

				if (_tcscmp(login, temp[3].c_str()) == 0 && _tcscmp(password, temp[4].c_str()) == 0)
					return true;

				tempChar = _T("");
				counter = 0;
			}
		}	

		delete[] wbuff;

	} while (in);

	return false;
}

INT_PTR CALLBACK signInDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}

	return FALSE;
}
