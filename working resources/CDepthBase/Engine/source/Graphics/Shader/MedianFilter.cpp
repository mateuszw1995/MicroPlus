#pragma once

#include "../../../Graphics/Shader/PostFXShader.h"


extern GLint DB::Window::getWidth();
extern GLint DB::Window::getHeight();

__Shader::Component MedianFilter::term[] = {
	__Shader::Component(0, 1, "funiform float", "step"),
};

__Shader::Component 
	&MedianFilter::STEP 	(MedianFilter::term[0]);


const int MedianFilter::comps = sizeof(MedianFilter::term)/sizeof(__Shader::Component);

void MedianFilter::set_uni() {
	STEP.uni	= (uni_flags & STEP ?		&uni_step	: 0 );
}				

void MedianFilter::set_names() {
	set_uni();
	*STEP.act_name	= (STEP.arr = (bool)STEP.uni) ?	STEP.name : _string::d2s(step);
}				

void MedianFilter::Update(unsigned __int64 flags) {
	sp.Use();
	if(flags & STEP) 
		uni_step.set(step);
}

MedianFilter::MedianFilter(_string* BlendMode, GLint _width, GLfloat step) : step(step), delta_width(_width < 1 ? 1 : _width), PostFX(BlendMode, false) {
}
template<class It>
void median_of_medians(It left, It right, int n) {
	/*
	 * Puts the n-th element at its correct position
	 * in sorted order (indices count from 0).
	 */
	int len = right-left;
	if (len < 25 || n < 2) {
		for (int i = 0; i <= n; i++) {
			It comp = left+i;
			It p = left+i;
			while (p != right) {
				++p;
				if (*p < *comp) swap(*p, *comp);
			}
		}
		return;
	}
	int d = (len+4)/5;
	for (int p = 0; p < d; p++) {
		for (int s = 0; s < 3; s++) {
			It comp = left+p+d*s;
			for (int m = p+d*(s+1); m < len; m += d) {
				if (*(left+m) < *comp) swap(*(left+m), *comp);
			}
		}
	}
	median_of_medians(left+(d*2), left+min(d*3, len), (d-1)/2);
	int p = partition(left, right, *(left+(d*2+(d-1)/2)));
	if (n < p)
		median_of_medians(left, left+p, n);
	else
		median_of_medians(left+p, right, n-p);
}

template<class It, class T>
inline int partition(It a, It b, T part) {
	It begin = a;
	--b;
	while ((b-a) >= 0) {
		while (*a < part) ++a;
		while (*b > part) --b;
		if ((b-a) < 0) break;
		swap(*a, *b);
		++a;
		--b;
	}
	return (a-begin);
}


void MedianFilter::Build() {
	int d = delta_width;
	_string vsrc, fsrc, sw = _string::i2s((2*d+1)*(2*d+1));

	__Shader::clr_use<MedianFilter>();
	set_names();

#define vUNI(u,i) (u.act_name[i])


	__Shader::write_uni<MedianFilter>(vsrc, fsrc);

	vsrc("\n varying vec2 texCoord;")
		("\n attribute vec2 at_Vertex;")
		("\n void main(void)")
		("\n {")
		("\n texCoord  = at_Vertex;")
		("\n gl_Position =  vec4(at_Vertex, 1.0, 1.0) * 2.0 - 1.0;")
		("\n }");

	fsrc("\n uniform sampler2D diffuseMap;")
		("\n #define swap(a,b)  temp=(a);(a)=(b);(b)=temp; ")
		("\n #define pcmp(aa,bb)   ((aa).r + (aa).g + (aa).b < (bb).r + (bb).g + (bb).b)")
		("\n #define sort(aa,bb)   temp = min (aa , bb); bb = aa + bb - temp; aa = temp; ")
		("\n varying vec2 texCoord; ")
		("\n vec4 p[")(sw)("];")
		("\n void main(void)")
		("\n { \n vec4 sum = vec4(0.0); \n vec4 temp;");
	int i = 0;
	for(int y = -d; y <= d; ++y) 
		for(int x = -d; x <= d; ++x) {
			int coord = y*((2*d)+1)+x;
			fsrc("\n p[")(_string::i2s(i++))("]= texture2D(diffuseMap, texCoord + vec2(")
				(_string::d2s(GLdouble((x))/GLdouble(DB::Window::getWidth())))("*")vUNI(STEP, 0)(", ")(_string::d2s(GLdouble((y))/GLdouble(DB::Window::getHeight())))("*")vUNI(STEP, 0)("));");
		}
		if(d == 1) {
			fsrc("\n sort(p[1], p[2]) ; sort(p[4], p[5]) ; sort(p[7], p[8]) ;")
				("\n sort(p[0], p[1]) ; sort(p[3], p[4]) ; sort(p[6], p[7]) ;")
				("\n sort(p[1], p[2]) ; sort(p[4], p[5]) ; sort(p[7], p[8]) ;")
				("\n sort(p[0], p[3]) ; sort(p[5], p[8]) ; sort(p[4], p[7]) ;")
				("\n sort(p[3], p[6]) ; sort(p[1], p[4]) ; sort(p[2], p[5]) ;")
				("\n sort(p[4], p[7]) ; sort(p[4], p[2]) ; sort(p[6], p[4]) ;")
				("\n sort(p[4], p[2]) ;	sum = p[4];			    ");
		}	
		else if(d == 2) {
			fsrc("\n sort(p[0], p[1]) ; sort(p[3], p[4]) ; sort(p[2], p[4]) ;")
			    ("\n sort(p[2], p[3]) ; sort(p[6], p[7]) ; sort(p[5], p[7]) ;")
			    ("\n sort(p[5], p[6]) ; sort(p[9], p[10]) ; sort(p[8], p[10]) ;")
			    ("\n sort(p[8], p[9]) ; sort(p[12], p[13]) ; sort(p[11], p[13]) ;")
			    ("\n sort(p[11], p[12]) ; sort(p[15], p[16]) ; sort(p[14], p[16]) ;")
			    ("\n sort(p[14], p[15]) ; sort(p[18], p[19]) ; sort(p[17], p[19]) ;")
			    ("\n sort(p[17], p[18]) ; sort(p[21], p[22]) ; sort(p[20], p[22]) ;")
			    ("\n sort(p[20], p[21]) ; sort(p[23], p[24]) ; sort(p[2], p[5]) ;")
			    ("\n sort(p[3], p[6]) ; sort(p[0], p[6]) ; sort(p[0], p[3]) ;")
			    ("\n sort(p[4], p[7]) ; sort(p[1], p[7]) ; sort(p[1], p[4]) ;")
			    ("\n sort(p[11], p[14]) ; sort(p[8], p[14]) ; sort(p[8], p[11]) ;")
			    ("\n sort(p[12], p[15]) ; sort(p[9], p[15]) ; sort(p[9], p[12]) ;")
			    ("\n sort(p[13], p[16]) ; sort(p[10], p[16]) ; sort(p[10], p[13]) ;")
			    ("\n sort(p[20], p[23]) ; sort(p[17], p[23]) ; sort(p[17], p[20]) ;")
			    ("\n sort(p[21], p[24]) ; sort(p[18], p[24]) ; sort(p[18], p[21]) ;")
			    ("\n sort(p[19], p[22]) ; sort(p[8], p[17]) ; sort(p[9], p[18]) ;")
			    ("\n sort(p[0], p[18]) ; sort(p[0], p[9]) ; sort(p[10], p[19]) ;")
			    ("\n sort(p[1], p[19]) ; sort(p[1], p[10]) ; sort(p[11], p[20]) ;")
			    ("\n sort(p[2], p[20]) ; sort(p[2], p[11]) ; sort(p[12], p[21]) ;")
			    ("\n sort(p[3], p[21]) ; sort(p[3], p[12]) ; sort(p[13], p[22]) ;")
			    ("\n sort(p[4], p[22]) ; sort(p[4], p[13]) ; sort(p[14], p[23]) ;")
			    ("\n sort(p[5], p[23]) ; sort(p[5], p[14]) ; sort(p[15], p[24]) ;")
			    ("\n sort(p[6], p[24]) ; sort(p[6], p[15]) ; sort(p[7], p[16]) ;")
			    ("\n sort(p[7], p[19]) ; sort(p[13], p[21]) ; sort(p[15], p[23]) ;")
			    ("\n sort(p[7], p[13]) ; sort(p[7], p[15]) ; sort(p[1], p[9]) ;")
			    ("\n sort(p[3], p[11]) ; sort(p[5], p[17]) ; sort(p[11], p[17]) ;")
			    ("\n sort(p[9], p[17]) ; sort(p[4], p[10]) ; sort(p[6], p[12]) ;")
			    ("\n sort(p[7], p[14]) ; sort(p[4], p[6]) ; sort(p[4], p[7]) ;")
			    ("\n sort(p[12], p[14]) ; sort(p[10], p[14]) ; sort(p[6], p[7]) ;")
			    ("\n sort(p[10], p[12]) ; sort(p[6], p[10]) ; sort(p[6], p[17]) ;")
			    ("\n sort(p[12], p[17]) ; sort(p[7], p[17]) ; sort(p[7], p[10]) ;")
			    ("\n sort(p[12], p[18]) ; sort(p[7], p[12]) ; sort(p[10], p[18]) ;")
			    ("\n sort(p[12], p[20]) ; sort(p[10], p[20]) ; sort(p[10], p[12]) sum = p[12];");
		}
		else {/*
			int dd = (2*d+1)*(2*d+1);
			for (int n = dd-1; n > (dd/2-1); --n)
				for (int i = 0; i < n; ++i) 
					fsrc("\n sort (p[")(_string::i2s(i))("], p[")(_string::i2s(i+1))("]);");
			fsrc("\n sum = p[")(_string::i2s((2*d+1)*(2*d+1)/2))("];"); */

	fsrc("\n int i,j,l=0,m=")(sw)("-1, k=")(sw)("/2; vec4 x;")
		("\n while (l<m) {")
		("\n 	x=p[k] ;")
		("\n 	i=l ;")
		("\n 	j=m ;")
		("\n 	do {")
		("\n 		while (pcmp(p[i],x)) i++ ;")
		("\n 		while (pcmp(x,p[j])) j-- ;")
		("\n 		if (i<=j) {")
		("\n 			swap(p[i],p[j]) ;")
		("\n 			i++ ; j-- ;")
		("\n 		}")
		("\n 	} while (i<=j) ;")
		("\n 	if (j<k) l=i ;")
		("\n 	if (k<i) m=j ;")
		("\n }")
		("\n sum = p[k] ;");
/*
	fsrc("\n int low = 0 , high = ")(sw)("-1 , median = (low + high) / 2, middle, ll, hh;")
		("\n for (;;) {")
		("\n if (high <= low) {")
		("\n 	sum = p[median]; break; };")
		("\n if (high == low + 1) {")
		("\n 	sum = p[median]; break; }")
		("\n middle = (low + high) / 2;")
		("\n if (pcmp(p[middle],p[high])) swap(p[middle], p[high]) ;")
		("\n if (pcmp(p[low],p[high])) swap(p[low], p[high]) ;")
		("\n if (pcmp(p[middle],p[low])) swap(p[middle], p[low]) ;")
		("\n swap(p[middle], p[low+1]) ;")
		("\n ll = low + 1;")
		("\n hh = high;")
		("\n for (;;) {")
		("\n 	ll++; ")
		("\n 	hh--; ")
		("\n	for(;ll < ")(sw)(";++ll) if(!(pcmp(p[low],p[ll]))) break;")
		("\n 	for(;hh > 0;--hh) if(!(pcmp(p[hh],p[low]))) break;")
		("\n 	if (hh < ll)")
		("\n 		break;")
		("\n 	swap(p[ll], p[hh]) ;")
		("\n }")
		("\n swap(p[low], p[hh]) ;")
		("\n if (hh <= median)")
		("\n 	low = ll;")
		("\n if (hh >= median)")
		("\n 	high = hh - 1;")
		("\n }");*/

	//	}

		}
		fsrc(*BlendMode)
			("\n}");
#undef vUNI

		sp.vsrc = vsrc;
		sp.fsrc = fsrc;
		sp.Build();
		
		set_uni();
		__Shader::get_uni<MedianFilter>(sp.p);
}