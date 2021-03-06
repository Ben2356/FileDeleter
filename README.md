A simple file deleting program written in C++ utilizing the boost library, this is meant to be run automatically in the background before another program/operation given a set of command line arguments. When no command line arguments are specified, a console window interface is presented, providing an overview of the directory file structure where a command can be entered. All operations regardless of program input are logged to a .txt file named log.txt created in the same directory as the program executable. 

- Currently supported commands via command line arguments:	
```
fullPath delete filename
```
```
fullPath delete_all fileExtension
```

- Currently supported commands via program interface:			
```
delete filename
```
```
delete_all fileExtension
```											
**NOTES (delete command): if file is in a folder the path of the file from that folder must be provided**

**NOTES (delete_all command): ALL files with matching fileExtension in specified path**

Visual C++ Redistributable for Visual Studio 2015: https://www.microsoft.com/en-us/download/details.aspx?id=48145

Developed by: Benjamin Moreno Version: 1.0.0 Last Updated: 7/25/2016
