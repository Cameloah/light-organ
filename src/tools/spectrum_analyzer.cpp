#include <tools/spectrum_analyzer.h>

spectrum_analyzer_raw_buffer spectrum_analyzer_results;


void spectrum_analyzer_init() {
  //Set spectrum Shield pin configurations
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(DC_left, INPUT);
  pinMode(DC_right, INPUT);
  digitalWrite(STROBE, HIGH);
  digitalWrite(RESET, HIGH);

  //Initialize Spectrum Analyzers
  digitalWrite(STROBE, LOW);
  delay(1);
  digitalWrite(RESET, HIGH);
  delay(1);
  digitalWrite(STROBE, HIGH);
  delay(1);
  digitalWrite(STROBE, LOW);
  delay(1);
  digitalWrite(RESET, LOW);
}


void spectrum_read_frequencies()
{
  for (int freq_amp = 0; freq_amp < 7; freq_amp++)
  {
    spectrum_analyzer_results.amplitude_left[freq_amp] = analogRead(DC_left);
    spectrum_analyzer_results.amplitude_right[freq_amp] = analogRead(DC_right);
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
  }
}

