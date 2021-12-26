//
//  OPTTravel.cpp
//  USandCanadaOptimalTravel
//
//  Created by Avi Patel on 12/19/21.
//

#include <iostream>
#include <limits>
#include <cmath>
#include <vector>
#include <getopt.h>
#include "xcode_redirect.hpp"
using namespace std;

struct Cage {
    long long int xCord; // Longitude
    long long int yCord; // Latitude
    string name;
};

struct MST {
    bool k = false;
    double d = numeric_limits<double>::infinity();
    uint32_t p;
    uint32_t v;
};

class OPTTravel {
private:
    vector<Cage> masterCage;
    
    double distance(Cage a, Cage b) { // TODO: Change
        return sqrt(((double)b.xCord - (double)a.xCord) * ((double)b.xCord - (double)a.xCord) + ((double)b.yCord - (double)a.yCord) * ((double)b.yCord - (double)a.yCord));
    }
    
    vector<uint32_t> path;
    vector<uint32_t> bestPath;
    double curCost = 0;
    double bestCost = 0;
    
    bool promising(size_t permLength) {
        double edge1 = numeric_limits<double>::infinity();
        double edge2 = numeric_limits<double>::infinity();
        
        vector<MST> use;
        use.resize(path.size() - permLength);
        use[0].d = 0;
        size_t counter = 0;
        for (size_t i = permLength; i < path.size(); ++i) {
            use[counter].v = path[i];
            if (distance(masterCage[path[0]], masterCage[path[i]]) < edge1) {
                edge1 = distance(masterCage[path[0]], masterCage[path[i]]);
            }
            if (distance(masterCage[path[permLength - 1]], masterCage[path[i]]) < edge2) {
                edge2 = distance(masterCage[path[permLength - 1]], masterCage[path[i]]);
            }
            ++counter;
        }
        counter = 0;
        while (counter < use.size()) {
            uint32_t smallestFalse = 0;
            bool counter2 = false;
            for (size_t i = 0; i < use.size(); ++i) {
                if (!counter2) {
                    if (!use[i].k) {
                        smallestFalse = (uint32_t)i;
                        counter2 = true;
                    }
                }
                else if ((use[i].d < use[smallestFalse].d) && use[i].k == false) {
                    smallestFalse = (uint32_t)i;
                }
            }
            use[smallestFalse].k = true;
            for (size_t i = 0; i < use.size(); ++i) {
                if (use[i].k == false) {
                    double dist = distance(masterCage[use[smallestFalse].v], masterCage[use[i].v]);
                    if (dist < use[i].d) {
                        use[i].d = dist;
                    }
                }
            }
            ++counter;
        }
        
        double weight = 0;
        for (size_t i = 0; i < use.size(); ++i) {
           weight += use[i].d;
        }
        
        if ((curCost + weight + edge1 + edge2) < bestCost) {
            return true;
        }
        return false;
    }
    
    void genPerms(size_t permLength) {
        if (permLength == path.size()) {
            // Do something with the path
            curCost += distance(masterCage[path[0]], masterCage[path[path.size() - 1]]);
            if (curCost < bestCost) {
                bestCost = curCost;
                bestPath = path;
            }
            curCost -= distance(masterCage[path[0]], masterCage[path[path.size() - 1]]);
            return;
        } // if
        if (!promising(permLength)) {
            return;
        }
        for (size_t i = permLength; i < path.size(); ++i) {
            swap(path[permLength], path[i]);
            curCost += distance(masterCage[path[permLength - 1]], masterCage[path[permLength]]);
            genPerms(permLength + 1);
            curCost -= distance(masterCage[path[permLength - 1]], masterCage[path[permLength]]);
            swap(path[permLength], path[i]);
        } // for
        
    } // genPerms()
    
public:
    void get_options(int argc, char** argv) {
        int option_index = 0, option = 0;
        opterr = false;

        struct option longOpts[] = {{ "help", no_argument, nullptr, 'h'},
            { nullptr, 0, nullptr, '\0' }};
        while ((option = getopt_long(argc, argv, "h", longOpts, &option_index)) != -1) {
            switch (option) {
                case 'h':
                    cout << "Use cout to print out.\n";
                    exit(0);
                    break;
            }
        }
    }
    
    void readInFile() {
        Cage in;
        for (size_t i = 0; i < 15; ++i) {
            string city;
            string junk;
            char add = '\0';
            while (add != ',') {
                cin >> add;
                if (add != ',') {
                    city += add;
                }
            }
            in.name = city;
            int num;
            cin >> num;
            in.yCord = num;
            cin >> num >> num;
            in.xCord = num * -1;
            cin >> num >> junk >> junk;
            masterCage.push_back(in);
        }
    }
    
    void theOPT() {
        path.push_back(0);
        path.push_back(1);
        path.push_back(2);
        path.push_back(0);
        
        uint32_t counter = 3;
        while (counter < 15) {
            double dist = numeric_limits<double>::infinity();
            uint32_t valPos = 0;
            for (size_t i = 0; i < path.size() - 1; ++i) {
                double distCalc = distance(masterCage[path[i]], masterCage[counter]) + distance(masterCage[path[i + 1]], masterCage[counter]) - distance(masterCage[path[i]], masterCage[path[i + 1]]);
                if (distCalc < dist) {
                    dist = distCalc;
                    valPos = (uint32_t)i + 1;
                }
                
            }
            path.insert(path.begin() + valPos, counter);
            
            ++counter;
        }
        bestPath = path;
        for (size_t i = 0; i < (size_t)bestPath.size() - 1; ++i) {
            bestCost += distance(masterCage[bestPath[i]], masterCage[bestPath[i + 1]]);
        }
        
        path.pop_back();
        bestPath.pop_back();
        
        genPerms(1);
        
        // output
        cout << bestCost * 69 << " miles is your approximate distance traveled!" << "\n" << "Here is your path:\n";
        for (size_t i = 0; i < bestPath.size(); ++i) {
            cout << masterCage[bestPath[i]].name << " ";
        }
    }
};

int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    OPTTravel travel;
    travel.get_options(argc, argv);
    travel.readInFile();
    travel.theOPT();
    return 0;
}
