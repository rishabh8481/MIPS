//
//  main.cpp
//  MISP
//
//  Created by Rishabh on 11/17/17.
//  Copyright © 2017 Rishabh. All rights reserved.
//


/* Aim : - MIPS Multi-cycled CPU Simulation Project
 * Author: - Rishabh Sharma
 * Student Id:- 109874160
 * Compiler version used for project: - Clang C++ compiler, with LLVM as backend which is conform to the C++11 standart and uses libc++ as the standart library.
 * IDE used for Project:- Xcode
 * Input :- File containing the MIPS machine instructions and data.
 * 			First line must contain the number of instruction and number data lines present in the file.
 *
 * Output :- In Console and In result.txt file
 
 * Description:- Simullation is build in 2 phases
 *	Phase 1:- ALU implementation
 *			In this phase 6 ALU operations were implemented: - add, sub, or, and, nor, slt
 *			all the ALU operations were performed on 32-bit binary numbers stores in arrays using three_two_bit_alu() function.
 *			three_two_bit_alu function will calls one_bit_alu and one_bit_alu_of(for checking offset bit) function for performing the Arithmetic and Logical operations
 *
 * Phase 2:- MIPS pipeline implementation
 * 			 Instructions/Data first loaded from the file to the global variable memory[200][8] (0-99 represent instruction memory and 100-199 represent data memory) each memory address stores 8 bits of information
 * 			 All the 32 register for MIPS is stored and initialized in register[32][32] represents 0 - 32 register each can contain data of 32 bits
 *			 Special Registers like PC - Program Counter, IR - Instruction register, ALU-Out - ALU output register, MDR - Memory Data Register, A and B register used as a input for ALU are also initialized and used.
 * 			 MIPS pipeline is implemented using 5 functions representing 5 different clock cycles of MIPS namely Instruction Fetch, Instruction Decode, Execution, Instruction Memory Access, and Write Back.
 * 			 All the five clock cycles uses 2*1 and 4*1 multiplexer with different control signals for proper execution of the instruction.
 *
 * Requirement for the simullation to execute sucessfully:- Termination instruction op=63 must be present in the instruction file (after all the instruction lines and before the data) for successfull termination of the simulation
 *
 *
 * Program finished with exit code 0 :- success
 * Program finished with exit code 1 :- failure
 *
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// instruction and data memory
// 0 to 100 instruction memory
// 100 to 200 data memory
int memory[200][8] = {0};

// 32-bits 32 registers
int register_file[32][32]={0};
// Other required register
int PC[32]  	=  {0};
int bit_4[32] 	=  {0};
int IR[32]  	=  {0};
int A[32]       =  {0};
int B[32]   	=  {0};
int ALU_OUT[32] =  {0};
int MDR[32]     =  {0};
int number_of_instruction = 0;
int number_of_data = 0;

// file for writing the outputs of the execution
ofstream res_file("result.txt");


// Required control signals
int MemRead = 1;
int ALUSrcA = 0;
int LoD = 0;
int IRWrite = 1;
int ALUSrcB[2] = {0,1};
int ALUOp[2] = {0,0};
int PCWrite = 1;
int PCSource[2] = {0,0};
int PCWriteCond = 1;
int MemWrite = 1;
int RegDst = 1;
int RegWrite = 1;
int MemtoReg = 0;

// functions for phase 1 - ALU operations
int and_func(int x, int y);
int or_func(int x,int y);
int one_bit_full_adder(int a,int b,int carryin,int& carryout);
int one_bit_alu(int a,int b,int carryin,int ainv,int binv,int less,int *op,int& carryout);
int one_bit_alu_of(int a,int b,int carryin,int ainv,int binv,int less,int *op,int& carryout,int& of);
int* three_two_bit_alu(int A[],int B[],int ainv,int binv,int *op,int& of,int& zero);

// functions for Initialization of the program
void load_instruction();
void register_initialization();

//function for MISP pipeline execution
int* mux_2_1(int X[],int Y[],int control);
int* mux_4_1(int X1[],int X2[],int X3[],int X4[],int control[]);
void instruction_fetch();
void instruction_decode();
void instruction_execution();
void instruction_memory();
void instruction_writeBack();
void display_register_contents(int clock_counter);
/*
 * Function Description: - calls other functions for
 *						   1. initializing the register content
 *						   2. initializing the instruction and data content
 *						   3. performing R-type, LOAD-Store and Branch type instruction in proper order of the MISP pipeline (i.e., IF->ID->EX->Mem->WB) based on the Finite State Machine
 *						   4. displaying the contents on the console and storing in file.
 *						   maintains the clock counter for the instructions
 */
int main(int argc, const char * argv[]) {
    // insert code here...
    // cout << "Hello, World!\n";
    /********************Test Begin Phase 1**********************/
	/*
    int A[32],B[32],zero = 0;
    int *result;
    int op[2] = {1,0};
    int ainv = 0;
    int binv = 1;
    int of = 0;
    // temp run start
    for(int i=0;i<32;i++){
        A[i] = 0;
        B[i] = 0;
    }
    B[0] = 0;
    A[0] = 1;
    result = three_two_bit_alu(A, B, ainv, binv, op, of, zero);
    cout<<"A:"<<endl;
    for(int i=0;i<32;i++){
        cout<<A[i];
    }
    cout<<endl;
    cout<<"B:"<<endl;
    for(int i=0;i<32;i++){
        cout<<B[i];
    }
    cout<<endl;
    cout<<"Result:"<<endl;
    for(int i=0;i<32;i++){
        cout<<result[i];
    }
    cout<<endl<<endl<<endl;
	*/
    /********************Test Ends Phase 1***********************/
	int Opcode = 0;
	// set clock to 0
	int clock_counter = 0;
	// load the instruction from the external file to the instruction memory and data to the data memory
	load_instruction();
	// initialize the register content before the begning of the execution
	register_initialization();
	cout<<endl<<endl<<endl;
	res_file<<endl<<endl<<endl;
	// Display the register file content at the start of the execution clock cycle 0
	cout<<"Register contents"<<endl;
	res_file<<"Register contents"<<endl;
	for(int i=0;i<32;i++){
		cout<<"Reg Address :-" <<i<<"  register content: - ";
		res_file<<"Reg Address :-" <<i<<"  register content: - ";
		for(int j=31;j>=0;j--){
			cout<<register_file[i][j]<<" ";
			res_file<<register_file[i][j]<<" ";
		}
		cout<<endl;
		res_file<<endl;
	}
	// Display the instruction memory content at the start of the execution clock cycle 0
	cout<<"Instruction in memory"<<endl;
	res_file<<"Instruction in memory"<<endl;
	for(int i=0;i<number_of_instruction*4;i++){
		cout<<"Mem Address :-" <<i<<"  Mem content: - ";
		res_file<<"Mem Address :-" <<i<<"  Mem content: - ";
		for(int j=0;j<8;j++){
			cout<<memory[i][j]<<" ";
			res_file<<memory[i][j]<<" ";
		}
		cout<<endl;
		res_file<<endl;
	}
	cout<<endl<<endl<<endl;
	res_file<<endl<<endl<<endl;
	// Display the data memory content at the start of the execution clock cycle 0
	cout<<"Data in memory"<<endl;
	res_file<<"Data in memory"<<endl;
	for(int i=100;i<100 + number_of_data * 4;i++){
		cout<<"Data Address :-" <<i<<"  Data content: - ";
		res_file<<"Data Address :-" <<i<<"  Data content: - ";
		for(int j=0;j<8;j++){
			cout<<memory[i][j]<<" ";
			res_file<<memory[i][j]<<" ";
		}
		cout<<endl;
		res_file<<endl;
	}
	// Display the register content at the start of the execution clock cycle 0
	display_register_contents(clock_counter);
	while(Opcode != 63){
		// fetch the instruction and calculate the opcode
		instruction_fetch();
		for(int i=31;i>=26;i--){
			Opcode = Opcode << 1 | IR[i];
		}
		// end of instruction
		if(Opcode == 63){
			break;
		}// if R type instruction
		else if(Opcode == 0 || Opcode == 43){
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_decode();
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_execution();
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_memory();
			clock_counter+=1;
			display_register_contents(clock_counter);
		}// if LOAD instruction
		else if(Opcode == 35){
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_decode();
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_execution();
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_memory();
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_writeBack();
			clock_counter+=1;
			display_register_contents(clock_counter);
		}// if BEQ, BNE, JUMP instruction
		else if(Opcode == 4 || Opcode == 5 || Opcode == 2){
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_decode();
			clock_counter+=1;
			display_register_contents(clock_counter);
			instruction_execution();
			clock_counter+=1;
			display_register_contents(clock_counter);
		}
		Opcode = 0;
	}
	cout<<endl<<endl<<endl;
	res_file<<endl<<endl<<endl;
	// Display the register file content after the completion of all the instructions
	cout<<"Register contents"<<endl;
	res_file<<"Register contents"<<endl;
	for(int i=0;i<32;i++){
		cout<<"Reg Address :-" <<i<<"  register content: - ";
		res_file<<"Reg Address :-" <<i<<"  register content: - ";
		for(int j=31;j>=0;j--){
			cout<<register_file[i][j]<<" ";
			res_file<<register_file[i][j]<<" ";
		}
		cout<<endl;
		res_file<<endl;
	}
	// Display the instruction memory content after the completion of all the instructions
	cout<<"Instruction in memory"<<endl;
	res_file<<"Instruction in memory"<<endl;
	for(int i=0;i<number_of_instruction*4;i++){
		cout<<"Mem Address :-" <<i<<"  Mem content: - ";
		res_file<<"Mem Address :-" <<i<<"  Mem content: - ";
		for(int j=0;j<8;j++){
			cout<<memory[i][j]<<" ";
			res_file<<memory[i][j]<<" ";
		}
		cout<<endl;
		res_file<<endl;
	}
	cout<<endl<<endl<<endl;
	res_file<<endl<<endl<<endl;
	// Display the data memory content after the completion of all the instructions
	cout<<"Data in memory"<<endl;
	res_file<<"Data in memory"<<endl;
	for(int i=100;i<100 + number_of_data * 4;i++){
		cout<<"Data Address :-" <<i<<"  Data content: - ";
		res_file<<"Data Address :-" <<i<<"  Data content: - ";
		for(int j=0;j<8;j++){
			cout<<memory[i][j]<<" ";
			res_file<<memory[i][j]<<" ";
		}
		cout<<endl;
		res_file<<endl;
	}
	return 0;
	 
}
/*********************************************************Code for Phase 2*********************************************/

/*
 * Function Description: - function will display the content of the PC, IR, ALU_OUT, A, B, MDR register on the console / and write the output on the result file
 * Inputs: -
 * Output: -
 */
void display_register_contents(int clock_counter){
	cout<<"*************************** clock cycle: - >"<<clock_counter<<" ****************************"<<endl;
	res_file<<"*************************** clock cycle: - >"<<clock_counter<<" ****************************"<<endl;
	cout<<"PC"<<endl;
	res_file<<"PC"<<endl;
	for(int i=31;i>=0;i--){
		cout<<PC[i]<<" ";
		res_file<<PC[i]<<" ";
	}
	cout<<endl;
	res_file<<endl;
	cout<<"IR"<<endl;
	res_file<<"IR"<<endl;
	for(int i=31;i>=0;i--){
		cout<<IR[i]<<" ";
		res_file<<IR[i]<<" ";
	}
	cout<<endl;
	res_file<<endl;
	cout<<"ALU_OUT"<<endl;
	res_file<<"ALU_OUT"<<endl;
	for(int i=31;i>=0;i--){
		cout<<ALU_OUT[i]<<" ";
		res_file<<ALU_OUT[i]<<" ";
	}
	cout<<endl;
	res_file<<endl;
	cout<<"A"<<endl;
	res_file<<"A"<<endl;
	for(int i=31;i>=0;i--){
		cout<<A[i]<<" ";
		res_file<<A[i]<<" ";
	}
	cout<<endl;
	res_file<<endl;
	cout<<"B"<<endl;
	res_file<<"B"<<endl;
	for(int i=31;i>=0;i--){
		cout<<B[i]<<" ";
		res_file<<B[i]<<" ";
	}
	cout<<endl;
	res_file<<endl;
	cout<<"MDR"<<endl;
	res_file<<"MDR"<<endl;
	for(int i=31;i>=0;i--){
		cout<<MDR[31-i]<<" ";
		res_file<<MDR[31-i]<<" ";
	}
	cout<<endl;
	res_file<<endl;
	cout<<"***************************************************************************"<<endl;
	res_file<<"***************************************************************************"<<endl;
}

/*
 * Function Description: - function will implement 2*1 multiplexer
 * Inputs: - [1-2]. 32-bit binary input arrays X,Y
 *				 3. 1-bit control signal for performing 4*1 mux operation
 * Output: - 32-bit binary input array
 */
int* mux_2_1(int X[],int Y[],int control){
	int *result;
	if(control == 0){
		result = X;
	}else{
		result = Y;
	}
	return result;
}

/*
 * Function Description: - function will implement 4*1 multiplexer
 * Inputs: - [1-4]. 32-bit binary input arrays X1,X2,X3,X4
 *				 5. 2-bit control signal for performing 4*1 mux operation
 * Output: - 32-bit binary input array
 */
int* mux_4_1(int X1[],int X2[],int X3[],int X4[],int *control){
	int *result;
	if(control[0] == 0 && control[1] == 0){
		result = X1;
	}else if(control[0] == 0 && control[1] == 1){
		result = X2;
	}else if(control[0] == 1 && control[1] == 0){
		result = X3;
	}else{
		result = X4;
	}
	return result;
}

/*
 * Function Description: - function will perfrom Fetch operation (ALL supported instructions) (takes 1 clock cycle for perfroming the operation)
 * Inputs: -
 * Output: -
 */
void instruction_fetch(){
	int *adder,dec_Address = 0,count = 0;
	int *temp_A, *temp_B,*result;
	int sign_ext[32] = {0}, shft_2[32] = {0};
	bit_4[2] = 1;
	// setting the control signals
	MemRead = 1;
	ALUSrcA = 0;
	LoD = 0;
	IRWrite = 1;
	ALUSrcB[0] = 0;
	ALUSrcB[1] = 1;
	ALUOp[0] = 0;
	ALUOp[1] = 0;
	PCWrite = 1;
	PCSource[0] = 0;
	PCSource[1] = 0;
	// As ALUOp = {0,0}
	int ainv = 0,binv = 0, op[2] = {1,0}, of = 0, zero = 0;
	// perfrom operation fetch from the memory to IR register
	// IR = Mem[PC]
	adder = mux_2_1(PC, ALU_OUT, LoD);
	for(int i=31;i>=0;i--){
		dec_Address = dec_Address << 1 | adder[i];
	}
	for(int i = 31;i>=0;i--){
		if(count < 8){
			IR[i] = memory[dec_Address][count];
			count++;
		}else{
			count = 0;
			dec_Address = dec_Address + 1;
			IR[i] = memory[dec_Address][count];
			count++;
		}
	}
	// Perfrom operation
	// PC = PC + 4
	for(int i=0;i<=15;i++){
		sign_ext[i] = IR[i];
	}
	//shift left 2
	for(int i=0;i<30;i++){
		shft_2[i] = sign_ext[i+2];
	}
	shft_2[30] = 0;
	shft_2[31] = 0;
	temp_A = mux_2_1(PC, A, ALUSrcA);
	temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
	ainv = 0;
	binv = 0;
	op[0] = 1;
	op[1] = 0;
	result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
	// update the value of PC
	for(int i=31;i>=0;i--){
		PC[i] = result[i];
	}
}

/*
 * Function Description: - function will perfrom Decode operation (ALL supported instructions) (takes 1 clock cycle for perfroming the operation)
 * Inputs: -
 * Output: -
 */
void instruction_decode(){
	// setting the control signals
	ALUSrcA = 0;
	ALUSrcB[0] = 1;
	ALUSrcB[1] = 1;
	ALUOp[0] = 0;
	ALUOp[1] = 0;
	int ainv = 0,binv = 0, op[2] = {1,0}, of = 0, zero = 0;
	int *temp_A, *temp_B,*result;
	int sign_ext[32] = {0}, shft_2[32] = {0};
	int dec_A = 0, dec_B =0;
	//A = Reg[IR[25:21]]
	//B = Reg[IR[20:16]]
	// calculate the register address for A and B register values
	for(int i=25;i>=21;i--){
		dec_A = dec_A << 1 | IR[i];
	}
	for(int i=20;i>=16;i--){
		dec_B = dec_B << 1 | IR[i];
	}
	// update the value of A and B register
	for(int i=31;i>=0;i--){
		A[i] = register_file[dec_A][i];
		B[i] = register_file[dec_B][i];
	}
	// perform the operation
	// ALUOut = PC + (sign-extend(IR[15:0])<<2)
	for(int i=0;i<=15;i++){
		sign_ext[i] = IR[i];
	}
	//shift left 2
	for(int i=31;i>=2;i--){
		shft_2[i] = sign_ext[i-2];
	}
	shft_2[0] = 0;
	shft_2[1] = 0;
	temp_A = mux_2_1(PC, A, ALUSrcA);
	temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
	result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
	for(int i=0;i<32;i++){
		ALU_OUT[i] = result[i];
	}
}

/*
 * Function Description: - function will perfrom Execution operation (ALL supported instructions) (takes 1 clock cycle for perfroming the operation)
 * Assign the value of PC in BEQ/BNE/JUMP instructions.
 * Inputs: -
 * Output: -
 */
void instruction_execution(){
	int dec_OP = 0,dec_function_code = 0;
	int *temp_A,*temp_B,*result;
	int sign_ext[32] = {0}, shft_2[32] = {0};
	int ainv,binv, op[2], of = 0, zero = 0;
	int temp_zero[32] = {0},shft_PC[32] = {0},*temp_PC;
	// check the opcode of the instruction to determine the instruction type from the instruction in IR register
	for(int i=31;i>=26;i--){
		dec_OP = dec_OP << 1 | IR[i];
	}
	// perfrom sign extension of the first 16 bits from the instruction in IR register
	for(int i=0;i<=15;i++){
		sign_ext[i] = IR[i];
	}
	// perfrom shift left 2 bit on sign extension of the first 16 bits from the instruction in IR register
	for(int i=31;i>=2;i--){
		shft_2[i] = sign_ext[i-2];
	}
	shft_2[0] = 0;
	shft_2[1] = 0;
	switch (dec_OP){
		case 0:
			// R - Type instructions
			ALUSrcA = 1;
			ALUSrcB[0] = 0;
			ALUSrcB[1] = 0;
			ALUOp[0] = 1;
			ALUOp[1] = 0;
			// type of R- type instruction using functional code
			for(int i=5;i>=0;i--){
				dec_function_code = dec_function_code << 1 | IR[i];
			}
			switch (dec_function_code) {
				case 32:
					// add operation
					temp_A = mux_2_1(PC, A, ALUSrcA);
					temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
					// Alu controls for addition operations
					ainv = 0;
					binv = 0;
					op[0] = 1;
					op[1] = 0;
					// call 32-bit ALU for performing addition operations
					result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
					for(int i=0;i<32;i++){
						ALU_OUT[i] = result[i];
					}
					break;
				case 34:
					// sub operations
					temp_A = mux_2_1(PC, A, ALUSrcA);
					temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
					// ALU controls for substraction operations
					ainv = 0;
					binv = 1;
					op[0] = 1;
					op[1] = 0;
					// call 32-bit ALU for performing substraction operations
					result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
					for(int i=0;i<32;i++){
						ALU_OUT[i] = result[i];
					}
					break;
				case 36:
					//and operations
					temp_A = mux_2_1(PC, A, ALUSrcA);
					temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
					// ALU controls for and operations
					ainv = 0;
					binv = 0;
					op[0] = 0;
					op[1] = 0;
					// call 32-bit ALU for performing and operations
					result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
					for(int i=0;i<32;i++){
						ALU_OUT[i] = result[i];
					}
					break;
				case 37:
					//or operations
					temp_A = mux_2_1(PC, A, ALUSrcA);
					temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
					// ALU controls for or operations
					ainv = 0;
					binv = 0;
					op[0] = 0;
					op[1] = 1;
					// call 32-bit ALU for performing or operations
					result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
					for(int i=0;i<32;i++){
						ALU_OUT[i] = result[i];
					}
					break;
				case 39:
					//nor operations
					temp_A = mux_2_1(PC, A, ALUSrcA);
					temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
					// ALU controls for nor operations
					ainv = 1;
					binv = 1;
					op[0] = 0;
					op[1] = 0;
					// call 32-bit ALU for performing nor operations
					result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
					for(int i=0;i<32;i++){
						ALU_OUT[i] = result[i];
					}
					break;
				case 42:
					//slt
					temp_A = mux_2_1(PC, A, ALUSrcA);
					temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
					// ALU controls for slt operations
					ainv = 0;
					binv = 1;
					op[0] = 1;
					op[1] = 1;
					// call 32-bit ALU for performing slt operations
					result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
					for(int i=0;i<32;i++){
						ALU_OUT[i] = result[i];
					}
					break;
			}
			break;
		case 35:
			//load operations
			ALUSrcA = 1;
			ALUSrcB[0] = 1;
			ALUSrcB[1] = 0;
			ALUOp[0] = 0;
			ALUOp[1] = 0;
			temp_A = mux_2_1(PC, A, ALUSrcA);
			temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
			// ALU controls for addition operations
			ainv = 0;
			binv = 0;
			op[0] = 1;
			op[1] = 0;
			// call 32-bit ALU for performing addition operations
			result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
			for(int i=0;i<32;i++){
				ALU_OUT[i] = result[i];
			}
			break;
		case 43:
			//store operations
			ALUSrcA = 1;
			ALUSrcB[0] = 1;
			ALUSrcB[1] = 0;
			ALUOp[0] = 0;
			ALUOp[1] = 0;
			temp_A = mux_2_1(PC, A, ALUSrcA);
			temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
			// ALU controls for addition operations
			ainv = 0;
			binv = 0;
			op[0] = 1;
			op[1] = 0;
			// call 32-bit ALU for performing addition operations
			result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
			for(int i=0;i<32;i++){
				ALU_OUT[i] = result[i];
			}
			break;
		case 4:
			//beq operation
			ALUSrcA = 1;
			ALUSrcB[0] = 0;
			ALUSrcB[1] = 0;
			ALUOp[0] = 0;
			ALUOp[1] = 1;
			PCWriteCond = 1;
			PCSource[0] = 0;
			PCSource[1] = 1;
			temp_A = mux_2_1(PC, A, ALUSrcA);
			temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
			// ALU controls for addition operations
			ainv = 0;
			binv = 1;
			op[0] = 1;
			op[1] = 0;
			of = 0;
			zero = 0;
			// call 32-bit ALU for performing addition operations
			result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
			// perfrom shift IR operation for calculating PC
			for(int i=0;i<=25;i++){
				shft_PC[i] = IR[i];
			}
			for(int i=2;i<28;i++){
				shft_PC[i] = shft_PC[i-2];
			}
			shft_PC[0] = 0;
			shft_PC[1] = 0;
			for(int i = 28;i<32;i++){
				shft_PC[i] = PC[i];
			}
			temp_PC = mux_4_1(result, ALU_OUT, shft_PC, temp_zero, PCSource);
			// Update the PC if the value of A and B register are equal
			for(int i=0;i<32;i++){
				if(result[i] != temp_zero[i]){
					PCWriteCond = 0;
					break;
				}
			}
			if(PCWriteCond == 1){
				for(int i=0;i<32;i++){
					PC[i] = temp_PC[i];
				}
			}
			break;
		case 5:
			//bne
			ALUSrcA = 1;
			ALUSrcB[0] = 0;
			ALUSrcB[1] = 0;
			ALUOp[0] = 0;
			ALUOp[1] = 1;
			PCWriteCond = 1;
			PCSource[0] = 0;
			PCSource[1] = 1;
			temp_A = mux_2_1(PC, A, ALUSrcA);
			temp_B = mux_4_1(B, bit_4, sign_ext, shft_2, ALUSrcB);
			// ALU controls for addition operations
			ainv = 0;
			binv = 1;
			op[0] = 1;
			op[1] = 0;
			of = 0;
			zero = 0;
			// call 32-bit ALU for performing addition operations
			result = three_two_bit_alu(temp_A, temp_B, ainv, binv, op, of, zero);
			// perfrom shift IR operation for calculating PC
			for(int i=0;i<=25;i++){
				shft_PC[i] = IR[i];
			}
			for(int i=2;i<28;i++){
				shft_PC[i] = shft_PC[i-2];
			}
			shft_PC[0] = 0;
			shft_PC[1] = 0;
			for(int i = 28;i<32;i++){
				shft_PC[i] = PC[i];
			}
			temp_PC = mux_4_1(result, ALU_OUT, shft_PC, temp_zero, PCSource);
			// Update the PC if the value of A and B register are equal
			for(int i=0;i<32;i++){
				if(result[i] == temp_zero[i]){
					PCWriteCond = 0;
					break;
				}
			}
			if(PCWriteCond == 1){
				for(int i=0;i<32;i++){
					PC[i] = temp_PC[i];
				}
			}
			break;
		case 2:
			//jump operation
			PCWrite = 1;
			PCSource[0] = 1;
			PCSource[1] = 0;
			// perfrom shift IR operation for calculating PC
			for(int i=0;i<32;i++){
				shft_PC[i] = 0;
			}
			for(int i=0;i<=25;i++){
				shft_PC[i] = IR[i];
			}
			for(int i=27;i>=2;i--){
				shft_PC[i] = shft_PC[i-2];
			}
			shft_PC[0] = 0;
			shft_PC[1] = 0;
			for(int i = 28;i<32;i++){
				shft_PC[i] = PC[i];
			}
			temp_PC = mux_4_1(temp_zero, ALU_OUT, shft_PC, temp_zero, PCSource);
			// checking conditions for jump operations and performing jump operations
			if((zero and PCWriteCond) or PCWrite){
				for(int i=0;i<32;i++){
					PC[i] = temp_PC[i];
				}
			}
			break;
	}
}

/*
 * Function Description: - function will perfrom memory-access operation (LOAD/STORE instructions) (takes 1 clock cycle for perfroming the operation)
 *	Write-Back operation for R - type instructions
 * Inputs: -
 * Output: -
 */
void instruction_memory(){
	int dec_OP = 0,dec = 0;
	int *temp_Write_Back,*temp_Alu_Out;
	for(int i=31;i>=26;i--){
		dec_OP = dec_OP << 1 | IR[i];
	}
	switch (dec_OP) {
		case 0:
			// R - Type instruction
			RegDst = 1;
			RegWrite = 1;
			MemtoReg = 0;
			dec = 0;
			// calculate the destination address of the register where the information is to be stored
			for(int i=15;i>=11;i--){
				dec = dec << 1 | IR[i];
			}
			temp_Write_Back = mux_2_1(ALU_OUT, MDR, MemtoReg);
			// load the content from ALU_Out register to the memory
			for(int i=0;i<32;i++){
				register_file[dec][i] = temp_Write_Back[i];
			}
			break;
			
		case 35:
			//Load instruction memory access
			MemRead = 1;
			LoD = 1;
			dec = 0;
			temp_Alu_Out = mux_2_1(PC, ALU_OUT, LoD);
			// calculate the memory address from where the load operation is to be perfromed.
			for(int i=31;i>=0;i--){
				dec = dec << 1 | temp_Alu_Out[i];
			}
			// Load the content from the memory to the register using the calculated value of the address
			for(int i=0;i<32;i++){
				MDR[i] = memory[dec][i];
			}
			break;
		case 43:
			//Store instruction memory access
			MemWrite = 1;
			LoD = 1;
			dec = 0;
			temp_Alu_Out = mux_2_1(PC, ALU_OUT, LoD);
			// calculate the memory address from where the store operation is to be perfromed.
			for(int i=31;i>=0;i--){
				dec = dec << 1 | temp_Alu_Out[i];
			}
			// Store the content from the register B to the Memory using the calculated value of the address
			for(int i=0;i<32;i++){
				memory[dec][31-i] = B[i];
			}
			break;
	}
}

/*
 * Function Description: - function will perfrom write-back operation (LOAD instruction) (takes 1 clock cycle for perfroming the operation)
 * Inputs: -
 * Output: - write the value of MDR register to the register (destination register address)
 */
void instruction_writeBack(){
	int *temp,dec = 0;
	RegDst = 0;
	RegWrite = 1;
	MemtoReg = 1;
	temp = mux_2_1(ALU_OUT, MDR, MemtoReg);
	// calculate the destination register address
	for(int i=20;i>=16;i--){
		dec = dec << 1 | IR[i];
	}
	// perform write back operation
	for(int i=0;i<32;i++){
		register_file[dec][i] = MDR[31-i];
	}
}

/*
 * Function Description: - function will load the register file of (32 - bit per address)
 * Inputs: -
 * Output: - assign the $gp register value to 100 in binary.
 */
void register_initialization(){
    // storing 100 at $gp
    int temp_gp[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0};
    for(int i = 0;i<32;i++){
        register_file[28][i] = temp_gp[31-i];
    }
}

/*
 * Function Description: - function will load the instruction in the Big - endian Mode from the file to the Instruction memory and data to the Data Memory.(8 - bit per address)
 * Inputs: -
 * Output: - loading of the instruction is successfull or file open/reading operation fails.
 */
void load_instruction(){
    string line;
	
    int inst_counter = 0,inst_len = 0;
    int data_counter =0,data_len = 0;
    vector<int> temp;
	// read the file containing the instruction and data in binary
    ifstream machine_code("input.txt");
    if(machine_code.is_open()){
        getline(machine_code, line);
        istringstream iss(line);
        string s;
		// check the number of instruction and data lines present in the file
        while(iss >> s){
            temp.push_back(stoi(s));
        }
        number_of_instruction = temp.at(0);
        number_of_data = temp.at(1);
		
		// load instruction to the memory in Big-endian mode - memory address 0 to 99
        while(inst_counter<number_of_instruction){
            getline(machine_code, line);
            inst_len = 0;
            for(int i=0+inst_counter*4;i<100;i++){
                for(int j=0;j<8;j++){
                    if(inst_len<32){
                        memory[i][j] = int(line[inst_len]) - (int)48;
                        inst_len++;
                    }
                }
            }
            inst_counter++;
        }
        // load data to the memory in Big-endian mode - memory address 100 to 199
        while(data_counter<number_of_data){
            getline(machine_code, line);
            data_len = 0;
            for(int i=100+data_counter*4;i<200;i++){
                for(int j=0;j<8;j++){
                    if(data_len<32){
                        memory[i][j] = int(line[data_len]) - (int)48;
                        data_len++;
                    }
                }
            }
            data_counter++;
        }
    }else{
        cout<<"file does not open"<<endl;
    }
}

/*********************************************************Code for Phase 1*********************************************/
/*
 * Function Description: - 1 bit and operation of two one-bit numbers
 * Inputs: - 1. 1 - bit number a
 *			 2. 1 - bit number b
 * Output: - 1. 1 - bit number result containing output of the and operation on two 1 - bit number a and b
 */
int and_func(int x,int y){
    if(x == 0 || y == 0){
        return 0;
    }else{
        return 1;
    }
    return -1;
}
/*
 * Function Description: - 1 bit or operation of two one-bit numbers
 * Inputs: - 1. 1 - bit number a
 *			 2. 1 - bit number b
 * Output: - 1. 1 - bit number result containing output of the or operation on two 1 - bit number a and b
 */
int or_func(int x,int y){
    if(x== 0 && y == 0){
        return 0;
    }else{
        return 1;
    }
    return -1;
}
/*
 * Function Description: - 1 bit full adder performing addition operation of two one-bit numbers
 * Inputs: - 1. 1 - bit number a
 *			 2. 1 - bit number b
 *			 3. 1 - bit carry - in signal
 * Output: - 1. 1 - bit number result containing output of the addition operation on two 1 - bit number a and b
 * 			 2. 1 - carryout signal
 */
int one_bit_full_adder(int a,int b,int carryin,int& carryout){
    if(a==0 && b == 0 && carryin == 0){
        carryout = 0;
        return 0;
    }else if(a==0 && b == 0 && carryin == 1){
        carryout = 0;
        return 1;
    }else if(a==0 && b == 1 && carryin == 0){
        carryout = 0;
        return 1;
    }else if(a==0 && b == 1 && carryin == 1){
        carryout = 1;
        return 0;
    }else if(a==1 && b == 0 && carryin == 0){
        carryout = 0;
        return 1;
    }else if(a==1 && b == 0 && carryin == 1){
        carryout = 1;
        return 0;
    }else if(a==1 && b == 1 && carryin == 0){
        carryout = 1;
        return 0;
    }else if(a==1 && b == 1 && carryin == 1){
        carryout = 1;
        return 1;
    }
    return -1;
}
/*
 * Function Description: - 1 bit ALU implementation for LSB to MSB - 1 bit
 * Inputs: - 1. 1 - bit number a
 *			 2. 1 - bit number b
 *			 3. 1 - bit carry - in signal
 *			 4. 1 - bit control signal ainv
 * 			 5. 1 - bit control signal binv
 *			 6. 1 - bit number for slt operation (less)
 * 			 7. 2 - bit control signal op
 * Output: - 1. 1 - bit number result containing output of the ALU operation on two 1 - bit number a and b
 * 			 2. 1 - carryout signal
 */
int one_bit_alu(int a,int b,int carryin,int ainv,int binv,int less, int *op,int& carryout){
    // 2 * 1 mulitplexer for A
    if(ainv == 0){
        a = a;
    }else{
        if(a == 0){
            a = 1;
        }else{
            a = 0;
        }
    }
    // 2 * 1 mulitplexer for B
    if(binv == 0){
        b = b;
    }else{
        if(b == 0){
            b = 1;
        }else{
            b = 0;
        }
    }
    // 4 * 1 multiplexer for ALU operation
    if(op[0] == 0 && op[1] == 0){
        return and_func(a, b);
    }else if(op[0] == 0 && op[1] == 1){
        return or_func(a, b);
    }else if(op[0] == 1 && op[1] == 0){
        return one_bit_full_adder(a, b, carryin, carryout);
        //slt operation
    }else if(op[0] == 1 && op[1] == 1){
        return one_bit_full_adder(a, b, carryin, carryout);
    }
    return -1;
}
/*
 * Function Description: - 1 bit ALU implementation for most significant bit and check the occurance of the overflow
 * Inputs: - 1. 1 - bit number a
 *			 2. 1 - bit number b
 *			 3. 1 - bit carry - in signal
 *			 4. 1 - bit control signal ainv
 * 			 5. 1 - bit control signal binv
 *			 6. 1 - bit number for slt operation (less)
 * 			 7. 2 - bit control signal op
 * Output: - 1. 1 - bit number result containing output of the ALU operation on two 1 - bit number a and b
 * 			 2. 1 - carryout signal
 * 			 3. 1 - bit offset bit = 1 if the output is zero else = 0
 */
int one_bit_alu_of(int a,int b,int carryin,int ainv,int binv,int less,int *op,int& carryout,int& of){
    // 2 * 1 mulitplexer for A
	// if ainv = 1 then inverse a bit
    if(ainv == 0){
        a = a;
    }else{
        if(a == 0){
            a = 1;
        }else{
            a = 0;
        }
    }
    // 2 * 1 mulitplexer for B
	// if binv == 1 then inverse b bit
    if(binv == 0){
        b = b;
    }else{
        if(b == 0){
            b = 1;
        }else{
            b = 0;
        }
    }
    // 4 * 1 multiplexer for ALU operation
	// op = 00 perfrom and operation
    if(op[0] == 0 && op[1] == 0){
        return and_func(a, b);
    }//op == 00
	else if(op[0] == 0 && op[1] == 1){
        return or_func(a, b);
    }else if(op[0] == 1 && op[1] == 0){
        int temp =  one_bit_full_adder(a, b, carryin, carryout);
        //check overflow for addition operation
        // http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
        if(carryout != carryin){
            of = 1;
            cout<<"Overflow occur \'Overflow bit :-\' "<< of <<endl;
        }
        return temp;
        //slt operation
    }else if(op[0] == 1 && op[1] == 1){
        return one_bit_full_adder(a, b, carryin, carryout);
    }
    return -1;
}
/*
 * Function Description: - 32 bit ALU implementation, calls 1 - bit ALU 32 times for performing slt, add, sub, and, or, nor operations.
 * Inputs: - 1. 32 - bit number A
 *			 2. 32 - bit number B
 *			 3. 1 - bit control signal ainv
 * 			 4. 1 - bit control signal binv
 * 			 5. 2 - bit control signal op
 * Output: - 1. 32 - bit number result containing output of the ALU operation on two 32 - bit number A and B
 * 			 2. 1 - offset bit = 1 if the offset occurs else = 0
 * 			 3. 1 - bit zero bit = 1 if the output is zero else = 0
 */
int* three_two_bit_alu(int A[],int B[],int ainv,int binv,int *op,int& of,int& zero){
    int carryin = binv;
    of = 0;
    int carryout = 0;
    static int result[32] = {0};
    int less = 0;
	// call one - bit ALU 32 times
	for(int i = 0; i<32; i++){
		// for the 0th bit carry - in for ALU one bit  = binv
        if(i == 0){
            carryin = binv;
            result[i] =  one_bit_alu(A[i], B[i], carryin, ainv, binv, less, op, carryout);
        }// ALU operation perfromed on 31st bit for last bit
		else if(i == 31){
            carryin = carryout;
            result[i] =  one_bit_alu_of(A[i], B[i], carryin, ainv, binv, less, op, carryout, of);
            //handle slt operation
            if(op[0] == 1 && op[1] == 1){
                if(result[i] == 0){
                    for(int j = 0;j<32;j++){
                        result[j] = 0;
                    }
                }else{
                    result[0] = 1;
                    for(int j = 1;j<32;j++){
                        result[j] = 0;
                    }
                }
				return result;
            }
        }// ALU operation for all the other than MSB and LSB
		else{
            carryin = carryout;
            result[i] =  one_bit_alu(A[i], B[i], carryin, ainv, binv, less, op, carryout);
        }
    }
    return result;
}
