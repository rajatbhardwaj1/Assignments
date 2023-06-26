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
#include <utility>
#include <tuple>
#include <set>
#include <math.h>

using namespace std;
struct IF
{
    int nop;
    int PC;
    bool jump;
    bool branch;
    bool take_branch;
    int new_branch;
};

struct ID
{
    int nop;
    std::vector<std::string> Ins;
    int value1;
    int value2;
    bool write_reg;
    bool use_mem;
    bool read_mem;
    bool write_mem;
    int saved_PC;
    int mem_write_val ; 
};

struct EX
{
    int nop;
    std::string reg_write;
    int exe_result_op;
    bool write_mem;
    bool read_mem;
    bool write_reg;
    bool use_mem;
    int mem_write_val ; 

};

struct MEM
{
    int nop;
    std::string reg_write;
    int memAddr;
    int read_value;
    int exe_result_op;
    int mem_result_op;
    bool use_mem;
    bool write_reg;
};

struct WB
{
    int nop;
    bool write_reg;
    bool running;
};

struct state
{
    IF ins_fetch;
    ID ins_dec;
    EX ins_exec;
    MEM ins_mem;
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
    set<string> r_type = {"add", "sub", "mul", "slt"};
    set<string> i_type = {"addi", "subi", "muli", "sll", "slr"};
    set<string> lwsw = {"lw", "sw"};
    set<string> branch = {"beq", "bne"};
    set<string> jump = {"j"};
    enum exit_code
    {
        SUCCESS = 0,
        INVALID_REGISTER,
        INVALID_LABEL,
        INVALID_ADDRESS,
        SYNTAX_ERROR,
        MEMORY_ERROR
    };

    // constructor to initialise the instruction set
    MIPS_Architecture(std::ifstream &file)
    {
        instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}};

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
        return (r1 + r2) / 4;
    }
    int sw(int r1, int r2, std::string unused1 = "")
    {
        return (r1 + r2) / 4;
    }

    string extractreg(string location)
    {
        if (location.back() == ')')
        {
            try
            {
                int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
                std::string reg = location.substr(lparen + 1);
                reg.pop_back();
                if (!checkRegister(reg))
                    return {-3, 0};
                return reg;
            }
            catch (std::exception &e)
            {
                return "-4";
            }
        }
        try
        {
            return location;
        }
        catch (std::exception &e)
        {
            return "-4";
        }
    }

    std::pair<int, int> locateAddress(std::string location)
    {
        if (location.back() == ')')
        {
            try
            {
                int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
                std::string reg = location.substr(lparen + 1);
                reg.pop_back();
                if (!checkRegister(reg))
                    return {-9, 0};

                std::pair<int, int> address = {registers[registerMap[reg]], offset};
                return address;
            }
            catch (std::exception &e)
            {
                return {-4, 0};
            }
        }
        try
        {
            int address = stoi(location);

            return {address, 0};
        }
        catch (std::exception &e)
        {
            return {-4, 0};
        }
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

        int clockCycles = 0;
        state cur_state, new_state;

        cur_state.ins_fetch.PC = -1;
        cur_state.ins_fetch.nop = 0;
        cur_state.ins_dec.nop = 1;
        cur_state.ins_exec.nop = 1;
        cur_state.ins_mem.nop = 1;
        cur_state.ins_wb.nop = 1;

        //		----------IF-----------
        cur_state.ins_fetch.take_branch = 0;
        cur_state.ins_fetch.new_branch = 0;
        cur_state.ins_fetch.branch = 0;
        cur_state.ins_fetch.jump = 0;

        //		---------ID--------------
        cur_state.ins_dec.write_mem = 0;
        cur_state.ins_dec.write_reg = 0;
        cur_state.ins_dec.use_mem = 0;
        cur_state.ins_dec.read_mem = 0;

        //		----------EX------------

        cur_state.ins_exec.write_mem = 0;
        cur_state.ins_exec.read_mem = 0;
        cur_state.ins_exec.write_reg = 0;
        cur_state.ins_exec.use_mem = 0;

        //		---------MEM-------------
        cur_state.ins_mem.use_mem = 0;
        cur_state.ins_mem.write_reg = 0;

        //		-----------WB-------------
        cur_state.ins_wb.write_reg = 0;
        printRegistersAndMemoryDelta(clockCycles);
        while (PCcurr < commands.size())
        {
            new_state.ins_fetch.nop = 0;
            new_state.ins_dec.nop = 1;
            new_state.ins_exec.nop = 1;
            new_state.ins_mem.nop = 1;
            new_state.ins_wb.nop = 1;

            //		----------IF-----------
            new_state.ins_fetch.take_branch = 0;
            new_state.ins_fetch.new_branch = 0;
            new_state.ins_fetch.branch = 0;
            new_state.ins_fetch.jump = 0;

            //		---------ID--------------
            new_state.ins_dec.write_mem = 0;
            new_state.ins_dec.write_reg = 0;
            new_state.ins_dec.use_mem = 0;
            new_state.ins_dec.read_mem = 0;

            //		----------EX------------

            new_state.ins_exec.write_mem = 0;
            new_state.ins_exec.read_mem = 0;
            new_state.ins_exec.write_reg = 0;
            new_state.ins_exec.use_mem = 0;

            //		---------MEM-------------
            new_state.ins_mem.use_mem = 0;
            new_state.ins_mem.write_reg = 0;

            //		-----------WB-------------
            new_state.ins_wb.write_reg = 0;
            // if(clockCycles>20)break;
            ++clockCycles;
            //std::cout << "-------------First Half Cycle--------------\n";
            //std::cout << "--------------WB--------------\n";

            if (!cur_state.ins_wb.nop)
            {
                if (cur_state.ins_mem.write_reg)
                {
                    if (!cur_state.ins_mem.use_mem)
                    {
                        //std::cout << "Using value from ALU\n";
                        registers[registerMap[cur_state.ins_mem.reg_write]] = cur_state.ins_mem.exe_result_op;
                        //std::cout << "Writing value " << cur_state.ins_mem.exe_result_op << " in the register " << cur_state.ins_mem.reg_write << " From ALU output" << '\n';
                    }
                    else
                    {
                        //std::cout << "Using value from memory\n";
                        registers[registerMap[cur_state.ins_mem.reg_write]] = cur_state.ins_mem.mem_result_op;
                        //std::cout << "Writing value " << cur_state.ins_mem.mem_result_op << " in the register " << cur_state.ins_mem.reg_write << " From the Memory output" << '\n';
                    }
                    reg_lock[cur_state.ins_mem.reg_write] -= 1;
                    //std::cout << "Unlocking " << cur_state.ins_mem.reg_write << '\n';
                }
                else
                {
                    //std::cout << "Nothing to write back\n";
                }
            }
            else
            {
                //std::cout << "Write back stage NOP\n";
            }
            new_state.ins_wb.nop = cur_state.ins_mem.nop;

            //std::cout << "--------------Second Half---------------\n";
            //std::cout << "--------------MEM--------------\n";

            if (!cur_state.ins_mem.nop)
            {
                new_state.ins_mem.reg_write = cur_state.ins_exec.reg_write;
                new_state.ins_mem.write_reg = cur_state.ins_exec.write_reg;
                new_state.ins_mem.use_mem = cur_state.ins_exec.use_mem;
                new_state.ins_mem.exe_result_op = cur_state.ins_exec.exe_result_op;
                // new_state.ins_exec.use_mem = cur_state.ins_mem.use_mem;
                if (cur_state.ins_exec.read_mem)
                {
                    new_state.ins_mem.mem_result_op = data[cur_state.ins_exec.exe_result_op];
                    //std::cout << new_state.ins_mem.mem_result_op << " was read from address " << cur_state.ins_exec.exe_result_op << '\n';
                }
                else if (cur_state.ins_exec.write_mem)
                {
                    //std::cout << "register writing " << new_state.ins_mem.reg_write << '\n';
                    data[cur_state.ins_exec.exe_result_op] = cur_state.ins_exec.mem_write_val;

                    memoryDelta[cur_state.ins_exec.exe_result_op] = registers[registerMap[new_state.ins_mem.reg_write]];
                    //std::cout << registers[registerMap[new_state.ins_mem.reg_write]] << " was written at address " << cur_state.ins_exec.exe_result_op << '\n';
                }
            }
            else
            {
                //std::cout << "MEM stage NOP\n";
            }
            new_state.ins_mem.nop = cur_state.ins_exec.nop;

            // -------------------- EX stage ----------
            //std::cout << "--------------EX--------------\n";
            if (!cur_state.ins_exec.nop)
            {

                //std::cout << "EX stage is running\n";
                new_state.ins_exec.reg_write = cur_state.ins_dec.Ins[1];
                new_state.ins_exec.exe_result_op = instructions[cur_state.ins_dec.Ins[0]](*this, cur_state.ins_dec.value1, cur_state.ins_dec.value2, cur_state.ins_dec.Ins[3]);
                cur_state.ins_fetch.take_branch = (bool)new_state.ins_exec.exe_result_op;
                new_state.ins_exec.use_mem = cur_state.ins_dec.use_mem;
                new_state.ins_exec.write_reg = cur_state.ins_dec.write_reg;
                new_state.ins_exec.read_mem = cur_state.ins_dec.read_mem;
                new_state.ins_exec.write_mem = cur_state.ins_dec.write_mem;
                new_state.ins_exec.mem_write_val = cur_state.ins_dec.mem_write_val ;
                //std::cout << "ALU result: " << new_state.ins_exec.exe_result_op << "\n";
            
                if (branch.find(cur_state.ins_dec.Ins[0]) != branch.end())
				{
					new_state.ins_fetch.branch = cur_state.ins_fetch.branch;
					new_state.ins_fetch.new_branch = cur_state.ins_fetch.new_branch;
					//std::cout << "New branch = " << cur_state.ins_fetch.new_branch << " " << cur_state.ins_fetch.PC << '\n';

					new_state.ins_fetch.PC = cur_state.ins_dec.saved_PC;
					//std::cout << cur_state.ins_dec.saved_PC << "\n";
					if (new_state.ins_exec.exe_result_op == 1)
					{
						new_state.ins_fetch.take_branch = 1;
					}
					new_state.ins_dec.nop = 1;
					new_state.ins_exec.nop = 1;
					// new_state.ins_mem.nop = 1;
					new_state.ins_fetch.nop = 0;
					goto flush;
				}
            }
            else
            {
                //std::cout << "EXEC is NOP\n";
            }
            new_state.ins_exec.nop = cur_state.ins_dec.nop;
            //-------------ID STAGE -------------------
            //std::cout << "--------------ID--------------\n";

            if (!cur_state.ins_dec.nop)
            {
                new_state.ins_dec.Ins = commands[cur_state.ins_fetch.PC];
                new_state.ins_dec.saved_PC = cur_state.ins_fetch.PC;
                //std::cout << "Decoded command: ";
                for (string s : new_state.ins_dec.Ins)
                    //std::cout << s << ' ';
                //std::cout << '\n';
                // new_state.ins_dec.value1=;

                new_state.ins_dec.use_mem = false;
                new_state.ins_dec.write_reg = false;
                new_state.ins_dec.read_mem = false;
                new_state.ins_dec.write_mem = false;
                new_state.ins_dec.write_reg = true;
                   
                if (i_type.find(new_state.ins_dec.Ins[0]) != i_type.end())
                {

                    if (reg_lock[new_state.ins_dec.Ins[2]] != 0 || reg_lock[new_state.ins_dec.Ins[3]] != 0)
                    {
                        
                        //std::cout << "stalling!!!!!\n";

						new_state.ins_exec.nop = 1;

						goto stall;
                    }

                    reg_lock[new_state.ins_dec.Ins[1]] += 1;
                    new_state.ins_dec.value1 = registers[registerMap[new_state.ins_dec.Ins[2]]];
                    new_state.ins_dec.value2 = stoi(new_state.ins_dec.Ins[3]);
                
                }
                else if (branch.find(new_state.ins_dec.Ins[0]) != branch.end())
                {

                    new_state.ins_dec.write_reg = false;

                    if (reg_lock[new_state.ins_dec.Ins[1]] != 0 || reg_lock[new_state.ins_dec.Ins[2]] != 0)
                    {
                       
                       //std::cout<<"Stalling!!\n";
                       new_state.ins_exec.nop = 1; 
                       goto stall; 
                    }

    
                    new_state.ins_fetch.new_branch = address[new_state.ins_dec.Ins[3]];
                    //std::cout << "New branch!!!!!"

                    new_state.ins_dec.value1 = registers[registerMap[new_state.ins_dec.Ins[1]]];
                    new_state.ins_dec.value2 = registers[registerMap[new_state.ins_dec.Ins[2]]];
					new_state.ins_fetch.branch = true;
					new_state.ins_dec.nop = 1;
					new_state.ins_fetch.nop = 0;   
                    goto flush; 
                }
                else if (r_type.find(new_state.ins_dec.Ins[0]) != r_type.end())
                {
                    new_state.ins_dec.value1 = registers[registerMap[new_state.ins_dec.Ins[2]]];
                    new_state.ins_dec.value2 = registers[registerMap[new_state.ins_dec.Ins[3]]];
                    if (reg_lock[new_state.ins_dec.Ins[2]] != 0 || reg_lock[new_state.ins_dec.Ins[3]] != 0)
                    {
                        // stall !!
						//std::cout << "stalling!!!!!\n";
						new_state.ins_exec.nop = 1;
						goto stall;
                    }
                    // destination !!!!!!??
                    reg_lock[new_state.ins_dec.Ins[1]] += 1;
                }
                else if (lwsw.find(new_state.ins_dec.Ins[0]) != lwsw.end())
                {
                    new_state.ins_dec.write_reg = false;

                    // stall it
                    //std::cout << "USE MEM signal enabled for this instruction \n";
                    if (new_state.ins_dec.Ins[0] == "lw")
                    {
                        new_state.ins_dec.use_mem = true;
                        new_state.ins_dec.write_reg = true;
                        new_state.ins_dec.read_mem = true;
                        //std::cout << "Read value for MEM stage is enabled!\n";
                        new_state.ins_dec.write_mem = false;
                        if (reg_lock[extractreg(new_state.ins_dec.Ins[2])] != 0)
                        {
                            //std::cout << "stalling!!\n";
                            new_state.ins_exec.nop = 1;
                            goto stall;
                        }
                        reg_lock[new_state.ins_dec.Ins[1]] += 1;
                    }
                    if (new_state.ins_dec.Ins[0] == "sw")
                    {
                        new_state.ins_dec.use_mem = true;
                        new_state.ins_dec.write_reg = false;
                        new_state.ins_dec.read_mem = false;
                        new_state.ins_dec.write_mem = true;
                        //std::cout << "Write value for MEM stage is enabled!\n";
                        new_state.ins_dec.mem_write_val = registers[registerMap[new_state.ins_dec.Ins[1]]];
                        if (reg_lock[new_state.ins_dec.Ins[1]] != 0 || reg_lock[extractreg(new_state.ins_dec.Ins[2])] != 0)
                        {
                            //std::cout << "Locking ID stage\n";

                            new_state.ins_exec.nop = 1;
                            goto stall;
                        }
                    }
                    //std::cout << "reg write at dec = " << new_state.ins_dec.Ins[1] << '\n';
                    new_state.ins_dec.value1 = locateAddress(new_state.ins_dec.Ins[2]).first;
                    new_state.ins_dec.value2 = locateAddress(new_state.ins_dec.Ins[2]).second;
                }
                else if (jump.find(new_state.ins_dec.Ins[0]) != jump.end())
                { // jump
                    new_state.ins_fetch.new_branch = address[new_state.ins_dec.Ins[1]];
                    new_state.ins_fetch.jump = true;
                    new_state.ins_exec.nop = 1 ; 
                    new_state.ins_fetch.nop = 0;
                    new_state.ins_dec.nop = 1;
                    //std::cout << "Jump instruction found! Disable ALU for next cycle\n";
                    goto flush;
                }
            }
            else
            {
                //std::cout << "ID stage NOP\n";
            }

            new_state.ins_dec.nop = cur_state.ins_fetch.nop;
            //std::cout << "--------------IF--------------\n";

            if (!cur_state.ins_fetch.nop)
            {
                if (cur_state.ins_fetch.jump)
				{
					new_state.ins_fetch.PC = cur_state.ins_fetch.new_branch;
				}
				else if (cur_state.ins_fetch.branch && cur_state.ins_fetch.take_branch)
				{
					new_state.ins_fetch.PC = cur_state.ins_fetch.new_branch;
				}
				else
				{
					new_state.ins_fetch.PC = cur_state.ins_fetch.PC + 1;
				}
				if ((int)new_state.ins_fetch.PC >= (int)commands.size())
				{

					new_state.ins_fetch.nop = 1;
                    new_state.ins_dec.nop = 1;
					//std::cout << "IF1 is NOP\n";
				}
				else
				{
					//std::cout << "Value of PC in IF1: " << new_state.ins_fetch.PC << '\n';
				}
			}
			else
			{

				new_state.ins_fetch.nop = cur_state.ins_fetch.nop;
			}

			printRegistersAndMemoryDelta(clockCycles);
			if (new_state.ins_fetch.nop && new_state.ins_dec.nop && new_state.ins_exec.nop && new_state.ins_mem.nop && new_state.ins_wb.nop)
				break;
			cur_state = new_state;
			continue;
		stall:
			printRegistersAndMemoryDelta(clockCycles);

			new_state.ins_fetch = cur_state.ins_fetch;
			new_state.ins_dec = cur_state.ins_dec;
			new_state.ins_dec.nop = cur_state.ins_dec.nop;
			new_state.ins_fetch.nop = cur_state.ins_fetch.nop;

			cur_state = new_state;
			continue;
        flush:
			printRegistersAndMemoryDelta(clockCycles);


			cur_state = new_state;
			continue ;
            // std::vector<std::string> &command = commands[PCcurr];
            // if (instructions.find(command[0]) == instructions.end())
            // {
            //     handleExit(SYNTAX_ERROR, clockCycles);
            //     return;
            // }
            // exit_code ret = (exit_code)instructions[command[0]](*this, command[1], command[2], command[3]);
            // if (ret != SUCCESS)
            // {
            //     handleExit(ret, clockCycles);
            //     return;
            // }
            // ++commandCount[PCcurr];
            // PCcurr = PCnext;
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
            std::cout << p.first << ' ' << p.second ;
            cout<<'\n';
        memoryDelta.clear();
    }
};

#endif