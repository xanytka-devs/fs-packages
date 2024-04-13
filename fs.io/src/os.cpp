#include "../include/os.hpp"
#include <wtypes.h>
#include <shellapi.h>
#include <filesystem>

namespace firesteel {

	std::string OS::file_dialog::open() const {
        OPENFILENAME ofn;       //Common dialog box structure.
        char szFile[260]{};     //Buffer for file name.
        HWND hwnd = nullptr;    //Owner window.
        HANDLE hf = nullptr;    //File handle.
        //Initialize OPENFILENAME.
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        //Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
        // use the contents of szFile to initialize itself.
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = filter_id;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = path;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        //Display the Open dialog box. 
        if(GetOpenFileName(&ofn) == TRUE)
            return std::filesystem::u8path(ofn.lpstrFile).u8string();
        return default_file;
	}

    void OS::set_window_style(WindowStyle t_style) {
        HWND window_handle = GetForegroundWindow();
        long style = GetWindowLong(window_handle, GWL_STYLE);
        switch (t_style) {
        case firesteel::OS::WS_NO_MAXIMIZE:
            style &= ~WS_MAXIMIZEBOX;
            break;
        case firesteel::OS::WS_NO_MINIMIZE:
            style &= ~WS_MINIMIZEBOX;
            break;
        case firesteel::OS::WS_NO_RESIZE:
            style &= ~WS_THICKFRAME;
            break;
        default:
            break;
        }
        SetWindowLong(window_handle, GWL_STYLE, style);
    }

    void OS::open_url(const char* t_url) {
        ShellExecute(0, 0, t_url, 0, 0, SW_SHOW);
    }

}