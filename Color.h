#ifndef COLOR_H_
#define COLOR_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <math.h>

static void error(const char* filename, const char* errtext)
{
    std::cerr<<"Error reading file: "<<filename<<":"<<errtext<<std::endl;
    //system("pause");
}

class Color
{
public:
    double r() const{ return _r;}
    double g() const{ return _g;}
    double b() const{ return _b;}
    
    void set_r(double r){ _r = r; }
    void set_g(double g){ _g = g; }
    void set_b(double b){ _b = b; }
    
    Color(void):_r(0.0f),_g(0.0f),_b(0.0f){};
    ~Color(void){};
    Color(const Color& cz):_r(cz._r),_g(cz._g),_b(cz._b){}
    
    Color( double r, double g, double b) :_r(r),_g(g), _b(b){}
    
    Color& operator = (const Color& v)
    {
        _r = v._r;
        _g = v._g;
        _b = v._b;
        return *this;
    }
    
    // scalar product
    Color operator *(double s) const
    {
        return Color( _r * s, _g * s, _b * s);
    }
    // color product
    Color operator * (Color c) const
    {
        return Color( _r * c._r, _g * c._g, _b * c._b );
    }
    // color plus
    Color operator +(const Color& c) const
    {
        return Color( _r + c._r, _g + c._g, _b + c._b);
    }
    // color minus
    Color operator -(const Color& c) const
    {
        return Color( _r - c._r, _g - c._g, _b - c._b);
    }
    
    friend inline double Max( const Color& c)
    {
        double t = (c._r > c._g)?c._r:c._g;
        return (t > c._b)?t:c._b;
    }
    
    friend inline double Min( const Color& c)
    {
        double t = (c._r < c._g)?c._r:c._g;
        return (t < c._b)?t:c._b;
    }
    
    
    // scalar left product
    friend inline Color operator * ( double s, const Color& c)
    {
        return Color( s * c._r, s * c._g, s * c._b );
    }
    
private:
    double _r,_g,_b;
};


struct Pixel {
    unsigned char r, g, b;
};

class Image {
public:
    Image(const std::string& name);
    Image(int xres, int yres);
    ~Image();
    
    void set(int x, int y, const Color& c) {
        Pixel p;
        p.r = (unsigned char)(c.r() < 0.f? 0: c.r() >= 1.f? 255 : (int)(c.r() * 255.f));
        p.g = (unsigned char)(c.g() < 0.f? 0: c.g() >= 1.f? 255 : (int)(c.g() * 255.f));
        p.b = (unsigned char)(c.b() < 0.f? 0: c.b() >= 1.f? 255 : (int)(c.b() * 255.f));
        data[y][x] = p;
    }
    
    void write(const std::string& filename) const;
    
    double aspect_ratio() const {
        return double(xres)/double(yres);
    }
    
    int getXresolution() {
        return xres;
    }
    
    int getYresolution() {
        return yres;
    }
    
    Color interpolate(double x, double y) const {
        x *= xres; y *= yres;
        int ix = int(floor(x))%xres;
        if(ix<0)
            ix += xres;
        int ix1 = (ix+1)%xres;
        int iy = int(floor(y))%yres;
        if(iy<0)
            iy += yres;
        int iy1 = (iy+1)%yres;
        double fx = x-ix;
        double fy = y-iy;
        
        Color c00 = Color(data[iy][ix].r,   data[iy][ix].g,   data[iy][ix].b);
        Color c01 = Color(data[iy][ix1].r,  data[iy][ix1].g,  data[iy][ix1].b);
        Color c10 = Color(data[iy1][ix].r,  data[iy1][ix].g,  data[iy1][ix].b);
        Color c11 = Color(data[iy1][ix1].r, data[iy1][ix1].g, data[iy1][ix1].b);
        Color c = c00*(1-fx)*(1-fy) + c01*fx*(1-fy) + c10*(1-fx)*fy + c11*fx*fy;
        return c*(1.0f/255);
    }
    
protected:
    Pixel** data;
    int xres, yres;
    Image(const Image&);
    Image& operator=(const Image&);
};

Image::Image(const std::string& filename)
{
    std::cerr << "Reading " << filename << "...";
    std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
    if(!in)
        error(filename.c_str(), "Error opening file");
    if(in.get() != 'P' || !in)
        error(filename.c_str(), "Error reading magic number (1st digit)");
    if(in.get() != '6' || !in)
        error(filename.c_str(), "Error reading magic number (2nd digit)");
    int maxValue;
    in >> xres >> yres >> maxValue;
    in.get();
    std::cerr << " " << xres << "x" << yres << "...";
    if(!in)
        error(filename.c_str(), "Error reading metadaa");
    data = new Pixel*[yres];
    Pixel* p = new Pixel[xres*yres];
    for(int i=0;i<yres;i++)
        data[i] = p + i*xres;
    for(int i=yres-1;i>=0;i--)
        in.read(reinterpret_cast<char*>(data[i]), sizeof(Pixel)*xres);
    if(!in)
        error(filename.c_str(), "Error reading pixels");
    std::cerr << " done\n";
}

Image::Image(int xres, int yres)
: xres(xres), yres(yres)
{
    data = new Pixel*[yres];
    Pixel* p = new Pixel[xres*yres];
    for(int i=0;i<yres;i++)
        data[i] = p + i*xres;
}

Image::~Image()
{
    delete[] data[0];
    delete[] data;
	
	data[0] = NULL;
	data = NULL;
}

void Image::write(const std::string& filename) const
{
    std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);
    out << "P6 " << xres << " " << yres << " 255\n";
    for(int i=yres-1;i>=0;i--)
        out.write(reinterpret_cast<char*>(data[i]), sizeof(Pixel)*xres);
}
#endif
