
#include <rack.hpp>

using float_4 = rack::simd::float_4;

class Autowah4 {

/** Autowah module

    Original author(s) :

    DaisySP version: Ported from soundpipe by Ben Sergentanis, May 2020

    Adapted to faster, 4 string version by @SquinkyLabs, Jan 2023
*/

public:
    Autowah4() {}
    ~Autowah4() {}
    /** Initializes the Autowah module.
        \param sample_rate - The sample rate of the audio engine being run.
    */
    void Init(float sample_rate);

    /** Initializes the Autowah module.
        \param in - input signal to be wah'd
    */
    rack::simd::float_4 Process(rack::simd::float_4 in);

    /** Change the sample rate
        \param sample_rate
    */
   void setSampleRate(float sample_rate);

    /** sets wah
        \param wah : set wah amount, , 0...1.0
    */
    inline void SetWah(float wah) { wah_ = wah; }
    /** sets mix amount
        \param drywet : set effect dry/wet, 0...100.0
    */
    inline void SetDryWet(float drywet) { wet_dry_ = drywet; }
    /** sets wah level
        \param level : set wah level, 0...1.0
    */
    inline void SetLevel(float level) { level_ = level; }

private:
    float sampling_freq_, const1_, const2_, wah_, level_, wet_dry_;
    float_4 const4_;
    float_4 rec3_[2], rec0_[3], rec1_[2], rec2_[2], rec4_[2], rec5_[2];
};

inline void Autowah4::Init(float sample_rate) {
    sampling_freq_ = sample_rate;
    const1_ = 1413.72f / sampling_freq_;
    const2_ = expf(0.0f - (100.0f / sampling_freq_));
    const4_ = expf(0.0f - (10.0f / sampling_freq_));

    wet_dry_ = 100.0f;
    level_ = 0.1f;
    wah_ = 0.0;

    for (int i = 0; i < 2; i++) {
        rec1_[i] = rec2_[i] = rec3_[i] = rec4_[i] = rec5_[i] = 0.0f;
    }

    for (int i = 0; i < 3; i++) {
        rec0_[i] = 0.0f;
    }
}

void Autowah4::setSampleRate(float sample_rate) {
    sampling_freq_ = sample_rate;
    const1_ = 1413.72f / sampling_freq_;
    const2_ = expf(0.0f - (100.0f / sampling_freq_));
    const4_ = expf(0.0f - (10.0f / sampling_freq_));
    
    for (int i = 0; i < 2; i++) {
        rec1_[i] = rec2_[i] = rec3_[i] = rec4_[i] = rec5_[i] = 0.0f;
    }

    for (int i = 0; i < 3; i++) {
        rec0_[i] = 0.0f;
    }

}

inline float_4 min4(float_4 a, float_4 b) {
    return ifelse(a < b, a, b);
}
inline float_4 max4(float_4 a, float_4 b) {
    return ifelse(a > b, a, b);
}

inline rack::simd::float_4 Autowah4::Process(rack::simd::float_4 in) {
    rack::simd::float_4 out;
    float fSlow2 = (0.01f * (wet_dry_ * level_));
    float fSlow3 = (1.0f - 0.01f * wet_dry_) + (1.f - wah_);

    float_4 fTemp1 = abs(in);
    {
        float_4 temp = (const4_ * rec3_[1]) + ((1.0f - const4_) * fTemp1);

        rec3_[0] = max4(fTemp1, temp);
    }
    rec2_[0] = (const2_ * rec2_[1]) + ((1.0f - const2_) * rec3_[0]);
    float_4 fTemp2 = min4(1.0f, rec2_[0]);
    float_4 fTemp3 = rack::simd::pow(float_4(2.0f), (float_4(2.3f) * fTemp2));
    float_4 fTemp4 = 1.0f - (const1_ * fTemp3 / rack::simd::pow(2.0f, (1.0f + 2.0f * (1.0f - fTemp2))));
    rec1_[0] = ((0.999f * rec1_[1]) + (0.001f * (0.0f - (2.0f * (fTemp4 * rack::simd::cos((const1_ * 2 * fTemp3)))))));
    rec4_[0] = ((0.999f * rec4_[1]) + (0.001f * fTemp4 * fTemp4));
    rec5_[0] = ((0.999f * rec5_[1]) + (0.0001f * rack::simd::pow(4.0f, fTemp2)));
    rec0_[0] = (0.0f - (((rec1_[0] * rec0_[1]) + (rec4_[0] * rec0_[2])) - (fSlow2 * (rec5_[0] * in))));

    out = ((wah_ * (rec0_[0] - rec0_[1])) + (fSlow3 * in));
    rec3_[1] = rec3_[0];
    rec2_[1] = rec2_[0];
    rec1_[1] = rec1_[0];
    rec4_[1] = rec4_[0];
    rec5_[1] = rec5_[0];
    rec0_[2] = rec0_[1];
    rec0_[1] = rec0_[0];

    return out;
}