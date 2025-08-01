# Bitcoin Private Key Generator

(asked AI to write the readme for me, you will notice it is different from the #old-readme.md file)

A cryptographically secure Bitcoin private key generator that creates valid Bitcoin private keys and derives corresponding addresses. This tool is designed for real-world use with proper security practices and comprehensive error handling.

## What it does

- **Cryptographically Secure**: Uses OpenSSL for secure random number generation
- **Valid Bitcoin Keys**: Generates proper 256-bit private keys within valid range
- **Address Derivation**: Automatically derives Bitcoin addresses from private keys
- **Multiple Formats**: Supports hex, WIF (Wallet Import Format), and binary output
- **Compressed Keys**: Option to generate compressed public keys
- **Batch Generation**: Generate multiple keys at once
- **Comprehensive Validation**: Validates all generated keys and addresses
- **Memory Security**: Securely zeros sensitive data from memory
- **Error Handling**: Robust error handling and user feedback

## Installation

### Prerequisites

- GCC compiler
- OpenSSL development libraries
- Make

### Building

```bash
# Check dependencies
make check-deps

# Build the project
make

# Install (optional)
sudo make install
```

### Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install build-essential libssl-dev
```

On CentOS/RHEL:
```bash
sudo yum install gcc openssl-devel
```

On macOS:
```bash
brew install openssl
```

## Usage

### Basic Usage

Generate a single private key:
```bash
./btc_keygen
```

Generate multiple keys:
```bash
./btc_keygen -c 10
```

### Output Formats

Hex format (default):
```bash
./btc_keygen -f hex
```

WIF format:
```bash
./btc_keygen -f wif
```

Binary format:
```bash
./btc_keygen -f binary
```

### Advanced Options

Generate with Bitcoin address:
```bash
./btc_keygen -a
```

Use compressed public keys:
```bash
./btc_keygen -p
```

Verbose output:
```bash
./btc_keygen -v
```

Generate testnet addresses:
```bash
./btc_keygen -t
```

### Examples

Generate 5 keys with addresses in verbose mode:
```bash
./btc_keygen -c 5 -a -v
```

Generate WIF format with compressed keys:
```bash
./btc_keygen -f wif -p -a
```

Generate 100 keys quietly:
```bash
./btc_keygen -c 100 -q
```

## Command Line Options

| Option | Long Option | Description |
|--------|-------------|-------------|
| `-c NUM` | `--count NUM` | Generate NUM keys (default: 1) |
| `-f FORMAT` | `--format FORMAT` | Output format: hex, wif, binary |
| `-a` | `--with-address` | Include Bitcoin address in output |
| `-p` | `--compressed` | Use compressed public key format |
| `-t` | `--testnet` | Generate testnet addresses |
| `-v` | `--verbose` | Verbose output |
| `-q` | `--quiet` | Suppress error messages |
| `-h` | `--help` | Show help message |
| `-V` | `--version` | Show version information |

## Security Features

- **Cryptographically Secure Random Generation**: Uses OpenSSL's secure random number generator
- **Key Range Validation**: Ensures private keys are within valid Bitcoin range
- **Memory Protection**: Securely zeros sensitive data from memory
- **Input Validation**: Comprehensive validation of all inputs
- **Error Handling**: Graceful handling of cryptographic failures
- **No Logging**: Private keys are never logged or stored

## Technical Details

### Private Key Generation

The tool generates 256-bit private keys using cryptographically secure random number generation. Keys are validated to ensure they fall within the valid Bitcoin private key range:

- Minimum: 1
- Maximum: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364140

### Address Derivation

1. **Private Key**: 256-bit random number
2. **Public Key**: Derived using secp256k1 elliptic curve
3. **Hash**: SHA256 + RIPEMD160 of public key
4. **Address**: Base58Check encoding with version byte

### Supported Address Types

- **P2PKH (Legacy)**: Version byte 0x00
- **P2SH (SegWit)**: Version byte 0x05
- **Testnet**: Different version bytes for testnet addresses

## Build Options

### Debug Build
```bash
make debug
```

### Release Build
```bash
make release
```

### Testing
```bash
make test
```

### Distribution
```bash
make dist
```

## Project Structure

```
btc-private-key-graph/
├── src/
│   ├── main.c          # Main application entry point
│   ├── keygen.c        # Key generation logic
│   ├── crypto.c        # Cryptographic functions
│   ├── address.c       # Address derivation
│   └── utils.c         # Utility functions
├── include/
│   ├── keygen.h        # Key generation interface
│   ├── crypto.h        # Cryptographic interface
│   ├── address.h       # Address derivation interface
│   └── utils.h         # Utility function interface
├── docs/
│   ├── explanation.md  # Technical documentation
│   └── whats-next.md   # Development roadmap
├── Makefile            # Build configuration
└── README.md           # This file
```

## Development

### Building from Source

```bash
git clone https://github.com/1cbyc/btc-private-key-graph.git
cd btc-private-key-graph
make
```

### Running Tests

```bash
make test
```

### Code Style

The project follows strict C coding standards:
- No comments in source code (documentation in separate files)
- Comprehensive error handling
- Memory safety practices
- Secure coding practices

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure all tests pass
5. Submit a pull request

## License

This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.

## Security Considerations

- **Never share private keys**: Generated keys should be kept secure
- **Use in secure environment**: Run on trusted systems only
- **Verify addresses**: Always verify generated addresses before use
- **Backup securely**: Store private keys in secure locations
- **Test thoroughly**: Test with small amounts before large transactions

## Disclaimer

This tool is provided for educational and development purposes. Users are responsible for the security of their generated keys and should follow proper security practices when using Bitcoin.

## Support

For issues and questions:
- Check the documentation in `docs/`
- Review the technical explanation in `docs/explanation.md`
- Check the development roadmap in `docs/whats-next.md`

## Version History

- **v2.0.0**: Complete rewrite with cryptographic security
- **v1.0.0**: Original basic implementation