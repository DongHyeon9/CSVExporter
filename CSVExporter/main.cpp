#include "CSVExpoter.h"
#include "HeaderGenerater.h"

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    ComInit com;
    CComPtr<IFileOpenDialog> dlg;
    if (SUCCEEDED(dlg.CoCreateInstance(CLSID_FileOpenDialog))) {
        // 파일 선택, 다중선택 허용
        DWORD opts = 0;
        dlg->GetOptions(&opts);
        dlg->SetOptions(opts
            | FOS_ALLOWMULTISELECT
            | FOS_PATHMUSTEXIST
            | FOS_FILEMUSTEXIST
            | FOS_FORCEFILESYSTEM);

        dlg->SetTitle(L"하나 이상의 .xlsx 파일을 선택하세요");

        // 파일 타입 필터: .xlsx 전용
        const COMDLG_FILTERSPEC filter = {
            L"Excel Files (*.xlsx)", L"*.xlsx"
        };
        dlg->SetFileTypes(1, &filter);
        dlg->SetFileTypeIndex(1);

        if (SUCCEEDED(dlg->Show(nullptr))) {
            CComPtr<IShellItemArray> itemArray;
            if (SUCCEEDED(dlg->GetResults(&itemArray))) {
                DWORD count = 0;
                itemArray->GetCount(&count);
                std::wcout << L"선택된 파일 수: " << count << L"\n";
                for (DWORD i = 0; i < count; ++i) {
                    CComPtr<IShellItem> item;
                    itemArray->GetItemAt(i, &item);
                    if (item) {
                        PWSTR pszPath = nullptr;
                        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &pszPath))) {
                            std::wcout << L"• " << pszPath << L"\n";
                            CoTaskMemFree(pszPath);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
