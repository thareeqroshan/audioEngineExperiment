#pragma once

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED


#include<typeinfo>
#include<iostream>
#include<cstddef>

using namespace std;

//TODO : Audio abstract utility declarations- Listiing 4.5 -	DOOONNNNEE
typedef short AudioSample; // single audio sample
typedef unsigned char AudioByte;

long ReadIntMsb(istream& in, int bytes);
long BytesToIntMsb(void* buff, int bytes);
long ReadIntLsb(istream& in, int bytes);
long BytesToIntLsb(void* buff, int bytes);
void SkipBytes(istream& in, int bytes);

void WriteIntMsb(ostream& out, long l, int bytes);
void WriteIntLsb(ostream& out, long l, int bytes);

class AudioAbstract
{

	//TODO: AudioAbstract Interface- Listiing 4.6
private:
	AudioAbstract *_previous; //object to get data from
	AudioAbstract *_next;	  //object pulling data from us
public:
	AudioAbstract *Previous(void) { return _previous;}
	void Previous(AudioAbstract* a) { _previous = a; }
	AudioAbstract* Next(void) { return _next; }
	void Next(AudioAbstract* a) { _next = a; }

public:
	AudioAbstract(void) {
		_previous = 0;
		_next = 0;
		_samplingRate = 0;	_samplingRateFrozen = false;
		_channels = 0;		_channelsFrozen = false;
	};

public:
	AudioAbstract(AudioAbstract *audio) {
		_previous = audio;
		_next = 0;
		audio->Next(this);
		_samplingRate = 0;	_samplingRateFrozen = false;
		_channels = 0;		_channelsFrozen = false;
	}

public:
	virtual ~AudioAbstract(void) {};

public:
	//Returns number of samples actually read, 0 an error
	//This should always return the full request unless there is and error or an end of data 
	virtual size_t GetSamples(AudioSample*, size_t) = 0;

public:
	virtual size_t ReadBytes(AudioByte* buff, size_t length) {
		return Previous()->ReadBytes(buff, length);
	}

	//TODO: AudioAbstract Sampling rate negotiation interface- Listiing 4.7

private:
	long _samplingRate;
	bool _samplingRateFrozen;

public:
	virtual long SamplingRate(void) {
		if (!_samplingRateFrozen)	//Not frozen??
			NegotiateSamplingRate();	//Go figure out
		return _samplingRate;
	};

	virtual void SamplingRate(long s) {
		if (_samplingRateFrozen) {
			cerr << "Cant change the sampling rate.\n";
			exit(1);
		}
		_samplingRate = s;
	};
public:
	virtual void NegotiateSamplingRate(void);

public:
	virtual void MinMaxSamplingRate(long* min, long* max, long* preffered);
	virtual void SetSamplingRateRecursive(long s);

	//TODO: AudioAbstract Channel Negotitation Interface- Listiing 4.9

private:
	long _channels;
	bool _channelsFrozen;
public:
	virtual int Channels(void) {
		if (!_channelsFrozen) NegotiateChannels();
		return _channels;
	};

	virtual int Channels(int ch) {
		if (_channelsFrozen) {
			cerr << "Can't change the number of channels.\n";
			exit(1);
		}
		_channels = ch;
	};

	virtual void NegotiateChannels(void);
	virtual void MinMaxChannels(int* min, int* max, int* preffered);
	virtual void SetChannelsRecursive(int s);

};

#endif