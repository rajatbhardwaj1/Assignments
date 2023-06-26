#include <iostream>
#include <fstream>
#include "BranchPredictor.hpp"
#include <string>

using namespace std;

int main()
{
    ifstream file("branch_trace.txt");
    double correct = 0;
    double total = 0;
    double mask = pow(2, 14);
    // for (int iff = 0; iff < 4; iff++)
    // {
        BHRBranchPredictor bp = {2};
        for (int i = 0; i < 548; i++)
        {
            total += 1;
            string br;
            int ans;
            file >> br >> ans;
            unsigned int value = std::stoul(br, nullptr, 16);
            bool pred = bp.predict(value);
            bp.update(value, ans);
            if (pred == ans)
                correct += 1;
        }
        double acc = correct / total;
        acc *= 100;
        cout << "Accuracy = " << acc << '\n';
    // }
}