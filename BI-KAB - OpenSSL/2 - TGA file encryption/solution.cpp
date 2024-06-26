// Josef Jech
#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

struct crypto_config
{
	const char *m_crypto_function;
	std::unique_ptr<uint8_t[]> m_key;
	std::unique_ptr<uint8_t[]> m_IV;
	size_t m_key_len;
	size_t m_IV_len;
};

#endif /* _PROGTEST_ */

const int header_len = 18;

bool check_gen_key_and_iv_encrypt(crypto_config &config, const EVP_CIPHER *cipher)
{

	size_t key_length = EVP_CIPHER_key_length(cipher);
	if (config.m_key.get() == nullptr || config.m_key_len != key_length)
	{
		config.m_key.reset();
		config.m_key = std::make_unique<uint8_t[]>(key_length);
		if (RAND_bytes(config.m_key.get(), key_length) == false)
			return false;
	}
	size_t iv_length = EVP_CIPHER_iv_length(cipher);
	if (iv_length != 0 && (config.m_IV.get() == nullptr || config.m_IV_len != iv_length))
	{
		config.m_IV.reset();
		config.m_IV = std::make_unique<uint8_t[]>(iv_length);
		if (RAND_bytes(config.m_IV.get(), iv_length) == false)
			return false;
	}
	config.m_key_len = key_length;
	config.m_IV_len = iv_length;
	return true;
}

bool check_key_and_iv_decrypt(crypto_config &config, const EVP_CIPHER *cipher)
{
	size_t key_length = EVP_CIPHER_key_length(cipher);
	if (config.m_key.get() == nullptr || config.m_key_len != key_length)
		return false;

	size_t iv_length = EVP_CIPHER_iv_length(cipher);
	if (iv_length != 0 && (config.m_IV.get() == nullptr || config.m_IV_len != iv_length))
		return false;

	return true;
}

bool encrypt_data(const std::string &in_filename, const std::string &out_filename, crypto_config &config)
{
	using namespace std;
	ifstream ifs;
	ifs.open(in_filename, ios::binary);
	ofstream ofs;
	ofs.open(out_filename, ios::binary | ios::trunc);

	char *tmp = new char[header_len];
	unsigned char ot[1024]; // open text
	unsigned char st[1024]; // sifrovany text
	int tmpLength = 0;

	OpenSSL_add_all_ciphers();
	
	const EVP_CIPHER *cipher = EVP_get_cipherbyname(config.m_crypto_function);
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); // context structure
	
	if (!cipher || ctx == NULL || !ifs.is_open() || !ofs.is_open())
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] tmp;
		return false;
	}

	if (!ifs.read(tmp, header_len) || ifs.gcount() < header_len)
	{
		delete[] tmp;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	if (!ofs.write(tmp, header_len))
	{
		delete[] tmp;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	delete[] tmp;

	if (check_gen_key_and_iv_encrypt(config, cipher) == false)
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	/* Sifrovani */
	if (!EVP_EncryptInit_ex(ctx, cipher, NULL, config.m_key.get(), config.m_IV.get())) // context init - set cipher, key, init vector
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	size_t chars_read = 0;
	while ((chars_read = ifs.readsome((char *)ot, sizeof(ot))) != 0)
	{
		if (!EVP_EncryptUpdate(ctx, st, &tmpLength, ot, chars_read)) // encryption of pt
		{
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
		if (!ofs.write((char *)st, tmpLength))
		{
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
	}
	if (!EVP_EncryptFinal_ex(ctx, st, &tmpLength)) // get the remaining ct
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	if (!ofs.write((char *)st, tmpLength))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return true;
}

bool decrypt_data(const std::string &in_filename, const std::string &out_filename, crypto_config &config)
{
	using namespace std;
	ifstream ifs;
	ifs.open(in_filename, ios::binary);
	ofstream ofs;
	ofs.open(out_filename, ios::binary | ios::trunc);

	char *tmp = new char[header_len];
	unsigned char ot[1024]; // open text
	unsigned char st[1024]; // sifrovany text
	int tmpLength = 0;

	OpenSSL_add_all_ciphers();
	
	const EVP_CIPHER *cipher = EVP_get_cipherbyname(config.m_crypto_function);
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); // context structure

	if (!cipher || ctx == NULL || !ifs.is_open() || !ofs.is_open())
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] tmp;
		return false;
	}

	if (!ifs.read(tmp, header_len) || ifs.gcount() < header_len)
	{
		delete[] tmp;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	if (!ofs.write(tmp, header_len))
	{
		delete[] tmp;
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	delete[] tmp;

	if (check_key_and_iv_decrypt(config, cipher) == false)
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	/* Desifrovani */
	if (!EVP_DecryptInit_ex(ctx, cipher, NULL, config.m_key.get(), config.m_IV.get())) // context init - set cipher, key, init vector
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	size_t chars_read = 0;
	while ((chars_read = ifs.readsome((char *)st, sizeof(st))) != 0)
	{
		if (!EVP_DecryptUpdate(ctx, ot, &tmpLength, st, chars_read)) // encryption of pt
		{
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
		if (!ofs.write((char *)ot, tmpLength))
		{
			EVP_CIPHER_CTX_free(ctx);
			return false;
		}
	}
	if (!EVP_DecryptFinal_ex(ctx, ot, &tmpLength)) // get the remaining ct
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	if (!ofs.write((char *)ot, tmpLength))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return true;
}

#ifndef __PROGTEST__

bool compare_files(const char *name1, const char *name2)
{
	ifstream ifs1(name1, ios::binary);
	ifstream ifs2(name2, ios::binary);
	if (ifs1.is_open() && ifs2.is_open())
	{
		char a, b;
		while (ifs1.eof() == false && ifs2.eof() == false)
		{
			ifs1 >> a;
			ifs2 >> b;
			if (a != b)
				return false;
		}
		return true;
	}
	return false;
}

int main(void)
{
	crypto_config config{nullptr, nullptr, nullptr, 0, 0};

	// ECB mode
	config.m_crypto_function = "AES-128-ECB";
	config.m_key = std::make_unique<uint8_t[]>(16);
	memset(config.m_key.get(), 0, 16);
	config.m_key_len = 16;

	assert(encrypt_data("homer-simpson.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "homer-simpson_enc_ecb.TGA"));

	assert(decrypt_data("homer-simpson_enc_ecb.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "homer-simpson.TGA"));

	assert(encrypt_data("UCM8.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "UCM8_enc_ecb.TGA"));

	assert(decrypt_data("UCM8_enc_ecb.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "UCM8.TGA"));

	assert(encrypt_data("image_1.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_1_enc_ecb.TGA"));

	assert(encrypt_data("image_2.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_2_enc_ecb.TGA"));

	assert(decrypt_data("image_3_enc_ecb.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_3_dec_ecb.TGA"));

	assert(decrypt_data("image_4_enc_ecb.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_4_dec_ecb.TGA"));

	// CBC mode
	config.m_crypto_function = "AES-128-CBC";
	config.m_IV = std::make_unique<uint8_t[]>(16);
	config.m_IV_len = 16;
	memset(config.m_IV.get(), 0, 16);

	assert(encrypt_data("UCM8.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "UCM8_enc_cbc.TGA"));

	assert(decrypt_data("UCM8_enc_cbc.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "UCM8.TGA"));

	assert(encrypt_data("homer-simpson.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "homer-simpson_enc_cbc.TGA"));

	assert(decrypt_data("homer-simpson_enc_cbc.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "homer-simpson.TGA"));

	assert(encrypt_data("image_1.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_5_enc_cbc.TGA"));

	assert(encrypt_data("image_2.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_6_enc_cbc.TGA"));

	assert(decrypt_data("image_7_enc_cbc.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_7_dec_cbc.TGA"));

	assert(decrypt_data("image_8_enc_cbc.TGA", "out_file.TGA", config) &&
		   compare_files("out_file.TGA", "ref_8_dec_cbc.TGA"));
	return 0;
}

#endif /* _PROGTEST_ */
