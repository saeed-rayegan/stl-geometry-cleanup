/*
*		Author:			Saeed Rayegan
* 
*
*		Description:	This function is designed to identify and remove facets from the geometry file that 
*						have an area close to zero or equal to zero.
* 
*		Date: 2023-10-21
*/

#include "cmath"
#include "iostream"
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <iomanip>
#include <omp.h>
#include <cstdlib>
#include <algorithm>
#include <tuple>

using namespace std;

#include "triangle_area_calculator.h"

//define name to read input files and print output files
std::ifstream input;
std::ofstream output;
std::ofstream geo_clean;

std::ofstream print_error;//output file to print errors
std::ofstream print_warning;//output file to print warnings

//Declare global variables//
string chr;//string to save characters read from input files
double num;
string null;

//Declare functions//
void open_export_error_warning();//open output_error.txt and output_warning.txt files to print out possible error and warning messages
void clean_up_the_input_geometry_stl_file_and_new_geometry_file();//clean up the input geometry stl file and generate a new geometry file

int main() {

	//open output_error.txt and output_warning.txt files to print out possible error and warning messages
	open_export_error_warning();

	//open a file to write a new geometry data
	geo_clean.open("Input_City_scale_geometry_clean.stl", std::ofstream::out | std::ofstream::trunc);

	//clean up the input geometry stl file and generate a new geometry file
	clean_up_the_input_geometry_stl_file_and_new_geometry_file();
	
	return 0;
}

//open output_error.txt and output_warning.txt files to print out possible error and warning messages
void open_export_error_warning() {


	print_error.open("Output_error_messages.txt", std::ofstream::out);
	print_error << "This file contains all error messages during the simulation" << endl << endl;


	print_warning.open("Output_warning_messages.txt", std::ofstream::out);
	print_warning << "This file contains all warning messages during the simulation" << endl << endl;
}

//clean up the input geometry stl file and generate a new geometry file
void clean_up_the_input_geometry_stl_file_and_new_geometry_file() {
	//declare local variables
	int solid_num = 0;//total number of solids in the geometry file
	std::vector<int> tri_num;//total number of facets in each solid in the geometry file
	int j = 0;//number of facet counter
	input.open("Input_City_scale_geometry.stl", std::ifstream::in);
	if (input.is_open())
	{
	next_solid0:;
		while (input >> chr)
		{
			if (chr == "endsolid") {
				solid_num++;
				tri_num.push_back(j);
				j = 0;
				goto next_solid0;
			}
			else if (chr == "facet") {
				j++;
			}	
		}
	}
	else
	{
		print_error << "Unable to open Input_City_scale_geometry.stl file";
		exit(EXIT_FAILURE);
	}
	input.close();

	//defining some other local variables
	int b_triangle_id = 0;//the index of a triangle in the stl file for a specific building (renew after each solid) (e.g. 0 to 10 for solid number 1, and again, 0 t0 20 for the next solid, and so on)
	int tri_ind = 0;//the counter for the triangle index from top top to down in the .stl file. (0, 1, 2, ...)
	j = 0;//building number counter in the .stl file
	double* x, * y, * z;//this is the coordination of each triangle. We use it to calculate the area of the triangle
	x = new double[3];
	y = new double[3];
	z = new double[3];
	
	//initialization
	for (int i = 0; i <= 2; i++) {
		x[i] = 0.;
		y[i] = 0.;
		z[i] = 0.;
	}

	string solid_name;

	//open the file and extract necessary data
	input.open("Input_City_scale_geometry.stl", std::ifstream::in);
	if (input.is_open())
	{
	next_building:;
		b_triangle_id = 0;//this is the counter for numbers of triangles within each solid
		while (input >> chr)
		{
			if (chr == "solid")
			{
				input >> chr;
				solid_name=chr;
				geo_clean << "solid" << "	" << solid_name << endl;

			next_triangle:;
				input >> chr;

				if (chr == "facet")
				{
					input >> chr >> num >> num >> num >> chr >> chr;
					for (int i = 0; i <= 2; i++)
					{
						input >> chr >> x[i] >> y[i] >> z[i];
					}
					
					//calculating the area of each facet
					Point3D p1 = { x[0], y[0], z[0] };
					Point3D p2 = { x[1], y[1], z[1] };
					Point3D p3 = { x[2], y[2], z[2] };
					Triangle3D triangle(p1, p2, p3);
					double area = triangle.calculateArea();
					
					if (area > 0.001) {
						geo_clean << "	" << "facet" << "	" << "normal" << "	" << 0.0 << "	" << 0.0 << "	" << 0.0 << endl;
						geo_clean << "	" << "	" << "outer" << "	" << "loop" << endl;
						geo_clean << "	" << "	" << "	" << "vertex" << "	" << x[0] << "	" << y[0] << "	" << z[0] << endl;
						geo_clean << "	" << "	" << "	" << "vertex" << "	" << x[1] << "	" << y[1] << "	" << z[1] << endl;
						geo_clean << "	" << "	" << "	" << "vertex" << "	" << x[2] << "	" << y[2] << "	" << z[2] << endl;
						geo_clean << "	" << "	" << "endloop" << endl;
						geo_clean << "	" << "endfacet" << endl;
					}

					input >> chr >> chr;

					//update the index of the triangle/facet within a solid building
					b_triangle_id++;

					//update the index of the triangle/facet within the whole .stl file
					tri_ind++;

					goto next_triangle;
				}
				else if (chr == "endsolid")
				{
					geo_clean << "endsolid" << "	" << solid_name << endl;
					input >> chr;
					j++;//solid building counter
					goto next_building;
				}

			}
		}
	}
	else
	{
		print_error << "Unable to open Input_City_scale_geometry.stl file";
		exit(EXIT_FAILURE);
	}
	input.close();
}
