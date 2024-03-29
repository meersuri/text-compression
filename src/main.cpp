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
    bool compress, extract = false;
    CLI::Option *compress_opt = app.add_flag("-c", compress, "compress file");
    CLI::Option *extract_opt = app.add_flag("-x", extract, "extract archive");
    CLI::Option *file_opt = app.add_option("-f, --file", fpath, "file to compress");
    CLI::Option *out_opt = app.add_option("-o, --out", out_fpath, "name of output archive");
    file_opt->required();
    CLI11_PARSE(app, argc, argv);
    Huffman<char> coder;
    if (compress) {
        std::vector<char> source = load(fpath);
        std::cout << "file size " << source.size() << std::endl;
        auto encoded = coder.encode(source);
        auto packed = pack(encoded);
        std::cout << "compressed size " << packed->data.size() << std::endl;
        if (!out_fpath.size()) {
            out_fpath = std::string(fpath);
            auto pos = out_fpath.find_last_of(".");
            if (pos != std::string::npos)
                out_fpath.replace(pos + 1, out_fpath.size() - pos - 1, "bin");
        }
        save_compressed(out_fpath, coder.codebook(), packed);
        std::cout << "Compressed to " << out_fpath << '\n';
    }
    if (extract) {
        auto pos = fpath.find_last_of(".");
        if (pos != std::string::npos && fpath.substr(pos) != ".bin") {
            std::cerr << "Input file: " << fpath << " is not a compressed file\n";
            return -1;
        }
        std::vector<char> decoded = coder.decode(fpath);
        std::cout << "decoded size " << decoded.size() << std::endl;
        if (!out_fpath.size()) {
            out_fpath = std::string(fpath);
            auto pos = out_fpath.find_last_of(".");
            if (pos != std::string::npos)
                out_fpath.replace(pos + 1, out_fpath.size() - pos - 1, "orig");
        }
        save(out_fpath, decoded);
        std::cout << "Extracted to " << out_fpath << '\n';
    }
    return 0;
}
