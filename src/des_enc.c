#include <stdio.h>
#include <stdint.h>

static const uint8_t des_pc_table_1[56] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

static const uint8_t des_pc_table_2[48] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

static const uint8_t des_ip_table[64] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

static const uint8_t des_e_table[48] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

static const uint8_t des_s1_table[4][16] = {
	{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
	{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
	{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
	{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
};

static const uint8_t des_s2_table[4][16] = {
	{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
	{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
	{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
	{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
};

static const uint8_t des_s3_table[4][16] = {
	{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
	{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1}
	{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
	{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
};

static const uint8_t des_s4_table[4][16] = {
	{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
	{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
	{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
	{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
};

static const uint8_t des_ls_order[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 
	1, 2, 2, 2, 2, 2, 2, 1,
};

void printbin32(uint32_t n)
{
	char buf[33];
	for (int i = 0; i < 32; i++) {
		if ((n >> (31 - i)) & 1)
			buf[i] = '1';
		else
			buf[i] = '0';
	}

	buf[32] = '\0';
	printf("%s\n", buf);
}

void printbin64(uint64_t n)
{
	char buf[65];
	for (int i = 0; i < 64; i++) {
		if ((n >> (63 - i)) & 1)
			buf[i] = '1';
		else
			buf[i] = '0';
	}

	buf[64] = '\0';
	printf("%s\n", buf);
}


uint64_t e(uint32_t val)
{
	uint64_t tmp = 0;
	for (int i = 0; i < 48; i++) {
		int bit = (val >> (32 - des_e_table[i])) & 1;
		if (bit)
			tmp |= (1ULL << (47 - i));
	}
	return tmp;
}


uint32_t f(uint32_t data, uint64_t key)
{
	uint32_t tmp = key ^ e(data);
	return tmp;
}

uint64_t des_enc(uint64_t msg, uint64_t key)
{
	uint64_t k_plus = 0;

	for (int i = 0; i < 56; i++) {
		int bit = (key >> (64 - des_pc_table_1[i])) & 1;
		if (bit)
			k_plus |= (1ULL << (55 - i));
	}

	uint32_t c0 = (k_plus >> 28) & 0x0fffffff;
	uint32_t d0 = k_plus & 0x0fffffff;

	uint32_t c0_16[17];
	uint32_t d0_16[17];
	c0_16[0] = c0;
	d0_16[0] = d0;

	for (int i = 1; i < 17; i++) {
		int iter_num_shifts = des_ls_order[i - 1];
		for (int j = 0; j < iter_num_shifts; j++) {
			uint32_t cN_minus_one = (iter_num_shifts == 2) && (j == 1) ? c0_16[i] : c0_16[i - 1];
			int cN_last_bit = (cN_minus_one >> 27) & 1;
			c0_16[i] = ((cN_minus_one << 1) & 0x0fffffff) | cN_last_bit;

			uint32_t dN_minus_one = (iter_num_shifts == 2) && (j == 1) ? d0_16[i] : d0_16[i - 1];
			int dN_last_bit = (dN_minus_one >> 27) & 1;
			d0_16[i] = ((dN_minus_one << 1) & 0x0fffffff) | dN_last_bit;
		}
	}

	uint64_t k1_16[16];

	for (int i = 0; i < 16; i++) {
		uint64_t cN = c0_16[i + 1] & 0xfffffff;
		uint64_t dN = d0_16[i + 1] & 0xfffffff;
		uint64_t cNdN = (cN << 28) | dN;
		for (int j = 0; j < 48; j++) {
			int bit = (cNdN >> (56 - des_pc_table_2[j])) & 1;
			if (bit)
				k1_16[i] |= (1ULL << (47 - j));
		}
	}

	uint64_t msg_ip = 0;

	for (int i = 0; i < 64; i++) {
		int bit = (msg >> (64 - des_ip_table[i])) & 1;
		if (bit)
			msg_ip |= (1ULL << (63 - i));
	}
	
	uint32_t l0 = (msg_ip >> 32) & 0xffffffff;
	uint32_t r0 = msg_ip & 0xffffffff; 

	printbin32(r0);
	uint32_t data = f(r0, k1_16[0]);

	return data;
}

int main(void)
{
	uint64_t msg = 0x0123456789ABCDEF;
	uint64_t key = 0x133457799BBCDFF1;

	uint64_t result = des_enc(msg, key);

	printf("Ciphertext: 0x%llx\n", result);
	return 0;
}