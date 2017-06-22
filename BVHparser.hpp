#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class BVHparser
{
	const char* mPath;
	float** data;
public :
	BVHparser(const char* path);
	vector<string> usrSettings;
	float frames;
	float frameTime;
};