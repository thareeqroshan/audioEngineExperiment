#include "audio.h"


//TODO: AudioAbstract Sampling Rate Negotiation Implementation - Listiing 4.8

void AudioAbstract::NegotiateSamplingRate(void) {

	if (Next())	//Are we the leftmost object
		Next()->NegotiateSamplingRate();	//No?? Keep going bruh
	else { // We have reached the left most object
		long min = 8000, max = 44100, preffered = 44100;
		MinMaxSamplingRate(&min, &max, &preffered);	//get preffered values
		if (min > max) {
			cerr << "Couldnt negotiate the sample rate.\n";
			exit(1);
		}
		SetSamplingRateRecursive(preffered);
	}
}

void AudioAbstract::MinMaxSamplingRate(long* min, long* max, long* preffered) {
	if (Previous()) Previous()->MinMaxSamplingRate(min, max, preffered);
	if (_samplingRate)*preffered = _samplingRate;
	if (*preffered < *min) *preffered = *min;
	if (*preffered > *max) *preffered = *max;
}

void AudioAbstract::SetSamplingRateRecursive(long s) {
	if (Previous()) //Set towards the right first
		Previous()->SetSamplingRateRecursive(s);
	SamplingRate(s);//Set it
	_samplingRateFrozen = true; //Yes we've negotiated
}

//TODO: AudioAbstract Channel Negotiation Implementation- Listiing 4.10

void AudioAbstract::NegotiateChannels(void) {
	if (Next())
		Next()->NegotiateChannels();
	else {
		int min = 1, max = 2, preffered = 1;	//Some reasonable default
		MinMaxChannels(&min, &max, &preffered);
		if (min > max) {
			cerr << "Couldn't negotiate the channels.\n";
			exit(1);
		}
		SetChannelsRecursive(preffered);
	}
}

void AudioAbstract::MinMaxChannels(int* min, int* max, int* preffered) {
	if (Previous()) Previous()->MinMaxChannels(min, max, preffered);
	if (_channels) *preffered = _channels;
	if (*preffered < *min) *preffered = *min;
	if (*preffered > * max) *preffered = *max;
}

void AudioAbstract::SetChannelsRecursive(int ch) {
	if (Previous()) Previous()->SetChannelsRecursive(ch);
	Channels(ch);
	_channelsFrozen = true;
}

//TODO: AudioAbstract Utility Definitions- Listiing 4.11

long ReadIntMsb(istream& in, int size) {
	if (size <= 0) return 0;
	long l = ReadIntMsb(in, size - 1) << 8;
	l |= static_cast<long>(in.get()) & 255;
	return l;
}

long BytesToIntMsb(void *vBuff, int size) {
	unsigned char* buff = reinterpret_cast<unsigned char*>(vBuff);
	if (size <= 0) return 0;
	long l = BytesToIntMsb(buff, size - 1) << 8;
	l |= static_cast<long>(buff[size - 1]) & 255;
	return l;
}

long ReadIntLsb(istream& in, int size) {
	if (size <= 0) return 0;
	long l = static_cast<long>(in.get()) & 255;
	l |= ReadIntLsb(in, size - 1) << 8;
	return l;
}

long BytesToIntLsb(void *vBuff, int size) {
	unsigned char* buff = reinterpret_cast<unsigned char*>(vBuff);
	if (size <= 0) return 0;
	long l = static_cast<long>(buff[size - 1]) & 255;
	l |= BytesToIntLsb(buff, size - 1) << 8;
	return l;
}

void SkipBytes(istream& in, int size) {
	while (size-- > 0)
		in.get();
}


void WriteIntMsb(ostream& out, long l, int size) {
	if (size <= 0) return;
	WriteIntMsb(out, l >> 8, size - 1);	//Write MS Bytes
	out.put(l & 255);	//Write LS Byte	
}

void WriteIntLsb(ostream& out, long l, int size) {
	if (size <= 0) return;
	out.put(l & 255);	//Write LS Byte
	WriteIntLsb(out, l >> 8, size - 1);	//Write rest
}



