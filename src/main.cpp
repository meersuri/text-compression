#include <iostream>
#include <cassert>

#include "CLI11.hpp"
#include "node.hpp"
#include "pack.hpp"
#include "huffman.hpp"

int main(int argc, char *argv[]) {
    CLI::App app{"My very own home-made compressor"};
    std::string fpath;
    std::string out_fpath;
    CLI::Option *file_opt = app.add_option("-f, --file", fpath, "file to compress");
    CLI::Option *out_opt = app.add_option("-o, --out", out_fpath, "name of output archive");
    file_opt->required();
    CLI11_PARSE(app, argc, argv);
    if (!out_fpath.size())
        out_fpath = "compressed.bin";
    std::vector<char> source = load(fpath);
    std::cout << "file size " << source.size() << std::endl;
    Huffman<char> coder;
    auto encoded = coder.encode(source);
    auto packed = pack(encoded);
    std::cout << "compressed size " << packed->data.size() << std::endl;
    save_compressed(out_fpath, packed);
    std::vector<char> decoded = coder.decode(out_fpath);
    std::cout << "decoded size " << decoded.size() << std::endl;
    assert (decoded == source);
    std::cout << "Success!\n";
    std::cout << "Compressed to " << out_fpath << '\n';
    return 0;
}
