//FileCahceManager implementation
#include "AlgServer.h"
//constructor
FileCacheManager :: FileCacheManager(std::string &path)
{
	this->toLoadAndSaveFrom = path;
}
//load calculated solutions
void FileCacheManager :: loadSolutions()
{
	std::ifstream f;
	std::string line = "", problem = "", solution = "";
	std::vector<std::string> lines;
	unsigned int size = 0, i = 0;
	f.open(this->toLoadAndSaveFrom);
	if (!f)
	{
		return;
	}
	else
	{
		while (getline(f, line))
		{
			lines.push_back(line);
		}
		size = lines.size();
		for (; i < size; ++i)
		{
			while (i < size && lines[i] != "route")
			{
				problem += lines[i];
				problem += "$";
				++i;
			}
			++i;
			solution = lines[i];
			++i;
			(this->solutions)[problem] = solution;
			problem = "";
			solution = "";
		}
	}
}
//save calculated solutions
void FileCacheManager :: saveSolutions()
{
	std::ofstream f;
	unsigned int size = 0, i = 0;
	std::string key = "", val = "", tmp = "";
	f.open(this->toLoadAndSaveFrom);
	std::map<std::string, std::string>::iterator it;
	for (it = (this->solutions).begin(); it != (this->solutions).end(); ++it)
	{
		key = it->first;
		val = it->second;
		size = key.size();
		while (i < size)	//write the matrix
		{
			while (key.at(i) != '$')	//$ sign seperates lines in the matrix
			{
				tmp.push_back(key.at(i));
				++i;
			}
			++i;
			f << tmp << std::endl;
			tmp = "";
		}
		i = 0;
		f << "route" << std::endl;
		f << val;
		++it;
		if (it != (this->solutions).end())
		{
			f << std::endl;
		}
		--it;
	}
}
