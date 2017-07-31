/*///////////////////////////////////////////////////////
C++ class for reading text files.
How to use:

FileReader myfile = {"my_file_name"};
cout << myfile.str();
//or
FileReader myfile2;
myfile2.get_file("my_file_name2");
cout << myfile2.str();

The default constructor leaves content
as an empty string(""). You'll need to call get_file()
///////////////////////////////////////////////////////*/

#ifndef FILE_READER_H
#define FILE_READER_H

#include <string>
#include <fstream>

class FileReader{
public:
	// constructors
	FileReader() { }
	FileReader(const std::string& file_name) { get_file(file_name); }
	enum Type{ OTHER, PNG, WEBP };
	Type type;
	int size;
	// get_file() can be used to open or switch files.
	const char* get_file(const std::string& file_name) {
		name = file_name;
		clear_content();
		read_file(name);
		return c_str();
	}
	// return a string representation of the file's content.
    std::string str() const { return content; }
	// return a C-style string.
	const char* c_str() const { return content.c_str(); }
	// return file name.
	const char* file_name() const { return name.c_str(); }
	// operator overloads
	explicit operator bool() const { return is_loaded; }
private:
    std::string content = ""; // file text/content
    std::string name = ""; // file name
    void read_file(const std::string& file_name)
	{
		std::ifstream infile(file_name, std::ios::in | std::ios::binary);

		// check if file is loaded
		// ensures ifstream can throw exceptions:
		//infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		is_loaded = false;
		type = Type::OTHER;
		size = 0;
		
		if (infile) {
			is_loaded = true;
			// start getting lines from the file
			infile.seekg(0, std::ios::end);
			content.resize(infile.tellg());
			infile.seekg(0, std::ios::beg);
			infile.read(&content[0], content.size());
			infile.close();

			size = content.size();

			constexpr char png_header[5]{ 0x89, 0x50, 0x4E, 0x47 };
			constexpr char webp_header[5]{ 0x52, 0x49, 0x46, 0x46 };
			std::string header = content.substr(0, 4);
			if (header == png_header) type = Type::PNG;
			else if (header == webp_header) type = Type::WEBP;
		}
	}
    void clear_content() { content = ""; }
	bool is_loaded = true;
};

#endif //FILE_READER_H
