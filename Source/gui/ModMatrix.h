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


#pragma once
#include "../GlobalIncludes.h"
#include <cstring>

// contains all modulation destinations in an oscillator (all types)
struct ModDestOsc {
  float pitch_linear;
  float pitch_exponential;
  float vol;
  // float drift;
  float pulse_width;
  float carrier_ratio;
  float modulator_ratio;
  float fm_amount;
  float hp_freq;
  float lp_freq;
  float x;
  float y;
  float position;
  float detune;
  float spread;
  float arp_speed;
};

// contains all modulation destinations in a filter (all types)
struct ModDestFilter {
  float freq;
  float res;
  float drive;
  float gain;
  float saturation;
  float env_amount;
  float vel_amount;
  float kbd_amount;
  float SEM_transition;
  float formant_transition;
  float ringmod_amount;
};

// contains all modulation destinations for an envelope
struct ModDestADSR {
  float attack;
  float decay;
  float sustain;
  float release;
};

// contains all modulation destination within an envelope
struct ModDestLFO {
  float freq;
};

struct ModDestAmp {
  float gain;
  float pan;
  float vel;
};

struct ModDestDistortion {
  float boost;
  float drywet;
};

struct ModDestDelay {
  float time;
  float feedback;
  float hp_freq;
  float ducking;
  float dry;
  float wet;
};

struct ModDestPhaser {
  float amount;
  float rate;
  float drywet;
  float freq;
  float feedback;
};

struct ModDestFlanger {
  float amount;
  float freq;
  float feedback;
  float drywet;
};

struct ModDestChorus {
  float amount;
  float freq;
  float feedback;
  float drywet;
};

struct ModDestMisc {
  float master;
  float glide;
};

struct ModDestArp {
  float speed;
  float gate;
};

struct ModDestXY {
  float x;
  float y;
};

struct ModDestVoice {
  ModDestOsc osc[3];
  ModDestFilter filter[2];
  ModDestADSR adsr[3];
  ModDestLFO lfo[3];
  ModDestAmp amp;
  ModDestDistortion distortion;
  float pitch_linear;
  float pitch_exponential;
};

struct ModDestinations {
  ModDestVoice voice[VOICES];
  ModDestADSR global_adsr;
  ModDestLFO global_lfo;
  ModDestFilter filter3;
  ModDestDelay delay;
  ModDestPhaser phaser;
  ModDestFlanger flanger;
  ModDestChorus chorus;
  ModDestArp arp;
  ModDestXY xy;
  ModDestMisc misc;
};

struct ModSourceVoice {
  float* osc[3];
  float* filter[2];
  float* adsr[3];
  float* lfo[3];
  float* MIDI_key;
  float* MIDI_velocity;
  float* random;
  float* unison_position;
  float* arp_mod_1;
  float* arp_mod_2;

  //this was abandoned, but is here to avoid crashes:
  float* MIDI_aftertouch;
};

struct ModSources {
  ModSourceVoice voice[VOICES];
  float* global_lfo;
  float* global_adsr;
  float* filter3;
  float* modwheel;
  float* pitchwheel;
  float* x;
  float* y;
  float* MIDI_channel_pressure;
  float* MIDI_breath;
  float* constant;
  float* soft_pedal;
  float* sustain_pedal;
};

class ModMatrixRow {
public:
  ModMatrixRow(){}
  //ModMatrixRow(ModSources &p_source, ModSources &p_destination);
  void setSourcesAndDestinations(ModSources* p_source, ModDestinations* p_destination);

  operator bool() const {
    return m_active_1 || m_active_2;
  }

  void applyModulation();

  //this sets for source and scale
  void setModSource(int p_source, float** p_source_pointers, int& p_source_store);

  //this sets for dest1 and dest2
  void setModDestination(int p_destination, float** p_destination_pointers, bool &p_dest_poly, int &p_destination_store);

  void checkRowActive();

  void setModScale(int p_scale);
  void setModSource(int p_scale); //this only sets source (as opposed to function with same name)

  void setModAmount1(float p_mod_amount){
    m_mod_amount_1 = p_mod_amount;
  }

  void setModAmount2(float p_mod_amount){
    m_mod_amount_2 = p_mod_amount;
  }

  void setScaleAmount(float p_scale_amount){
    m_scale_amount = p_scale_amount;
  }

  void setMostRecentVoice(int p_voice){
    m_most_recent_voice = p_voice;
  }
  
  void setModDestination1(int p_destination);
  void setModDestination2(int p_destination);

  bool usesLFO0();
  bool usesLFO1();
  bool usesLFO2();
  bool usesLFO3();//global
  bool usesADSR0();//mod
  bool usesADSR1();//global

private:
  int m_most_recent_voice = 0;

  int m_source = 0;
  int m_destination_1 = 0;
  int m_destination_2 = 0;
  int m_scale = 0;
  bool m_active_1 = false;//only for one of the two modulation slots
  bool m_active_2 = false;//only for one of the two modulation slots

  float m_mod_amount_1 = 0.f;
  float m_mod_amount_2 = 0.f;
  float m_scale_amount = 0.f;

  float* m_source_value[VOICES];
  float* m_destination_1_value[VOICES];
  float* m_destination_2_value[VOICES];
  float* m_scale_value[VOICES];

  bool m_destination_1_poly = false;
  bool m_destination_2_poly = false;
  bool m_source_poly = false;


  ModSources* m_sources;
  ModDestinations* m_destinations;
};

class ModMatrix {
public:
  ModMatrix(){}

  void setSourcesAndDestinations(ModSources *p_source, ModDestinations *p_destination);

  void applyModulation();

  void setModSource(int p_row, int p_source);
  void setModDestination1(int p_row, int p_destination);
  void setModDestination2(int p_row, int p_destination);
  void setModScale(int p_row, int p_scale);
  void setModAmount1(int p_row, float p_mod_amount);
  void setModAmount2(int p_row, float p_mod_amount);
  void setScaleAmount(int p_row, float p_scale_amount);
  void setMostRecentVoice(int p_voice){
    m_most_recent_voice = p_voice;
    for(int row = 0; row < MODMATRIX_ROWS; ++row){
      m_row[row].setMostRecentVoice(p_voice);
    }
  }
  void checkWhichSourceToRender();


  void zeroAllSources();
  void zeroAllDestinations();

  std::function<void(bool, bool, bool, bool, bool, bool)> setSourcesToRender;
private:
  int m_most_recent_voice = 0;
  ModMatrixRow m_row[MODMATRIX_ROWS];
  ModSources* m_sources;
  ModDestinations* m_destinations;
};