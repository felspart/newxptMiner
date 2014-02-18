#include"global.h"
#include "ticker.h"
bool maxcoinOpenCLKernelInited = false;

struct  
{
	// input buffer (block data)
	void* buffer_blockInputData;
	cl_mem clBuffer_blockInputData;
	// output buffer (nonce indices)
	void* buffer_nonceOutputData;
	cl_mem clBuffer_nonceOutputData;
	// kernel
	cl_kernel kernel_keccak;
}maxcoinGPU;




void maxcoin_init()
{

}

inline uint32 maxcoin_swapEndianU32(uint32 v)
{
	return (v>>24)|((v>>8)&0xFF00)|((v<<8)&0xFF0000)|(v<<24);
}

void xptMiner_submitShare_test(minerMaxcoinBlock_t* block)
{
	// debug method to check if share is 100% valid without server access
	uint32* blockInputData = (uint32*)block;
	uint64 hash0[4];
	sph_keccak256_context	 ctx_keccak;
	sph_keccak256_init(&ctx_keccak);	
	sph_keccak256(&ctx_keccak, blockInputData, 80);
	sph_keccak256_close(&ctx_keccak, hash0);
	if( (hash0[3]>>32) <= *(uint32*)(block->targetShare+32-4) )
	{
		printf("Share valid (nonce: %08x)\n", block->nonce);
	}
	else
	{
		printf("Share invalid (nonce: %08x)\n", block->nonce);
		totalRejectedShareCount++;
	}
}



void maxcoin_process(minerMaxcoinBlock_t* block)
{
	//memset(block, 0x00, 80); // debug

	__attribute__((aligned(32))) uint64 hash0[4];
	// endian swap block data (input data needs to be big endian)
	//uint32 blockDataBE[80/4];
	uint32* blockInputData = (uint32*)block;
	//for(sint32 i=0; i<20-1; i++)
	//	blockDataBE[i] = maxcoin_swapEndianU32(blockInputData[i]);
	//blockDataBE[19] = 0; // set nonce to zero	
	uint64 targetU64 = *(uint64*)(block->targetShare+24);
	//keccak_core_prepare(&ctx_keccak, block, keccakPre);
	for(uint32 n=0; n<0x10000; n++)
	{
		if( block->height != monitorCurrentBlockHeight )
			break;
		if( (block->nTime+60) < monitorCurrentBlockTime )
		{
			// need to update time
			block->nTime = monitorCurrentBlockTime;
			//blockInputData[68/4] = blockInputData[68/4];
		}
		for(uint32 f=0; f<0x8000; f++ )
		{	
			if( keccak256_maxcoin_opt_v((unsigned long long*)blockInputData) <= targetU64 )
			{
				totalShareCount++;
				block->nonce = blockInputData[19];
				xptMiner_submitShare(block);
			}
			blockInputData[19]++;
			//void metis4_core_opt_p1(unsigned int* data, unsigned int* pOut);
			//unsigned int metis4_core_opt_p2(unsigned int* pIn);
		}
		totalCollisionCount += 1; // count in steps of 0x8000
	}
	// printf("[DEBUG] Exited with final nonce 0x%08x\n", blockDataBE[19]);
}