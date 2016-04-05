#include "writer.h"
#include <stdio.h>
#include <string>

bool writetd(char* fn, CTracks* t) {
	std::string eowrvniofrevn;
	eowrvniofrevn = "..\\data\\" + std::string(fn);
	FILE* fp = fopen(eowrvniofrevn.c_str(),"wb");
	if(!fp) return false;
	CPatterns* p = &t->ptns;
	fwrite(FILEHEADER,sizeof(FILEHEADER),1,fp);;
	{ //header
		Header h;
		h.ptncount=p->size();
		h.trackcount = t->nTrackCount;
		h.maxptnlen=MAXPTNLEN;
		h.totalrowcount = 0;
		for(int c=0;c<t->nTrackCount;c++) {
			h.noteoffs[c] = ((CIntTrack*)&*(*t)[c])->nNoteOff;
		}
		for(CPatterns::iterator j=p->begin();j!=p->end();j++) {
			h.totalrowcount+=j->length;
		}
		fwrite(&h,sizeof(h),1,fp);

	}{ //ptns
		for(CPatterns::iterator j=p->begin();j!=p->end();j++) {
			fwrite(&j->length,sizeof(j->length),1,fp);
		}

	}{ //data
		int c;
		int di = 0;
		CIntTrack* it;
		for(c=0;c<t->nTrackCount;c++) {
			di = 0;
			it = (CIntTrack*)&*(*t)[c];
			for(CPatterns::iterator j=p->begin();j!=p->end();j++) {
				fwrite(it->pData+di,sizeof(int)*j->length,1,fp);
				di+=MAXPTNLEN;
			}
			di = 0;
			for(CPatterns::iterator j=p->begin();j!=p->end();j++) {
				fwrite(it->pInterp+di,sizeof(bool)*j->length,1,fp);
				di+=MAXPTNLEN;
			}
		}
	}
	fclose(fp);
	return true;
}

bool readtd(char* fn, CTracks* t) {
	std::string eowrvniofrevn;
	eowrvniofrevn = "..\\data\\" + std::string(fn);
	FILE* fp = fopen(eowrvniofrevn.c_str(),"rb");
	if(!fp) return false;
	CPatterns* p = &t->ptns;
	char buf[sizeof(FILEHEADER)];
	fread(buf,sizeof(FILEHEADER),1,fp);
	Header h;
	fread(&h,sizeof(h),1,fp);
	{
		CPattern pd;
		p->clear();
		for(pd.index=0;pd.index<h.ptncount;pd.index++) {
			fread(&pd.length,sizeof(pd.length),1,fp);
			p->push_back(pd);
		}
	}{
		int c, di=0;
		CIntTrack* it;
		for(c=0;c<h.trackcount;c++) {
			di=0;
			it = (CIntTrack*)&*(*t)[c];
			for(CPatterns::iterator j=p->begin();j!=p->end();j++) {
				fread(it->pData+di,sizeof(int)*j->length,1,fp);
				di+=MAXPTNLEN;
			}
			di=0;
			for(CPatterns::iterator j=p->begin();j!=p->end();j++) {
				fread(it->pInterp+di,sizeof(bool)*j->length,1,fp);
				di+=MAXPTNLEN;
			}
		}
	}
	fclose(fp);
	return true;
}