#ifndef AUDIO_H
#define AUDIO_H

#include <string>

namespace firesteel {
	class AudioManager {
	public:
		/// <summary>
		/// Initializes MiniAudio.
		/// </summary>
		static void initialize();
		/// <summary>
		/// Logs host device information.
		/// </summary>
		static void print_host_info();
		/// <summary>
		/// Is audio manager initialized.
		/// </summary>
		/// <returns>State.</returns>
		static bool is_active();
		/// <summary>
		/// Shutdown AudioManager (cleanup).
		/// </summary>
		static void remove();
	private:
		static bool m_is_active;
	};

	struct AudioLayer {
		const char* name = "new_audio_layer";
		float volume = 100.f;
		float pitch = 1.f;
	};

	static const AudioLayer master_layer{"master"};
	class Audio {
	public:
		Audio() : loop(false), m_source("") { }
		Audio(std::string t_source, bool t_loop = false, const AudioLayer t_layer = master_layer, bool t_play_on_awake = false);
		/// <summary>
		/// Is audio source currently playing.
		/// </summary>
		/// <returns>State.</returns>
		bool is_playing() const;

		/// <summary>
		/// Play audio source.
		/// </summary>
		void play();
		/// <summary>
		/// Stops audio source.
		/// </summary>
		void stop();
		/// <summary>
		/// Remove audio (cleanup).
		/// </summary>
		void remove();
		/// <summary>
		/// Get audio file source.
		/// </summary>
		/// <returns>Audio file path.</returns>
		std::string get_source() { return m_source; }
		/// <summary>
		/// Get length of audio file.
		/// </summary>
		/// <returns>Length of audio file.</returns>
		int get_length();
		/// <summary>
		/// Set current source.
		/// </summary>
		/// <param name="t_source">New audio file path.</param>
		void set_source(std::string t_source);
		bool loop;
	private:
		std::string m_source;
		AudioLayer m_layer;
	};
}

#endif // AUDIO_H