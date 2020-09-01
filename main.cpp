#include<iostream>
#include"Compressors.h"

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		std::cerr << "usage - " << argv[0] << "  " << "input file" << 
			"  " << "output file\n";
		return -1;
	}
	std::string in_file = argv[1];
	std::string out_file = argv[2];
	Compressor* comp = new HuffmanCompressor(in_file, out_file);
	comp->encode();
	return 0;
}
