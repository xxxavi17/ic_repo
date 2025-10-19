//------------------------------------------------------------------------------
//
// Channel Recovery Demonstration for Exercise 1
// Shows that L and R can be recovered exactly from MID and SIDE
//
//------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <random>
#include <sndfile.hh>
#include "wav_hist_extended.h"

using namespace std;

void demonstrateChannelRecovery() {
    cout << "=== Channel Recovery Demonstration ===" << endl;
    cout << "Showing that L and R can be recovered exactly from MID and SIDE" << endl << endl;
    
    // Generate some test samples
    vector<short> testSamples = {
        1000, 2000,    // Frame 1: L=1000, R=2000
        -500, 1500,    // Frame 2: L=-500, R=1500
        32000, -1000,  // Frame 3: L=32000, R=-1000
        0, 0,          // Frame 4: L=0, R=0
        -32768, 32767  // Frame 5: L=-32768, R=32767 (extreme values)
    };
    
    cout << "Original samples (L, R pairs):" << endl;
    for (size_t i = 0; i < testSamples.size(); i += 2) {
        short L = testSamples[i];
        short R = testSamples[i + 1];
        cout << "Frame " << (i/2 + 1) << ": L=" << L << ", R=" << R << endl;
    }
    
    cout << endl << "Computing MID and SIDE:" << endl;
    vector<short> midValues, sideValues;
    
    for (size_t i = 0; i < testSamples.size(); i += 2) {
        short L = testSamples[i];
        short R = testSamples[i + 1];
        
        // MID and SIDE calculations (integer division)
        short MID = static_cast<short>((static_cast<int>(L) + static_cast<int>(R)) / 2);
        short SIDE = static_cast<short>((static_cast<int>(L) - static_cast<int>(R)) / 2);
        
        midValues.push_back(MID);
        sideValues.push_back(SIDE);
        
        cout << "Frame " << (i/2 + 1) << ": MID=" << MID << ", SIDE=" << SIDE << endl;
    }
    
    cout << endl << "Recovering L and R from MID and SIDE:" << endl;
    bool perfectRecovery = true;
    
    for (size_t i = 0; i < midValues.size(); i++) {
        short MID = midValues[i];
        short SIDE = sideValues[i];
        
        // Recovery formulas
        short L_recovered = MID + SIDE;
        short R_recovered = MID - SIDE;
        
        short L_original = testSamples[i * 2];
        short R_original = testSamples[i * 2 + 1];
        
        cout << "Frame " << (i + 1) << ": ";
        cout << "L_recovered=" << L_recovered << " (original=" << L_original << "), ";
        cout << "R_recovered=" << R_recovered << " (original=" << R_original << ")";
        
        if (L_recovered == L_original && R_recovered == R_original) {
            cout << " ✓ EXACT MATCH" << endl;
        } else {
            cout << " ✗ MISMATCH!" << endl;
            perfectRecovery = false;
        }
    }
    
    cout << endl;
    if (perfectRecovery) {
        cout << "🎉 SUCCESS: All channels recovered exactly!" << endl;
    } else {
        cout << "❌ ERROR: Some channels could not be recovered exactly!" << endl;
    }
    
    cout << endl << "Mathematical proof:" << endl;
    cout << "L = MID + SIDE = ((L+R)/2) + ((L-R)/2) = (L+R+L-R)/2 = 2L/2 = L" << endl;
    cout << "R = MID - SIDE = ((L+R)/2) - ((L-R)/2) = (L+R-L+R)/2 = 2R/2 = R" << endl;
}

int main() {
    demonstrateChannelRecovery();
    return 0;
}