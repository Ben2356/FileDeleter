/**
	AUTHOR: Benjamin Moreno
	LAST MODIFIED: 7/25/16
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <boost/filesystem.hpp>

//prints the names of the all the files in a folder and adds them to the file list
void printSubfolders(const boost::filesystem::path &p, std::vector<boost::filesystem::path> &files, int level = 1, const bool quiet = false)
{
	for (boost::filesystem::directory_entry &item : boost::filesystem::directory_iterator(p))
	{
		if (!quiet)
		{
			for (int i = 0; i < level; i++)
				std::cout << "---";
			std::cout << item.path().filename().string() << (boost::filesystem::is_directory(item.path()) ? "\\\n" : "\n");
		}
		files.push_back(item.path());
		if (boost::filesystem::is_directory(item.path()))
		{
			printSubfolders(item.path(), files, level + 1, quiet);
		}
	}
}

std::string timeStamp()
{
	std::stringstream out;
	std::chrono::system_clock::time_point currTime = std::chrono::system_clock::now();
	std::time_t currTime_t = std::chrono::system_clock::to_time_t(currTime);
	out << std::put_time(std::localtime(&currTime_t), "%F %T") << " ";
	return out.str();
}

//for use with delete command
void op_delete(boost::filesystem::path &p, std::ofstream &log, const bool quiet = false)
{
	if (boost::filesystem::is_regular_file(p))
	{
		if (boost::filesystem::remove(p))
		{
			if(!quiet)
				std::cout << "operation completed successfully" << std::endl;
			log << timeStamp() << "delete operation completed successfully" << "\n";
		}
			
		else
		{
			if(!quiet)
				std::cerr << "operation failed: path does not exist" << std::endl;
			log << timeStamp() << "ERROR - delete operation failed: path does not exist" << std::endl;
		}
	}
	else
	{
		unsigned int filesDeleted = boost::filesystem::remove_all(p);
		if (filesDeleted >= 1)
		{
			if(!quiet)
				std::cout << "operation completed successfully: " << filesDeleted << " file(s) deleted." << std::endl;
			log << timeStamp() << "delete operation completed successfully: " << filesDeleted << " file(s) deleted." << "\n";
		}	
		else
		{
			if(!quiet)
				std::cerr << "operation failed" << std::endl;
			log << timeStamp() << "ERROR - operation failed" << "\n";
		}
	}
}

//for use with delete_all command
void op_delete_all(const std::string filetype, const std::vector<boost::filesystem::path> &files, std::ofstream &log, const bool quiet = false)
{
	if (filetype[0] != '.')
		return;
	size_t foundFileExt;
	unsigned int filesDeleted = 0;
	for (int i = 0; i < files.size(); i++)
	{
		//if the file extension that is being searched for is not at the end of the file then ignore the file
		foundFileExt = files[i].string().rfind(filetype);

		if (foundFileExt != std::string::npos && foundFileExt+filetype.size() == files[i].string().size())
		{
			filesDeleted++;
			if(!quiet)
				std::cout << "FILE: " << files[i] << " deleted => ";
			log << timeStamp() << "FILE: " << files[i] << " deleted => ";
			if (boost::filesystem::remove(files[i]))
			{
				if(!quiet)
					std::cout << "successfully" << std::endl;
				log << timeStamp() << "successfully" << "\n";
			}
			else
			{
				if(!quiet)
					std::cerr << "FAILED" << std::endl;
				log << timeStamp() << "ERROR - FAILED" << "\n";
			}
		}
	}
	if (filesDeleted == 0)
	{
		if(!quiet)
			std::cerr << "No matching files found with " << filetype << " extension" << std::endl;
		log << timeStamp() << "ERROR - No matching files found with " << filetype << " extension" << "\n";
	}
}

//breaks an string into the command and arguments returned in an array
std::vector<std::string> parseInput(const std::string in, std::ofstream &log, const bool quiet = false)
{
	std::vector<std::string> parsedInput;
	int start = 0;

	//arguments are separated via spaces
	size_t spaceFound = in.find(" ");

	if (spaceFound != std::string::npos)
	{
		parsedInput.push_back(in.substr(start, spaceFound));
		start = spaceFound + 1;
	}

	//if there are no spaces then incomplete command and return empty array
	else
	{
		if(!quiet)
			std::cerr << "invalid command sequence" << std::endl;
		log << timeStamp() << "Invalid command sequence" << "\n";
		return parsedInput;
	}

	while (spaceFound != std::string::npos)
	{
		spaceFound = in.find(" ", start);
		if (spaceFound != std::string::npos)
		{
			parsedInput.push_back(in.substr(start, spaceFound - start));
			start = spaceFound + 1;
		}
	}
	if (start < in.size())
	{
		parsedInput.push_back(in.substr(start));
	}

	return parsedInput;
}

int main(int argc, char** argv)
{
	std::ofstream outLog("log.txt", std::ios::app);
	outLog << timeStamp() << "Program start" << "\n";
	std::string currPath = "D:\\test";
	bool isCommandLine = false;

	//command line arguments
	if (argc > 1)
	{
		currPath = argv[1];
		isCommandLine = true;
	}
	
	boost::filesystem::path p(currPath);
	outLog << timeStamp() << "Path input: " << p << "\n";
	std::vector<boost::filesystem::path> files;
	try
	{
		if (boost::filesystem::is_empty(p))
		{
			if(!isCommandLine)
				std::cerr << "Directory Empty" << "\n";
			outLog << timeStamp() << "ERROR - Directory Empty" << "\n";
		}
		else if (!boost::filesystem::is_directory(p))
		{
			if(!isCommandLine)
				std::cerr << "Not A Valid Directory" << "\n";
			outLog << timeStamp() << "ERROR - Not a Valid Directory" << "\n";
		}
		else
		{
			for (boost::filesystem::directory_entry& ent : boost::filesystem::directory_iterator(p))
			{
				if (boost::filesystem::is_regular_file(ent.path()))
				{
					//std::time_t t = boost::filesystem::last_write_time(ent.path());
					if(!isCommandLine)
						std::cout << ent.path().filename().string() << std::endl; /*<< "\t" << boost::filesystem::file_size(ent.path()) << " bytes" << "\t\t" << std::ctime(&t);*/
				}
				else
				{
					if(!isCommandLine)
						std::cout << ent.path().filename().string() << "\\" << std::endl;
					printSubfolders(ent.path(), files, 1, isCommandLine);
				}
				files.push_back(ent.path());
			}
			if (!isCommandLine)
			{
				std::cout << "\nThe directory: " << p << " contains " << files.size() << " items\n\n";
				std::cout << std::endl;
			}	
		}

		/*
		OPERATIONS:
		delete (path)
		delete_all (filetype) (level?)
		delete_older (filetype, date/time) //deletes all files older than a specified date/time
		copy
		copy_all
		*/

		//element 0 = operation command, 1 = fileDetails, 2... other operation parameters
		std::vector<std::string> parsedInput;

		if (!isCommandLine)
		{
			std::string input;
			std::getline(std::cin, input);
			parsedInput = parseInput(input, outLog);
			outLog << timeStamp() << "getting data from user input string" << "\n";
		}
		else
		{
			for (int i = 2; i < argc; i++)
			{
				parsedInput.push_back(argv[i]);
			}
			outLog << timeStamp() << "getting data from command line argument" << "\n";
		}
		if (parsedInput.empty())
		{
			std::cerr << "ERROR - no input" << std::endl;
			outLog << timeStamp() << "ERROR - no input in array" << "\n";
			return -1;
		}
		//handle the operation and the parameters from the parsedInput array
		if (parsedInput[0] == "delete" && parsedInput.size() == 2)
		{
			outLog << timeStamp() << "delete command with parameter: " << parsedInput[1] << "\n";

			//looking to see if : exists in the path as it command be included in a folder/file name and if false then it is not a full path
			//eg C:\windows
			if (parsedInput[1][1] != ':')
			{
				parsedInput[1] = currPath + "\\" + parsedInput[1];
				outLog << timeStamp() << "shorthand path detected and expanded: " << parsedInput[1] << "\n";
			}

			op_delete(boost::filesystem::path(parsedInput[1]), outLog, isCommandLine);
		}
		else if (parsedInput[0] == "delete_all" && parsedInput.size() == 2)
		{
			outLog << timeStamp() << "delete_all command with parameter: " << parsedInput[1] << "\n";
			op_delete_all(parsedInput[1], files, outLog, isCommandLine);
		}
		else
		{
			if(!isCommandLine)
				std::cerr << "Invalid operation command" << std::endl;
			outLog << timeStamp() << "ERROR - Invalid operation commamd" << "\n";
		}
	}
	catch (const boost::filesystem::filesystem_error& e)
	{
		std::cout << std::endl;
		std::cerr << "EXCEPTION CAUGHT: " << e.what() << std::endl;
		outLog << timeStamp() << "EXCEPTION CAUGHT: " << e.what() << "\n";
	}
	outLog << timeStamp() << "Exiting program" << "\n";
	outLog << "====================" << std::endl;

	/*std::cin.clear();
	std::cin.ignore();
	std::cin.get();*/
	return 0;
}