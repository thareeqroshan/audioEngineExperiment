#include <fstream>
#include "audio.h"
#include "open.h"
#include "aplayer.h"
#include "winplayr.h"
#include <commdlg.h>
#include <WinBase.h>
#include <crtdbg.h>
#include <cassert>


//Play One file under windows	// 6.7
void PlayFile(char* fileName) {
	cerr << "File: " << fileName << "\n";
	ifstream input(fileName, ios::in || ios::binary);
	if (!input.is_open()) {
		cerr << "Couldnt open file " << fileName << "\n";
		return;
	}
	AudioAbstract* audioFile = OpenFormat(input);
	if (audioFile) {
		WinPlayer player(audioFile);
		player.Play();
		delete audioFile;
		cerr << "Finished playing " << fileName << ".\n";
	}
	cerr << "\n";
	//Paranoia:	Check that the heap is still okay
	assert(_CrtCheckMemory());
}
//Play files interactively under windows	//6.5
void Interactive() {
	while (1) {
		OPENFILENAME ofn;
		char fileName[512] = "";
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFile = (LPWSTR)fileName;
		ofn.lpstrTitle = L"Play Sound File";
		ofn.nMaxFile = 512;
		if (GetOpenFileName(&ofn))	//Throw up dialog
			PlayFile(fileName);
		else
			return;
	}
}
//Play files non interactively under windows	//6.6
void Batch(char *filePattern) {
	WIN32_FIND_DATA fileData;
	bool finished = false;
	HANDLE hSearch = FindFirstFile((LPWSTR)filePattern, &fileData);
	if (hSearch == INVALID_HANDLE_VALUE) {
		cerr << "Cant find " << filePattern << "\n";
		finished = true;
	}
	while (!finished)
	{
		//Skip directories especially . and ..
		if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			char fileName[2048];
			strcpy(fileName, filePattern);
			char *p = fileName + strlen(fileName);
			while ((p > fileName) && (*p != '\\') && (*p != '/'))
				p--;
			strcpy(p + 1, (char*)fileData.cFileName);
			PlayFile(fileName);
		}
		finished = !FindNextFile(hSearch, &fileData);
	}
	FindClose(hSearch);
}


int main(int argc, char** argv) {
	//For testing you can hardwire a collection of files by
	//uncommenting the following lines
	//Batch("C:\\samples\\*.*");
	//exit(0)

	if (argc == 1) Interactive();
	else
		while (--argc)
			Batch(*++argv);
	return 0;
}

