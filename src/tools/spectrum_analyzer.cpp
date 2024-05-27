#include <tools/spectrum_analyzer.h>

SPECTRUM_ANALYZER_RAW_BUFFER_t spectrum_analyzer_results;


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
    spectrum_analyzer_results.amplitude_left[freq_amp] = map(analogRead(DC_left),0 , 4096, 0, 1024);
    spectrum_analyzer_results.amplitude_right[freq_amp] = map(analogRead(DC_right), 0, 4096, 0, 1024);
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
  }

#ifdef DEBUG_DISPLAY_SPECTRUM_AMPLITUDES
    Serial.print(spectrum_analyzer_results.amplitude_left[5]);
    Serial.print("\t");
    Serial.print(spectrum_analyzer_results.amplitude_left[5]);
    Serial.print("\t");

  Serial.print(spectrum_analyzer_results.amplitude_left[6]);
  Serial.print("\t");
  Serial.println(spectrum_analyzer_results.amplitude_right[6]);
  Serial.print("\t");
#endif

}

uint16_t spectrum_total_volume() {
  uint16_t greatestVol = 0;
  for (int i = 0; i < 7; i++) {
    if (spectrum_analyzer_results.amplitude_left[i] > greatestVol) {
      greatestVol = spectrum_analyzer_results.amplitude_left[i];
    }
    if (spectrum_analyzer_results.amplitude_right[i] > greatestVol) {
      greatestVol = spectrum_analyzer_results.amplitude_right[i];
    }
  }
  return greatestVol;
}

