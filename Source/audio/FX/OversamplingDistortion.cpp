/*
** Odin 2 Synthesizer Plugin
** Copyright (C) 2020 - 2021 TheWaveWarden
**
** Odin 2 is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Odin 2 is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#include "OversamplingDistortion.h"
#include "../../GlobalIncludes.h"
#include <cmath>

OversamplingDistortion::OversamplingDistortion() {
}

OversamplingDistortion::~OversamplingDistortion() {
}

double OversamplingDistortion::doDistortion(double p_input) {

	// do linear interpolation
	double input_upsampled[3] = {
	    0.66666666 * m_last_input + 0.33333333 * p_input, 0.33333333 * m_last_input + 0.66666666 * p_input, p_input};

	m_last_input = p_input;

	m_threshold_smooth =
	    m_threshold_smooth * THRESHOLD_SMOOTHIN_FACTOR + (1 - THRESHOLD_SMOOTHIN_FACTOR) * (m_threshold);

	//theshold is now boost, so we need to subtract mod (control setter is inverted as well)
	float threshold_modded = (m_threshold_smooth - *m_threshold_mod) * (1.f - THRESHOLD_MIN) + THRESHOLD_MIN;
	threshold_modded       = threshold_modded > 1 ? 1 : threshold_modded;
	threshold_modded       = threshold_modded < THRESHOLD_MIN ? THRESHOLD_MIN : threshold_modded;

	// do distortion
	switch (m_algorithm) {
	case Clamp:
		for (int sample = 0; sample < 3; ++sample) {
			if (input_upsampled[sample] > m_bias && input_upsampled[sample] > m_bias + threshold_modded) {
				input_upsampled[sample] = m_bias + threshold_modded;
			} else if (input_upsampled[sample] < m_bias && input_upsampled[sample] < m_bias - threshold_modded) {
				input_upsampled[sample] = m_bias - threshold_modded;
			}
		}
		break;
	case Zero:
		// half "boost" for zero
		threshold_modded = 0.5f + threshold_modded * 0.5f;
		for (int sample = 0; sample < 3; ++sample) {
			if (input_upsampled[sample] > m_bias && input_upsampled[sample] > m_bias + threshold_modded) {
				input_upsampled[sample] = 0.;
			} else if (input_upsampled[sample] < m_bias && input_upsampled[sample] < m_bias - threshold_modded) {
				input_upsampled[sample] = 0.;
			}
		}
		break;
	case Sine:
		for (int sample = 0; sample < 3; ++sample) {
			input_upsampled[sample] = sin(input_upsampled[sample] /*/ threshold_modded*/);
		}
	case Cube:
		for (int sample = 0; sample < 3; ++sample) {
			//input_upsampled[sample] /= threshold_modded;
			input_upsampled[sample] *= input_upsampled[sample] * input_upsampled[sample];
		}
		break;
	case Fold:
		for (int sample = 0; sample < 3; ++sample) {
			//threshold_modded = threshold_modded < 0.05 ? 0.05 : threshold_modded;
			while (fabs(input_upsampled[sample]) > threshold_modded) {
				if (input_upsampled[sample] > threshold_modded) {
					input_upsampled[sample] = 2 * threshold_modded - input_upsampled[sample];
				} else {
					input_upsampled[sample] = -2 * threshold_modded - input_upsampled[sample];
				}
			}
		}
		break;
	}

	m_downsampler.doFilter(input_upsampled[0]);
	m_downsampler.doFilter(input_upsampled[1]);
	const auto downsampled = m_downsampler.doFilter(input_upsampled[2]);

	float drywet_modded = m_drywet + *m_drywet_mod;
	drywet_modded       = drywet_modded > 1 ? 1 : drywet_modded;
	drywet_modded       = drywet_modded < 0 ? 0 : drywet_modded;

	// return only the last of the three samples
	switch (m_algorithm) {
	case Clamp:
	case Fold:
	case Zero:
		return downsampled * drywet_modded / threshold_modded * DISTORTION_OUTPUT_SCALAR + p_input * (1.f - drywet_modded);
	case Sine:
	case Cube:
		return downsampled * drywet_modded + p_input * (1.f - drywet_modded);
	default:
		return p_input;
		break;
	}
}
