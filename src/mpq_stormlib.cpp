#include "mpq_stormlib.h"
#include "wowmapview.h"

#include <vector>
#include <string>
#include "util.h"

using namespace std;

typedef vector< pair< string, HANDLE* > > ArchiveSet;
static ArchiveSet gOpenArchives;

MPQArchive::MPQArchive(const char* filename) : ok(false)
{
	if (!SFileOpenArchive(filename, 0, MPQ_OPEN_READ_ONLY, &mpq_a )) {
		int nError = GetLastError();
		gLog("Error opening archive %s, error #: 0x%x\n", filename, nError);
		return;
	}
	gLog("Opening archive %s\n", filename);
	
	ok = true;
	gOpenArchives.push_back( make_pair( filename, &mpq_a ) );
}

MPQArchive::~MPQArchive()
{
	SFileCloseArchive(mpq_a);
}

bool MPQArchive::isPartialMPQ(const char* filename)
{
	if (StartsWith(AfterLast(filename,SLASH),"wow-update-"))
		return true;
	return false;
}

bool MPQArchive::applyPatch(const char* filename)
{
	if (!SFileOpenPatchArchive(mpq_a, filename, "", 0))
	{
		gLog("Failed to patch %s\n", filename);
		return false;
	}

	gLog("Patching archive %s\n", filename);
	return true;
}

void MPQArchive::close()
{
	if (ok == false)
		return;
	SFileCloseArchive(mpq_a);
	for(ArchiveSet::iterator it=gOpenArchives.begin(); it!=gOpenArchives.end();++it)
	{
		HANDLE &mpq_b = *it->second;
		if (&mpq_b == &mpq_a) {
			gOpenArchives.erase(it);
			//delete (*it);
			return;
		}
	}
	
}

bool MPQFile::isPartialMPQ(const char* filename)
{
	if (StartsWith(AfterLast(filename,SLASH),"wow-update-"))
		return true;
	return false;
}

void
MPQFile::openFile(const char* filename)
{
	eof = false;
	buffer = 0;
	pointer = 0;
	size = 0;

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end(); ++i)
	{
		HANDLE &mpq_a = *i->second;

		HANDLE fh;

		if( !SFileOpenFileEx( mpq_a, filename, SFILE_OPEN_PATCHED_FILE, &fh ) )
			continue;

		// Found!
		DWORD filesize = SFileGetFileSize( fh );
		size = filesize;

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1) {
			eof = true;
			buffer = 0;
			return;
		}

		buffer = new unsigned char[size];
		SFileReadFile( fh, buffer, (DWORD)size );
		SFileCloseFile( fh );

		return;
	}

	eof = true;
	buffer = 0;
	gLog("cant find file %s in MPQ's\n",filename);
}

MPQFile::MPQFile(const char* filename):
	eof(false),
	buffer(0),
	pointer(0),
	size(0)
{
	openFile(filename);
}

MPQFile::~MPQFile()
{
	close();
}

bool MPQFile::exists(const char* filename)
{
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE &mpq_a = *i->second;

		if( SFileHasFile( mpq_a, filename ) )
			return true;
	}

	return false;
}

void MPQFile::save(const char* filename)
{
/*
	wxFile f;
	f.Open(const char*(filename, wxConvUTF8), wxFile::write);
	f.Write(buffer, size);
	f.Close();
*/
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = rpos;

	return bytes;
}

bool MPQFile::isEof()
{
    return eof;
}

void MPQFile::seek(ssize_t offset)
{
	pointer = offset;
	eof = (pointer >= size);
}

void MPQFile::seekRelative(ssize_t offset)
{
	pointer += offset;
	eof = (pointer >= size);
}

void MPQFile::close()
{
	if (buffer) delete[] buffer;
	buffer = 0;
	eof = true;
}

size_t MPQFile::getSize()
{
	return size;
}

int MPQFile::getSize(const char* filename)
{
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE &mpq_a = *i->second;
		HANDLE fh;
		
		if( !SFileOpenFileEx( mpq_a, filename, SFILE_OPEN_PATCHED_FILE, &fh ) )
			continue;

		DWORD filesize = SFileGetFileSize( fh );
		SFileCloseFile( fh );
		return filesize;
	}

	return 0;
}

const char* MPQFile::getArchive(const char* filename)
{
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE &mpq_a = *i->second;
		HANDLE fh;
		
		if( !SFileOpenFileEx( mpq_a, filename, SFILE_OPEN_PATCHED_FILE, &fh ) )
			continue;

		return i->first.c_str();
	}

	return "unknown";
}

size_t MPQFile::getPos()
{
	return pointer;
}

unsigned char* MPQFile::getBuffer()
{
	return buffer;
}

unsigned char* MPQFile::getPointer()
{
	return buffer + pointer;
}
