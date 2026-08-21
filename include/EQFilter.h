#pragma once
#include <math.h>
#include <stdint.h>

// Simple 5-band graphic EQ built from cascaded RBJ "peaking" biquad filters
// (one filter per band, applied in series). Kept self-contained with no
// dependency on State.h so it can be included from AudioCodecs.h, which is
// itself included by State.h.
//
// One EQBand holds coefficients shared by both stereo channels, plus
// separate filter history (x1/x2/y1/y2) per channel so left/right don't
// bleed into each other.

#define EQ_BAND_COUNT 5

inline const float EQ_BAND_FREQS[EQ_BAND_COUNT] = {60.0f, 250.0f, 1000.0f, 4000.0f, 12000.0f};

// Persisted state (settings.cfg) - gain in dB per band, -12..+12, and a
// master on/off toggle. Defaults to flat/off until loaded or changed.
inline bool eqEnabled = false;
inline int8_t eqGainDb[EQ_BAND_COUNT] = {0, 0, 0, 0, 0};

// Sample rate most recently reported by the active codec (music files and
// radio streams can differ, e.g. 22050Hz vs 44100Hz), so coefficients can be
// recomputed correctly whenever it changes.
inline float eqLastSampleRate = 44100.0f;

struct EQBand
{
    float freq = 1000.0f;
    float b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
    float x1[2] = {0, 0}, x2[2] = {0, 0}, y1[2] = {0, 0}, y2[2] = {0, 0};

    // Q fixed at ~1.0 (roughly one-octave bandwidth) - simple and musical
    // enough for a small on-device graphic EQ.
    void computeCoeffs(float gainDb, float sampleRate)
    {
        if (sampleRate < 1000.0f)
            sampleRate = 44100.0f;
        const float Q = 1.0f;
        float A = powf(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * (float)M_PI * freq / sampleRate;
        float alpha = sinf(w0) / (2.0f * Q);
        float cosw0 = cosf(w0);

        float bb0 = 1 + alpha * A;
        float bb1 = -2 * cosw0;
        float bb2 = 1 - alpha * A;
        float aa0 = 1 + alpha / A;
        float aa1 = -2 * cosw0;
        float aa2 = 1 - alpha / A;

        b0 = bb0 / aa0;
        b1 = bb1 / aa0;
        b2 = bb2 / aa0;
        a1 = aa1 / aa0;
        a2 = aa2 / aa0;
    }

    inline int16_t process(int16_t in, int ch)
    {
        float x0 = (float)in;
        float y0 = b0 * x0 + b1 * x1[ch] + b2 * x2[ch] - a1 * y1[ch] - a2 * y2[ch];
        x2[ch] = x1[ch];
        x1[ch] = x0;
        y2[ch] = y1[ch];
        y1[ch] = y0;
        if (y0 > 32767.0f)
            y0 = 32767.0f;
        else if (y0 < -32768.0f)
            y0 = -32768.0f;
        return (int16_t)y0;
    }

    void resetHistory()
    {
        x1[0] = x1[1] = x2[0] = x2[1] = 0;
        y1[0] = y1[1] = y2[0] = y2[1] = 0;
    }
};

inline EQBand eqBands[EQ_BAND_COUNT];

// Recomputes all band coefficients from the current eqGainDb values for the
// given sample rate. Call whenever a gain changes or the output sample rate
// changes.
inline void eqRecompute(float sampleRate)
{
    eqLastSampleRate = sampleRate;
    for (int i = 0; i < EQ_BAND_COUNT; i++)
    {
        eqBands[i].freq = EQ_BAND_FREQS[i];
        eqBands[i].computeCoeffs((float)eqGainDb[i], sampleRate);
    }
}

// Clears filter history - call at the start of a new stream/track so the
// EQ doesn't carry over stale state from whatever played before.
inline void eqResetHistory()
{
    for (int i = 0; i < EQ_BAND_COUNT; i++)
        eqBands[i].resetHistory();
}

// Applies the 5-band cascade in place to one interleaved stereo sample pair.
inline void eqProcessSample(int16_t sample[2])
{
    for (int i = 0; i < EQ_BAND_COUNT; i++)
    {
        sample[0] = eqBands[i].process(sample[0], 0);
        sample[1] = eqBands[i].process(sample[1], 1);
    }
}
