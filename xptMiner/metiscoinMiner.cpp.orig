#include"global.h"
//#include "sph_sha2.h"

#define GROUPED_HASHES  (64)

void metiscoin_process(minerMetiscoinBlock_t* block)
{
	sph_keccak512_context	 ctx_keccak;
	sph_shavite512_context ctx_shavite[GROUPED_HASHES];
	sph_shavite512_context ctx_shavite_init[GROUPED_HASHES];
	
	sph_metis512_context ctx_metis[GROUPED_HASHES];
	sph_metis512_context ctx_metis_init[GROUPED_HASHES];
	
	static unsigned char pblank[1];
	block->nonce = 0;
	sph_shavite512_init(&ctx_shavite_init[0]);
//	sph_shavite512_init(&ctx_shavite_init[1]);
	sph_metis512_init(&ctx_metis_init[0]);
//	sph_metis512_init(&ctx_metis_init[1]);
	
//	for(uint32_t i = 0;(2<<i)+1<GROUPED_HASHES;i++)
//	{
//		memcpy(&ctx_shavite_init[(2<<i)-1], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*(2<<i));
//		memcpy(&ctx_metis_init[(2<<i)-1], &ctx_metis_init[0], sizeof(sph_metis512_context)*(2<<i));
//		//sph_shavite512_init(&ctx_shavite_init[i]);
//	}
	//testing stuffs
	//memcpy(&ctx_shavite_init[0], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*1);	
	//memcpy(&ctx_shavite_init[1], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*2);
	//memcpy(&ctx_shavite_init[3], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*4);
	//memcpy(&ctx_shavite_init[7], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*8);
	//memcpy(&ctx_shavite_init[15], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*16);
	for(uint32_t i = 1;i<GROUPED_HASHES;i++)
	{
		memcpy(&ctx_metis_init[i], &ctx_metis_init[0], sizeof(sph_metis512_context));
		
		//sph_metis512_init(&ctx_metis_init[i]);
	}
	for(uint32_t i = 1;i<GROUPED_HASHES;i++)
	{
		memcpy(&ctx_shavite_init[i], &ctx_shavite_init[0], sizeof(sph_shavite512_context));
		//sph_shavite512_init(&ctx_shavite_init[i]);
	}	
	uint32 target = *(uint32*)(block->targetShare+28);
	uint64 hash0[8*GROUPED_HASHES];
	uint64 hash2[8];
	// since only the nonce changes we can calculate the first keccak round in advance
	unsigned long long keccakPre[25];
	sph_keccak512_init(&ctx_keccak);
	keccak_core_prepare(&ctx_keccak, block, keccakPre);
	for(uint32 n=0; n<0x1000; n++)
	{
		if( block->height != monitorCurrentBlockHeight )
			break;
		for(uint32 f=0; f<0x8000; f += GROUPED_HASHES)
		{
			memcpy(&ctx_shavite[0], &ctx_shavite_init[0], sizeof(sph_shavite512_context)*GROUPED_HASHES);
			memcpy(&ctx_metis[0], &ctx_metis_init[0], sizeof(sph_metis512_context)*GROUPED_HASHES);
			// todo: Generate multiple hashes for multiple nonces at once
			block->nonce = n*0x10000+f;
			for(uint32 i=0; i<GROUPED_HASHES; i++)
			{
				keccak_core_opt(&ctx_keccak, keccakPre, *(unsigned long long*)(&block->nBits), hash0+i*8);
				block->nonce++;
			}
			for(uint32 i=0; i<GROUPED_HASHES; i++)
			{
				sph_shavite512(&ctx_shavite[i], hash0+i*8, 64);
				sph_shavite512_close(&ctx_shavite[i], hash0+i*8);
			}
				block->nonce = n*0x10000+f;
			for(uint32 i=0; i<GROUPED_HASHES; i++)
			{
				sph_metis512(&ctx_metis[i], hash0+i*8, 64);
				sph_metis512_close(&ctx_metis[i], hash2);
				if( *(uint32*)((uint8*)hash2+28) <= target )
				{
				  totalShareCount++;
				  //block->nonce = rawBlock.nonce;
				  xptMiner_submitShare(block);
				}
				block->nonce++;
			}
		}
		totalCollisionCount += 1; // count in steps of 0x8000
	}
}