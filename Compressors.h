#include<string>
#include<vector>

class Compressor
{
public:
	Compressor(std::string in_file, std::string out_file);
	virtual void compress() = 0;
protected:
	std::string m_in_file;
	std::string m_out_file;
	std::vector<char> m_data;
};

class HuffmanCompressor : public Compressor
{
public:
	HuffmanCompressor(std::string in_file, std::string out_file);
	virtual void compress() override;
};
