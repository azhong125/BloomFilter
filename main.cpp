#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <unordered_set>
#include <algorithm>
#include <stdlib.h>
#include <map>
using namespace std;
using namespace std::chrono;

random_device rd;
mt19937 gen(rd());

class bloomFilter {
private:
    vector<long long> seeds; //save k seeds
    vector<pair<int, int>> universal; //save k tuples of a and b to represent universal hash

    vector<int> bloomFilterSeed;
    vector<int> bloomFilterUniversal;
    map<int, int> seedMap;
    map<int, int> uniMap;

    int k; //number of hash functions
    int N; //size of universe
    int n; //size of subset
    int m; //size of table
    int c;
    int prime;

public:
    bloomFilter(int n1, int c1, int k1) {
        N = INT_MAX - 1;
        n = n1;
        c = c1;
        k = k1;
        m = c * n;
        prime = INT_MAX;
        bloomFilterSeed = vector<int>(m, 0);
        bloomFilterUniversal = vector<int>(m, 0);

        createSeeds();
        createUniversal();
    }

    void createSeeds() {
        for (int i = 0; i < k; i++) {
            milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
            long long seed = ms.count();
            //srand(seed);
            seeds.push_back(seed);
            _sleep(100);
            //cout << seed << endl;
        }
    }

    int getSeed(int i, int x) {
        long long seed = seeds[i] + x;
        minstd_rand0 generator (seed);
        int result = generator() % m;
        return result;
    }


//    int getSeed(int i, int x) {
//        long long seed = seeds[i] + x;
//        srand(seed);
//        //double a = rand()/(double)RAND_MAX;
//        //unsigned long long random_n = (unsigned long long)(INT_MAX*a);
//        //int result = random_n % m;
//        //cout << i << ", " << x << ", " << seeds[i] << ", " << seed << ", " << result << endl;
//        int result = rand() % m;
//        return result;
//    }

    void createUniversal() {
        for (int i = 0; i < k; i++) {
            auto distA = uniform_int_distribution<int>(1, prime - 1);
            int a = distA(gen);
            auto distB = uniform_int_distribution<int>(0, prime - 1);
            int b = distB(gen);
            //cout << "create: "  << a << ", " << b << endl;

            universal.push_back(pair<int, int> (a,b));
        }
    }

    int getUniversal(int i, int x) {
        pair<int, int> p = universal[i];
        int a = p.first;
        int b = p.second;
        //cout << i << " get: " << x << ": " << a << ", " << b << ", " << (((long long) a * x + b) % prime) % m << endl;

        return (((long long) a * x + b) % prime) % m;
    }

    void add(int x) {
        for (int i = 0; i < k; i++) {
            int s = getSeed(i,x);
            bloomFilterSeed[s] = 1;
            if (seedMap.find(s) != seedMap.end()) {
                seedMap[s] = seedMap[s] + 1;
            }
            else seedMap[s] = 1;
           // cout << i << " seed: " << x << ", " << s << endl;

            int u = getUniversal(i,x);
            bloomFilterUniversal[u] = 1;
            if (uniMap.find(u) != uniMap.end()) uniMap[u] = uniMap[u] + 1;
            else uniMap[u] = 1;
            //cout << i << " univ: " << x << ", " << u << endl;
        }//cout << count << endl;
    }

    bool seedContains(int x) {
        for (int i = 0; i < k; i++) {
            int s = getSeed(i, x);
             //cout << i << ", " << x << ": " << s << ", " << bloomFilterSeed[s] << endl;
            if (bloomFilterSeed[s] == 0) return false;
        }
        return true;
    }

    bool univContains(int x) {
        for (int i = 0; i < k; i++) {
            int u = getUniversal(i, x);
           // cout << i << ", " << x << ": " << u << ", " << bloomFilterUniversal[u] << endl;
            if (bloomFilterUniversal[u] == 0) return false;
            //else cout << i << " found univ: " << x << ", " << u << endl;
        }
        return true;
    }

    void printSeedMap() {
        int count = 0;
        for (auto const& x : seedMap) {
            //cout << x.first << ", " << x.second << endl;
            count += x.second;
        }
        cout << seedMap.size() << endl;
        cout << count / (float) seedMap.size() << endl;
    }
    void printUniMap() {
        int count = 0;
        for (auto const& x : uniMap) {
           // cout << x.first << ", " << x.second << endl;
            count += x.second;
        }
        cout << uniMap.size() << endl;
        cout << count / (float) uniMap.size() << endl;

    }

};

void printVec(string name, vector<int> v) {
    cout << name << " = [";
    for(int i = 0; i < v.size(); i++) {
        if (i != v.size() - 1) cout << v[i] << ", ";
        else cout << v[i];
    }
    cout << "]";
    cout << endl;
}
void problem1RandData() {
    //generate a bunch of random numbers
    //scatter plot with values inputted on x axis, hashed values on y axis
    vector<int> rands;
    vector<int> seedVals;
    vector<int> univVals;

    int n = 100;

    for(int i = 0; i < n; i++) {
        bloomFilter b(n, 10, 1);
        auto dist = uniform_int_distribution<int>(1, INT_MAX);
        int r = dist(gen);
        rands.push_back(r);
        seedVals.push_back(b.getSeed(0,r));
        univVals.push_back(b.getUniversal(0,r));
    }
    printVec("Numbers", rands);
    printVec("SeedHash", seedVals);
    printVec("UniversalHash", univVals);

}

void problem1IncreasingData() {
    //generate a bunch of random numbers
    //scatter plot with values inputted on x axis, hashed values on y axis
    vector<int> rands;
    vector<int> seedVals;
    vector<int> univVals;
    int n = 100;
    int k = 1;


    for(int i = 0; i < n; i++) {
        bloomFilter b(n, 10, k);
        rands.push_back(i);
        seedVals.push_back(b.getSeed(0, i));
        univVals.push_back(b.getUniversal(0, i));
    }
    printVec("Numbers", rands);
    printVec("SeedHash", seedVals);
    printVec("UniversalHash", univVals);

}

pair<float,float> trialv2(int kNum) {
    //run trial with c and k val, return false positive rate of this trial
    int numFalsePosSeed = 0;
    int numFalsePosUniv = 0;

    int truePosSeed = 0;
    int truePosUniv = 0;

    int totalSeedNeg = 0;
    int totalUnivNeg = 0;

    int n = 100;

    bloomFilter b(n, 10, kNum);

    unordered_set<int> rands;

    for(int i = 1; i < n; i++) {
        b.add(i);
    }

    for(int i = 1; i < 1000; i++) { //how many times is this supposed to be
        //auto dist = uniform_int_distribution<int>(0, INT_MAX);
        //int r = dist(gen);
        int r = i;
        if(b.seedContains(r)) {
            if (r >= n) {
                numFalsePosSeed++;
            }  else truePosSeed++;
        } else totalSeedNeg++;
        if(b.univContains(r)) {
            if (r >= n) {
                numFalsePosUniv++;
            } else truePosUniv++;
        } else totalUnivNeg++;
    }
    cout << "seed fp: " << numFalsePosSeed << ", tp: " << truePosSeed <<  ", neg: " << totalSeedNeg << endl;
    cout << "univ fp: " << numFalsePosUniv << ", tp: " << truePosUniv << ", neg: " << totalUnivNeg << endl;
    return pair<float, float> ( numFalsePosSeed / (float) totalSeedNeg, numFalsePosUniv / (float) totalUnivNeg );

}

pair<float,float> trial(int kNum, int c) {
    //run trial with c and k val, return false positive rate of this trial
    int numFalsePosSeed = 0;
    int numFalsePosUniv = 0;

    int truePosSeed = 0;
    int truePosUniv = 0;

    int totalSeedNeg = 0;
    int totalUnivNeg = 0;

    int n = 10000;

    bloomFilter b(n, c, kNum);

    unordered_set<int> rands;

    for(int i = 1; i < n; i++) {
        auto dist = uniform_int_distribution<int>(1, INT_MAX - 1);
        int r = dist(gen);
        rands.insert(r);
        b.add(r);
    }
//    b.printSeedMap();
//    cout << "__________________________________________________________________________________________________________________________________________________________" << endl;
//    b.printUniMap();

    for(int i = 1; i < 10000; i++) { //how many times?
        auto dist = uniform_int_distribution<int>(1, INT_MAX - 1);
        int r = dist(gen);
        //unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        //minstd_rand0 generator (seed);
        //int r = generator();
        if(b.seedContains(r)) {
            if (rands.find(r) == rands.end()) {
                numFalsePosSeed++;
                totalSeedNeg++;
            } else truePosSeed++;
        } else totalSeedNeg++;
        if(b.univContains(r)) {
            if (rands.find(r) == rands.end()) {
                numFalsePosUniv++;
                totalUnivNeg++;
            } else truePosUniv++;
        } else totalUnivNeg++;
    }
//    cout << "seed fp: " << numFalsePosSeed << ", tp: " << truePosSeed <<  ", neg: " << totalSeedNeg << endl;
//    cout << "univ fp: " << numFalsePosUniv << ", tp: " << truePosUniv << ", neg: " << totalUnivNeg << endl;

    return pair<float, float> ( numFalsePosSeed / (float) totalSeedNeg, numFalsePosUniv / (float) totalUnivNeg );

}

float findMedian(vector<float> v) {
    sort(v.begin(), v.end());
    return (v[v.size() / 2 - 1] + v[v.size() / 2]) / 2;
}

void falsePositive() {
    int c = 10;
    for (int i = 4; i <= 10; i++){
        vector<float> seedVec;
        vector<float> univVec;
        for (int j = 0; j < 10; j++) {
            pair<float, float> p = trial(i, c);
            seedVec.push_back(p.first);
            univVec.push_back(p.second);
        }
        float seedMed = findMedian(seedVec);
        float univMed = findMedian(univVec);
        cout << "c: " << c << ", k: " << i << ", Pseudorandom: " << seedMed << ", Universal: " << univMed << endl;
    }
}

void test() {
    int k = 4;
    int n = 100;
    int c = 10;
    bloomFilter b(n, c, k);
    for (int i = 0; i < 10; i++) {
        cout << b.getSeed(0, 10) << endl;
    }
}

int main() {
    trial(4, 10);
    //falsePositive();

    //testBloomFilter(10000);
    //problem1IncreasingData();
    //problem1RandData();
    return 0;
}
