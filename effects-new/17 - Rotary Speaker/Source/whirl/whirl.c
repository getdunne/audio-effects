//#define DEBUG_SPEED  // debug acceleration,deceleration
//#define HORN_COMB_FILTER
#define _XOPEN_SOURCE 700
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eqcomp.h"
#include "whirl.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#define DENORMAL_HACK (1e-14)
#define IS_DENORMAL(f) (((*(unsigned int*)&f) & 0x7f800000) == 0)

void
initValues (struct b_whirl* w)
{
	w->bypass     = 0;
	w->hnBrakePos = 0.0;
	w->drBrakePos = 0.0;

	/* rotational frequency and time-constats were taken from the paper
	 * "Discrete Time Emulation of the Leslie Speaker"
	 * by Jorge Herrera, Craig Hanson, and Jonathan S. Abel
	 * Presented at the 127th Convention
	 * 2009 October 9–12 New York NY, USA
	 *
	 *  horn: fast:7.056 Hz, slow: 0.672 Hz
	 *  drum: fast:5.955 Hz, slow: 0.101 Hz (wrong?)
	 *
	 * alternate values:
	 * http://www.dairiki.org/HammondWiki/LeslieRotationSpeed
	 *  horn: fast: 400 RPM, slow: 48 RPM
	 *  drum: fast: 342 RPM, slow: 40 RPM
	 */
	w->hornRPMslow = 60.0f * 0.672f;
	w->hornRPMfast = 60.0f * 7.056f;
	w->drumRPMslow = 60.0f * 0.600f;
	w->drumRPMfast = 60.0f * 5.955f;

	/* time constants [s] -- first order differential */
	w->hornAcc = 0.161f;
	w->hornDec = 0.321f;
	w->drumAcc = 4.127f;
	w->drumDec = 1.371f;

	w->hornAcDc = w->drumAcDc = 0;

	/* clang-format off */

	/* The current angle of rotating elements */
	w->hornAngleGRD = 0.0; /* 0..1 */
	w->drumAngleGRD = 0.0;
	w->micAngle     = 0.0;

	/* angular speed: deg / sample */
	w->hornIncr      = 0.0; /* horn's current angular speed */
	w->drumIncr      = 0.0; /* drum's current angular speed */

	w->hornTarget    = 0.0; /* target angular speed */
	w->drumTarget    = 0.0; /* target angular speed */

	w->airSpeed      = 340.0f; /* Meters per second */
	w->micDistCm     =  42.0f; /* From mic to origin */
	w->hornXOffsetCm =   0.0f; /* offset of horn, towards left mic */
	w->hornZOffsetCm =   0.0f; /* offset of horn, perpendicular to mic to front */
	w->hornRadiusCm  =  19.2f;
	w->drumRadiusCm  =  22.0f;

	w->lpT = EQC_HIGH;
	w->lpF = 811.9695;
	w->lpQ =   1.6016;
	w->lpG = -38.9291;

	w->haT = EQC_LPF;
	w->haF = 4500.0f;
	w->haQ =    2.7456f;
	w->haG =  -30.0f;

	w->hbT = EQC_LOW;
	w->hbF = 300.0f;
	w->hbQ =   1.0f;
	w->hbG = -30.0f;
	/* clang-format on */

	w->hornMicWidth = w->drumMicWidth = 0.0f;
	w->hornMic_hll = w->drumMic_dll = 1.0f;
	w->hornMic_hlr = w->drumMic_dlr = 0.0f;
	w->hornMic_hrl = w->drumMic_drl = 0.0f;
	w->hornMic_hrr = w->drumMic_drr = 1.0f;

	w->hornLevel = 0.7f;
	w->leakLevel = 0.15f;
	w->leakage   = 0.0f;

#ifdef HORN_COMB_FILTER
	w->cb0fb = -0.55f;
	w->cb0dl = 38;
	w->cb1fb = -0.3508f;
	w->cb1dl = 120;
#else
	w->cb0fb = 0.0f;
	w->cb0dl = 0;
	w->cb1fb = 0.0f;
	w->cb1dl = 0;
#endif
}

struct b_whirl*
allocWhirl (void)
{
	struct b_whirl* w = (struct b_whirl*)calloc (1, sizeof (struct b_whirl));
	if (!w)
		return NULL;
	initValues (w);
	return (w);
}

void
freeWhirl (struct b_whirl* w)
{
	free (w);
}

static void
setIIRFilter (iir_t        W[],
              int          T,
              const double F,
              const double Q,
              const double G,
              const double SR)
{
	double C[6];
	if (Q <= 0.1)
		return;
	if (Q >= 6.00)
		return;
	if (F / SR <= 0.0002)
		return;
	if (F / SR >= 0.4998)
		return;
	if (G <= -48.0)
		return;
	if (G >= 48.0)
		return;
	if (T < EQC_LPF || T > EQC_HIGH)
		return;

	eqCompute (T, F, Q, G, C, SR);
	W[a1] = (iir_t)C[EQC_A1];
	W[a2] = (iir_t)C[EQC_A2];
	W[b0] = (iir_t)C[EQC_B0];
	W[b1] = (iir_t)C[EQC_B1];
	W[b2] = (iir_t)C[EQC_B2];
}

void
useRevOption (struct b_whirl* w, int n, int signals)
{
	int i = n % 9;

	w->hornTarget = w->revoptions[i].hornTarget;
	w->drumTarget = w->revoptions[i].drumTarget;

	if (w->hornIncr < w->hornTarget) {
		w->hornAcDc = 1;
	} else if (w->hornTarget < w->hornIncr) {
		w->hornAcDc = -1;
	}
	if (w->drumIncr < w->drumTarget) {
		w->drumAcDc = 1;
	} else if (w->drumTarget < w->drumIncr) {
		w->drumAcDc = -1;
	}

//    if (signals & 1) {
//        notifyControlChangeByName (w->midi_cfg_ptr, "rotary.speed-select", ceilf (n * 15.875f));
//    }
	if (signals & 2) {
		const int hr = (n / 3) % 3; /* horn 0:off, 1:chorale  2:tremolo */
		switch (hr) {
			case 2:
				w->revSelect = WHIRL_FAST;
				break;
			case 1:
				w->revSelect = WHIRL_SLOW;
				break;
			default:
				w->revSelect = WHIRL_STOP;
				break;
		}
		//notifyControlChangeByName (w->midi_cfg_ptr, "rotary.speed-preset", ceilf (w->revSelect * 63.5f));
	}
}

void
setRevSelect (struct b_whirl* w, int n)
{
	int i;

	w->revSelect = n % revSelectEnd;
	i            = w->revselects[w->revSelect];
	useRevOption (w, i, 1);
}

/* used by rotary.speed-toggle */
void
setWhirlSustainPedal (void* d, unsigned char u)
{
	struct b_whirl* w = (struct b_whirl*)d;
	if (u > 63 /* press */) {
		useRevOption (w, (w->revSelect == WHIRL_SLOW) ? w->revselects[WHIRL_FAST] : w->revselects[WHIRL_SLOW], 3);
	}
}

static void
setRevOption (struct b_whirl* w,
              int             i,
              double          hnTgt,
              double          drTgt)
{
	w->revoptions[i].hornTarget = hnTgt;
	w->revoptions[i].drumTarget = drTgt;
}

void
computeRotationSpeeds (struct b_whirl* w)
{
	const double hfast = w->hornRPMfast / (w->SampleRateD * 60.0);
	const double hslow = w->hornRPMslow / (w->SampleRateD * 60.0);
	const double hstop = 0;
	const double dfast = w->drumRPMfast / (w->SampleRateD * 60.0);
	const double dslow = w->drumRPMslow / (w->SampleRateD * 60.0);
	const double dstop = 0;

	setRevOption (w, 8, hfast, dfast);
	setRevOption (w, 7, hfast, dslow);
	setRevOption (w, 6, hfast, dstop);
	setRevOption (w, 5, hslow, dfast);
	setRevOption (w, 4, hslow, dslow);
	setRevOption (w, 3, hslow, dstop);
	setRevOption (w, 2, hstop, dfast);
	setRevOption (w, 1, hstop, dslow);
	setRevOption (w, 0, hstop, dstop);

	w->revselects[0] = 4; /* both slow */
	w->revselects[1] = 0; /* stop */
	w->revselects[2] = 8; /* both fast */
	setRevSelect (w, w->revSelect);
}

/* interpolate angular IR */
static void
_ipoldraw (struct b_whirl* sw,
           double          degrees,
           double          level,
           int             partial,
           double*         ipx,
           double*         ipy)
{
	double d;
	double e;
	double range;
	int    fromIndex;
	int    toIndex;
	int    i;

	d = *ipx;
	while (d < 0.0)
		d += 360.0;
	fromIndex = (int)((d * (double)WHIRL_DISPLC_SIZE) / 360.0);

	*ipx = degrees;

	e = *ipx;
	while (e < d)
		e += 360.0;
	toIndex = (int)((e * (double)WHIRL_DISPLC_SIZE) / 360.0);

	range = (double)(toIndex - fromIndex);
	for (i = fromIndex; i <= toIndex; i++) {
		double x                                  = (double)(i - fromIndex);
		double w                                  = (*ipy) + ((x / range) * (level - (*ipy)));
		sw->bfw[i & WHIRL_DISPLC_MASK].b[partial] = (float)w;
	}

	*ipy = level;
}

/* clang-format off */
#define ipolmark(degrees, level) \
{                                \
        ipx = degrees;           \
        ipy = level;             \
}
#define ipoldraw(degrees, level, partial) _ipoldraw (w, degrees, level, partial, &ipx, &ipy)
/* clang-format on */

static void
initTables (struct b_whirl* w)
{
	unsigned int i, j;
	double       ipx;
	double       ipy;
	double       sum;
	ipx = ipy = 0.0;

	/* clang-format off */

	/* Horn angle-dependent impulse response coefficents.
	 *
	 * These were derived from 'Doppler simulation and the leslie',
	 * Julius Smith, Stefania Serafin, Jonathan Abel, David Berners,
	 * Proc. of the 5th Conference on Digital Audio Effects (DAFx-02),
	 * Hamburg, Germany, September 26-28, 2002.
	 * In this article figure 8 depicts the 'First 5 principal components
	 * weighted by their corresponding singular values'. I have been unable
	 * to clearly understand what this means, but anyway ploughed ahead
	 * interpreted the components as impulse response coefficients. The
	 * figure plots the five components as horizontal wavy lines, where
	 * the x axis is the angle and the y axis is relative to each component.
	 * So, the following code consists of 'drawing' instructions that simply
	 * 'paints' a copy of each line as interpreted from the figure, from
	 * left (-180 degrees) to right (180 degrees). The points between the
	 * given coordinates are linearly interpolated and inserted into the
	 * bfw matrix. Then (below) we normalise the lot to unit gain.
	 */
	ipolmark (-180.0, 1.052);
	ipoldraw (-166.4,  .881, 0);
	ipoldraw (-150.5,  .881, 0);
	ipoldraw (-135.3,  .881, 0);
	ipoldraw (-122.4,  .792, 0);
	ipoldraw (-106.5,  .792, 0);
	ipoldraw ( -91.2,  .836, 0);
	ipoldraw ( -75.8,  .881, 0);
	ipoldraw ( -59.4,  .851, 0);
	ipoldraw ( -44.7,  .941, 0);
	ipoldraw ( -30.0, 1.298, 0);
	ipoldraw ( -14.7, 2.119, 0);
	ipoldraw (   0.0, 2.820, 0);
	ipoldraw (  15.6, 2.313, 0);
	ipoldraw (  30.0, 1.492, 0);
	ipoldraw (  44.7,  .926, 0);
	ipoldraw (  60.0,  .836, 0);
	ipoldraw (  74.7,  .866, 0);
	ipoldraw (  90.6,  .792, 0);
	ipoldraw ( 100.0,  .777, 0);
	ipoldraw ( 105.0,  .777, 0);
	ipoldraw ( 120.0,  .836, 0);
	ipoldraw ( 135.3,  .836, 0);
	ipoldraw ( 150.0,  .881, 0);
	ipoldraw ( 164.5,  .874, 0);
	ipoldraw ( 180.0, 1.052, 0);

	ipolmark (-180.0, -0.07);
	ipoldraw (-150.0,  0.10, 1);
	ipoldraw (-135.0, -0.10, 1);
	ipoldraw (-122.2,  0.16, 1);
	ipoldraw (-105.0,  0.15, 1);
	ipoldraw ( -91.2,  0.37, 1);
	ipoldraw ( -75.3,  0.32, 1);
	ipoldraw ( -60.1,  0.39, 1);
	ipoldraw ( -44.5,  0.70, 1);
	ipoldraw ( -30.0,  0.53, 1);
	ipoldraw ( -12.0, -0.40, 1);
	ipoldraw (   0.0, -0.81, 1);
	ipoldraw (   2.7, -0.77, 1);
	ipoldraw (  15.0, -0.52, 1);
	ipoldraw (  33.1,  0.38, 1);
	ipoldraw (  43.7,  0.68, 1);
	ipoldraw (  57.7,  0.49, 1);
	ipoldraw (  74.1,  0.19, 1);
	ipoldraw (  89.4,  0.33, 1);
	ipoldraw ( 105.0,  0.03, 1);
	ipoldraw ( 120.0,  0.12, 1);
	ipoldraw ( 134.0, -0.13, 1);
	ipoldraw ( 153.3,  0.08, 1);
	ipoldraw ( 180.0, -0.07, 1);

	ipolmark (-180.0,  0.40);
	ipoldraw (-165.0,  0.20, 2);
	ipoldraw (-150.0,  0.48, 2);
	ipoldraw (-135.0,  0.27, 2);
	ipoldraw (-121.2,  0.22, 2);
	ipoldraw ( -89.2,  0.30, 2);
	ipoldraw ( -69.2,  0.22, 2);
	ipoldraw ( -58.0,  0.11, 2);
	ipoldraw ( -40.2, -0.43, 2);
	ipoldraw ( -29.0, -0.53, 2);
	ipoldraw ( -15.6, -0.43, 2);
	ipoldraw (   0.0,  0.00, 2);
	ipoldraw (  14.3, -0.44, 2);
	ipoldraw (  30.3, -0.60, 2);
	ipoldraw (  60.3,  0.11, 2);
	ipoldraw (  74.9,  0.32, 2);
	ipoldraw (  91.5,  0.23, 2);
	ipoldraw ( 104.9,  0.32, 2);
	ipoldraw ( 121.7,  0.19, 2);
	ipoldraw ( 135.0,  0.27, 2);
	ipoldraw ( 150.0,  0.45, 2);
	ipoldraw ( 165.0,  0.20, 2);
	ipoldraw ( 180.0,  0.40, 2);

	ipolmark (-180.0, -0.08);
	ipoldraw (-165.2, -0.19, 3);
	ipoldraw (-150.0,  0.00, 3);
	ipoldraw (-133.9, -0.20, 3);
	ipoldraw (-120.0, -0.15, 3);
	ipoldraw (-106.0,  0.09, 3);
	ipoldraw ( -89.3, -0.15, 3);
	ipoldraw ( -76.3,  0.00, 3);
	ipoldraw ( -60.3,  0.29, 3);
	ipoldraw ( -44.6, -0.02, 3);
	ipoldraw ( -15.6, -0.22, 3);
	ipoldraw (   0.0,  0.24, 3);
	ipoldraw (  14.5,  0.11, 3);
	ipoldraw (  30.1, -0.10, 3);
	ipoldraw (  44.6,  0.17, 3);
	ipoldraw (  60.4,  0.22, 3);
	ipoldraw (  75.9,  0.16, 3);
	ipoldraw (  90.4, -0.05, 3);
	ipoldraw ( 104.9,  0.07, 3);
	ipoldraw ( 122.8, -0.07, 3);
	ipoldraw ( 136.2, -0.07, 3);
	ipoldraw ( 150.0,  0.08, 3);
	ipoldraw ( 165.0, -0.19, 3);
	ipoldraw ( 180.0, -0.08, 3);

	ipolmark (-180.0,  0.13);
	ipoldraw (-165.2,  0.00, 4);
	ipoldraw (-150.0,  0.17, 4);
	ipoldraw (-135.2, -0.20, 4);
	ipoldraw (-120.5,  0.00, 4);
	ipoldraw (-105.0,  0.00, 4);
	ipoldraw ( -90.0,  0.04, 4);
	ipoldraw ( -75.0, -0.09, 4);
	ipoldraw ( -60.3, -0.14, 4);
	ipoldraw ( -45.0,  0.16, 4);
	ipoldraw ( -15.6,  0.00, 4);
	ipoldraw (   0.0,  0.22, 4);
	ipoldraw (  15.6, -0.21, 4);
	ipoldraw (  30.1, -0.09, 4);
	ipoldraw (  45.0,  0.10, 4);
	ipoldraw (  60.3, -0.07, 4);
	ipoldraw (  74.8, -0.15, 4);
	ipoldraw (  90.4, -0.03, 4);
	ipoldraw ( 104.9, -0.14, 4);
	ipoldraw ( 120.5,  0.00, 4);
	ipoldraw ( 135.2, -0.26, 4);
	ipoldraw ( 150.0,  0.16, 4);
	ipoldraw ( 165.0, -0.02, 4);
	ipoldraw ( 180.0,  0.13, 4);

	/* clang-format on */

	sum = 0.0;
	/* Compute the normalisation factor */
	for (i = 0; i < WHIRL_DISPLC_SIZE; i++) {
		double colsum = 0.0;
		for (j = 0; j < 5; j++) {
			colsum += fabs (w->bfw[i].b[j]);
		}
		if (sum < colsum) {
			sum = colsum;
		}
	}
	/* Apply normalisation */
	for (i = 0; i < WHIRL_DISPLC_SIZE; i++) {
		for (j = 0; j < 5; j++) {
			w->bfw[i].b[j] *= (float)(1.0 / sum);
			w->bbw[WHIRL_DISPLC_SIZE - i - 1].b[j] = w->bfw[i].b[j];
		}
	}
}

static void
zeroBuffers (struct b_whirl* w)
{
	w->adi0 = w->adi1 = w->adi2 = 0;
	w->outpos                   = 0;

	memset (w->HLbuf, 0, sizeof (float) * WHIRL_BUF_SIZE_SAMPLES);
	memset (w->HRbuf, 0, sizeof (float) * WHIRL_BUF_SIZE_SAMPLES);
	memset (w->DLbuf, 0, sizeof (float) * WHIRL_BUF_SIZE_SAMPLES);
	memset (w->DRbuf, 0, sizeof (float) * WHIRL_BUF_SIZE_SAMPLES);

	memset (w->adx0, 0, sizeof (float) * AGBUF);
	memset (w->adx1, 0, sizeof (float) * AGBUF);
	memset (w->adx2, 0, sizeof (float) * AGBUF);
}

void
computeOffsets (struct b_whirl* w)
{
	unsigned int i;

	zeroBuffers (w);

	/* clang-format off */
	/* Spacing between reflections in samples, normalized for 22.1k.
	 * The first can't be zero, since we must allow for the swing of
	 * the extent to wander close to the reader.
	 */
	w->hornSpacing[0] =  12.0; /*  18.4cm  1842Hz -- Primary L */
	w->hornSpacing[1] =  18.0; /*  27.7cm  1227Hz -- Primary R */
	w->hornSpacing[2] =  53.0; /*  81.5cm   417Hz -- First reflection (backwards) */
	w->hornSpacing[3] =  50.0; /*  76.9cm   442Hz */
	w->hornSpacing[4] = 106.0; /* 163.0cm   208Hz -- Secondary reflection */
	w->hornSpacing[5] = 116.0; /* 178.5cm   190Hz */

	w->drumSpacing[0] =  36.0; /*  55.3cm   614Hz */
	w->drumSpacing[1] =  39.0; /*  60.0cm   567Hz */
	w->drumSpacing[2] =  79.0; /* 121.5cm   280Hz */
	w->drumSpacing[3] =  86.0; /* 132.3cm   257Hz */
	w->drumSpacing[4] = 123.0; /* 189.2cm   179Hz */
	w->drumSpacing[5] = 116.0; /* 178.5cm   190Hz */
	/* clang-format on */

	const double hornRadiusSamples = (w->hornRadiusCm * w->SampleRateD / 100.0) / w->airSpeed;
	const double drumRadiusSamples = (w->drumRadiusCm * w->SampleRateD / 100.0) / w->airSpeed;
	const double micDistSamples    = (w->micDistCm * w->SampleRateD / 100.0) / w->airSpeed;
	const double micXOffsetSamples = (w->hornXOffsetCm * w->SampleRateD / 100.0) / w->airSpeed;
	const double micZOffsetSamples = (w->hornZOffsetCm * w->SampleRateD / 100.0) / w->airSpeed;

	double maxhn = 0;
	double maxdr = 0;
	for (i = 0; i < WHIRL_DISPLC_SIZE; i++) {
		/* Compute angle around the circle */
		double v = (2.0 * M_PI * (double)i) / (double)WHIRL_DISPLC_SIZE;
		/* Distance between the mic and the rotor korda */
		double a = micDistSamples - (hornRadiusSamples * cos (v));
		/* Distance between rotor and mic-origin line */
		double b = micZOffsetSamples + hornRadiusSamples * sin (v);

		const double dist                          = sqrt ((a * a) + (b * b));
		w->hnFwdDispl[i]                           = (float)(dist + micXOffsetSamples);
		w->hnBwdDispl[WHIRL_DISPLC_SIZE - (i + 1)] = (float)(dist - micXOffsetSamples);

		if (maxhn < w->hnFwdDispl[i])
			maxhn = w->hnFwdDispl[i];
		if (maxhn < w->hnBwdDispl[WHIRL_DISPLC_SIZE - (i + 1)])
			maxhn = w->hnBwdDispl[WHIRL_DISPLC_SIZE - (i + 1)];

		a                                          = micDistSamples - (drumRadiusSamples * cos (v));
		b                                          = drumRadiusSamples * sin (v);
		w->drFwdDispl[i]                           = (float)(sqrt ((a * a) + (b * b)));
		w->drBwdDispl[WHIRL_DISPLC_SIZE - (i + 1)] = w->drFwdDispl[i];

		if (maxdr < w->drFwdDispl[i])
			maxdr = w->drFwdDispl[i];
	}

	w->hornPhase[0] = 0;
	w->hornPhase[1] = WHIRL_DISPLC_SIZE >> 1;

	w->hornPhase[2] = ((WHIRL_DISPLC_SIZE * 2) / 6);
	w->hornPhase[3] = ((WHIRL_DISPLC_SIZE * 5) / 6);

	w->hornPhase[4] = ((WHIRL_DISPLC_SIZE * 1) / 6);
	w->hornPhase[5] = ((WHIRL_DISPLC_SIZE * 4) / 6);

	for (i = 0; i < 6; i++) {
		w->hornSpacing[i] = w->hornSpacing[i] * (float)(w->SampleRateD / 22100.0 + hornRadiusSamples + 1.0);
		assert ((maxhn + w->hornSpacing[i]) < WHIRL_BUF_SIZE_SAMPLES);
	}

	w->drumPhase[0] = 0;
	w->drumPhase[1] = WHIRL_DISPLC_SIZE >> 1;

	w->drumPhase[2] = ((WHIRL_DISPLC_SIZE * 2) / 6);
	w->drumPhase[3] = ((WHIRL_DISPLC_SIZE * 5) / 6);

	w->drumPhase[4] = ((WHIRL_DISPLC_SIZE * 1) / 6);
	w->drumPhase[5] = ((WHIRL_DISPLC_SIZE * 4) / 6);

	for (i = 0; i < 6; i++) {
		w->drumSpacing[i] = w->drumSpacing[i] * (float)(w->SampleRateD / 22100.0 + drumRadiusSamples + 1.0);
		assert ((maxdr + w->drumSpacing[i]) < WHIRL_BUF_SIZE_SAMPLES);
	}
}

static void
initialize (struct b_whirl* w)
{
	unsigned int i;
	for (i          = 0; i < 4; ++i)
		w->z[i] = 0;

	w->leakage = w->leakLevel * w->hornLevel;

	memset (w->drfL, 0, 8 * sizeof (iir_t));
	memset (w->drfR, 0, 8 * sizeof (iir_t));
	memset (w->hafw, 0, 8 * sizeof (iir_t));
	memset (w->hbfw, 0, 8 * sizeof (iir_t));
#ifdef HORN_COMB_FILTER
	memset (w->comb0, 0, sizeof (float) * COMB_SIZE);
	memset (w->comb1, 0, sizeof (float) * COMB_SIZE);
#endif

	setIIRFilter (w->drfL, w->lpT, w->lpF, w->lpQ, w->lpG, w->SampleRateD);
	setIIRFilter (w->drfR, w->lpT, w->lpF, w->lpQ, w->lpG, w->SampleRateD);
	setIIRFilter (w->hafw, w->haT, w->haF, w->haQ, w->haG, w->SampleRateD);
	setIIRFilter (w->hbfw, w->hbT, w->hbF, w->hbQ, w->hbG, w->SampleRateD);

#ifdef HORN_COMB_FILTER
	w->cb0rp = &(w->comb0[COMB_SIZE - w->cb0dl]);
	w->cb0wp = &(w->comb0[0]);
	w->cb0bp = &(w->comb0[0]);
	w->cb0es = &(w->comb0[COMB_SIZE]);

	w->cb1rp = &(w->comb1[COMB_SIZE - w->cb1dl]);
	w->cb1wp = &(w->comb1[0]);
	w->cb1bp = &(w->comb1[0]);
	w->cb1es = &(w->comb1[COMB_SIZE]);
#endif

	computeOffsets (w);
	initTables (w);
}

/*
 * Displays the settings of a filter.
 */
//static void
//displayFilter (const char* id, int T, double F, double Q, double G)
//{
//#if 0 // not in RT callback
//  const char * type = eqGetTypeString (T);
//  printf ("\n%s:T=%3.3s:F=%10.4f:Q=%10.4f:G=%10.4f", id, type, F, Q, G);
//  fflush (stdout);
//#endif
//}

/* clang-format off */
#define UPDATE_A_FILTER setIIRFilter (w->hafw, w->haT, w->haF, w->haQ, w->haG, w->SampleRateD)
#define UPDATE_B_FILTER setIIRFilter (w->hbfw, w->hbT, w->hbF, w->hbQ, w->hbG, w->SampleRateD)
#define UPDATE_D_FILTER \
{                                                                               \
        setIIRFilter (w->drfL, w->lpT, w->lpF, w->lpQ, w->lpG, w->SampleRateD); \
        setIIRFilter (w->drfR, w->lpT, w->lpF, w->lpQ, w->lpG, w->SampleRateD); \
}
/* clang-format on */

/*
 * This function initialises this module. It is run after whirlConfig.
 */
void
initWhirl (struct b_whirl* w, void* m, double rate)
{
	w->SampleRateD  = rate;
	w->midi_cfg_ptr = m; /* used for notify -- translate "rotary.speed-*" */

#if 0
    // SD I don't know what these are
	useMIDIControlFunction (m, "rotary.speed-toggle", setWhirlSustainPedal, (void*)w);
	useMIDIControlFunction (m, "rotary.speed-preset", revControl, (void*)w);
	useMIDIControlFunction (m, "rotary.speed-select", revControlAll, (void*)w);

	useMIDIControlFunction (m, "whirl.horn.filter.a.type", setHornFilterAType, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.a.hz", setHornFilterAFrequency, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.a.q", setHornFilterAQ, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.a.gain", setHornFilterAGain, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.b.type", setHornFilterBType, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.b.hz", setHornFilterBFrequency, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.b.q", setHornFilterBQ, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.filter.b.gain", setHornFilterBGain, (void*)w);

	useMIDIControlFunction (m, "whirl.horn.brakepos", setHornBrakePosition, (void*)w);
	useMIDIControlFunction (m, "whirl.drum.brakepos", setDrumBrakePosition, (void*)w);

	useMIDIControlFunction (m, "whirl.horn.acceleration", setHornAcceleration, (void*)w);
	useMIDIControlFunction (m, "whirl.horn.deceleration", setHornDeceleration, (void*)w);
	useMIDIControlFunction (m, "whirl.drum.acceleration", setDrumAcceleration, (void*)w);
	useMIDIControlFunction (m, "whirl.drum.deceleration", setDrumDeceleration, (void*)w);
#endif

	initialize (w);
	computeRotationSpeeds (w);
}
#define x_fmodf fmodf
#define x_modf fmod
#define x_iroundf (unsigned int)roundf
#define x_ifloorf (unsigned int)floorf
#define x_floorf floorf

void
whirlProc2 (struct b_whirl* w,
            const float*    inbuffer,
            float* outL, float* outR,
            float* outHL, float* outHR,
            float* outDL, float* outDR,
            size_t bufferLengthSamples)
{
	const float* xp = inbuffer;
	unsigned int i;

	if (w->bypass) {
		for (i = 0; i < bufferLengthSamples; i++) {
			if (outL)
				*outL++ = inbuffer[i];
			if (outR)
				*outR++ = inbuffer[i];
			if (outHL)
				*outHL++ = inbuffer[i];
			if (outHR)
				*outHR++ = inbuffer[i];
			if (outDL)
				*outDL++ = 0;
			if (outDR)
				*outDR++ = 0;
		}
		return;
	}

	/* compute rotational speeds for this cycle */

	if (w->hornAcDc) {
		/* brake position notch - see comment on brake below */
		int flywheel = 0; /* disable deceleration to smoothly reach desired stop position */

		const float hardstop = (float)(10.0 / (60.0 * w->SampleRateD)); /* limit deceleration 10 to 0. RPM */

		if (w->hnBrakePos > 0 && w->hornTarget == 0 && w->hornIncr > 0 && w->hornIncr < hardstop) {
			const double targetPos = fmod (1.25 - w->hnBrakePos, 1.0);
			if (fabs (w->hornAngleGRD - targetPos) < (2.0 / WHIRL_DISPLC_SIZE)) {
				w->hornAngleGRD = targetPos;
				w->hornIncr     = 0;
			} else {
				/* keep going. at most: speed needed to reach brake-pos, at least 3RPM. */
				const float minspeed = (float)(3.0 / (60.0 * w->SampleRateD));
				const float diffinc  = (float)fmod (1. + targetPos - w->hornAngleGRD, 1.0) / (float)bufferLengthSamples;
				if (w->hornIncr > diffinc) {
					w->hornIncr = diffinc;
				} else if (w->hornIncr < minspeed) {
					w->hornIncr = minspeed;
				}
				flywheel = 1;
			}
		}

		if (!flywheel) {
			const double l = exp (-1.0 / (w->SampleRateD / bufferLengthSamples * (w->hornAcDc > 0 ? w->hornAcc : w->hornDec)));
			w->hornIncr += (1 - l) * (w->hornTarget - w->hornIncr);
		}

		if (fabs (w->hornTarget - w->hornIncr) < (.05 / (60.f * w->SampleRateD))) {
/* provide a dead-zone for rounding */
#ifdef DEBUG_SPEED
			printf ("AcDc Horn off\n");
#endif
			w->hornAcDc = 0;
			w->hornIncr = w->hornTarget;
		}
	}

	if (w->drumAcDc) {
		int flywheel = 0; /* disable deceleration to smoothly reach desired stop position */

		const float hardstop = (float)(8.0 / (60.0 * w->SampleRateD)); /* limit deceleration */

		if (w->drBrakePos > 0 && w->drumTarget == 0 && w->drumIncr > 0 && w->drumIncr < hardstop) {
			const double targetPos = fmod (w->drBrakePos + .75, 1.0);
			if (fabs (w->drumAngleGRD - targetPos) < (2.0 / WHIRL_DISPLC_SIZE)) {
				w->drumAngleGRD = targetPos;
				w->drumIncr     = 0;
			} else {
				/* keep going; at most: speed needed to reach brake-pos, at least 3RPM. */
				const float minspeed = (float)(3.0 / (60.0 * w->SampleRateD));
				const float diffinc  = (float)fmod (1. + targetPos - w->drumAngleGRD, 1.0) / (float)bufferLengthSamples;
				if (w->drumIncr > diffinc) {
					w->drumIncr = diffinc;
				} else if (w->drumIncr < minspeed) {
					w->drumIncr = minspeed;
				}
				flywheel = 1;
			}
		}

		if (!flywheel) {
			const double l = exp (-1.0 / (w->SampleRateD / bufferLengthSamples * (w->drumAcDc > 0 ? w->drumAcc : w->drumDec)));
			w->drumIncr += (1 - l) * (w->drumTarget - w->drumIncr);
		}

		if (fabs (w->drumTarget - w->drumIncr) < (.05 / (60.f * w->SampleRateD))) {
#ifdef DEBUG_SPEED
			printf ("ACDC Drum off\n");
#endif
			w->drumAcDc = 0;
			w->drumIncr = w->drumTarget;
		}
	}

	/* break position -- don't stop anywhere..
	 * the original Leslie can not do this, sometimes the horn is aimed at the back of
	 * the cabinet when it comes to a halt, which results in a less than desirable sound.
	 *
	 * move the horn and drum to the center position after it actually came to a stop.
	 * (this is used if brake pos is changed with stopped motors
	 *
	 * internal Pos = 0    towards left mic
	 * internal Pos = 0.5  towards right mic
	 * config: 1 = front, .5 = back
	 */
	int brake_enagaged = 0;
	if (w->hnBrakePos > 0) {
		const double targetPos = fmod (1.25 - w->hnBrakePos, 1.0);
		if (!w->hornAcDc && w->hornIncr == 0 && w->hornAngleGRD != targetPos) {
			brake_enagaged |= 1;
			if (fabs (w->hornAngleGRD - targetPos) < (2.0 / WHIRL_DISPLC_SIZE)) {
				/* provide a dead-zone for rounding */
				w->hornAngleGRD = targetPos;
			} else {
				const float limit = (float)(60.0 /* RPM */ / (60.0 * w->SampleRateD));
				w->hornIncr       = (float)fmod (1. + targetPos - w->hornAngleGRD, 1.0) / (float)bufferLengthSamples;
				if (w->hornIncr > limit)
					w->hornIncr = limit;
			}
		}
	}
	if (w->drBrakePos > 0) {
		const double targetPos = fmod (w->drBrakePos + .75, 1.0);
		if (!w->drumAcDc && w->drumIncr == 0 && w->drumAngleGRD != targetPos) {
			brake_enagaged |= 2;
			if (fabs (w->drumAngleGRD - targetPos) < (2.0 / WHIRL_DISPLC_SIZE)) {
				w->drumAngleGRD = targetPos;
			} else {
				const float limit = (float)(100.0 /* RPM */ / (60.0 * w->SampleRateD));
				w->drumIncr       = (float)fmod (1. + targetPos - w->drumAngleGRD, 1.0) / (float)bufferLengthSamples;
				if (w->drumIncr > limit)
					w->drumIncr = limit;
			}
		}
	}

	/* localize struct variables */
	double       hornAngleGRD = w->hornAngleGRD;
	double       drumAngleGRD = w->drumAngleGRD;
	unsigned int outpos       = w->outpos;
	const double fwAng        = w->micAngle * .25;
	const double bwAng        = 1. + w->micAngle * -.25;

	const float  leakage   = w->leakage;
	const float  hornLevel = w->hornLevel;
	const double hornIncr  = w->hornIncr;
	const double drumIncr  = w->drumIncr;

	const int* const   hornPhase   = w->hornPhase;
	const int* const   drumPhase   = w->drumPhase;
	const float* const hornSpacing = w->hornSpacing;
	const float* const drumSpacing = w->drumSpacing;
	const float* const hnFwdDispl  = w->hnFwdDispl;
	const float* const hnBwdDispl  = w->hnBwdDispl;
	const float* const drFwdDispl  = w->drFwdDispl;
	const float* const drBwdDispl  = w->drBwdDispl;

	iir_t* const hafw  = w->hafw;
	iir_t* const hbfw  = w->hbfw;
	float* const HLbuf = w->HLbuf;
	float* const HRbuf = w->HRbuf;
	float* const DLbuf = w->DLbuf;
	float* const DRbuf = w->DRbuf;
	float* const adx0  = w->adx0;
	float* const adx1  = w->adx1;
	float* const adx2  = w->adx2;
	iir_t* const drfL  = w->drfL;
	iir_t* const drfR  = w->drfR;
	float* const z     = w->z;

	const struct _bw* const bfw = w->bfw;
	const struct _bw* const bbw = w->bbw;

#ifdef DEBUG_SPEED
	char const* const acdc[3] = { "<", "#", ">" };
	static int        fgh     = 0;
	if ((fgh++ % (int)(w->SampleRateD / 128 / 5)) == 0) {
		printf ("H:%.3f D:%.3f | HS:%.3f DS:%.3f [Hz]| HT:%.2f DT:%.2f [Hz]| %s %s\n",
		        hornAngleGRD, drumAngleGRD,
		        w->SampleRateD * (double)hornIncr, w->SampleRateD * (double)drumIncr,
		        w->SampleRateD * (double)w->hornTarget, w->SampleRateD * (double)w->drumTarget,
		        acdc[w->hornAcDc + 1], acdc[w->drumAcDc + 1]);
	}
#endif

/* clang-format off */
#define ADDHIST(DX, DI, XS)              \
{                                        \
        DI     = (DI + AGMASK) & AGMASK; \
        DX[DI] = XS;                     \
}

#define HN_MOTION(P, BUF, DSP, BW, DX, DI, ANG)                             \
{                                                                           \
        const float        h1   = (ANG)*WHIRL_DISPLC_SIZE + hornPhase[(P)]; \
        const float        hd   = x_fmodf (h1, 1.f);                        \
        const unsigned int hl   = x_ifloorf (h1) & WHIRL_DISPLC_MASK;       \
        const unsigned int hh   = (hl + 1) & WHIRL_DISPLC_MASK;             \
        const float        intp = DSP[hl] * (1.f - hd) + hd * DSP[hh];      \
        const unsigned int k    = x_iroundf (h1) & WHIRL_DISPLC_MASK;       \
        const float        t    = hornSpacing[(P)] + intp + (float)outpos;  \
        const float        r    = x_floorf (t);                             \
        float              xa;                                              \
        xa = BW[k].b[0] * x;                                                \
        xa += BW[k].b[1] * DX[(DI)];                                        \
        xa += BW[k].b[2] * DX[((DI) + 1) & AGMASK];                         \
        xa += BW[k].b[3] * DX[((DI) + 2) & AGMASK];                         \
        xa += BW[k].b[4] * DX[((DI) + 3) & AGMASK];                         \
        const float q = xa * (t - r);                                       \
        n             = ((unsigned int)r) & WHIRL_BUF_MASK_SAMPLES;         \
        BUF[n] += xa - q;                                                   \
        n = (n + 1) & WHIRL_BUF_MASK_SAMPLES;                               \
        BUF[n] += q;                                                        \
}

#define DR_MOTION(P, BUF, DSP)                                                       \
{                                                                                    \
        const float        d1   = (float)(drumAngleGRD * WHIRL_DISPLC_SIZE + drumPhase[(P)]); \
        const float        dd   = x_fmodf (d1, 1.f);                                 \
        const unsigned int dl   = x_ifloorf (d1) & WHIRL_DISPLC_MASK;                \
        const unsigned int dh   = (dl + 1) & WHIRL_DISPLC_MASK;                      \
        const float        intp = DSP[dl] * (1.f - dd) + dd * DSP[dh];               \
        const float        t    = drumSpacing[(P)] + intp + (float)outpos;           \
        const float        r    = x_floorf (t);                                      \
        const float        q    = x * (t - r);                                       \
        n                       = ((unsigned int)r) & WHIRL_BUF_MASK_SAMPLES;        \
        BUF[n] += x - q;                                                             \
        n = (n + 1) & WHIRL_BUF_MASK_SAMPLES;                                        \
        BUF[n] += q;                                                                 \
}

/* This is just a bum filter to take some high-end off. */
#define FILTER_C(W0, W1, I)                    \
{                                              \
        float temp = x;                        \
        x          = ((W0)*x) + ((W1)*z[(I)]); \
        z[(I)]     = temp;                     \
}

#define EQ_IIR(W, X, Y)                                                  \
{                                                                        \
        float temp = (X) - (W[a1] * W[z0]) - (W[a2] * W[z1]);            \
        Y          = (temp * W[b0]) + (W[b1] * W[z0]) + (W[b2] * W[z1]); \
        W[z1]      = W[z0];                                              \
        W[z0]      = temp;                                               \
}

#define EQ_IIR_NAN(W)      \
{                          \
        if (isnan (W[z0])) \
                W[z0] = 0; \
        if (isnan (W[z1])) \
                W[z1] = 0; \
}

#ifdef HORN_COMB_FILTER

#define COMB(WP, RP, BP, ES, FB, X) \
{                                   \
        X += ((*(RP)++) * (FB));    \
        *(WP)++ = X;                \
        if ((RP) == (ES))           \
                RP = BP;            \
        if ((WP) == (ES))           \
                WP = BP;            \
}

#endif
	/* clang-format off */

	/* process each sample */
	for (i = 0; i < bufferLengthSamples; i++) {
		unsigned int n;
		float x = (float)((*xp++) + DENORMAL_HACK);
		float xx = x;
		float leak = 0;

		/* 1) apply filters A,B -- horn-speaker characteristics
		 * input: x
		 * output: x', leak
		 */

		EQ_IIR(hafw, x, x);
		EQ_IIR(hbfw, x, x);

		leak = x * leakage;

#ifdef HORN_COMB_FILTER
		/* only causes hiss-noise - in particular on 'E-4,F-4' ~660Hz
		 * no audible benefit to leslie effect so far, needs tweaking
		 */
		COMB(w->cb0wp, w->cb0rp, w->cb0bp, w->cb0es, w->cb0fb, x);
		COMB(w->cb1wp, w->cb1rp, w->cb1bp, w->cb1es, w->cb1fb, x);
#endif

		/* 2) now do doppler shift for the horn -- FM
		 * input: x' (filtered x)
		 * output: HLbuf, HRbuf, leak
		 */

		/* --- STATIC HORN FILTER --- */
		/* HORN PRIMARY */
		HN_MOTION(0, HLbuf, hnFwdDispl, bbw, adx0, w->adi0, (float)(hornAngleGRD + fwAng));
		HN_MOTION(1, HRbuf, hnBwdDispl, bfw, adx0, w->adi0, (float)(hornAngleGRD + bwAng));
		ADDHIST(adx0, w->adi0, x);

		/* HORN FIRST REFLECTION FILTER */
		FILTER_C(0.4f, 0.4f, 0);

		/* HORN FIRST REFLECTION */
		HN_MOTION(2, HLbuf, hnBwdDispl, bfw, adx1, w->adi1, (float)(hornAngleGRD + fwAng));
		HN_MOTION(3, HRbuf, hnFwdDispl, bbw, adx1, w->adi1, (float)(hornAngleGRD + bwAng));
		ADDHIST(adx1, w->adi1, x);

		/* HORN SECOND REFLECTION FILTER */
		FILTER_C(0.4f, 0.4f, 1);

		/* HORN SECOND REFLECTION */
		HN_MOTION(4, HLbuf, hnFwdDispl, bbw, adx2, w->adi2, (float)(hornAngleGRD + fwAng));
		HN_MOTION(5, HRbuf, hnBwdDispl, bfw, adx2, w->adi2, (float)(hornAngleGRD + bwAng));
		ADDHIST(adx2, w->adi2, x);

		/* 1A) do doppler shift for drum (actually orig signal -- FM
		 * input: x
		 * output: DLbuf, DRbuf
		 */

		x = xx; /* use original input signal ('x' was modified by horn filters) */

		/* --- DRUM --- */
		DR_MOTION(0, DLbuf, drFwdDispl);
		DR_MOTION(1, DRbuf, drBwdDispl);

		/* DRUM FIRST REFLECTION FILTER */
		FILTER_C(0.4f, 0.4f, 2);

		/* DRUM FIRST REFLECTION */
		DR_MOTION(2, DLbuf, drBwdDispl);
		DR_MOTION(3, DRbuf, drFwdDispl);

		/* DRUM SECOND REFLECTION FILTER */
		FILTER_C(0.4f, 0.4f, 3);

		/* DRUM SECOND REFLECTION */
		DR_MOTION(4, DLbuf, drFwdDispl);
		DR_MOTION(5, DRbuf, drBwdDispl);


		/* 1B) apply filter to drum-signal - and add horn */

		{
			float y;
			EQ_IIR(drfL, DLbuf[outpos], y);
			if (outL)
				*outL++ = y + hornLevel * HLbuf[outpos] + leak;
			if (outHL)
				*outHL++ = hornLevel * HLbuf[outpos] + leak;
			if (outDL)
				*outDL++ = y;

			EQ_IIR(drfR, DRbuf[outpos], y);
			if (outR)
				*outR++ =  y + hornLevel * HRbuf[outpos] + leak;
			if (outHR)
				*outHR++ = hornLevel * HRbuf[outpos] + leak;
			if (outDR)
				*outDR++ = y;
		}

		HLbuf[outpos] = 0.0;
		HRbuf[outpos] = 0.0;
		DLbuf[outpos] = 0.0;
		DRbuf[outpos] = 0.0;

		/* rotate speakers */

		outpos = (outpos + 1) & WHIRL_BUF_MASK_SAMPLES;

		hornAngleGRD = x_modf (hornAngleGRD + hornIncr, 1.0);
		drumAngleGRD = x_modf (drumAngleGRD + drumIncr, 1.0);
	}

	EQ_IIR_NAN(hafw)
		EQ_IIR_NAN(hbfw)
		EQ_IIR_NAN(drfL)
		EQ_IIR_NAN(drfR)

		if (isnan(z[0])) z[0] = 0;
	if (isnan(z[1])) z[1] = 0;
	if (isnan(z[2])) z[2] = 0;
	if (isnan(z[3])) z[3] = 0;

	/* copy back variables */
	w->hornAngleGRD = hornAngleGRD;
	w->drumAngleGRD = drumAngleGRD;
	if (brake_enagaged & 1) { w->hornIncr = 0; }
	if (brake_enagaged & 2) { w->drumIncr = 0; }
	w->outpos = outpos;
}

void whirlProc (struct b_whirl *w,
                const float * inbuffer,
                float * outbL,
                float * outbR,
                size_t bufferLengthSamples)
{
	whirlProc2(w, inbuffer, outbL, outbR,
			NULL, NULL,
			NULL, NULL,
			bufferLengthSamples);
}


void whirlProc3 (struct b_whirl *w,
                 const float * inbuffer,
                 float * outL, float * outR,
                 float * tmpL, float * tmpR,
                 size_t bufferLengthSamples)
{

	size_t i;
	const float dll = w->drumMic_dll;
	const float dlr = w->drumMic_dlr;
	const float drl = w->drumMic_drl;
	const float drr = w->drumMic_drr;
	const float hll = w->hornMic_hll;
	const float hlr = w->hornMic_hlr;
	const float hrl = w->hornMic_hrl;
	const float hrr = w->hornMic_hrr;

	whirlProc2(w,
	           inbuffer, NULL, NULL,
	           outL, outR,
	           tmpL, tmpR,
	           bufferLengthSamples);

	for (i = 0; i < bufferLengthSamples; ++i) {
		const float tmp = outL[i];
		outL[i] = outL[i] * hll + outR[i] * hlr + tmpL[i] * dll + tmpR[i] * dlr;
		outR[i] = tmp     * hrl + outR[i] * hrr + tmpL[i] * drl + tmpR[i] * drr;
	}
}
