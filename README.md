# LZ77

A C implementation of the LZ77 (sliding window) compression algorithm,
usable as a command-line tool.

## Usage

```sh
./lz77 -c <file> [options]   # compress
./lz77 -d <file> [options]   # decompress
```

Options:

| Option              | Description                                | Default      |
|---------------------|---------------------------------------------|--------------|
| `-c, --compress`    | Input file to compress                       | -            |
| `-d, --decompress`  | Input file to decompress                     | -            |
| `-o, --output`      | Output file                                  | `data.lz77`  |
| `-l, --lookahead`   | Lookahead buffer size (max 128)              | `32`         |
| `-s, --search`      | Search buffer size (max 65535)               | `4096`       |
| `--help`            | Show help                                    | -            |

Examples:

```sh
./lz77 -c input.txt -o compressed.lz77 --lookahead 16 --search 64
./lz77 --decompress compressed.lz77 -o original.txt
```

## Compressed format

The output file starts with the original (uncompressed) size, stored as
an 8-byte (uint64_t) little-endian value, followed by the encoded payload.

