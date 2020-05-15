#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include "aplayer.h"
#include "winplayr.h"


//WinPlayer Implemenatation	Listing 6.3

void WinPlayer::Play(void) {
	if (SelectDevice()) return;	//Open a suitable device
	waveOutPause(_device);	//Dont start playing yet
	_paused = true;

	InitializeQueue(128 * 1024L);	//Allocate 128k Queue

	WAVEHDR	waveHdr[2];
	for (int i = 0; i < 2; i++)
	{
		waveHdr[i].dwBufferLength = winBufferSize * _sampleWidth / 8;	//Size in bytes
		waveHdr[i].dwFlags = 0;
		waveHdr[i].dwLoops = 0;
		waveHdr[i].lpData = reinterpret_cast<LPSTR>(new BYTE[waveHdr[i].dwBufferLength * Channels()]);
		waveOutPrepareHeader(_device, &waveHdr[i], sizeof(waveHdr[i]));
		NextBuff(&waveHdr[i]);	//Fill and write buffer to output
	}
	
	//Wait until finishd and both buffers become free
	_paused = false;
	waveOutRestart(_device);	//Start playing now
	while (!_endOfQueue	// queue empty ???
		|| ((waveHdr[0].dwFlags & WHDR_DONE) == 0)	//buffers finished?
		|| ((waveHdr[1].dwFlags & WHDR_DONE) == 0)) {
		FillQueue();	//Top off the queue
		if (_paused) {	//If server thread paused, restart it
			_paused = false;
			cerr << "Sound output restarted\n";
			waveOutRestart(_device);
		}

		Sleep(50 /* ms */);	//Loop about 20 times a second
	}

	MMRESULT err = waveOutClose(_device);
	while (err == WAVERR_STILLPLAYING) {	//If its still playing
		Sleep(250);	//Wait for a bit....
		waveOutClose(_device);	//try again
	};

	for (int i1 = 0; i1 < 2; i1++)
	{
		waveOutUnprepareHeader(_device, &waveHdr[i1], sizeof(waveHdr[i1]));
		delete waveHdr[i1].lpData;
	}
}

//Callback
void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	WinPlayer* me = reinterpret_cast<WinPlayer*>(dwInstance);
	switch (uMsg) {
	case WOM_DONE:	//Done with this buffer
	{
		WAVEHDR* pWaveHdr = reinterpret_cast<WAVEHDR*>(dwParam1);
		me->NextBuff(pWaveHdr);
		break;
	}
	default:
		break;
	}
}

void WinPlayer::NextBuff(WAVEHDR* pWaveHdr) {
	long samplesRead = 0;
	switch (_sampleWidth) {
	case 16:
		samplesRead = FromQueue(reinterpret_cast<Sample16 *>(pWaveHdr->lpData),
			winBufferSize);
		break;
	case 8:
		samplesRead = FromQueue(reinterpret_cast<Sample8 *>(pWaveHdr->lpData),
			winBufferSize);
		break;
	}

	if (samplesRead != 0) {	//I got data, so write it
		pWaveHdr->dwBufferLength = samplesRead * _sampleWidth/8;
		waveOutWrite(_device, pWaveHdr, sizeof(*pWaveHdr));
	}
	else if (!_endOfQueue) {	//Whoops source couldnt keep up
		waveOutPause(_device);	//pause the output
		_paused = true;
		cerr << "Sound output paused due to lack of data\n";
		//Write some zeroes to keep this block in windows queue 
		memset(pWaveHdr->lpData, 0, winBufferSize);
		pWaveHdr->dwBufferLength = 256;
		waveOutWrite(_device, pWaveHdr, sizeof(*pWaveHdr));
	}
	else {	//No data, everythings done
		pWaveHdr->dwFlags |= WHDR_DONE;	//Mark buffer as finished
	}
}

// These are the primary formats supported by Windows 
static struct {
	DWORD format;	//constant
		UINT rate;	//break down for this constant
		UINT channels;
		UINT width;

}winFormats[] = {
	{WAVE_FORMAT_1S16,11025,2,16},
	{WAVE_FORMAT_1S08,11025,2,8},
	{WAVE_FORMAT_1M16,11025,2,16},
	{WAVE_FORMAT_1M08,11025,2,8},
	{WAVE_FORMAT_2S16,22050,2,16},
	{WAVE_FORMAT_2S08,22050,2,8},
	{WAVE_FORMAT_2M16,22050,2,16},
	{WAVE_FORMAT_2M08,22050,2,8},
	{WAVE_FORMAT_4S16,44100,2,16},
	{WAVE_FORMAT_4S08,44100,2,8},
	{WAVE_FORMAT_4M16,44100,2,16},
	{WAVE_FORMAT_4M08,44100,2,8},
	{0,0,0,0}
};

//Negotiate the sound format and open a suitable output device

int WinPlayer::SelectDevice(void) {
	//Get everyone elses idea of format
	int channelsMin = 1, channelsMax = 12, channelsPreffered = 0;
	long rateMin = 8000, rateMax = 44100, ratePreffered = 22050;

	MinMaxChannels(&channelsMin, &channelsMax, &channelsPreffered);
	if (channelsMin > channelsMax) {
		cerr << "Couldnt negotiate channels.\n";
		exit(1);
	}

	MinMaxSamplingRate(&rateMin, &rateMax, &ratePreffered);
	if (rateMin > rateMax) {
		cerr << "Couldnt negotiate rate.\n";
		exit(1);
	}

	//First try for an exact match
	static const int NO_MATCH = 100000;
	UINT matchingDevice = NO_MATCH;
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = channelsPreffered;
	waveFormat.nSamplesPerSec = ratePreffered;
	waveFormat.wBitsPerSample = 8 * sizeof(Sample16);
	waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
	waveFormat.cbSize = 0;
	MMRESULT err = waveOutOpen(0, WAVE_MAPPER, &waveFormat, 0, 0, WAVE_FORMAT_QUERY);

	if (err == 0) {
		matchingDevice = WAVE_MAPPER;
		channelsMax = channelsMin = channelsPreffered;
		rateMax = rateMin = ratePreffered;
		_sampleWidth = 16;
	}
	else {
		cerr << "WinPlay: Custom format failed, ";
		cerr << "trying standard formats.\n";
	}

	//Get count of available devices
	UINT numDevs = waveOutGetNumDevs();
	if (numDevs == 0) {
		cerr << "No sound output devices found!?\n";
		exit(1);
	}
	
	//Checke ach available devices
	for (UINT i = 0; (i < numDevs) && (matchingDevice == NO_MATCH); i++) {
		//What formats does this device support?
		WAVEOUTCAPS waveOutCaps;
		MMRESULT err = waveOutGetDevCaps(i, &waveOutCaps, sizeof(waveOutCaps));
		if (err == MMSYSERR_NOERROR) {
			cerr << "Couldnt get capabilities of device" << i << "\n";
			continue;
		}

		//Check each standard format
		for (UINT j = 0; winFormats[j].format != 0; j++) {
			if ((winFormats[j].format & waveOutCaps.dwFormats)	//supported?
				&& (rateMin <= winFormats[j].rate)	//rate ok?
				&& (rateMax >= winFormats[j].rate)
				&& (channelsMin <= winFormats[j].channels)
				&& (channelsMax >= winFormats[j].channels)) {
				//set up my parameters
				matchingDevice = 1;
				rateMin = rateMax = ratePreffered = winFormats[j].rate;
				channelsPreffered = winFormats[j].channels;
				channelsMin = channelsMax = channelsPreffered;
				_sampleWidth = winFormats[j].width;

				//Set up WAVEFORMATEX structure accordingly
				waveFormat.wFormatTag = WAVE_FORMAT_PCM;
				waveFormat.nChannels = winFormats[j].channels;
				waveFormat.nSamplesPerSec = winFormats[j].rate;
				waveFormat.wBitsPerSample = winFormats[j].width;
				waveFormat.nBlockAlign = waveFormat.wBitsPerSample/8 * waveFormat.nChannels;
				waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
				waveFormat.cbSize = 0;
			}
		}
	}

	if (matchingDevice == NO_MATCH) {
		cerr << "cant handle this sound format.\n";
		cerr << "Rate: " << rateMin << "-" << rateMax << "\n";
		cerr << "Channels: " << channelsMin << "-" << channelsMax << "\n";
		return 1;
	}

	//If we found a match, set everything '
	SetChannelsRecursive(channelsPreffered);
	SetSamplingRateRecursive(ratePreffered);

	//Open the matching device
	MMRESULT err2 = waveOutOpen(&_device, matchingDevice, &waveFormat, reinterpret_cast<DWORD>(WaveOutCallback), reinterpret_cast<DWORD>(this), CALLBACK_FUNCTION);

	if (err2) {
		cerr << "Couldnt open WAVE output device.\n";
		exit(1);
	}

	return 0;
}
