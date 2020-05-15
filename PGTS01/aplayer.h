#pragma once
#ifndef ABSTRACTPLAYER_H_INCLUDED
#define ABSTRACTPLAYER_H_INCLUDED
#include "audio.h"


//TODO: Abstract player class		Listing - 5.2
class AbstractPlayer :
	public AudioAbstract
{
protected:
	typedef short Sample16;
	typedef signed char Sample8;

	volatile AudioSample* _queue, * _queueEnd; //Begin/End of the queue memory
	volatile AudioSample* volatile _queueFirst;//First sample
	volatile AudioSample* volatile _queueLast;//Last sample

	void InitializeQueue(unsigned long queueSize); //Create a queue
	void FillQueue(void);	//Fill it up
	long FromQueue(Sample8* pDest, long bytes);
	long FromQueue(Sample16* pDest, long bytes);

private:
	void DataToQueue(long);	//Used by FillQueue
	void DataFromQueue(Sample8*, long);	//Used by FromQueue(Sample8*..)
	void DataFromQueue(Sample16*, long);	//Used by FromQueue(Sample16*..)

private:
	size_t GetSamples(AudioSample*, size_t) { exit(1); return 0; };

protected:
	bool _endOfSource;	//true -> last data read from source
	bool _endOfQueue;	//true -> last data read from queue
public:
	AbstractPlayer(AudioAbstract* a);
	~AbstractPlayer();
	virtual void Play() = 0;
};

#endif
