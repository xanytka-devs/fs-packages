#ifndef IO_OS_H
#define IO_OS_H

#include <string>

namespace firesteel {
	class OS {
	public:
		/// <summary>
		/// Open a dialog to get file path from user.
		/// </summary>
		struct file_dialog {
			// Deafult return value (if file isn't gotten).
			std::string default_file = "";
			// Path to start in.
			const char* path = NULL;
			// Extension filter.
			const char* filter = "All\0*.*\0";
			// Index of filter's item.
			int filter_id = 0;
			/// <summary>
			/// Opens file dialog and gets file name. Otherwise returns set default.
			/// </summary>
			/// <returns>File path.</returns>
			std::string open() const;
		};

		enum WindowStyle {
			WS_NONE = 0x0,
			WS_NO_MAXIMIZE = 0x1,
			WS_NO_MINIMIZE = 0x2,
			WS_NO_RESIZE = 0x3
		};
		static void set_window_style(WindowStyle t_style);
		static void open_url(const char* t_url);
	};
}

#endif // IO_OS_H