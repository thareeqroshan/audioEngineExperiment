#pragma once
#ifndef SINEWAVE_H_INCLUDED
#define SINEWAVE_H_INCLUDED

#include "audio.h"
class SineWave :
	public AudioAbstract
{

	//TODO:	SineWave Member lising	Listing 4.14
protected:
	void MinMaxChannels(int* min, int* max, int* preffered) {
		*min = *max = *preffered = 1;
	};

protected:
	size_t GetSamples(AudioSample* buff, size_t bytesWanted);

private:
	AudioSample* sine;	//Table of sine values
	int length;	//Length of the table
	int pos;	//current position in the table 
	int frequency;	//desired output frequency

public:
	void Frequency(int f) { frequency = f; }
private:
	void Init(void) {
		sine = (AudioSample*)0;
		length = 0; pos = 0; frequency = 1;
	}

public:
	SineWave(void) :AudioAbstract() { Init(); }
	SineWave(int f) :AudioAbstract() {
		Init();
		Frequency(f);
	};

	~SineWave(void) {
		if (sine) delete[] sine;
	}
private:
	void BuildTable(long rate);
	
public:
	void SamplingRate(long rate) {
		AudioAbstract::SamplingRate(rate);
		BuildTable(rate);
	}
};



#endif