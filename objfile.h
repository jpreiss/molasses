#pragma once

#include "model.h"

#include <vector>
#include <string>
#include <iosfwd>

std::istream &operator>>(std::istream &s, Vec &v)
{
	s >> v.x;
	s >> v.y;
	s >> v.z;
	return s;
}

// reads OBJ file, just vertices and tris for now.
void readObj(std::istream &file, 
			 std::vector<Vec> &vertices,
			 std::vector<Vec> *normals,
			 std::vector<Vec> *texCoords,
			 std::vector<int> &triVerts,
			 std::vector<int> *triNorms,
			 std::vector<int> *triCoords)
{
	std::string prefix;

	while (file)
	{
		file >> prefix;

		if (prefix == "v")
		{
			Vec v;
			file >> v;
			vertices.push_back(v);
		}
		else if (prefix == "vt")
		{
			Vec v;
			file >> v;
			if (texCoords) texCoords->push_back(v);
		}
		else if (prefix == "vn")
		{
			Vec v;
			file >> v;
			if (normals) normals->push_back(v);
		}
		else if (prefix == "f")
		{
			char slash;

			for (int i = 0; i < 3; ++i)
			{
				int v;
				file >> v;
				triVerts.push_back(v - 1);

				file >> slash;
				if (file.peek() != '/') // have texture
				{
					int c;
					file >> c;
					if (triCoords) triCoords->push_back(c - 1);					
				}

				file >> slash;
				int peek = file.peek();

				if ('0' <= peek && peek <= '9')
				{
					int n;
					file >> n;
					if (triNorms) triNorms->push_back(n - 1);
				}
			}
		}
	}
}