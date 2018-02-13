/*
 This file is part of JustGarble.

 JustGarble is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 JustGarble is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with JustGarble.  If not, see <http://www.gnu.org/licenses/>.

 */
/*
 This file is part of TinyGarble. It is modified version of JustGarble
 under GNU license.

 TinyGarble is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TinyGarble is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TinyGarble.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "garbled_circuit/garbled_circuit.h"

#include "scd/scd.h"
#include "scd/scd_evaluator.h"
#include "crypto/aes.h"
#include "crypto/BN.h"
#include "crypto/OT.h"
#include "crypto/OT_extension.h"
#include "garbled_circuit/garbled_circuit_util.h"
#include "garbled_circuit/garbled_circuit_high_mem.h"
#include "garbled_circuit/garbled_circuit_low_mem.h"
#include "tcpip/tcpip.h"
#include "util/log.h"
#include "util/common.h"
#include "util/util.h"

int GarbleStr(const string& file_address, uint64_t clock_cycles,
		const string& output_mask, int64_t terminate_period,
		OutputMode output_mode, bool disable_OT, bool low_mem_foot,
		string* output_str, int connfd) {
	if (clock_cycles == 0) {
		return FAILURE;
	}

	GarbledCircuitCollection garbled_circuit_collection;
	if (ReadTGX(file_address, &garbled_circuit_collection) == FAILURE) {
		LOG(ERROR) << "Error while reading tgx file: " << file_address
				<< endl;
		return FAILURE;
	}

	//FIX need to handle multiple circuits --- connection of public wires
	for (int i = 0; i < garbled_circuit_collection.number_of_circuits; i++) {
		FillFanout(&garbled_circuit_collection.garbled_circuits[i]);
	}

	//FIX needs to handle terminate
//	if (terminate_period != 0 && garbled_circuit.terminate_id == 0) {
//		LOG(ERROR) << "There is no terminate signal in the circuit."
//				" The terminate period should be 0." << endl;
//		return FAILURE;
//	}

// parse init and input
	//NEXT: read all files and store them in a struct and pass it to GarbleStr and EvaluatorStr
	BIGNUM* p_init = BN_new();
	BIGNUM* p_input = BN_new();
	BIGNUM* g_init = BN_new();
	BIGNUM* g_input = BN_new();
	BIGNUM* output_bn = BN_new();

	//FIX need to handle multiple inputs for different circuits
	string init_str = "0";
	string p_init_str = "0";
	string p_input_str = "0";
	string input_str  = ReadFileOrPassHex("./Inputs/0_g.txt");

	//garbler input and init
	CHECK(
			ParseInitInputStr(init_str, input_str, &g_init, &g_input));
	//pubic input and init
	CHECK(
			ParseInitInputStr(p_init_str, p_input_str, &p_init, &p_input));

	block R = RandomBlock();  // secret label
	*((short *) (&R)) |= 1;

	// global key
	block global_key = RandomBlock();
	CHECK(SendData(connfd, &global_key, sizeof(block)));  // send global key

	//FIX need to handle low_mem
//	if (low_mem_foot && clock_cycles > 1) {
//		CHECK(
//				GarbleBNLowMem(garbled_circuit, p_init, p_input, g_init,
//						g_input, &clock_cycles, output_mask, terminate_period,
//						output_mode, output_bn, R, global_key, disable_OT,
//						connfd));
//
//		CHECK(
//				OutputBN2StrLowMem(garbled_circuit, output_bn, clock_cycles,
//						output_mode, output_str));
//
//	} else {
	CHECK(
			GarbleBNHighMem(garbled_circuit_collection, p_init, p_input, g_init,
					g_input, &clock_cycles, output_mask, terminate_period,
					output_mode, output_bn, R, global_key, disable_OT, connfd));
	CHECK(
			OutputBN2StrHighMem(garbled_circuit_collection, output_bn,
					clock_cycles, output_mode, output_str));
//	}
	BN_free(p_init);
	BN_free(p_input);
	BN_free(g_init);
	BN_free(g_input);
	BN_free(output_bn);

	RemoveGarbledCircuitCollection(&garbled_circuit_collection);

	return SUCCESS;
}

int EvaluateStr(const string& file_address, uint64_t clock_cycles,
		const string& output_mask, int64_t terminate_period,
		OutputMode output_mode, bool disable_OT, bool low_mem_foot,
		string* output_str, int connfd) {
	if (clock_cycles == 0) {
		return FAILURE;
	}

	GarbledCircuitCollection garbled_circuit_collection;
	if (ReadTGX(file_address, &garbled_circuit_collection) == FAILURE) {
		LOG(ERROR) << "Error while reading tgx file: " << file_address
				<< endl;
		return FAILURE;
	}

	//FIX need to handle multiple circuits --- connection of public wires
	for (int i = 0; i < garbled_circuit_collection.number_of_circuits; i++) {
		FillFanout(&garbled_circuit_collection.garbled_circuits[i]);
	}

	//FIX needs to handle terminate
//	if (terminate_period != 0 && garbled_circuit.terminate_id == 0) {
//		LOG(ERROR) << "There is no terminate signal in the circuit."
//				" The terminate period should be 0." << endl;
//		return FAILURE;
//	}

	// allocate init and input values and translate form string
	BIGNUM* p_init = BN_new();
	BIGNUM* p_input = BN_new();
	BIGNUM* e_init = BN_new();
	BIGNUM* e_input = BN_new();
	BIGNUM* output_bn = BN_new();

	string init_str = "0";
	string p_init_str = "0";
	string p_input_str = "0";
	string input_str  = ReadFileOrPassHex("./Inputs/0_e.txt");

	CHECK(
			ParseInitInputStr(init_str, input_str, &e_init, &e_input));
	CHECK(
			ParseInitInputStr(p_init_str, p_input_str,&p_init, &p_input));

	// global key
	block global_key = RandomBlock();
	CHECK(RecvData(connfd, &global_key, sizeof(block)));  // receive global key

	//FIX
//	if (low_mem_foot && clock_cycles > 1) {
//		CHECK(
//				EvaluateBNLowMem(garbled_circuit, p_init, p_input, e_init,
//						e_input, &clock_cycles, output_mask, terminate_period,
//						output_mode, output_bn, global_key, disable_OT, connfd));
//		CHECK(
//				OutputBN2StrLowMem(garbled_circuit, output_bn, clock_cycles,
//						output_mode, output_str));
//	} else {
	CHECK(
			EvaluateBNHighMem(garbled_circuit_collection, p_init, p_input,
					e_init, e_input, &clock_cycles, output_mask,
					terminate_period, output_mode, output_bn, global_key,
					disable_OT, connfd));
	CHECK(
			OutputBN2StrHighMem(garbled_circuit_collection, output_bn,
					clock_cycles, output_mode, output_str));
//	}

	BN_free(p_init);
	BN_free(p_input);
	BN_free(e_init);
	BN_free(e_input);
	BN_free(output_bn);

	RemoveGarbledCircuitCollection(&garbled_circuit_collection);
	return SUCCESS;
}

