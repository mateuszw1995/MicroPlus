#pragma once

#include "../Config.h"
#include "../Math/maths.h"
Config::segment::segment(_string _tag, segtype_tag stype, int intval, float fval, _string strval) 
: segtype(stype),intv(intval),strv(strval),flov(fval),tag(_tag)  {}

Config::segment::segment(void){}

int Config::seg_cmp(const void* a, const void* b){
	return strcmp( ((segment*)a)->tag.cstr, ((segment*)b)->tag.cstr);
}

int Config::segment::operator>(const segment& b) {
	return strcmp(tag.cstr, b.tag.cstr) > 0;
}
int Config::segment::operator<(const segment& b){ 
	return strcmp(tag.cstr, b.tag.cstr) < 0;
}
int Config::segment::operator==(const segment& b) {
	return strcmp(tag.cstr, b.tag.cstr) == 0;
}

Config::Config(_string filename) : Values(_vector<segment>()){
	FILE* infile = fopen(filename.c_str(), "r");
	static char line[2000];
	while(!feof(infile)){
		char buf[200];
		char nbuf[600];
		char vbuf[600];
		int res = fscanf(infile, "%s %s %s", buf, nbuf, vbuf);
		if(res == 3) {
			if(buf[0] == ';') {
				fgets(line, sizeof(line), infile);
				continue;
			}
			if(strcmp(vbuf, "[EMPTY]"))
				if(!strcmp(buf, "int" )){
					Values.push_back(segment(nbuf,segment::INTEGER, _string::s2i(vbuf) )); 
				}
				else if(!strcmp(buf, "string")){ 
					Values.push_back(segment(nbuf,segment::STRING, 0, 0.0, vbuf ));
				}
				else if(!strcmp(buf, "float")){
					Values.push_back(segment(nbuf,segment::FLOAT, 0, (float)_string::s2d(vbuf) ));
				}
			buf[0] = 0;
		}
		else if(res >= 1) {
			if(buf[0] == ';') { 
				fgets(line, sizeof(line), infile);
				continue;
			}
		}

	}
	fclose(infile);
	math::sort::heap(Values.get_ptr(), Values.size());
}


Config::segment* Config::operator[](char* tag) {
	segment _key; 
	_key.tag = _string(tag);
	segment* result = math::binary_search(Values.get_ptr(), _key, Values.size());
	return result;
}