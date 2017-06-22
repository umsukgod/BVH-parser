#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include "BVHparser.hpp"
using namespace std;

class MotionNode
{
	string name;
	bool isRoot;
	bool isEnd;
	MotionNode* parent;
	vector<MotionNode*> childs;
	float offset[3];
	string channels[6];
	int channelNum;
	MotionNode* next;
public :
	float **data;
	MotionNode()
	{
		name = "";
		isRoot = false;
		isEnd = false;
		parent = NULL;
		next = NULL;
		childs = vector<MotionNode*>();
		channelNum = 0;
		for(int i =0; i<6; i++)
		{
			channels[i] = "";
		}
	}
	MotionNode* getParent()
	{
		return parent;
	}
	vector<MotionNode*> getChilds()
	{
		return childs;
	}
	void setParent(MotionNode* pnode)
	{
		parent = pnode;
		pnode->addChild(this);
	}
	void addChild(MotionNode* cnode)
	{
		childs.push_back(cnode);
	}
	void setRoot()
	{
		isRoot = true;
	}
	void setEnd()
	{
		isEnd = true;
	}
	void setName(string mname)
	{
		name = mname;
	}
	void setOffset(float x, float y, float z)
	{
		offset[0] = x;
		offset[1] = y;
		offset[2] = z;
	}
	void setChannels(int num, string* mChannels)
	{
		channelNum = num;
		for(int i=0;i<num;i++)
		{
			channels[i] = mChannels[i];
		}
	}
	void setNext(MotionNode *nextNode)
	{
		next = nextNode;
	}
	string getName()
	{
		return name;
	}
	int getChannelNum()
	{
		return channelNum;
	}
	MotionNode* getNextNode()
	{
		return next;
	}
	void initData(int frames)
	{
		data = new float*[frames];
		for(int i = 0; i < frames; i++)
		{
			data[i] = new float[channelNum];
		}
	}

};

BVHparser::BVHparser(const char* path)
{
	int lineNum = 0;
	int channelNum = 0;
	string channels[6];
	float offx, offy, offz;
	mPath = path;
	ifstream in;
	in.open(path, ios::in);
	if(!in)
	{
		cerr << "Cannot open "<<path<<endl; exit(1);
	}
	string line;
	getline(in, line);										//HIERARCHY
	lineNum++;

	if(line != "HIERARCHY")
	{ 
		cout << "Check the file format. The line number "<<lineNum<<" is not fit to the format"<<endl;
	}
	getline(in, line);										//ROOT Hips
	lineNum++;
	MotionNode* rootNode = new MotionNode();
	rootNode->setRoot();
	istringstream s(line);
	string bvh_keyword;
	string bvh_nodeName;
	s >> bvh_keyword; s >> bvh_nodeName;
	if(bvh_keyword != "ROOT")
	{
		cout << "Check the file format. The line number "<<lineNum<<" is not fit to the format"<<endl;
	}
	rootNode->setName(bvh_nodeName);
	getline(in, line);										//{

	getline(in, line);										//	OFFSET 0.00 0.00 0.00
	s.str("");
	s = istringstream(line);
	s >> bvh_keyword; s >> offx; s >> offy; s >> offz;
	rootNode->setOffset(offx, offy, offz);


	getline(in, line);										//	CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation
	s.str("");
	s = istringstream(line);
	s >> bvh_keyword; s >> channelNum;
	for(int i = 0;i << channelNum;i++)
	{
		s >> channels[i];
	}
	rootNode->setChannels(channelNum, channels);

	MotionNode* prevNode = rootNode;
	MotionNode* prevNode4NextNode = rootNode;
	getline(in, line);
	while(line!="MOTION")						
	{
		//cout<<endl;
		//cout<<line<<endl;
		s.str("");
		s = istringstream(line);
		s >> bvh_keyword; s >> bvh_nodeName;
		if(bvh_keyword == "JOINT")							//	JOINT LeftUpLeg
		{
			MotionNode *newNode = new MotionNode();
			newNode->setName(bvh_nodeName);

			//cout << bvh_nodeName <<endl;	//print to check

			getline(in, line);								//	{
			getline(in, line);								//		OFFSET 3.64953 0.00000 0.00000
			s.str("");
			s = istringstream(line);
			s >> bvh_keyword; s >> offx; s >> offy; s >> offz;
			newNode->setOffset(offx, offy, offz);

			getline(in, line);								//		CHANNELS 3 Xrotation Yrotation Zrotation
			s.str("");
			s = istringstream(line);
			s >> bvh_keyword; s >> channelNum;
			for(int i = 0;i << channelNum;i++)
			{
				s >> channels[i];
			}
			newNode->setChannels(channelNum, channels);
			newNode->setParent(prevNode);
			prevNode4NextNode->setNext(newNode);
			prevNode = newNode;
			prevNode4NextNode = newNode;
		}
		else if(bvh_keyword == "End")						//	End Site
		{
			MotionNode *newNode = new MotionNode();
			newNode->setName(bvh_nodeName);
			newNode->setEnd();
			getline(in, line);								//	{
			getline(in, line);								//		OFFSET 3.64953 0.00000 0.00000
			s.str("");
			s = istringstream(line);
			s >> bvh_keyword; s >> offx; s >> offy; s >> offz;
			newNode->setOffset(offx, offy, offz);

			newNode->setParent(prevNode);
			getline(in, line);								//	}
		}
		else if(bvh_keyword == "}")
		{
			prevNode = prevNode->getParent();
			//cout<<prevNode->getName()<<endl;
		}
		else
		{
			cout << "Check the file format." <<endl;
		}
		getline(in, line);
	}

// 	Start MotionNode										//MOTION
	string str1, str2;	//to get the string for format
	float fvalue;
	getline(in, line);										//Frames: 4692
	s.str("");
	s = istringstream(line);
	s >> str1; s >> fvalue;
	frames = fvalue;

	getline(in, line);										//Frame Time: 0.008333
	s.str("");
	s = istringstream(line);
	s >> str1; s >> str2; s >> fvalue;
	frameTime = fvalue;

	cout << "frames : " << frames <<", frame time : " << frameTime << endl;

	float f[6];
	MotionNode *curNode;
	for(int i = 0; i < 1; i++)
	{
		curNode = rootNode;
		getline(in, line);
		s.str("");
		s = istringstream(line);
		while(curNode != NULL)
		{
			// cout << curNode->getName()<<endl;		//print to check

			curNode->initData(frames);

			for(int j = 0; j < curNode->getChannelNum(); j++)
			{
				s >> f[j];
				curNode->data[i][j] = f[j];
				// cout<<f[j]<<", ";
			}
			// cout<<endl;
			curNode = curNode->getNextNode();
		}

	}

}