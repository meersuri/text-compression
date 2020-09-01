#include<string>
#include<vector>

class Compressor
{
public:
	Compressor(std::string in_file, std::string out_file);
	virtual void encode() = 0;
protected:
	std::string m_in_file;
	std::string m_out_file;
	std::vector<char> m_data;
	std::string m_encoded;
};

class HuffmanCompressor : public Compressor
{
public:
	HuffmanCompressor(std::string in_file, std::string out_file);
	virtual void encode() override;
};

class LZ77Compressor : public Compressor
{
public:
	LZ77Compressor(std::string in_file, std::string out_file, size_t left_buf_len,
		       	size_t right_buf_len);
	virtual void encode() override;
private:
	size_t m_left_buf_len;
	size_t m_right_buf_len;
};


class ArithmeticCompressor : public Compressor
{
public:
	ArithmeticCompressor(std::string in_file, std::string out_file);
	virtual void encode() override;
};
