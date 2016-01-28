/*
 * garbled_circuit_low_mem.h
 *
 *  Created on: Oct 26, 2015
 *      Author: ebi
 */

#ifndef GARBLED_CIRCUIT_GARBLED_CIRCUIT_LOW_MEM_H_
#define GARBLED_CIRCUIT_GARBLED_CIRCUIT_LOW_MEM_H_

#include <openssl/bn.h>
#include "garbled_circuit/garbled_circuit_util.h"
#include "crypto/aes.h"

int GarbleBNLowMem(const GarbledCircuit& garbled_circuit, BIGNUM* g_init,
                   BIGNUM* g_input, uint64_t clock_cycles,
                   const string& output_mask, OutputMode output_mode,
                   block* init_labels, block* input_labels,
                   block* output_labels, short* output_vals, BIGNUM* output_bn,
                   block R, block global_key, bool disable_OT, int connfd);
int EvaluateBNLowMem(const GarbledCircuit& garbled_circuit, BIGNUM* e_init,
                     BIGNUM* e_input, uint64_t clock_cycles,
                     const string& output_mask, OutputMode output_mode,
                     block* init_labels, block* input_labels,
                     block* output_labels, short* output_vals,
                     BIGNUM* output_bn, block global_key, bool disable_OT,
                     int connfd);
uint64_t GarbleLowMem(const GarbledCircuit& garbled_circuit, block* init_labels,
                      block* input_labels, block* garbled_tables,
                      uint64_t *garbled_table_ind, block R, AES_KEY& AES_Key,
                      uint64_t cid, int connfd, BlockPair *wires,
                      short* wires_val, int* fanout,
                      uint64_t* num_skipped_gates, block* output_labels,
                      short* output_vals);
uint64_t EvaluateLowMem(const GarbledCircuit& garbled_circuit,
                        block* init_labels, block* input_labels,
                        block* garbled_tables, uint64_t *garbled_table_ind,
                        AES_KEY& AES_Key, uint64_t cid, int connfd,
                        block *wires, short* wires_val, int* fanout,
                        block* output_labels, short* output_vals);
int GarbleAllocLabels(const GarbledCircuit& garbled_circuit,
                      block** init_labels, block** input_labels,
                      block** output_labels, short** output_vals, block R);
int GarbleGneInitLabels(const GarbledCircuit& garbled_circuit,
                        block* init_labels, block R);
int GarbleGenInputLabels(const GarbledCircuit& garbled_circuit,
                         block* input_labels, block R);
int EvaluateAllocLabels(const GarbledCircuit& garbled_circuit,
                        block** init_labels, block** input_labels,
                        block** output_labels, short** output_vals);
int GarbleTransferInitLabels(const GarbledCircuit& garbled_circuit,
                             BIGNUM* g_init, block* init_labels,
                             bool disable_OT, int connfd);
int GarbleTransferInputLabels(const GarbledCircuit& garbled_circuit,
                              BIGNUM* g_input, block* input_labels,
                              uint64_t cid, bool disable_OT, int connfd);
int EvaluateTransferInitLabels(const GarbledCircuit& garbled_circuit,
                               BIGNUM* e_init, block* init_labels,
                               bool disable_OT, int connfd);
int EvaluateTransferInputLabels(const GarbledCircuit& garbled_circuit,
                                BIGNUM* e_input, block* input_labels,
                                uint64_t cid, bool disable_OT, int connfd);
int OutputBN2StrLowMem(const GarbledCircuit& garbled_circuit, BIGNUM* outputs,
                       uint64_t clock_cycles, OutputMode output_mode,
                       string* output_str);
int GarbleTransferOutputLowMem(const GarbledCircuit& garbled_circuit,
                               block* output_labels, short* output_vals,
                               uint64_t cid, OutputMode output_mode,
                               const string& output_mask, BIGNUM* output_bn,
                               int connfd);
int EvaluateTransferOutputLowMem(const GarbledCircuit& garbled_circuit,
                                 block* output_labels, short* output_vals,
                                 uint64_t cid, OutputMode output_mode,
                                 const string& output_mask, BIGNUM* output_bn,
                                 int connfd);

#endif /* GARBLED_CIRCUIT_GARBLED_CIRCUIT_LOW_MEM_H_ */
