#ifndef COMPR_H_INCLUDED
#define COMPR_H_INCLUDED

#include "audio.h"
#include <iostream>
#include <cstddef>

class AbstractDecompressor
{
protected:
	AudioAbstract& _dataSource;	//The object to get raw bytes from
	size_t ReadBytes(AudioByte* buff, size_t length) {
		return _dataSource.ReadBytes(buff, length);
	};
public:
	AbstractDecompressor(AudioAbstract& a) : _dataSource(a) {};
	virtual ~AbstractDecompressor() {};
	virtual size_t GetSamples(AudioSample*, size_t) = 0;
	virtual void MinMaxSamplingRate(long*, long*, long*) {
		cerr << "MinMaxSamplingRate undefined\n";
		exit(1);
	}
	virtual void MinMaxChannels(int*, int*, int*) {
		cerr << "MinMaxChannels undefined\n";
		exit(1);
	}
};

//8 bit PCM classes //10.3
class DecompressPcm8Signed : public AbstractDecompressor {
public:
	DecompressPcm8Signed(AudioAbstract& a) :AbstractDecompressor(a) {
		cerr << "Encoding: 8 bit signed (two's compliment) PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

class DecompressPcm8UnSigned : public AbstractDecompressor {
public:
	DecompressPcm8UnSigned(AudioAbstract& a) :AbstractDecompressor(a) {
		cerr << "Encoding: 8 bit unsigned (excess-128) PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};
//16 bit PCM classes //10.5
class DecompressPcm16MsbSigned : public AbstractDecompressor {
public:
	DecompressPcm16MsbSigned(AudioAbstract& a) :AbstractDecompressor(a) {
		cerr << "Encoding: 16 bit MSB PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

class DecompressPcm16LsbSigned : public AbstractDecompressor {
public:
	DecompressPcm16LsbSigned(AudioAbstract& a) :AbstractDecompressor(a) {
		cerr << "Encoding: 16 bit LSB PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

#endif // !COMPR_H_INCLUDED




