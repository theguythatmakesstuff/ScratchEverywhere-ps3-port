/**
 * OggAudioPlayer - A threaded Ogg Vorbis audio player for Nintendo 3DS
 * 
 * Implementation file for OggAudioPlayer class
 */

#include "oggAudioPlayer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// ---- OggAudioPlayer Implementation ----

OggAudioPlayer::OggAudioPlayer() 
    : mFileHandle(nullptr), 
      mAudioBuffer(nullptr),
      mThreadId(nullptr),
      mQuit(false),
      mPlaying(false),
      mFinished(false),
      mLoaded(false),
      mChannels(0),
      mSampleRate(0) {
    
    // Initialize the synchronization event
    LightEvent_Init(&mEvent, RESET_ONESHOT);
}

OggAudioPlayer::~OggAudioPlayer() {
    // Stop playback and clean up
    stop();
    cleanupAudio();
}

bool OggAudioPlayer::load(const std::string& path) {
    // Cleanup any previous audio
    stop();
    cleanupAudio();
    
    // Reset state flags
    mFinished = false;
    mLoaded = false;
    
    // Open the Ogg Vorbis file
    mFileHandle = fopen(path.c_str(), "rb");
    if (!mFileHandle) {
        printf("OggAudioPlayer: Failed to open file: %s\n", path.c_str());
        return false;
    }
    
    // Initialize OggVorbis decoder
    int error = ov_open(mFileHandle, &mVorbisFile, NULL, 0);
    if (error) {
        printf("OggAudioPlayer: Failed to initialize decoder: error %d (%s)\n", 
               error, vorbisStrError(error));
        fclose(mFileHandle);
        mFileHandle = nullptr;
        return false;
    }
    
    // Get audio format info
    vorbis_info* vi = ov_info(&mVorbisFile, -1);
    mChannels = vi->channels;
    mSampleRate = vi->rate;
    
    // Initialize audio system
    if (!initAudio()) {
        printf("OggAudioPlayer: Failed to initialize audio system\n");
        ov_clear(&mVorbisFile);
        return false;
    }
    
    mLoaded = true;
    return true;
}

bool OggAudioPlayer::play() {
    if (!mLoaded) {
        printf("OggAudioPlayer: No file loaded\n");
        return false;
    }
    
    if (mPlaying) {
        // Already playing
        return true;
    }
    
    // Reset finished flag
    mFinished = false;
    
    // Set callback for NDSP
    ndspSetCallback(ndspCallback, this);
    
    // Set the thread priority to the main thread's priority ...
    int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    // ... then subtract 1, as lower number => higher actual priority ...
    priority -= 1;
    // ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;
    
    // Start the audio thread
    mQuit = false;
    mThreadId = threadCreate(audioThreadFunc, this, 32 * 1024, priority, -1, false);
    if (!mThreadId) {
        printf("OggAudioPlayer: Failed to create audio thread\n");
        return false;
    }
    
    mPlaying = true;
    return true;
}

void OggAudioPlayer::stop() {
    if (!mPlaying) {
        return;
    }
    
    // Signal the audio thread to quit
    mQuit = true;
    LightEvent_Signal(&mEvent);
    
    // Wait for thread to finish
    if (mThreadId) {
        threadJoin(mThreadId, UINT64_MAX);
        threadFree(mThreadId);
        mThreadId = nullptr;
    }
    
    // Reset NDSP channel
    ndspChnReset(0);
    mPlaying = false;
}

bool OggAudioPlayer::isFinished() const {
    return mFinished;
}

bool OggAudioPlayer::isPlaying() const {
    return mPlaying;
}

// Static callback for NDSP
void OggAudioPlayer::ndspCallback(void* data) {
    OggAudioPlayer* player = static_cast<OggAudioPlayer*>(data);
    if (player && !player->mQuit) {
        LightEvent_Signal(&player->mEvent);
    }
}

// Static audio thread function
void OggAudioPlayer::audioThreadFunc(void* arg) {
    OggAudioPlayer* player = static_cast<OggAudioPlayer*>(arg);
    if (player) {
        // Process audio buffers until quit flag is set
        while (!player->mQuit) {
            bool buffersFilled = false;
            
            // Search for available wave buffers and fill them
            for (size_t i = 0; i < ARRAY_SIZE(player->mWaveBufs); ++i) {
                if (player->mWaveBufs[i].status == NDSP_WBUF_DONE) {
                    if (!player->fillBuffer(&player->mWaveBufs[i])) {
                        // End of playback
                        player->mFinished = true;
                        player->mPlaying = false;
                        return;
                    }
                    buffersFilled = true;
                }
            }
            
            // If we didn't fill any buffers, wait for a signal
            if (!buffersFilled) {
                LightEvent_Wait(&player->mEvent);
            }
        }
    }
}

bool OggAudioPlayer::fillBuffer(ndspWaveBuf* waveBuf) {
    // Decode samples until our waveBuf is full
    size_t totalBytes = 0; // changed from int to size_t
    while (totalBytes < waveBuf->nsamples * sizeof(int16_t)) {
        int16_t* buffer = waveBuf->data_pcm16 + (totalBytes / sizeof(int16_t));
        const size_t bufferSize = (waveBuf->nsamples * sizeof(int16_t) - totalBytes);

        // Decode bufferSize bytes from vorbisFile into buffer
        const int bytesRead = ov_read(&mVorbisFile, (char*)buffer, bufferSize, NULL);
        if (bytesRead <= 0) {
            if (bytesRead == 0) break;  // End of file, no error

            printf("OggAudioPlayer: ov_read error %d (%s)", bytesRead,
                   vorbisStrError(bytesRead));
            break;
        }

        totalBytes += bytesRead;
    }

    // If no samples were read, we're at the end of the file
    if (totalBytes == 0) {
        return false;
    }

    // Pass samples to NDSP
    waveBuf->nsamples = totalBytes / sizeof(int16_t);
    ndspChnWaveBufAdd(0, waveBuf);
    DSP_FlushDataCache(waveBuf->data_pcm16, totalBytes);

    return true;
}

bool OggAudioPlayer::initAudio() {
    // Setup NDSP channel
    ndspChnReset(0);
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(0, mSampleRate);
    ndspChnSetFormat(0, mChannels == 1 ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16);
    
    // Allocate audio buffer
    // 120ms buffer for each waveBuf
    const size_t SAMPLES_PER_BUF = mSampleRate * 120 / 1000;
    const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * mChannels * sizeof(int16_t);
    const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(mWaveBufs);
    
    mAudioBuffer = (int16_t*)linearAlloc(bufferSize);
    if (!mAudioBuffer) {
        printf("OggAudioPlayer: Failed to allocate audio buffer\n");
        return false;
    }
    
    // Setup waveBufs for NDSP
    memset(&mWaveBufs, 0, sizeof(mWaveBufs));
    int16_t* buffer = mAudioBuffer;
    
    for (size_t i = 0; i < ARRAY_SIZE(mWaveBufs); ++i) {
        mWaveBufs[i].data_vaddr = buffer;
        mWaveBufs[i].nsamples = WAVEBUF_SIZE / sizeof(int16_t);
        mWaveBufs[i].status = NDSP_WBUF_DONE;
        
        buffer += WAVEBUF_SIZE / sizeof(int16_t);
    }
    
    return true;
}

void OggAudioPlayer::cleanupAudio() {
    // Ensure playback is stopped
    stop();
    
    // Free audio buffer
    if (mAudioBuffer) {
        linearFree(mAudioBuffer);
        mAudioBuffer = nullptr;
    }
    
    // Close vorbis file if open
    if (mLoaded) {
        ov_clear(&mVorbisFile);
        mLoaded = false;
    }
}

// Static helper to convert Vorbis error codes to strings
const char* OggAudioPlayer::vorbisStrError(int error) {
    switch (error) {
        case OV_FALSE:
            return "OV_FALSE: A request did not succeed.";
        case OV_HOLE:
            return "OV_HOLE: There was a hole in the page sequence numbers.";
        case OV_EREAD:
            return "OV_EREAD: An underlying read, seek or tell operation failed.";
        case OV_EFAULT:
            return "OV_EFAULT: A NULL pointer was passed where none was expected, or an internal library error was encountered.";
        case OV_EIMPL:
            return "OV_EIMPL: The stream used a feature which is not implemented.";
        case OV_EINVAL:
            return "OV_EINVAL: One or more parameters to a function were invalid.";
        case OV_ENOTVORBIS:
            return "OV_ENOTVORBIS: This is not a valid Ogg Vorbis stream.";
        case OV_EBADHEADER:
            return "OV_EBADHEADER: A required header packet was not properly formatted.";
        case OV_EVERSION:
            return "OV_EVERSION: The ID header contained an unrecognised version number.";
        case OV_EBADPACKET:
            return "OV_EBADPACKET: An audio packet failed to decode properly.";
        case OV_EBADLINK:
            return "OV_EBADLINK: We failed to find data we had seen before or the stream was sufficiently corrupt that seeking is impossible.";
        case OV_ENOSEEK:
            return "OV_ENOSEEK: An operation that requires seeking was requested on an unseekable stream.";
        default:
            return "Unknown error.";
    }
}
