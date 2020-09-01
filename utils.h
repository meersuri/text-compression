#include<vector>
#include<string>
#include<cmath>
#include<stdexcept>

void to_bin_str(int x, size_t width, std::string& out)
{
	if(x < 0)
		throw std::invalid_argument("input must be positive");
	if(width <= 0)
		throw std::invalid_argument("width must be positive");
	if(width < ceil(log2(x)))
		throw std::invalid_argument("insufficient width");
	out = std::string(width, '0');
	for(auto it = out.rbegin(); x > 0; ++it)
	{
		*it = x % 2 > 0 ? '1' : '0';
		x /= 2;
	}
}
	
