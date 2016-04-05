#include "reader.h"
#include <string>

Syncreader::Syncreader() {

}
Syncreader::~Syncreader() {}

void Syncreader::update(float row) {
	frow = row;
	irow = row; //integer
	drow = frow-irow; //fraction!
}

bool Syncreader::get_trig(Tracks t) {
	if(trig[t][irow] && (lasttrig[t] < irow)) {
		lasttrig[t] = irow;
		return true;
	} else { 
		return false;
	}
}

float Syncreader::get(Tracks t) {
	if(interp[t][irow]) {
		return data[t][irow] + (data[t][irow+1]-data[t][irow]) * drow;
	} else {
		return data[t][irow];
	}
}

bool Syncreader::getbool(Tracks t) {
	return (idata[t][irow]!=0);
}

bool Syncreader::load() {
	if(read(FILENAME))
	if(fill())
	return true;
	return false;
}

bool Syncreader::read(char* fn) {
	std::string eowrvniofrevn;
	eowrvniofrevn = "data\\" + std::string(fn);
	FILE* fp = fopen(eowrvniofrevn.c_str(),"rb");
	if(!fp) return false;

	//MessageBox(NULL,"moo","",MB_OK);

	{
		char buf[sizeof(FILEHEADER)];
		fread(buf,sizeof(FILEHEADER),1,fp);
	}
	fread(&fh,sizeof(fh),1,fp);

	interp.resize(fh.trackcount);
	data.resize(fh.trackcount);
	idata.resize(fh.trackcount);
	trig.resize(fh.trackcount);
	
	for(int i=0;i<fh.trackcount;i++) {
		interp[i].resize(fh.totalrowcount);
		data[i].resize(fh.totalrowcount);
		idata[i].resize(fh.totalrowcount);
		trig[i].resize(fh.totalrowcount);
	}

	{
		int dfg[303];
		fread(dfg, fh.ptncount, sizeof(int), fp);
	}{
		bool TIPOFTHEDAY;
		for(int j=0;j<fh.trackcount;j++) {
			//for(int i=0;i<h.totalrowcount;i++) {
			fread(&idata[j][0],fh.totalrowcount,sizeof(int),fp);
			for(int i=0;i<fh.totalrowcount;i++) {
				fread(&TIPOFTHEDAY,sizeof(bool),1,fp);
				interp[j][i] = TIPOFTHEDAY;
			}
			//fread(&interp[j][0],fh.totalrowcount,sizeof(bool),fp);
			bool moo = interp[j][0];
			bool moo0 = interp[j][1];
			int jklrngoiwengvolwen =345;
		}
	}
	fclose(fp);
	return true;
}

bool Syncreader::fill() {
	int d;
	int lastd = 0;
	int *trigpos = new int[fh.totalrowcount];
	int trigidx;
	for(int t=0;t<fh.trackcount;t++) {
		lasttrig[t] = -1;
		trigidx=0;
		for(int r=0;r<fh.totalrowcount;r++) {
			//fill
			d = idata[t][r];
			if(d!=fh.noteoffs[t]) {
				lastd = d;
				trig[t][r] = true;
				trigpos[trigidx++] = r;
			} else {
				idata[t][r] = lastd;
				trig[t][r] = false;
			}
		}
		trigpos[trigidx] = fh.totalrowcount - 1;

		//interpolate
		int q, begin, end;
		int diff;
		float diffval;
		for(int c=0;c<trigidx;c++) {
			begin = trigpos[c];
			end = trigpos[c+1];
			diff = end - begin;
			diffval = idata[t][end]-idata[t][begin];
			for(q=begin;q<end;q++) {
				if(interp[t][q]) {
					d = idata[t][q];
					data[t][q] = idata[t][begin] + diffval * ((q - begin) / (float)diff);
				} else {
					data[t][q] = idata[t][q];
				}
			}

		}
	}
	delete[] trigpos;

	return true;
}

