/**
 * OggAudioPlayer - A threaded Ogg Vorbis audio player for Nintendo 3DS
 * 
 * Provides an object-oriented interface for playing Ogg Vorbis audio files
 * using libvorbisidec (tremor) and libctru's NDSP service.
 * 
 * Based on original code by Théo B. (LiquidFenrir) and Lauren Kelly (thejsa)
 */

#ifndef OGG_AUDIO_PLAYER_HPP
#define OGG_AUDIO_PLAYER_HPP

#include <tremor/ivorbisfile.h>
#include <tremor/ivorbiscodec.h>
#include <3ds.h>

#include <string>
#include <atomic>

class OggAudioPlayer {
public:
    OggAudioPlayer();
    ~OggAudioPlayer();

    /**
     * Load an Ogg Vorbis file from the specified path
     * @param path Path to the Ogg Vorbis file
     * @return true if successful, false otherwise
     */
    bool load(const std::string& path);

    /**
     * Start or resume playback of the loaded audio file
     * @return true if playback started successfully, false otherwise
     */
    bool play();

    /**
     * Stop playback of the current audio file
     */
    void stop();

    /**
     * Check if audio playback has completed
     * @return true if playback has completed, false otherwise
     */
    bool isFinished() const;

    /**
     * Check if audio is currently playing
     * @return true if audio is playing, false otherwise
     */
    bool isPlaying() const;

private:
    // Audio thread function and callback
    static void audioThreadFunc(void* arg);
    static void ndspCallback(void* data);

    // Buffer filling function
    bool fillBuffer(ndspWaveBuf* waveBuf);

    // Helper functions
    bool initAudio();
    void cleanupAudio();
    static const char* vorbisStrError(int error);

    // Audio state
    OggVorbis_File mVorbisFile;
    FILE* mFileHandle;
    ndspWaveBuf mWaveBufs[3];
    int16_t* mAudioBuffer;
    LightEvent mEvent;
    Thread mThreadId;
    
    // Thread control flags
    std::atomic<bool> mQuit;
    std::atomic<bool> mPlaying;
    std::atomic<bool> mFinished;
    bool mLoaded;

    // Audio format info
    int mChannels;
    int mSampleRate;
};

#endif // OGG_AUDIO_PLAYER_HPP
