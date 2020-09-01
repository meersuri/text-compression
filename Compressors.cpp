#include<iostream>
#include<cstdint>
#include<string>
#include<fstream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include"utils.h"
#include"Compressors.h"

Compressor::Compressor(std::string in_file, std::string out_file): m_in_file(in_file),
	m_out_file(out_file)
{
	std::ifstream ifs(m_in_file);
	char c;
	while(ifs.get(c))
		m_data.push_back(c);
}

HuffmanCompressor::HuffmanCompressor(std::string in_file, std::string out_file): Compressor(in_file, out_file)
{
}

void HuffmanCompressor::encode() 
{
	std::unordered_map<char, uint64_t> symbol_counts;
	for(int i = 0; i < m_data.size(); ++i)
	{
		auto it = symbol_counts.find(m_data[i]);
		if(it != symbol_counts.end())
			it->second += 1;
		else
			symbol_counts.insert(std::make_pair(m_data[i], 1));
	}
	int alphabet_size = symbol_counts.size();

	std::vector<uint64_t> heap_ptrs(2*alphabet_size, 0);
	for(auto it = symbol_counts.begin(); it != symbol_counts.end(); ++it)
	{
		int i = std::distance(symbol_counts.begin(), it);
		heap_ptrs[i] = i + alphabet_size; 
		heap_ptrs[i + alphabet_size] = it->second; 
	}

	std::make_heap(heap_ptrs.begin(), heap_ptrs.begin() + alphabet_size, [heap_ptrs](const auto& a, const auto& b){ return heap_ptrs[a] > heap_ptrs[b];});
	for(int i = 0; i < alphabet_size - 1; ++i)
	{
		uint64_t rarest_1 = heap_ptrs[0];
	       	std::pop_heap(heap_ptrs.begin(), heap_ptrs.begin() + alphabet_size - i,  [heap_ptrs](const auto& a, const auto& b){ return heap_ptrs[a] > heap_ptrs[b];});
		uint64_t rarest_2 = heap_ptrs[0];
		heap_ptrs[0] = alphabet_size - i - 1;
		heap_ptrs[alphabet_size - i - 1] = heap_ptrs[rarest_1] + heap_ptrs[rarest_2]; 
		heap_ptrs[rarest_1] = alphabet_size - i - 1;
		heap_ptrs[rarest_2] = alphabet_size - i - 1;
		std::make_heap(heap_ptrs.begin(), heap_ptrs.begin() + alphabet_size - i - 1, [heap_ptrs](const auto& a, const auto& b){ return heap_ptrs[a] > heap_ptrs[b];});
	}

	std::vector<int> code_lens;
	int max_code_len = 0;
	for(int i = 0; i < alphabet_size; ++i)
	{
		int n_steps = 1;
		int x = heap_ptrs[alphabet_size + i];
		while(x != 1)
		{
			x = heap_ptrs[x];
			++n_steps;
		}
		code_lens.push_back(n_steps);
		max_code_len = std::max(max_code_len, n_steps);
	}
	
	std::vector<int> len_to_n_codes(max_code_len + 1, 0);
	for(int i = 0; i < code_lens.size(); ++i)
		len_to_n_codes[code_lens[i]]++;

	std::vector<std::vector<std::string>> codes(len_to_n_codes.size());
	int val = 0;
	for(int i = 0; i < len_to_n_codes.size(); ++i)
	{
		if(!len_to_n_codes[i])
		{
			val *= 2;
			continue;
		}
		for(int j = 0; j < len_to_n_codes[i]; ++j)
		{
			std::string codeword;
			try
			{
				to_bin_str(val++, i, codeword);
			}
			catch(const std::invalid_argument& e)
			{
				std::cout << e.what() << '\n';
			}
			codes[i].emplace_back(codeword);
		}
		val *= 2;
	}

	std::unordered_map<char, std::string> codebook;
	std::vector<int> code_ptrs(len_to_n_codes.size(), 0);
	for(auto it = symbol_counts.begin(); it != symbol_counts.end(); ++it)
	{
		char symbol = it->first;
		auto i = std::distance(symbol_counts.begin(), it);
		int len = code_lens[i];
		codebook.insert(std::make_pair(symbol, codes[len][code_ptrs[len]++]));
	}

	for(auto c: m_data)
		m_encoded += codebook[c];
	std::cout << m_encoded << '\n';

}
