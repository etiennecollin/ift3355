#pragma once

#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <cfloat>

#include "basic.h"

// Class to represent images, capable of reading/writing in PPM format.
// All external coordinates will be expressed with the origin at the bottom
// left, even though most image encodings start at the top left.

// Classe qui représente une seule frame.
// Les coordonnées sont exprimés dans un repere où l'origine est le coin inférieure gauche de l'image.
class Frame 
{
protected:
    int width, height;
	double *color;
    double *depth;

public:
	// Construit une frame
    Frame() : width(0), height(0), color(NULL) {}

    // Construit une frame avec les dimensions spécifiées.
    Frame(int width, int height) : width(width), height(height) 
	{ 
		color = new double[3 * width * height];
		depth = new double[3 * width * height];
	}

    // Destructor.
	~Frame() { 
		delete[] color;
		delete[] depth; }

	// Sauvegarde la couleur à l'endroit spécifiée.
	void show_color_to(std::string const &filename) const {
		show_to(filename, color);
	}

	// Sauvegarde la profondeur à l'endroit spécifiée.
	void show_depth_to(std::string const &filename) const {
		show_to(filename, depth);
	}

    // Modifie la couleur du pixel à la coordoonnée x,y
	void set_color_pixel(int x, int y, double3 color) {
		int offset = compute_offset(x,y);

		for (int i = 0; i < 3; i++) {
			this->color[offset + i] = color[i];
		}
	}

    // Modifie la profondeur du pixel à la coordoonnée x,y
	void set_depth_pixel(int x, int y, double gray) { 
		int offset = compute_offset(x,y);

		for (int i = 0; i < 3; i++) {
			this->depth[offset + i] = gray;
		}
	}

private:
	
	//Calcule le décalage dans le tableau plat.
	int compute_offset(int x, int y) {
		y = height - y - 1;
		return 3 * (y * width + x);
	}

	void show_to(std::string const &filename, double* values) const
	{
		unsigned char bmpfileheader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
		unsigned char bmpinfoheader[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };
		unsigned char bmppad[3] = { 0, 0, 0 };
		int filesize = 54 + 3 * width*height;

		bmpfileheader[2] = (unsigned char)(filesize);
		bmpfileheader[3] = (unsigned char)(filesize >> 8);
		bmpfileheader[4] = (unsigned char)(filesize >> 16);
		bmpfileheader[5] = (unsigned char)(filesize >> 24);

		bmpinfoheader[4] = (unsigned char)(width);
		bmpinfoheader[5] = (unsigned char)(width >> 8);
		bmpinfoheader[6] = (unsigned char)(width >> 16);
		bmpinfoheader[7] = (unsigned char)(width >> 24);
		bmpinfoheader[8] = (unsigned char)(height);
		bmpinfoheader[9] = (unsigned char)(height >> 8);
		bmpinfoheader[10] = (unsigned char)(height >> 16);
		bmpinfoheader[11] = (unsigned char)(height >> 24);

		double max_intensity = DBL_MIN, min_intensity = DBL_MAX;
		for (int i = 0; i < 3 * width*height; i++)
		{
			max_intensity = std::max(max_intensity,values[i]);
			min_intensity = std::min(min_intensity,values[i]);
		}

		FILE *f = NULL;
		f = fopen(filename.c_str(), "wb");
		if (!f) { puts("can't write output image to disk!"); return; }

		fwrite(bmpfileheader, 1, 14, f);
		fwrite(bmpinfoheader, 1, 40, f);
		unsigned char offsetBuf = 0;
		long lineOffset = width * 3 % 4;
		if (lineOffset != 0)
		{
			lineOffset = 4 - lineOffset;
		}
		unsigned char mapped_intensity;
		for (int i = height - 1; i >= 0; i--)	//bmp stores images upside down
		{
			for (int j = 0; j<width; j++)
			{
				int start = 3 * (i*width + j);
				
				//!!! maybe a better tone mapping algorithm
				mapped_intensity = (unsigned char)((values[start + 2] - min_intensity) / (max_intensity - min_intensity) * 255);
				//mapped_intensity = (unsigned char)(max(0.0, min(1.0, color[start + 2] / 16)) * 255);
				fwrite(&mapped_intensity, 1, 1, f);

				mapped_intensity = (unsigned char)((values[start + 1] - min_intensity) / (max_intensity - min_intensity) * 255);
				//mapped_intensity = (unsigned char)(max(0.0, min(1.0, color[start + 1] / 16)) * 255);
				fwrite(&mapped_intensity, 1, 1, f);

				mapped_intensity = (unsigned char)((values[start] - min_intensity) / (max_intensity - min_intensity) * 255);
				//mapped_intensity = (unsigned char)(max(0.0, min(1.0, color[start] / 16)) * 255);
				fwrite(&mapped_intensity, 1, 1, f);
			}

			fwrite(&offsetBuf, 1, lineOffset, f);
		}

		fclose(f);
	}

};