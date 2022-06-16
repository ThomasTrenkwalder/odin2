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

#include "ChiptuneOscillator.h"
#include <ctime>

#define NOISE_SEGMENT_LENGTH 30

ChiptuneOscillator::ChiptuneOscillator() {
	m_nr_of_wavetables = NUMBER_OF_WAVETABLES + 9; //+9 for draw tables

	// seed random
	std::srand(std::time(nullptr));
}

ChiptuneOscillator::~ChiptuneOscillator() {
}

float ChiptuneOscillator::doOscillate() {
    jassert(m_samplerate > 0);

	if (m_generate_noise) {
		return generateChipNoise() * m_volume_factor;
	} else {
		return doWavetable() * m_volume_factor;
	}
}

float ChiptuneOscillator::generateChipNoise() {

	// do 3x oversampling here to avoid aliasing
	m_read_index += m_wavetable_inc;
	if (m_read_index > NOISE_SEGMENT_LENGTH * 3) {
		m_read_index       = 0.f;
		m_last_noise_value = std::rand() % 16 - 8;
		m_last_noise_value *= 0.125f;
	}

	// return only the last of the three samples
	m_downsampler.doFilter(m_last_noise_value);
	m_downsampler.doFilter(m_last_noise_value);
	return m_downsampler.doFilter(m_last_noise_value);
}

void ChiptuneOscillator::update() {
	m_mod_exp_other = (float)m_chiptune_arp.doArpeggiator();
	WavetableOsc1D::update();
}

void ChiptuneOscillator::setSampleRate(float p_samplerate) {
	WavetableOsc1D::setSampleRate(p_samplerate);
	m_samplerate = p_samplerate;
	m_chiptune_arp.setSampleRate(p_samplerate);
	m_dc_blocking_filter.reset();
}

void ChiptuneOscillator::reset() {

	// call baseclass first
	WavetableOsc1D::reset();

	m_chiptune_arp.reset();
	m_read_index = 0.0;
	m_downsampler.reset();
}
