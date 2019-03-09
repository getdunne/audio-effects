#include "Leslie.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

Leslie::Leslie()
{
    b3w = nullptr;

    rev_select = 4.0f;      // speed select, 0 .. 8

    horn_brake = 0.0f;      // horn brake position, 0 - 1
    horn_accel = 0.161f;    // horn acceleration time-constant, 0.001 - 10 sec
    horn_decel = 0.321f;    // horn deceleration time-constant, 0.001 - 10 sec
    horn_slow = 40.32f;     // horn speed slow, 5 - 200 rpm
    horn_fast = 423.36f;    // horn speed fast, 100 - 1000 rpm

    drum_brake = 0.0f;      // drum brake position, 0 - 1
    drum_accel = 4.127f;    // drum acceleration time-constant, 0.01 - 20
    drum_decel = 1.371f;    // drum deceleration time-constant, 0.01 - 20
    drum_slow = 36.0f;      // drum speed slow, 5 - 100 rpm
    drum_fast = 357.3f;     // drum speed fast, 60 - 600 rpm

    horn_level = 0.0f;      // -20 to +20 dB
    drum_level = 0.0f;      // -20 to +20 dB
    drum_width = 1.0f;      // drum stereo width, 0 .. 2
    horn_leak = -16.47f;    // horn signal leakage, -80 to -3 dB
    horn_width = 1.0f;      // horn stereo width, 0 .. 2

    horn_radius = 19.2f;    // horn radius, 9 - 50 cm
    drum_radius = 22.0f;    // drum radius, 9 - 50 cm
    horn_xoff = 0.0f;       // horn X-axis offset, -20 to +20 cm
    horn_zoff = 0.0f;       // horn Z-axis offset, -20 to +20 cm
    mic_dist = 42.0f;       // microphone distance, 9 - 150 cm
    mic_angle = 180.0f;     // microphone angle, 0 - 180 deg

    p_resend_trigger = 0.0f;    // GUI to plugin notification, 0 .. 1
    p_link_speed = 0.0f;        // link speed control, -1 .. 1

    // filter ports
    filtA_type = 0.0f;      // horn filter-1 type, 0 .. 8
    filtA_freq = 4500.0f;   // horn filter-1 freq, 250 - 8000 Hz
    filtA_qual = 2.7456f;   // horn filter-1 quality, 0.01 - 6.0
    filtA_gain = -30.0f;    // horn filter-1 gain, -48 to +48 dB
    filtB_type = 7.0f;      // horn filter-2 type, 0 .. 8
    filtB_freq = 300.0f;    // horn filter-2 freq, 250 - 8000 Hz
    filtB_qual = 1.0f;      // horn filter-2 quality, 0.01 - 6.0
    filtB_gain = -30.0f;    // horn filter-2 gain, -48 to +48 dB
    filtD_type = 8.0f;      // drum filter type, 0 .. 8
    filtD_freq = 811.9695f; // drum filter freq, 50 - 8000 Hz
    filtD_qual = 1.6016f;   // drum filter quality, 0.01 - 6.0
    filtD_gain = -38.9291f; // drum filter gain, -48 to +48 dB

    // output ports
    c_horn_rpm = 0.0f;      // current horn speed, rpm
    c_drum_rpm = 0.0f;      // current drum speed, rpm
    c_horn_ang = 0.0f;      // current horn angle, 0 - 1
    c_drum_ang = 0.0f;      // current drum angle, 0 - 1
}

Leslie::~Leslie()
{
}

void Leslie::init(double rate)
{
    b3w = (B3W*)calloc(1, sizeof(B3W));
    b3w->whirl = allocWhirl();

    // SD Connect ports
    b3w->rev_select = &rev_select;
    b3w->horn_brake = &horn_brake;
    b3w->horn_accel = &horn_decel;
    b3w->horn_slow = &horn_slow;
    b3w->horn_fast = &horn_fast;
    b3w->drum_brake = &drum_brake;
    b3w->drum_accel = &drum_decel;
    b3w->drum_slow = &drum_slow;
    b3w->drum_fast = &drum_fast;
    b3w->horn_level = &horn_level;
    b3w->drum_level = &drum_level;
    b3w->drum_width = &drum_width;
    b3w->horn_leak = &horn_leak;
    b3w->horn_width = &horn_width;
    b3w->horn_radius = &horn_radius;
    b3w->drum_radius = &drum_radius;
    b3w->horn_xoff = &horn_xoff;
    b3w->horn_zoff = &horn_zoff;
    b3w->mic_dist = &mic_dist;
    b3w->mic_angle = &mic_angle;
    b3w->p_resend_trigger = &p_resend_trigger;
    b3w->p_link_speed = &p_link_speed;
    b3w->flt[0].type = &filtA_type;
    b3w->flt[0].freq = &filtA_freq;
    b3w->flt[0].qual = &filtA_qual;
    b3w->flt[0].gain = &filtA_gain;
    b3w->flt[1].type = &filtB_type;
    b3w->flt[1].freq = &filtB_freq;
    b3w->flt[1].qual = &filtB_qual;
    b3w->flt[1].gain = &filtB_gain;
    b3w->flt[2].type = &filtD_type;
    b3w->flt[2].freq = &filtD_freq;
    b3w->flt[2].qual = &filtD_qual;
    b3w->flt[2].gain = &filtD_gain;

    b3w->c_horn_rpm = &c_horn_rpm;
    b3w->c_drum_rpm = &c_drum_rpm;
    b3w->c_horn_ang = &c_horn_ang;
    b3w->c_drum_ang = &c_drum_ang;

    initWhirl(b3w->whirl, NULL, rate);

    // reference filters
    b3w->flt[0].W[0] = b3w->whirl->hafw;
    b3w->flt[1].W[0] = b3w->whirl->hbfw;
    b3w->flt[2].W[0] = b3w->whirl->drfL;
    b3w->flt[2].W[1] = b3w->whirl->drfR;

    // get defaults
    b3w->flt[0]._t = b3w->whirl->haT;
    b3w->flt[0]._f = b3w->whirl->haF;
    b3w->flt[0]._g = b3w->whirl->haG;
    b3w->flt[0]._q = b3w->whirl->haQ;

    b3w->flt[1]._t = b3w->whirl->hbT;
    b3w->flt[1]._f = b3w->whirl->hbF;
    b3w->flt[1]._g = b3w->whirl->hbG;
    b3w->flt[1]._q = b3w->whirl->hbQ;

    b3w->flt[2]._t = b3w->whirl->lpT;
    b3w->flt[2]._f = float(b3w->whirl->lpF);
    b3w->flt[2]._g = float(b3w->whirl->lpG);
    b3w->flt[2]._q = float(b3w->whirl->lpQ);

    b3w->o_horn_radius = b3w->whirl->hornRadiusCm;
    b3w->o_drum_radius = b3w->whirl->drumRadiusCm;
    b3w->o_mic_dist = b3w->whirl->micDistCm;
    b3w->o_horn_xoff = b3w->whirl->hornXOffsetCm;
    b3w->o_horn_zoff = b3w->whirl->hornZOffsetCm;
    b3w->o_horn_leak = b3w->whirl->leakLevel;

    b3w->fade_dir = false;
    b3w->fade = 0;

    b3w->rate = rate;
    b3w->nyq = rate * 0.4998;
    b3w->lpf1 = float(2000.0 / rate);
    b3w->lpf2 = float(880.0 / rate);

    b3w->resend_data_to_ui = 0;
    b3w->resend_trigger = 0;

    // fade in levels
    b3w->o_horn_level = 0.0;
    b3w->o_drum_level = 0.0;
    b3w->o_drum_width = 0.0;

    b3w->x_drum_width = 0.0;
    b3w->x_dll = b3w->x_drr = 1.0;
    b3w->x_dlr = b3w->x_drl = 0.0;

    b3w->x_horn_width = 0.0;
    b3w->x_hll = b3w->x_hrr = 1.0;
    b3w->x_hlr = b3w->x_hrl = 0.0;
}

void Leslie::deinit()
{
    if (b3w)
    {
        freeWhirl(b3w->whirl);
        free(b3w);
        b3w = nullptr;
    }
}

void Leslie::set_speed()
{
    if (b3w->o_rev_select == *b3w->rev_select) {
        return;
    }

    if (b3w->flt[0].type && !b3w->horn_radius) {
        // MOD version
        const int v = (int)floorf(*b3w->rev_select);
        useRevOption(b3w->whirl, v * 3 + v, 0);
    }
    else {
        const float l = b3w->p_link_speed ? (*b3w->p_link_speed) : 0;
        const int   v = (int)floorf(*b3w->rev_select);
        int         h = v / 3; // 0: stop, 1: slow, 2: fast
        int         d = v % 3; // 0: stop, 1: slow, 2: fast
        if (l <= -.5) {
            h = d;
        }
        if (l >= 0.5) {
            d = h;
        }
        useRevOption(b3w->whirl, h * 3 + d, 0);
    }

    b3w->o_rev_select = *b3w->rev_select;
}

#define SILENT 128 // cycles of 64
#define FADED 96   // cycles of 64

bool Leslie::faded()
{
    return b3w->fade >= FADED;
}

int Leslie::interpolate_filter(Filter* flt)
{
    assert(flt->type && flt->freq && flt->qual && flt->gain);

    int   t = (int)rintf(*flt->type);
    float f = *flt->freq;
    float q = *flt->qual;
    float g = *flt->gain;

    t = t % 9;

    if (t != flt->_t && !faded()) {
        return 1;
    }

    if (q < .01) {
        q = .01f;
    }
    if (q > 6.0) {
        q = 6.f;
    }
    if (f < 20.0) {
        f = 20.f;
    }
    if (f > b3w->nyq) {
        f = float(b3w->nyq);
    }
    if (g < -80) {
        g = -80.f;
    }
    if (g > 80) {
        g = 80.f;
    }

    if (flt->_f == f && flt->_g == g && flt->_q == q && flt->_t == t) {
        return 0;
    }

    const float _a = b3w->lpf1;
    const float _b = b3w->lpf2;

    if (faded()) {
        flt->_t = t;
        flt->_g = g;
        flt->_f = f;
        flt->_q = q;
    }
    else {
        // TODO think about linear rather that exp approach, or require fade.

        // limit large jumps per 64 samples
        const float w0 = flt->_f / float(b3w->rate);
        const float w1 = f / float(b3w->rate);
        if (fabsf(w0 - w1) > .20f) {
            return 1;
        } // unusual, whirl EQ range is 20Hz..8KHz
        if ((w0 - w1) > .02f) {
            f = float(b3w->rate * (w0 - .02 * b3w->rate));
        }
        if ((w0 - w1) < -.02f) {
            f = float(b3w->rate * (w0 + .02 * b3w->rate));
        }
        if ((flt->_g - g) > 10.0f) {
            g = flt->_g - 10.0f;
        }
        if ((flt->_g - g) < -10.0f) {
            g = flt->_g + 10.0f;
        }

        flt->_f += _a * (f - flt->_f);
        flt->_g += _a * (g - flt->_g);
        flt->_q += _b * (q - flt->_q);

        if (fabsf(flt->_g - g) < 1e-4) {
            flt->_g = g;
        }
        if (fabsf(flt->_f - f) < 1e-2) {
            flt->_f = f;
        }
        if (fabsf(flt->_q - q) < 1e-3) {
            flt->_q = q;
        }
    }

    double C[6];
    eqCompute(flt->_t, flt->_f, flt->_q, flt->_g, C, b3w->rate);

    flt->W[0][a1] = (iir_t)(C[EQC_A1]);
    flt->W[0][a2] = (iir_t)(C[EQC_A2]);
    flt->W[0][b0] = (iir_t)(C[EQC_B0]);
    flt->W[0][b1] = (iir_t)(C[EQC_B1]);
    flt->W[0][b2] = (iir_t)(C[EQC_B2]);
    if (faded()) {
        flt->W[0][z0] = 0;
        flt->W[0][z1] = 0;
    }

    if (!flt->W[1]) {
        return 0;
    }

    flt->W[1][a1] = (iir_t)(C[EQC_A1]);
    flt->W[1][a2] = (iir_t)(C[EQC_A2]);
    flt->W[1][b0] = (iir_t)(C[EQC_B0]);
    flt->W[1][b1] = (iir_t)(C[EQC_B1]);
    flt->W[1][b2] = (iir_t)(C[EQC_B2]);
    if (faded()) {
        flt->W[1][z0] = 0;
        flt->W[1][z1] = 0;
    }
    return 0;
}

#define SETVALUE(VAR, NAME, PROC, FN)               \
	if (b3w->NAME) {                                \
		if (b3w->o_##NAME != *(b3w->NAME)) {        \
			b3w->whirl->VAR = PROC (*(b3w->NAME));  \
			b3w->o_##NAME   = *(b3w->NAME);         \
			FN;                                     \
		}                                           \
	}

#define CHECKDIFF(VAR)                              \
	if (b3w->VAR && *b3w->VAR != b3w->o_##VAR) {    \
		changed = 1;                                \
	}

#define SETVAR(PARAM, VAR, MIN, MAX, MOD)           \
	{                                               \
		const float val = *b3w->VAR;                \
		b3w->o_##VAR    = val;                      \
		if (val >= MIN && val <= MAX) {             \
			b3w->whirl->PARAM = MOD val;            \
		}                                           \
	}

int Leslie::reconfigure()
{
    int changed = 0;

    CHECKDIFF(horn_radius);
    CHECKDIFF(drum_radius);
    CHECKDIFF(horn_xoff);
    CHECKDIFF(horn_zoff);
    CHECKDIFF(mic_dist);
    CHECKDIFF(mic_angle);

    if (!changed) {
        return 0;
    }

    if (!faded()) {
        return 1;
    }

    SETVAR(hornRadiusCm, horn_radius, 9, 50, )
    SETVAR(drumRadiusCm, drum_radius, 9, 50, )
    SETVAR(micDistCm, mic_dist, 9, 300, )
    SETVAR(hornXOffsetCm, horn_xoff, -20, 20, )
    SETVAR(hornZOffsetCm, horn_zoff, -20, 20, )
    SETVAR(micAngle, mic_angle, 0, 180, 1.f - 1.f / 180.f *)

        computeOffsets(b3w->whirl);
    return 0;
}

static inline float db_to_coefficient(const float d)
{
    return powf(10.0f, 0.05f * d);
}

void Leslie::process(uint32_t n_samples, const float* in, float* outL, float* outR)
{
    uint32_t i;
    assert(n_samples <= 64);
    const float lpf = b3w->lpf1;

    float horn_left[64];
    float horn_right[64];
    float drum_left[64];
    float drum_right[64];

    if (b3w->horn_leak) {
        const float lk = db_to_coefficient(*b3w->horn_leak);
        b3w->o_horn_leak += lpf * (lk - b3w->o_horn_leak) + 1e-15f;
        b3w->whirl->leakage = b3w->o_horn_leak * b3w->whirl->hornLevel;
    }

    whirlProc2(b3w->whirl, in, NULL, NULL,
        horn_left, horn_right, drum_left, drum_right, n_samples);

    // mixdown
    const float hl = db_to_coefficient(*b3w->horn_level);
    const float dl = db_to_coefficient(*b3w->drum_level);

    const float dw = *b3w->drum_width - 1.0f;
    const float hw = b3w->horn_width ? *b3w->horn_width - 1.0f : 0.0f;

    b3w->o_horn_level += lpf * (hl - b3w->o_horn_level) + 1e-15f;
    b3w->o_drum_level += lpf * (dl - b3w->o_drum_level) + 1e-15f;
    b3w->o_horn_width += lpf * (hw - b3w->o_horn_width) + 1e-15f;
    b3w->o_drum_width += lpf * (dw - b3w->o_drum_width) + 1e-15f;

    // re-calc coefficients only when changed
    if (fabsf(b3w->x_drum_width - b3w->o_drum_width) > 1e-8) {
        b3w->x_drum_width = b3w->o_drum_width;

        const float dwF = b3w->o_drum_width;
        const float dwP = dwF > 0.f ? (dwF > 1.f ? 1.f : dwF) : 0.f;
        const float dwN = dwF < 0.f ? (dwF < -1.f ? 1.f : -dwF) : 0.f;
        b3w->x_dll = sqrtf(1.f - dwP);
        b3w->x_dlr = sqrtf(0.f + dwP);
        b3w->x_drl = sqrtf(0.f + dwN);
        b3w->x_drr = sqrtf(1.f - dwN);
    }

    if (fabsf(b3w->x_horn_width - b3w->o_horn_width) > 1e-8) {
        b3w->x_horn_width = b3w->o_horn_width;

        const float hwF = b3w->o_horn_width;
        const float hwP = hwF > 0.f ? (hwF > 1.f ? 1.f : hwF) : 0.f;
        const float hwN = hwF < 0.f ? (hwF < -1.f ? 1.f : -hwF) : 0.f;
        b3w->x_hll = sqrtf(1.f - hwP);
        b3w->x_hlr = sqrtf(0.f + hwP);
        b3w->x_hrl = sqrtf(0.f + hwN);
        b3w->x_hrr = sqrtf(1.f - hwN);
    }

    // localize variable, small loop
    const float dll = b3w->o_drum_level * b3w->x_dll;
    const float dlr = b3w->o_drum_level * b3w->x_dlr;
    const float drl = b3w->o_drum_level * b3w->x_drl;
    const float drr = b3w->o_drum_level * b3w->x_drr;

    const float hll = b3w->o_horn_level * b3w->x_hll;
    const float hlr = b3w->o_horn_level * b3w->x_hlr;
    const float hrl = b3w->o_horn_level * b3w->x_hrl;
    const float hrr = b3w->o_horn_level * b3w->x_hrr;

    for (i = 0; i < n_samples; ++i) {
        outL[i] = horn_left[i] * hll + horn_right[i] * hlr + drum_left[i] * dll + drum_right[i] * dlr;
        outR[i] = horn_left[i] * hrl + horn_right[i] * hrr + drum_left[i] * drl + drum_right[i] * drr;
    }
}

void Leslie::render(int n_samples, const float* inBuffers[2], float* outBuffers[2])
{
    const float* inLeft = inBuffers[0];
    //const float* inRight = inBuffers[1];
    float* outLeft = outBuffers[0];
    float* outRight = outBuffers[1];

    SETVALUE(hnBrakePos, horn_brake, (double), );
    SETVALUE(hornAcc, horn_accel, , );
    SETVALUE(hornDec, horn_decel, , );

    SETVALUE(hornRPMslow, horn_slow, , computeRotationSpeeds(b3w->whirl); b3w->o_rev_select = -1;);
    SETVALUE(hornRPMfast, horn_fast, , computeRotationSpeeds(b3w->whirl); b3w->o_rev_select = -1;);
    SETVALUE(drumRPMslow, drum_slow, , computeRotationSpeeds(b3w->whirl); b3w->o_rev_select = -1;);
    SETVALUE(drumRPMfast, drum_fast, , computeRotationSpeeds(b3w->whirl); b3w->o_rev_select = -1;);

    SETVALUE(drBrakePos, drum_brake, (double), );
    SETVALUE(drumAcc, drum_accel, , );
    SETVALUE(drumDec, drum_decel, , );

    set_speed();

    const float* input = b3w->input = inLeft;
    float* outL = b3w->outL = outLeft;
    float* outR = b3w->outR = outRight;

    uint32_t k = n_samples;
    while (k > 0) {
        const uint32_t n = k > 64 ? 64 : k;

        int need_fade = 0;

        if (b3w->flt[0].type) { // extended and MOD variant
            need_fade |= interpolate_filter(&b3w->flt[0]);
            need_fade |= interpolate_filter(&b3w->flt[1]);
            need_fade |= interpolate_filter(&b3w->flt[2]);
        }
        if (b3w->horn_radius) { // extended version only
            need_fade |= reconfigure();
        }

        process(n, input, outL, outR);

        if (need_fade) {
            b3w->fade_dir = true;
        }

        float g0, g1;
        g0 = g1 = 1.0;

        if (!b3w->fade_dir && b3w->fade > 0 && b3w->fade <= FADED) {
            g0 = 1.0f - b3w->fade / (float)FADED;
            --b3w->fade;
            g1 = 1.0f - b3w->fade / (float)FADED;
        }
        else if (b3w->fade_dir && b3w->fade < FADED) {
            g0 = 1.0f - b3w->fade / (float)FADED;
            ++b3w->fade;
            g1 = 1.0f - b3w->fade / (float)FADED;
        }
        else if (b3w->fade >= FADED) {
            if (!b3w->fade_dir) {
                --b3w->fade;
            }
            else if (b3w->fade < SILENT) {
                ++b3w->fade;
            }
            else if (!need_fade) {
                b3w->fade_dir = false;
            }
            memset(outL, 0, sizeof(float) * n);
            memset(outR, 0, sizeof(float) * n);
        }

        if (g0 != g1) {
            uint32_t    i;
            const float d = (g1 - g0) / (float)n;
            float       g = g0;
            for (i = 0; i < n; ++i) {
                g += d;
                outL[i] *= g;
                outR[i] *= g;
            }
        }

        input += n;
        outL += n;
        outR += n;
        k -= n;
    }

    if (!b3w->c_horn_rpm || !b3w->c_horn_rpm) {
        // simple version
        return;
    }

    const float hspd = float(b3w->whirl->hornIncr * 60.0 * b3w->rate);
    const float dspd = float(b3w->whirl->drumIncr * 60.0 * b3w->rate);

    if (b3w->resend_trigger != (int)(floorf(b3w->p_resend_trigger[0]))) {
        b3w->resend_data_to_ui = int(ceilf(.5f * float(b3w->rate) / n_samples));
        b3w->resend_trigger = int(floorf(b3w->p_resend_trigger[0]));
    }

    if (b3w->resend_data_to_ui > 0) {
        // Force host to send update
        --b3w->resend_data_to_ui;
        *b3w->c_horn_rpm = -1.f - b3w->resend_data_to_ui / 100.f;
        *b3w->c_horn_ang = -1.f - b3w->resend_data_to_ui / 100.f;
        *b3w->c_drum_rpm = -1.f - b3w->resend_data_to_ui / 100.f;
        *b3w->c_drum_ang = -1.f - b3w->resend_data_to_ui / 100.f;
    }
    else {
        *b3w->c_horn_rpm = hspd;
        *b3w->c_horn_ang = float(fmod(1.25 - b3w->whirl->hornAngleGRD, 1.0));
        *b3w->c_drum_rpm = dspd;
        *b3w->c_drum_ang = float(fmod(b3w->whirl->drumAngleGRD + .25, 1.0));
    }

}
