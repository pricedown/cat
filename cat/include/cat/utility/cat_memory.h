////////////////////////////////////////////////////////////////////////////////
/// Copyright 2025 Daniel S. Buckstein
/// 
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
/// 
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
////////////////////////////////////////////////////////////////////////////////

/*! \file cat_memory.h
*   \brief Memory interface.
*/

#ifndef _CAT_MEMORY_H_
#define _CAT_MEMORY_H_


#include "cat/cat_platform.h"


cat_interface_begin;


//! \fn cat_memset
//! \brief Set all bytes in block to a specified byte value.
//! \param p_block Pointer to block.
//! \param value Byte value to assign to all bytes in block.
//! \param set_size Number of bytes to set.
//! \return \a p_block if success; null if failed.
cat_decl void* cat_memset(void* const p_block, uint8_t const value, size_t const set_size);

//! \fn cat_memclr
//! \brief Clear or zero all bytes in block.
//! \param p_block Pointer to block.
//! \param clr_size Number of bytes to clear.
//! \return \a p_block if success; null if failed.
cat_decl void* cat_memclr(void* const p_block, size_t const clr_size);

//! \fn cat_memcpy
//! \brief Copy bytes from source block to destination block.
//! \param p_block_dst Pointer to destination block.
//! \param p_block_src Pointer to source block.
//! \param cpy_size Number of bytes to copy.
//! \return \a p_block if success; null if failed.
cat_decl void* cat_memcpy(void* const p_block_dst, void const* const p_block_src, size_t const cpy_size);

//! \fn cat_memcmp
//! \brief Compare bytes in memory blocks.
//! \param p_block_lh Left-hand block.
//! \param p_block_rh Right-hand block.
//! \param cmp_size Number of bytes to compare.
//! \return True if blocks are equal.
cat_decl bool cat_memcmp(void const* const p_block_lh, void const* const p_block_rh, size_t const cmp_size);

//! \fn cat_malloc
//! \brief Wrapper for standard malloc.
//! \param block_size Size of block to allocate in bytes.
//! \return Pointer to block.
cat_decl void* cat_malloc(size_t const block_size);

//! \fn cat_calloc
//! \brief Wrapper for standard calloc.
//! \param element_count Number of elements to allocate.
//! \param element_size Size of single element in bytes.
//! \return Pointer to block.
cat_decl void* cat_calloc(size_t const element_count, size_t const element_size);

//! \fn cat_realloc
//! \brief Wrapper for standard realloc.
//! \param p_block Pointer to block.
//! \param block_size Size of block to allocate in bytes.
//! \return Pointer to block.
cat_decl void* cat_realloc(void* const p_block, size_t const block_size);

//! \fn cat_free
//! \brief Wrapper for standard free.
//! \param p_block Pointer to block.
cat_decl void cat_free(void* const p_block);

//! \fn cat_memory_pool_create
//! \brief Allocate and initialize managed memory pool.
//! \param pool_size Size of pool in bytes.
//! \return True if successful.
cat_decl bool cat_memory_pool_create(size_t const pool_size);

//! \fn cat_memory_pool_destroy
//! \brief Stop managing and deallocate memory pool.
//! \param p_pool Pointer to pre-allocated pool.
//! \return True if successful.
cat_decl bool cat_memory_pool_destroy(void);

//! \fn cat_memory_alloc
//! \brief Allocate block in managed memory pool.
//! \param block_size Size of block allocation in bytes.
//! \return Pointer to managed block if success; null if failed.
cat_decl void* cat_memory_alloc(size_t const block_size);

//! \fn cat_memory_dealloc
//! \brief Deallocate block in managed memory pool.
//! \param p_block Pointer to managed block.
//! \return True if successful.
cat_decl bool cat_memory_dealloc(void* const p_block);

typedef struct cat_memory_handle_t* phandle_t;

phandle_t test_alloc(size_t n);
void test_dealloc(phandle_t handle);
void* test_getptr(phandle_t);

cat_interface_end;


#endif // #ifndef _CAT_MEMORY_H_