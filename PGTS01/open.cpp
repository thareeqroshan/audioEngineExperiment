#include "open.h"

//Include headers for various formats

//------- Uncomment from Here -------

//#include "mpeg.h"
//#include "au.h"
//#include "voc.h"
//#include "wav.h"
//#include "aiff.h"
//#include "iff.h"
//#include "midi.h"
//#include "mod.h"
//
//AudioAbstract* OpenFormat(istream& file) {
//	if (isVocFile(file)) {
//		file.seekg(0);
//		return new VocRead(file);
//	}
//	if (isAuFile(file)) {
//		file.seekg(0);
//		return new AuRead(file);
//	}
//	if (isWaveFile(file)) {
//		file.seekg(0);
//		return new WaveRead(file);
//	}
//	if (isAiffFile(file)) {
//		file.seekg(0);
//		return new AiffRead(file);
//	}
//	if (isIffFile(file)) {
//		file.seekg(0);
//		return new IffRead(file);
//	}
//	if (isMidiFile(file)) {
//		file.seekg(0);
//		return new MidiRead(file);
//	}
//	if (isModFile(file)) {
//		file.seekg(0);
//		return new ModRead(file);
//	}
//	if (isMpegFile(file)) {
//		file.seekg(0);
//		return new MpegRead(file);
//	}
//	cerr << "I dont recognize this format\n";
//	return 0;
//}
