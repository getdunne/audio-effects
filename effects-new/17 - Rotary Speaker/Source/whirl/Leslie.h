#pragma once
#include <stdint.h>

extern "C" {
#include "eqcomp.h"
#include "whirl.h"
}

struct Filter {
    float *type, *freq, *qual, *gain; // ports
    iir_t* W[2];                      /* pointers to coeffs, drum-filter x2 */
    /* state */
    float _f, _q, _g;
    int   _t;
};

struct B3W {
    /* audio ports */
    const float *input;
    float *outL, *outR;

    /* control ports */
    float* rev_select; // speed select 0..8

    float *horn_brake, *horn_accel, *horn_decel, *horn_slow, *horn_fast;
    float *drum_brake, *drum_accel, *drum_decel, *drum_slow, *drum_fast;

    float *horn_level, *drum_level;
    float *drum_width, *horn_leak, *horn_width;

    float *horn_radius, *drum_radius;
    float *horn_xoff, *horn_zoff, *mic_dist, *mic_angle;

    float* p_resend_trigger; // GUI retrigger
    float* p_link_speed;     // GUI setting

    /* output ports */
    float *c_horn_rpm, *c_drum_rpm;
    float *c_horn_ang, *c_drum_ang;

    // Filter/EQ states & port-map
    Filter flt[3];

    /* internal state */
    float o_rev_select;
    float o_horn_brake, o_horn_accel, o_horn_decel, o_horn_slow, o_horn_fast;
    float o_drum_brake, o_drum_accel, o_drum_decel, o_drum_slow, o_drum_fast;
    float o_horn_level, o_drum_level;
    float o_drum_width, o_horn_leak, o_horn_width;

    float o_horn_radius, o_drum_radius;
    float o_horn_xoff, o_horn_zoff, o_mic_dist, o_mic_angle;

    int spd_horn, spd_drum, last_spd;

    // cached coefficients (for dB values)
    float x_drum_width;
    float x_dll, x_dlr, x_drl, x_drr;
    float x_horn_width;
    float x_hll, x_hlr, x_hrl, x_hrr;

    // fade in/out for re-configuration
    bool     fade_dir; // true: fade-out, false: fade-in
    uint32_t fade;     // fade counter 0..FADED

    /* actual effect instance, whirl.c */
    struct b_whirl* whirl;

    /* misc */
    double   rate, nyq;  // sample-rate, max EQ freq
    float    lpf1, lpf2; // Low-pass parameter
    int      resend_trigger;
    uint32_t resend_data_to_ui;
};

class Leslie
{
public:
    Leslie();
    ~Leslie();

    void init(double sampleRate);
    void deinit();

    // speed range is 0..8, expressed as float but in steps
    void setSpeed(float spd) { rev_select = spd; }
    float getSpeed() { return rev_select; }

    void render(int numSamples, const float* inBuffers[2], float* outBuffers[2]);

protected:

    // control ports
    float rev_select; // speed select 0..8

    float horn_brake, horn_accel, horn_decel, horn_slow, horn_fast;
    float drum_brake, drum_accel, drum_decel, drum_slow, drum_fast;

    float horn_level, drum_level;
    float drum_width, horn_leak, horn_width;

    float horn_radius, drum_radius;
    float horn_xoff, horn_zoff, mic_dist, mic_angle;

    float p_resend_trigger; // GUI retrigger
    float p_link_speed;     // GUI setting

    // filter ports
    float filtA_type, filtA_freq, filtA_qual, filtA_gain;
    float filtB_type, filtB_freq, filtB_qual, filtB_gain;
    float filtD_type, filtD_freq, filtD_qual, filtD_gain;

    // output ports
    float c_horn_rpm, c_drum_rpm;
    float c_horn_ang, c_drum_ang;

    void set_speed();
    int interpolate_filter(Filter* flt);
    bool faded();
    int reconfigure();
    void process(uint32_t n_samples, const float* in, float* outL, float* outR);

    B3W* b3w;
};
