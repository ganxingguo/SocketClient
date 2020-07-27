/****************************************************************************
*
* RC4 Encyption Class
* Copyright (c) 2007 Burlex
* Adapted from rc4.c to C++
*
* This file may be distributed under the terms of the Q Public License
* as defined by Trolltech ASA of Norway and appearing in the file
* COPYING included in the packaging of this file.
*
* This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
* WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#ifndef _RC4ENGINE_H_
#define _RC4ENGINE_H_

class RC4Engine
{


public:
	unsigned char perm[256];
	unsigned char index1, index2;
	bool Initialized;
	bool Encrypted;
	UINT	count;
	//! RC4Engine constructor. Must supply a key and the length of that array.
	RC4Engine(const unsigned char * keybytes, unsigned int keylen)
	{
		Setup(keybytes, keylen);
		Encrypted = false;
	}

	void UnInit()
	{
		memset(perm,0,256);
		index1 = index2 = 0;
		Initialized = false;
		count=0;
	}
	void SetInit(bool bInitialized){ Initialized = bInitialized; }
	bool IsInit(){return Initialized;}
	inline void Encrypt(bool encrypt) { Encrypted = encrypt; }
	inline bool IsEncrypt() {  return Encrypted; }
	RC4Engine() : Initialized(false),count(0), Encrypted(false){}

	//! Destructor
	~RC4Engine() { }

	//! Initializes permutation, etc.
	void Setup(const unsigned char * keybytes, const unsigned int keylen)
	{
		memset(perm,0,256);
		unsigned int i = 0;
		unsigned char j = 0, k;
		count=0;
		// Initialize RC4 state (all bytes to 0)
		for(; i < 256; ++i)
			perm[i] = (unsigned __int8)i;

		// Set index values
		index1 = index2 = 0;

		// Randomize the permutation
		for(j = 0, i = 0; i < 256; ++i)
		{//ÈÅÂÒÃÜÔ¿ 
			j += perm[i] + keybytes[i % keylen];
			k = perm[i];
			perm[i] = perm[j];
			perm[j] = k;
		}

		//Initialized = true;
	}

	//! Processes the specified array. The same function is used for both
	//  encryption and decryption.

	void Process(unsigned char * input, unsigned char * output, unsigned int len)
	{
		//static int ai = 0;
		//ai++;
		//printf("aaaaaa\n");
		if (Initialized)
			count++;
		if (!Encrypted && Initialized && input)
		{
			memcpy(output, input, len);
			return;
		}

		
		if(!Initialized || !input)
		{
			return;
		}


		unsigned int i = 0;
		unsigned char j, k;
		
		for(; i < len; ++i)
		{
			index1++;
			index2 += perm[index1];

			k = perm[index1];
			perm[index1] = perm[index2];
			perm[index2] = k;

			j = perm[index1] + perm[index2];
			output[i] = input[i] ^ perm[j];
		}
	}

	UINT GetPerm()
	{
		return count;
	}

	void ReadProcess(unsigned char * input, unsigned char * output, unsigned int len)
	{
		if(!Initialized || !input)
		{
			return;
		}
		if (!Encrypted)
		{
			memcpy(output, input, len);
			return;
		}
		unsigned int i = 0;
		unsigned char j, k;
		for(; i < len; ++i)
		{
			k = perm[index1];
			perm[index1] = perm[index2];
			perm[index2] = k;

			j = perm[index1] + perm[index2];
			output[i] = input[i] ^ perm[j];
		}
	}
};


//! Reverses the bytes in an array in the opposite order.
__inline void ReverseBytes(unsigned char * Pointer, unsigned int Length)
{
	unsigned char * Temp = (unsigned char*)malloc(Length);
	memcpy(Temp, Pointer, Length);

	for(unsigned int i = 0; i < Length; ++i)
		Pointer[i] = Temp[Length - i - 1];

	free(Temp);
}

#endif  // _SHARED_RC4ENGINE_H