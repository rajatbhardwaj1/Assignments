/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 *
 */

#ifndef __MIPS_PROCESSOR_HPP__
#define __MIPS_PROCESSOR_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <math.h>
#include <set>
using namespace std;

struct IF1
{
	int nop;
	int PC;
	bool jump;
	bool branch;
	bool take_branch;
	int new_branch;
};
struct IF2
{
	int nop;
	int PC;
	bool jump;
	bool branch;
	bool take_branch;
	int new_branch;
};

struct ID1
{
	int nop;
	int saved_PC;
	std::vector<std::string> Ins;
};

struct ID2
{
	int saved_PC;
	int nop;
	string operation;
	string reg1;
	string reg2;
	int number_found;
	string destination;
	bool write_reg;
	bool use_mem;
	bool read_mem;
	bool write_mem;
};

struct RR
{
	int saved_PC;
	int nop;
	string operation;
	int value1;
	int value2;
	string goto_addr;
	string destination;
	int mem_write_value;
	bool write_reg;
	bool use_mem;
	bool read_mem;
	bool write_mem;
};

struct ALU
{
	int nop;
	string destination;
	int mem_write_value;
	int exe_result_op;
	bool write_mem;
	bool read_mem;
	bool write_reg;
	bool use_mem;
};

struct MEM_Write
{
	int nop;
	string destination;
	int exe_result_op;
	int mem_write_value;
	bool use_mem;
	bool write_mem;
	bool write_reg;
	bool read_mem;
};

struct MEM_Read
{
	int nop;
	string destination;
	int exe_result_op;
	int mem_result_op;
	bool use_mem;
	bool write_reg;
	bool read_mem;
};

struct WB
{
	int nop;
	int value_to_write;
	string destination;
	bool write_reg;
	bool fetchfrom_alu;
};

struct state
{
	IF1 ins_fetch1;
	IF2 ins_fetch2;
	ID1 ins_dec1;
	ID2 ins_dec2;
	RR ins_read_reg;
	ALU ins_alu;
	MEM_Read ins_mem_read;
	MEM_Write ins_mem_write;
	WB ins_wb;
};

struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr = 0, PCnext;
	std::unordered_map<std::string, std::function<int(MIPS_Architecture &, int, int, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
	std::unordered_map<std::string, int> reg_lock;
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
	std::unordered_map<int, int> memoryDelta;
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;
	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};
	set<string> r_type = {"add", "sub", "mul", "slt"};
	set<string> i_type = {"addi", "subi", "muli", "sll", "slr"};
	set<string> sw_set = {"sw"};
	set<string> lw_set = {"lw"};
	set<string> branch = {"beq", "bne"};
	set<string> jump = {"j"};

	// constructor to initialise the instruction set
	MIPS_Architecture(std::ifstream &file)
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}, {"sll", &MIPS_Architecture::sll}, {"slr", &MIPS_Architecture::slr}};

		for (int i = 0; i < 32; ++i)
			registerMap["$" + std::to_string(i)] = i;
		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + std::to_string(i)] = i + 4;
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + std::to_string(i)] = i + 8, registerMap["$s" + std::to_string(i)] = i + 16;
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;

		for (int i = 0; i < 32; ++i)
			reg_lock["$" + std::to_string(i)] = 0;
		reg_lock["$zero"] = 0;
		reg_lock["$at"] = 0;
		reg_lock["$v0"] = 0;
		reg_lock["$v1"] = 0;
		for (int i = 0; i < 4; ++i)
			reg_lock["$a" + std::to_string(i)] = 0;
		for (int i = 0; i < 8; ++i)
			reg_lock["$t" + std::to_string(i)] = 0, reg_lock["$s" + std::to_string(i)] = 0;
		reg_lock["$t8"] = 0;
		reg_lock["$t9"] = 0;
		reg_lock["$k0"] = 0;
		reg_lock["$k1"] = 0;
		reg_lock["$gp"] = 0;
		reg_lock["$sp"] = 0;
		reg_lock["$s8"] = 0;
		reg_lock["$ra"] = 0;

		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

	// perform add operation
	int add(int r1, int r2, std::string unused = "")
	{
		return r1 + r2;
		// return op(r1, r2, r3, [&](int a, int b)
		// 		  { return a + b; });
	}

	// perform subtraction operation
	int sub(int r1, int r2, std::string unused = "")
	{
		return r1 - r2;
		// return op(r1, r2, r3, [&](int a, int b)
		// 		  { return a - b; });
	}

	// perform multiplication operation
	int mul(int r1, int r2, std::string unused = "")
	{
		return r1 * r2;
	}

	// perform the binary operation
	// int op(std::string r1, std::string r2, std::string r3, std::function<int(int, int)> operation)
	// {
	// 	if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
	// 		return 1;
	// 	registers[registerMap[r1]] = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
	// 	PCnext = PCcurr + 1;
	// 	return 0;
	// }

	// perform the beq operation
	int beq(int r1, int r2, std::string label)
	{
		if (r1 == r2)
			return 1;
		return 0;
	}

	// perform the bne operation
	int bne(int r1, int r2, std::string label)
	{
		if (r1 != r2)
			return 1;
		return 0;
	}

	// implements beq and bne by taking the comparator
	int bOP(int r1, int r2, std::string label, std::function<bool(int, int)> comp)
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		return comp(r1, r2) ? address[label] : PCcurr + 1;
		;
	}

	// implements slt operation
	int slt(int r1, int r2, std::string unused = "")
	{
		return r1 < r2;
	}

	// perform the jump operation
	int j(int unused, int unused2, std::string label)
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		return address[label];
	}

	// perform load word operation
	int lw(int r1, int r2, std::string unused1 = "")
	{
		return (r1 + r2)/4;
	}
	int sw(int r1, int r2, std::string unused1 = "")
	{
		return (r1 + r2)/4;
	}

	// perform add immediate operation
	int addi(int r1, int r2, std::string unused)
	{
		return r1 + r2;
	}

	int sll(int r1, int r2, std::string unused)
	{

		return r1 * pow(2, r2);
	}

	int slr(int r1, int r2, std::string unused)
	{
		return r1 / pow(2, r2);
	}

	pair<string, string> extract(std::string reg_off)
	{
		if (reg_off.back() == ')')
		{
			int lparen = reg_off.find('(');
			string offset = (lparen == 0 ? "0" : reg_off.substr(0, lparen));
			std::string reg = reg_off.substr(lparen + 1);
			reg.pop_back();
			return {reg, offset};
		}
		return {reg_off, "0"};
	}
	// checks if label is valid
	inline bool checkLabel(std::string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c)
														   { return (bool)isalnum(c); }) &&
			   instructions.find(str) == instructions.end();
	}

	// checks if the register is a valid one
	inline bool checkRegister(std::string r)
	{
		return registerMap.find(r) != registerMap.end();
	}

	// checks if all of the registers are valid or not
	bool checkRegisters(std::vector<std::string> regs)
	{
		return std::all_of(regs.begin(), regs.end(), [&](std::string r)
						   { return checkRegister(r); });
	}

	/*
		handle all exit codes:
		0: correct execution
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(exit_code code, int cycleCount)
	{
		//std:://std::cout << '\n';
		switch (code)
		{
		case 1:
			std::cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			std::cerr << "Label used not defined or defined too many times\n";
			break;
		case 3:
			std::cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			std::cerr << "Syntax error encountered\n";
			break;
		case 5:
			std::cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			std::cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				std::cerr << s << ' ';
			std::cerr << '\n';
		}
	}

	// parse the command assuming correctly formatted MIPS instruction (or label)
	void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
		{
			std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command.clear();
		}
		else if (command[0].back() == ':')
		{
			std::string label = command[0].substr(0, command[0].size() - 1);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command = std::vector<std::string>(command.begin() + 1, command.end());
		}
		else if (command[0].find(':') != std::string::npos)
		{
			int idx = command[0].find(':');
			std::string label = command[0].substr(0, idx);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command[0] = command[0].substr(idx + 1);
		}
		else if (command[1][0] == ':')
		{
			if (address.find(command[0]) == address.end())
				address[command[0]] = commands.size();
			else
				address[command[0]] = -1;
			command[1] = command[1].substr(1);
			if (command[1] == "")
				command.erase(command.begin(), command.begin() + 2);
			else
				command.erase(command.begin(), command.begin() + 1);
		}
		if (command.empty())
			return;
		if (command.size() > 4)
			for (int i = 4; i < (int)command.size(); ++i)
				command[3] += " " + command[i];
		command.resize(4);
		commands.push_back(command);
	}

	// construct the commands vector from the input file
	void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	}

	// execute the commands sequentially (no pipelining)
	void executeCommandspipelined()
	{
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}

		int clockCycles = 0;
		state cur_state, new_state;
		// initialize ......
		// --------IF1----------
		cur_state.ins_fetch1.nop = 0;
		cur_state.ins_fetch1.PC = -1;
		cur_state.ins_fetch1.jump = 0;
		cur_state.ins_fetch1.take_branch = 0;
		cur_state.ins_fetch1.new_branch = 0;

		// -----------IF2----------
		cur_state.ins_fetch2.nop = 1;
		cur_state.ins_fetch2.PC = -1;
		cur_state.ins_fetch2.jump = 0;
		cur_state.ins_fetch2.take_branch = 0;
		cur_state.ins_fetch2.new_branch = 0;

		//------------ID1----------
		cur_state.ins_dec1.nop = 1;

		//------------ID2----------
		cur_state.ins_dec2.nop = 1;
		cur_state.ins_dec2.write_mem = 0;
		cur_state.ins_dec2.read_mem = 0;
		cur_state.ins_dec2.use_mem = 0;
		cur_state.ins_dec2.write_reg = 0;

		// ------------RR-----------
		cur_state.ins_read_reg.nop = 1;
		cur_state.ins_read_reg.value1 = 0;
		cur_state.ins_read_reg.value2 = 0;
		cur_state.ins_read_reg.write_mem = 0;
		cur_state.ins_read_reg.read_mem = 0;
		cur_state.ins_read_reg.use_mem = 0;
		cur_state.ins_read_reg.write_reg = 0;

		// -----------ALU ------------
		cur_state.ins_alu.nop = 1;
		cur_state.ins_alu.destination = "";
		cur_state.ins_alu.exe_result_op = 0;
		cur_state.ins_alu.write_mem = 0;
		cur_state.ins_alu.read_mem = 0;
		cur_state.ins_alu.use_mem = 0;
		cur_state.ins_alu.write_reg = 0;

		// ------------MEM_READ----------

		cur_state.ins_mem_read.nop = 1;
		cur_state.ins_mem_read.destination = "";
		cur_state.ins_mem_read.exe_result_op = 0;
		cur_state.ins_mem_read.mem_result_op = 0;
		cur_state.ins_mem_read.use_mem = 0;
		cur_state.ins_mem_read.write_reg = 0;

		// ------------MEM_WRITE----------

		cur_state.ins_mem_write.nop = 1;
		cur_state.ins_mem_write.destination = "";
		cur_state.ins_mem_write.exe_result_op = 0;
		cur_state.ins_mem_write.use_mem = 0;
		cur_state.ins_mem_write.write_reg = 0;
		cur_state.ins_mem_write.read_mem = 0;

		//------------WB---------------
		cur_state.ins_wb.nop = 1;
		cur_state.ins_wb.write_reg = "";
		cur_state.ins_wb.fetchfrom_alu = 0 ; 

		while (PCcurr < commands.size())
		{

			// --------initialize new_states--------

			// --------IF1----------
			new_state.ins_fetch1.nop = 0;
			new_state.ins_fetch1.PC = -1;
			new_state.ins_fetch1.jump = 0;
			new_state.ins_fetch1.take_branch = 0;
			new_state.ins_fetch1.new_branch = 0;

			// -----------IF2----------
			new_state.ins_fetch2.nop = 1;
			new_state.ins_fetch2.PC = -1;
			new_state.ins_fetch2.jump = 0;
			new_state.ins_fetch2.take_branch = 0;
			new_state.ins_fetch2.new_branch = 0;

			//------------ID1----------
			new_state.ins_dec1.nop = 1;

			//------------ID2----------
			new_state.ins_dec2.nop = 1;
			new_state.ins_dec2.write_mem = 0;
			new_state.ins_dec2.read_mem = 0;
			new_state.ins_dec2.use_mem = 0;
			new_state.ins_dec2.write_reg = 0;

			// ------------RR-----------
			new_state.ins_read_reg.nop = 1;
			new_state.ins_read_reg.value1 = 0;
			new_state.ins_read_reg.value2 = 0;
			new_state.ins_read_reg.write_mem = 0;
			new_state.ins_read_reg.read_mem = 0;
			new_state.ins_read_reg.use_mem = 0;
			new_state.ins_read_reg.write_reg = 0;

			// -----------ALU ------------
			new_state.ins_alu.nop = 1;
			new_state.ins_alu.destination = "";
			new_state.ins_alu.exe_result_op = 0;
			new_state.ins_alu.write_mem = 0;
			new_state.ins_alu.read_mem = 0;
			new_state.ins_alu.use_mem = 0;
			new_state.ins_alu.write_reg = 0;

			// ------------MEM_READ----------

			new_state.ins_mem_read.nop = 1;
			new_state.ins_mem_read.destination = "";
			new_state.ins_mem_read.exe_result_op = 0;
			new_state.ins_mem_read.mem_result_op = 0;
			new_state.ins_mem_read.use_mem = 0;
			new_state.ins_mem_read.write_reg = 0;

			// ------------MEM_WRITE---------

			new_state.ins_mem_write.nop = 1;
			new_state.ins_mem_write.destination = "";
			new_state.ins_mem_write.exe_result_op = 0;
			new_state.ins_mem_write.use_mem = 0;
			new_state.ins_mem_write.write_reg = 0;
			new_state.ins_mem_write.read_mem = 0;

			//------------WB---------------
			new_state.ins_wb.nop = 1;
			new_state.ins_wb.write_reg = "";
			new_state.ins_wb.fetchfrom_alu = 0;

			// if(clockCycles > 20) break;
			++clockCycles;

			//std::cout << "------------WB----------------\n";

			if (!cur_state.ins_wb.nop)
			{
				if (!cur_state.ins_wb.fetchfrom_alu)
				{
					if (!cur_state.ins_mem_read.write_reg)
					{
						//std::cout << "Nothing to write in Register file\n";
					}
					else
					{
						if (cur_state.ins_mem_read.write_reg && cur_state.ins_mem_read.use_mem)
						{
							registers[registerMap[cur_state.ins_mem_read.destination]] = cur_state.ins_mem_read.mem_result_op;
							reg_lock[cur_state.ins_mem_read.destination]--;
							//std::cout << "Writing value: " << cur_state.ins_mem_read.mem_result_op << " On register: " << cur_state.ins_mem_read.destination << '\n';
						}
						else
						{

							reg_lock[cur_state.ins_mem_read.destination]--;
							registers[registerMap[cur_state.ins_mem_read.destination]] = cur_state.ins_mem_read.exe_result_op;
							//std::cout << "Writing value: " << cur_state.ins_mem_read.exe_result_op << " On register: " << cur_state.ins_mem_read.destination << '\n';
						}
					}
				}
				else
				{
					if (!cur_state.ins_alu.write_reg)
					{
						//std::cout << "Nothing to write in Register file (ALU)\n";
					}
					else
					{
	

							reg_lock[cur_state.ins_alu.destination]--;
							registers[registerMap[cur_state.ins_alu.destination]] = cur_state.ins_alu.exe_result_op;
							//std::cout << "Writing value: " << cur_state.ins_alu.exe_result_op << " On register: " << cur_state.ins_alu.destination << '\n';
						
					}
					
				}
			}
			else
			{
				//std::cout << "WB is NOP\n";
			}
			new_state.ins_wb.nop = cur_state.ins_mem_read.nop;

			//std::cout << "-------------Read MEM----------\n";
			if (!cur_state.ins_mem_read.nop)
			{
				if (cur_state.ins_mem_write.read_mem)
				{
					new_state.ins_mem_read.mem_result_op = data[cur_state.ins_mem_write.exe_result_op];
					//std::cout << "reading value " << new_state.ins_mem_read.mem_result_op << " from memory " << cur_state.ins_mem_write.exe_result_op << "\n";
				}
				new_state.ins_mem_read.exe_result_op = cur_state.ins_mem_write.exe_result_op;
				new_state.ins_mem_read.use_mem = cur_state.ins_mem_write.use_mem;
				new_state.ins_mem_read.read_mem = cur_state.ins_mem_write.read_mem;
				new_state.ins_mem_read.write_reg = cur_state.ins_mem_write.write_reg;
				new_state.ins_mem_read.destination = cur_state.ins_mem_write.destination;
			}
			else
			{
				//std::cout << "MEM read is NOP\n";
			}

			new_state.ins_mem_read.nop = cur_state.ins_mem_write.nop;

			//std::cout << "-------------Write MEM----------\n";

			if (!cur_state.ins_mem_write.nop)
			{
				if (!cur_state.ins_alu.write_mem)
				{
					//std::cout << "Nothing to write in memory\n";
				}
				else
				{
					//std::cout << "Writing value : " << cur_state.ins_alu.mem_write_value << " On memory location: " << cur_state.ins_alu.exe_result_op << '\n';
					data[cur_state.ins_alu.exe_result_op] = cur_state.ins_alu.mem_write_value;
                    memoryDelta[cur_state.ins_alu.exe_result_op] = cur_state.ins_alu.mem_write_value;
					
				}
				new_state.ins_mem_write.exe_result_op = cur_state.ins_alu.exe_result_op;
				
				new_state.ins_mem_write.destination = cur_state.ins_alu.destination;
				new_state.ins_mem_write.mem_write_value = cur_state.ins_alu.mem_write_value;
				new_state.ins_mem_write.use_mem = cur_state.ins_alu.use_mem;
				new_state.ins_mem_write.write_reg = cur_state.ins_alu.write_reg;
				new_state.ins_mem_write.write_mem = cur_state.ins_alu.write_mem;
				new_state.ins_mem_write.read_mem = cur_state.ins_alu.read_mem;
			}
			else
			{
				//std::cout << "MEM write is NOP\n";
			}
			new_state.ins_mem_write.nop = cur_state.ins_alu.nop;

			//std::cout << "-------------ALU----------\n";
			if (!cur_state.ins_alu.nop)
			{
				//std::cout << "Operands: " << cur_state.ins_read_reg.value1 << " " << cur_state.ins_read_reg.value2 << '\n';
				//std::cout << "Operation: " << cur_state.ins_read_reg.operation << '\n';
				new_state.ins_alu.exe_result_op = instructions[cur_state.ins_read_reg.operation](*this, cur_state.ins_read_reg.value1, cur_state.ins_read_reg.value2, cur_state.ins_read_reg.goto_addr);
				//std::cout << "Result: " << new_state.ins_alu.exe_result_op << '\n';
				new_state.ins_alu.write_mem = cur_state.ins_read_reg.write_mem;
				new_state.ins_alu.read_mem = cur_state.ins_read_reg.read_mem;
				new_state.ins_alu.write_reg = cur_state.ins_read_reg.write_reg;
				new_state.ins_alu.use_mem = cur_state.ins_read_reg.use_mem;
				new_state.ins_alu.destination = cur_state.ins_read_reg.destination;
				new_state.ins_alu.mem_write_value = cur_state.ins_read_reg.mem_write_value;
				//std::cout << "Read enable mem on next cycle : " << new_state.ins_alu.read_mem << '\n';
				if (branch.find(new_state.ins_read_reg.operation) != branch.end())
				{
					new_state.ins_fetch1.branch = cur_state.ins_fetch1.branch;
					new_state.ins_fetch1.new_branch = cur_state.ins_fetch1.new_branch;
					//std::cout << "New branch = " << cur_state.ins_fetch1.new_branch << " " << cur_state.ins_fetch1.PC << '\n';

					new_state.ins_fetch1.PC = cur_state.ins_read_reg.saved_PC;
					//std::cout << cur_state.ins_read_reg.saved_PC << "\n";
					if (new_state.ins_alu.exe_result_op == 1)
					{
						new_state.ins_fetch1.take_branch = 1;
					}
					new_state.ins_fetch2.nop = 1;
					new_state.ins_dec1.nop = 1;
					new_state.ins_dec2.nop = 1;
					new_state.ins_read_reg.nop = 1;
					new_state.ins_alu.nop = 1;
					new_state.ins_mem_write.nop = 1;
					new_state.ins_fetch1.nop = 0;
					goto flush;
				}
				if (cur_state.ins_read_reg.operation != "lw" && cur_state.ins_read_reg.operation != "sw")
				{
					if (!cur_state.ins_mem_read.write_reg && !cur_state.ins_mem_write.write_reg)
					{
						new_state.ins_wb.fetchfrom_alu = 1;
						// new_state.ins_mem_read.nop = 1;
						new_state.ins_mem_write.nop = 1;
						new_state.ins_wb.nop = 0;
						//std::cout << "Enabling WB\n";
					}
					else
					{
						//std::cout<<"go to stall 1\n";
						goto stall1 ; 
					}
				}
			}
			else
			{
				//std::cout << "ALU is NOP\n";
			}
			new_state.ins_alu.nop = cur_state.ins_read_reg.nop;

			//std::cout << "-------------RR----------\n";
			if (!cur_state.ins_read_reg.nop)
			{
				new_state.ins_read_reg.saved_PC = cur_state.ins_dec2.saved_PC;
				//std::cout << cur_state.ins_dec2.saved_PC << "\n";

				new_state.ins_read_reg.operation = cur_state.ins_dec2.operation;

				if (r_type.find(new_state.ins_read_reg.operation) != r_type.end())
				{
					new_state.ins_read_reg.write_reg = 1;
					new_state.ins_read_reg.value1 = registers[registerMap[cur_state.ins_dec2.reg1]];
					new_state.ins_read_reg.value2 = registers[registerMap[cur_state.ins_dec2.reg2]];
					new_state.ins_read_reg.destination = cur_state.ins_dec2.destination;
				}
				if (i_type.find(new_state.ins_read_reg.operation) != i_type.end())
				{

					new_state.ins_read_reg.write_reg = 1;

					new_state.ins_read_reg.destination = cur_state.ins_dec2.destination;
					new_state.ins_read_reg.value1 = registers[registerMap[cur_state.ins_dec2.reg1]];

					new_state.ins_read_reg.value2 = cur_state.ins_dec2.number_found;
					//std::cout << cur_state.ins_dec2.reg1 << " " << new_state.ins_read_reg.value1 << ' ' << new_state.ins_read_reg.value2 << '\n';
				}
				if (lw_set.find(new_state.ins_read_reg.operation) != lw_set.end())
				{
					new_state.ins_read_reg.write_reg = 1;
					new_state.ins_read_reg.use_mem = 1;
					new_state.ins_read_reg.read_mem = 1;

					new_state.ins_read_reg.value1 = registers[registerMap[cur_state.ins_dec2.reg1]] ;
					new_state.ins_read_reg.value2 = cur_state.ins_dec2.number_found ;
					new_state.ins_read_reg.destination = cur_state.ins_dec2.destination;
				}
				if (sw_set.find(new_state.ins_read_reg.operation) != sw_set.end())
				{
					new_state.ins_read_reg.use_mem = 1;
					new_state.ins_read_reg.write_mem = 1;

					new_state.ins_read_reg.value1 = registers[registerMap[cur_state.ins_dec2.destination]];
					new_state.ins_read_reg.value2 = cur_state.ins_dec2.number_found ;
					new_state.ins_read_reg.mem_write_value = registers[registerMap[cur_state.ins_dec2.reg1]];
				}
				if (branch.find(new_state.ins_read_reg.operation) != branch.end())
				{
					new_state.ins_read_reg.value1 = registers[registerMap[cur_state.ins_dec2.reg1]];
					new_state.ins_read_reg.value2 = registers[registerMap[cur_state.ins_dec2.reg2]];
					new_state.ins_fetch1.branch = cur_state.ins_fetch1.branch;
					new_state.ins_fetch1.new_branch = cur_state.ins_fetch1.new_branch;

					new_state.ins_fetch2.nop = 1;
					new_state.ins_dec1.nop = 1;
					new_state.ins_dec2.nop = 1;
					new_state.ins_read_reg.nop = 1;
					new_state.ins_fetch1.nop = 0;
					goto flush;
				}
				if (jump.find(new_state.ins_read_reg.operation) != jump.end())
				{

				}

				//std::cout << "Values transfered to ALU: " << new_state.ins_read_reg.value1 << " " << new_state.ins_read_reg.value2 << '\n';
			}
			else
			{
				//std::cout << "RR is NOP\n";
			}
			new_state.ins_read_reg.nop = cur_state.ins_dec2.nop;

			//std::cout << "-------------ID2----------\n";

			if (!cur_state.ins_dec2.nop)
			{
				new_state.ins_dec2.operation = cur_state.ins_dec1.Ins[0];

				//std::cout << "Operation decoded: " << new_state.ins_dec2.operation << '\n';

				new_state.ins_dec2.saved_PC = cur_state.ins_dec1.saved_PC;
				//std::cout << cur_state.ins_dec1.saved_PC << "\n";

				if (r_type.find(new_state.ins_dec2.operation) != r_type.end())
				{
					new_state.ins_dec2.write_reg = 1;
					new_state.ins_dec2.reg1 = cur_state.ins_dec1.Ins[2];
					new_state.ins_dec2.reg2 = cur_state.ins_dec1.Ins[3];
					if (reg_lock[new_state.ins_dec2.reg1] || reg_lock[new_state.ins_dec2.reg2])
					{

						// stall !!
						//std::cout << "stalling!!!!!\n";
						new_state.ins_read_reg.nop = 1;
						goto stall;
					}
					new_state.ins_dec2.destination = cur_state.ins_dec1.Ins[1];
					reg_lock[new_state.ins_dec2.destination]++;
				}
				if (i_type.find(new_state.ins_dec2.operation) != i_type.end())
				{
					new_state.ins_dec2.write_reg = 1;

					new_state.ins_dec2.reg1 = cur_state.ins_dec1.Ins[2];
					new_state.ins_dec2.destination = cur_state.ins_dec1.Ins[1];
					new_state.ins_dec2.number_found = stoi(cur_state.ins_dec1.Ins[3]);
					if (reg_lock[new_state.ins_dec2.reg1] > 0)
					{
						// stall !!
						//std::cout << "stalling!!!!!\n";
						//std::cout << new_state.ins_dec2.reg1 << "\n";
						//std::cout << reg_lock[new_state.ins_dec2.reg1] << '\n';
						new_state.ins_dec2.nop = cur_state.ins_dec1.nop;

						new_state.ins_read_reg.nop = 1;
						goto stall;
					}
					//std::cout << "increamenting " << new_state.ins_dec2.destination << '\n';
					reg_lock[new_state.ins_dec2.destination]++;
				}
				if (lw_set.find(new_state.ins_dec2.operation) != lw_set.end())
				{
					new_state.ins_dec2.write_reg = 1;
					new_state.ins_dec2.use_mem = 1;
					new_state.ins_dec2.read_mem = 1;
					new_state.ins_dec2.destination = cur_state.ins_dec1.Ins[1];
					new_state.ins_dec2.reg1 = extract(cur_state.ins_dec1.Ins[2]).first;
					new_state.ins_dec2.number_found = stoi(extract(cur_state.ins_dec1.Ins[2]).second);
					if (reg_lock[new_state.ins_dec2.reg1])
					{
						// stall !!
						//std::cout << "stalling!!!!!\n";

						new_state.ins_read_reg.nop = 1;
						goto stall;
					}
					reg_lock[new_state.ins_dec2.destination]++;
				}
				if (sw_set.find(new_state.ins_dec2.operation) != sw_set.end())
				{
					new_state.ins_dec2.use_mem = 1;
					new_state.ins_dec2.write_mem = 1;
					new_state.ins_dec2.reg1 = cur_state.ins_dec1.Ins[1];
					new_state.ins_dec2.destination = extract(cur_state.ins_dec1.Ins[2]).first;
					new_state.ins_dec2.number_found = stoi(extract(cur_state.ins_dec1.Ins[2]).second);
					if (reg_lock[new_state.ins_dec2.reg1] || reg_lock[new_state.ins_dec2.destination])
					{
						// stall !!
						//std::cout << "stalling!!!!!\n";

						new_state.ins_read_reg.nop = 1;

						goto stall;
					}
				}
				if (branch.find(new_state.ins_dec2.operation) != branch.end())
				{
					// add stalling ....... !!!!!!!!!!
					new_state.ins_dec2.reg1 = cur_state.ins_dec1.Ins[1];
					new_state.ins_dec2.reg2 = cur_state.ins_dec1.Ins[2];

					if (reg_lock[new_state.ins_dec2.reg1] || reg_lock[new_state.ins_dec2.reg2])
					{
						// stall !!
						//std::cout << "stalling!!!!!\n";

						new_state.ins_read_reg.nop = 1;
						goto stall;
					}
					new_state.ins_fetch1.branch = true;
					new_state.ins_fetch1.new_branch = address[cur_state.ins_dec1.Ins[3]];
					new_state.ins_fetch2.nop = 1;
					new_state.ins_dec1.nop = 1;
					new_state.ins_dec2.nop = 1;
					new_state.ins_fetch1.nop = 0;
					goto flush;
				}
				if (jump.find(new_state.ins_dec2.operation) != jump.end())
				{
					new_state.ins_fetch1.new_branch = address[cur_state.ins_dec1.Ins[1]];
					new_state.ins_fetch1.jump = true;

					new_state.ins_fetch2.nop = 1;
					new_state.ins_dec1.nop = 1;
					new_state.ins_dec2.nop = 1;
					new_state.ins_read_reg.nop = 1;
					new_state.ins_fetch1.nop = 0;
					goto flush;
				}
			}
			else
			{
				//std::cout << "ID2 is NOP\n";
			}
			new_state.ins_dec2.nop = cur_state.ins_dec1.nop;

			//std::cout << "-------------ID1----------\n";
			if (!cur_state.ins_dec1.nop)
			{

				new_state.ins_dec1.Ins = commands[cur_state.ins_fetch2.PC];
				//std::cout << "Instruction decoded: ";
				for (string s : new_state.ins_dec1.Ins)
					//std::cout << s << " ";
				//std::cout << '\n';
				new_state.ins_dec1.saved_PC = cur_state.ins_fetch2.PC;
				//std::cout << cur_state.ins_fetch2.PC << "\n";
			}
			else
			{
				//std::cout << "ID1 is NOP\n";
			}

			new_state.ins_dec1.nop = cur_state.ins_fetch2.nop;

			//std::cout << "-----------IF2------------\n";

			if (!cur_state.ins_fetch2.nop)
			{
				new_state.ins_fetch2.PC = cur_state.ins_fetch1.PC;
				//std::cout << "Value of PC in IF2: " << new_state.ins_fetch2.PC << '\n';
			}
			new_state.ins_fetch2.nop = cur_state.ins_fetch1.nop;

			//std::cout << "-----------IF1------------\n";

			if (!cur_state.ins_fetch1.nop)
			{
				if (cur_state.ins_fetch1.jump)
				{
					new_state.ins_fetch1.PC = cur_state.ins_fetch1.new_branch;
				}
				else if (cur_state.ins_fetch1.branch && cur_state.ins_fetch1.take_branch)
				{
					new_state.ins_fetch1.PC = cur_state.ins_fetch1.new_branch;
				}
				else
				{
					new_state.ins_fetch1.PC = cur_state.ins_fetch1.PC + 1;
				}
				if ((int)new_state.ins_fetch1.PC >= (int)commands.size())
				{

					new_state.ins_fetch1.nop = 1;
					//std::cout << "IF1 is NOP\n";
					new_state.ins_fetch2.nop = 1;
				}
				else
				{
					//std::cout << "Value of PC in IF1: " << new_state.ins_fetch1.PC << '\n';
				}
			}
			else
			{

				new_state.ins_fetch1.nop = cur_state.ins_fetch1.nop;
			}

			//std::cout << cur_state.ins_fetch1.nop << cur_state.ins_fetch2.nop << cur_state.ins_dec1.nop << cur_state.ins_dec2.nop << cur_state.ins_alu.nop << cur_state.ins_read_reg.nop << cur_state.ins_mem_read.nop << cur_state.ins_mem_write.nop << '\n';
			printRegistersAndMemoryDelta(clockCycles);
			if (new_state.ins_fetch1.nop && new_state.ins_fetch2.nop && new_state.ins_dec1.nop && new_state.ins_dec2.nop && new_state.ins_alu.nop && new_state.ins_read_reg.nop && new_state.ins_mem_read.nop && new_state.ins_mem_write.nop && new_state.ins_wb.nop)
				break;
			cur_state = new_state;
			continue;
		stall:
		printRegistersAndMemoryDelta(clockCycles);

			new_state.ins_fetch1 = cur_state.ins_fetch1;
			new_state.ins_fetch2 = cur_state.ins_fetch2;
			new_state.ins_dec1 = cur_state.ins_dec1;
			new_state.ins_dec2 = cur_state.ins_dec2;
			new_state.ins_dec2.nop = cur_state.ins_dec2.nop;
			new_state.ins_dec1.nop = cur_state.ins_dec1.nop;
			new_state.ins_fetch2.nop = cur_state.ins_fetch2.nop;
			new_state.ins_fetch1.nop = cur_state.ins_fetch1.nop;

			cur_state = new_state;
			continue;
		flush:
		printRegistersAndMemoryDelta(clockCycles);

			cur_state = new_state;
			continue ;
		stall1:
			new_state.ins_fetch1 = cur_state.ins_fetch1;
			new_state.ins_fetch2 = cur_state.ins_fetch2;
			new_state.ins_dec1 = cur_state.ins_dec1;
			new_state.ins_dec2 = cur_state.ins_dec2;
			new_state.ins_read_reg = cur_state.ins_read_reg;
			new_state.ins_alu = cur_state.ins_alu;
			new_state.ins_mem_read.nop = 1 ; 
			cur_state = new_state; 
		printRegistersAndMemoryDelta(clockCycles);

		}

		

		handleExit(SUCCESS, clockCycles);
	}

	// print the register data in hexadecimal
	void printRegistersAndMemoryDelta(int clockCycle)
	{
		for (int i = 0; i < 32; ++i)
			std::cout << registers[i] << ' ';
		std::cout << '\n';
		std::cout << memoryDelta.size() << ' ';
		for (auto &p : memoryDelta)
			std::cout << p.first << ' ' << p.second;
		cout << '\n';
		memoryDelta.clear();
	}
};

#endif