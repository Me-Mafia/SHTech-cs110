#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

/* declaration */
uint64_t logTwo(uint64_t x);

struct cashier *cashier_init(struct cache_config config) {
	struct cashier *ret = (struct cashier *)malloc(sizeof(struct cashier));
	if (ret == NULL) return NULL;
	/* config init */
	ret->config = config;
	ret->size = config.lines * config.line_size;
	ret->index_bits = logTwo(config.lines/config.ways);
	ret->offset_bits = logTwo(config.line_size);
	ret->tag_bits = config.address_bits - ret->index_bits - ret->offset_bits;
	/* initialize the masks */
	ret->tag_mask = ((1<<ret->tag_bits) - 1) << (ret->index_bits + ret->offset_bits);
	ret->index_mask = ((1<<ret->index_bits) - 1) << (ret->offset_bits);
	ret->offset_mask = ((1<<ret->offset_bits) - 1);
	ret->lines = (struct cache_line *)malloc(config.lines * sizeof(struct cache_line));
	/* if allocation failed */
	if (ret->lines == NULL) 
	{
		free(ret);
		return NULL;
	}
	/* lines init */
	for (uint64_t i = 0; i < (config.lines); i++)
	{
		/* initialize line i */
		ret->lines[i].dirty = 0;
		ret->lines[i].valid = 0;
		ret->lines[i].tag = 0;
		ret->lines[i].last_access = 0;
		/* allocate memory for data */
		ret->lines[i].data = (uint8_t *)malloc(config.line_size * sizeof(uint8_t));
		if (ret->lines[i].data == NULL)
		{
			/* if allocation failed */
			for (uint64_t j = 0; j < i; ++j)
			{
				free(ret->lines[j].data);
			}
			free(ret->lines);
			free(ret);
			return NULL;
		}
		/* handle the allocation fails */
	}
	return ret;
}

void cashier_release(struct cashier *cache) {
	/* nullptr */
	if (cache == NULL)
		return;
	/* evict the cache lines in the given order */
	uint64_t set_num = cache->config.lines / cache->config.ways;
	for (uint64_t j = 0; j < cache->config.ways; ++j)
	{
		for (uint64_t i = 0; i < set_num; ++i)
		{
			/* iter in indices */
			if(cache->lines[i*(cache->config.ways) + j].valid)
			{
				before_eviction(i, &(cache->lines[i*(cache->config.ways) + j]));
				uint64_t tag_addr = 
					(cache->lines[i*(cache->config.ways) + j].tag << (cache->index_bits + cache->offset_bits))
					+ (i << (cache->offset_bits));
				/* write the dirty lines */
				if (cache->lines[i*(cache->config.ways) + j].dirty)
				{
					for (uint64_t k=0; k<cache->config.line_size; k++)
					{
            			mem_write(tag_addr + k, cache->lines[j + i*(cache->config.ways)].data[k]);
          			}	
				}
			}
		}
	}
	/* free the memory allocated */
	for (uint64_t i=0; i < cache->config.lines; ++i)
		free(cache->lines[i].data);
  	free(cache->lines);
  	free(cache);
	return;
  	// YOUR CODE HERE
}

bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  	/* the tag, index and offset of the given address */
	uint64_t tag = (addr & cache->tag_mask) >> (cache->index_bits + cache->offset_bits);
  	uint64_t index = (addr & cache->index_mask) >> (cache->offset_bits);
  	uint64_t offset = (addr & cache->offset_mask);
	uint64_t set_num = cache->config.lines / cache->config.ways;
	for (uint64_t i = 0; i < set_num ; ++i)
  	{
		/* helps reduce time complexity when j gets too big */
		if (i == index)
		{
			for (uint64_t j = 0; j < cache->config.ways; ++j)
			{
				/* found the tag in certain cache line */
				if (cache->lines[i*(cache->config.ways) + j].tag == tag)
				{
					cache->lines[i*(cache->config.ways) + j].last_access = get_timestamp();
					if (cache->lines[i*(cache->config.ways) + j].valid)
					/* hit: tag exists and line valid */
					{
						*byte = cache->lines[i*(cache->config.ways) + j].data[offset];
						return true;
					}
					else
					{
						/* miss: empty line */
						cache->lines[i*(cache->config.ways) + j].valid = true;
						for (uint64_t k=0; k<cache->config.line_size; ++k)
						{
							cache->lines[i*(cache->config.ways) + j].data[k] = mem_read(addr - offset + k);
						}
						*byte = cache->lines[i*(cache->config.ways) + j].data[offset];
						return false;
					}
				}
			}
			/* a buffer for the desired part in the memory */
			/*
			uint64_t buffer;
			for (uint64_t j=0; j<cache->config.line_size ; ++j)
			{
				buffer += mem_read(addr - offset + j) << (cache->config.line_size - j);
			}
			*/
			/* find a emply line in the set */
			for (uint64_t j=0; j<cache->config.ways; ++j)
			{
				if (!(cache->lines[i*(cache->config.ways) + j].valid))
				{
					/* update the parameters */
					cache->lines[i*(cache->config.ways) + j].last_access = get_timestamp();
					cache->lines[i*(cache->config.ways) + j].tag = tag;
					cache->lines[i*(cache->config.ways) + j].dirty = false;
					cache->lines[i*(cache->config.ways) + j].valid = true;
					/* load the memory to line */
					for (uint64_t k=0; k<cache->config.line_size; ++k)
					{
						cache->lines[i*(cache->config.ways) + j].data[k] = mem_read(addr - offset + k);
					}
					/* load the data to the byte ptr */
					*byte = cache->lines[i*(cache->config.ways) + j].data[offset];
					return false;
				}
			}

			/* to overwrite an already valid cache_line */
			int way_num = 0;
			uint64_t last_time = __UINT64_MAX__;
			for (uint64_t j=0; j<cache->config.ways; ++j)
			{
				if(cache->lines[i*(cache->config.ways) + j].last_access < last_time)
				{
					/* update the last_Time line */
					way_num = j;
					last_time = cache->lines[i*(cache->config.ways) + j].last_access; 
				}
			}

			before_eviction(i,&(cache->lines[i*(cache->config.ways) + way_num]));
			/* if dirty, write back to memory */ 
			if(cache->lines[i*(cache->config.ways) + way_num].dirty){
				uint64_t addr_tmp =
					((cache->lines[i*(cache->config.ways) + way_num].tag) << (cache->offset_bits + cache->index_bits))
					+ (i << (cache->offset_bits));
				/* write for */
				for (uint64_t k=0; k<cache->config.line_size; ++k){
					mem_write(addr_tmp + k, cache->lines[i*(cache->config.ways) + way_num].data[k]);
				}
			}
			/* update the parameters */
			cache->lines[i*(cache->config.ways) + way_num].last_access = get_timestamp();
			cache->lines[i*(cache->config.ways) + way_num].tag = tag;
			cache->lines[i*(cache->config.ways) + way_num].dirty = false;
			/* write the part in the memory to  */
			for (uint64_t k=0; k<cache->config.line_size; ++k)
			{
				cache->lines[i*(cache->config.ways) + way_num].data[k] = mem_read(addr - offset + k);
			}
			/* load the data to the byte ptr */
			*byte = cache->lines[i*(cache->config.ways) + way_num].data[offset];
			return false;
		} 
  	}
	/* address not found */
	return false;
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
	/* get the related parameters of addr */
	uint64_t tag = (addr & cache->tag_mask) >> (cache->index_bits + cache->offset_bits);
	uint64_t index = (addr & cache->index_mask) >> (cache->offset_bits);
	uint64_t offset = (addr & cache->offset_mask);
	uint64_t set_num = cache->config.lines / cache->config.ways;
	/* iter in indices */
	for (uint64_t i=0; i<set_num; ++i)
	{
		if (i == index)
		{
			/* iter in ways */
			for (uint64_t j=0; j<cache->config.ways; ++j)
			{
				if (cache->lines[i*(cache->config.ways) + j].tag == tag)
				{
					cache->lines[i*(cache->config.ways) + j].last_access = get_timestamp();
					cache->lines[i*(cache->config.ways) + j].dirty = true;
					/* the way is hit */
					if (cache->lines[i*(cache->config.ways) + j].valid)
					{
						cache->lines[i*(cache->config.ways) + j].data[offset] = byte;
						return true;
					}
					/* the way is not occupied */
					else
					{
						cache->lines[i*(cache->config.ways) + j].valid = true;
						/* init the data */
						for (uint64_t k=0; k < cache->config.line_size; k++)
						{
							cache->lines[i*(cache->config.ways) + j].data[k] = mem_read(addr - offset + k);
						}
						/* write_cashier */
						cache->lines[i*(cache->config.ways) + j].data[offset] = byte;
						return false;
					}
				}
			}
			/* find another not occupied line */
			for (uint64_t j=0; j<cache->config.ways; ++j)
			{
				if (!(cache->lines[i*(cache->config.ways) + j].valid))
				{
					/* initialize... */
					cache->lines[i*(cache->config.ways) + j].last_access = get_timestamp();
					cache->lines[i*(cache->config.ways) + j].tag = tag;
					cache->lines[i*(cache->config.ways) + j].dirty = true;
					cache->lines[i*(cache->config.ways) + j].valid = true;
					/* init data */
					for (uint64_t k=0; k < cache->config.line_size; k++)
					{
						cache->lines[i*(cache->config.ways) + j].data[k] = mem_read(addr - offset + k);
					}
					/* write cashier */
					cache->lines[i*(cache->config.ways) + j].data[offset] = byte;
					return false;
				}
			}
			
			/* LRU select the victim */
			int way_num = 0;
			uint64_t last_time = __UINT64_MAX__;
			for (uint64_t j = 0; j < cache->config.ways; j++)
			{
				if (cache->lines[i*(cache->config.ways) + j].last_access < last_time)
				{
					/* update the last_time line */
					last_time = cache->lines[i*(cache->config.ways) + j].last_access;
					way_num = j;
				}
			}
			before_eviction(i, &(cache->lines[i*(cache->config.ways) + way_num]));
			/* for an already dirty line... */
			if(cache->lines[i*(cache->config.ways) + way_num].dirty)
			{
				uint64_t addr_tmp = 
					(cache->lines[i*(cache->config.ways) + way_num].tag << (cache->index_bits + cache->offset_bits))
					+ (i << (cache->offset_bits));
				/* write the dirty line to mem */
				for (uint64_t k = 0; k < cache->config.line_size; ++k)
				{
					mem_write(addr_tmp + k, cache->lines[i*(cache->config.ways) + way_num].data[k]);
				}
			}
			/* update the parameters */
			cache->lines[i*(cache->config.ways) + way_num].tag = tag;
			cache->lines[i*(cache->config.ways) + way_num].last_access = get_timestamp();
			for (uint64_t k = 0; k < cache->config.line_size; ++k)
			{
				cache->lines[i*(cache->config.ways) + way_num].data[k] = mem_read(addr - offset + k);
			}
			/* update the parameters */
			cache->lines[i*(cache->config.ways) + way_num].data[offset] = byte;
			cache->lines[i*(cache->config.ways) + way_num].dirty = true;
			return false;
			/* code */
		}
	}
	/* mysterious unexpected condition */
	return false;
}

/* compute ret = log2(x) */
uint64_t logTwo(uint64_t x)
{
  uint64_t ret = 0; uint64_t y = 1;
  for (ret = 0; x != y; ret++)
    y <<= 1;
  return ret;
}
