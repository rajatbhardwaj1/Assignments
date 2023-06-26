#ifndef __BRANCH_PREDICTOR_HPP__
#define __BRANCH_PREDICTOR_HPP__

#include <vector>
#include <bitset>
#include <cassert>
#include <math.h>

struct BranchPredictor
{
    virtual bool predict(uint32_t pc) = 0;
    virtual void update(uint32_t pc, bool taken) = 0;
};

struct SaturatingBranchPredictor : public BranchPredictor
{
    std::vector<std::bitset<2>> table;
    SaturatingBranchPredictor(int value) : table(1 << 14, value) {}

    bool predict(uint32_t pc)
    {

        int mask = pow(2, 14) - 1;
        int index = pc & mask;
        int currentval = table[index].to_ulong();
        if (currentval > 1)
            return true;
        // your code here
        return false;
    }

    void update(uint32_t pc, bool taken)
    {
        // your code here
        int mask = pow(2, 14) - 1;
        int index = pc & mask;
        int currentval = table[index].to_ulong();

        if (taken)
        {
            currentval = std::min(3, currentval + 1);
        }
        else
        {
            currentval = std::max(0, currentval - 1);
        }
        std::bitset<2> finalval(currentval);
        table[index] = finalval;
    }
};

struct BHRBranchPredictor : public BranchPredictor
{
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    BHRBranchPredictor(int value) : bhrTable(1 << 2, value), bhr(value) {}

    bool predict(uint32_t pc)
    {
        int index = bhr.to_ulong();
        int pred = bhrTable[index].to_ulong();
        if (pred > 1)
            return true;
        return false;
    }

    void update(uint32_t pc, bool taken)
    {
        int index1 = bhr.to_ulong();
        int currentval1 = bhrTable[index1].to_ulong();

        if (taken)
        {
            currentval1 = std::min(3, currentval1 + 1);
        }
        else
        {
            currentval1 = std::max(0, currentval1 - 1);
        }
        std::bitset<2> fin(currentval1);
        bhrTable[index1] = fin;
        bhr = bhr << 1;
        bhr.set(0, taken);
    }
};

struct SaturatingBHRBranchPredictor : public BranchPredictor
{
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    std::vector<std::bitset<2>> table;
    std::vector<std::bitset<2>> combination;
    SaturatingBHRBranchPredictor(int value, int size) : bhrTable(1 << 2, value), bhr(value), table(1 << 14, value), combination(size, value)
    {
        assert(size <= (1 << 16));
    }

    bool predict(uint32_t pc)
    {
        int mask = pow(2, 14) - 1;
        int index = pc & mask;

        if (combination[index][1] == 0)
        {
            index = bhr.to_ulong();
            int pred = bhrTable[index].to_ulong();
            if (pred > 1)
                return true;
            return false;
        }
        else
        {
            index = pc & mask;
            int currentval = table[index].to_ulong();
            if (currentval > 1)
                return true;
            // your code here
            return false;
        }
    }

    void update(uint32_t pc, bool taken)
    {
        // your code here
        int mask = pow(2, 14) - 1;
        int index = pc & mask;
        int currentval = table[index].to_ulong();
        int predicted = predict(pc);

        if (taken)
        {
            currentval = std::min(3, currentval + 1);
        }
        else
        {
            currentval = std::max(0, currentval - 1);
        }
        std::bitset<2> finalval(currentval);
        table[index] = finalval;

        int index1 = bhr.to_ulong();
        int currentval1 = bhrTable[index1].to_ulong();

        if (taken)
        {
            currentval1 = std::min(3, currentval1 + 1);
        }
        else
        {
            currentval1 = std::max(0, currentval1 - 1);
        }
        if (predicted == taken)
        {
            int current = combination[index].to_ulong();

            if (current > 1)
            {
                current = std::min(current + 1, 3);
            }
            else
            {
                current = std::max(current - 1, 0);
            }
            std::bitset<2> newval(current);
            combination[index] = newval;
        }
        else
        {
            int current = combination[index].to_ulong();

            if (current > 1)
            {
                current = current - 1 ;
            }
            else
            {
                current = current + 1 ;
            }

            std::bitset<2> newval(current);
            combination[index] = newval;
        }

        std::bitset<2> fin(currentval1);
        bhrTable[index1] = fin;
        bhr = bhr << 1;
        bhr.set(0) = taken;
    }
};

#endif
