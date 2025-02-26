/*
     T-result(Classifier 결과)와 V-result(Verify 결과)를 비교하여 정확도를 측정해주는 프로그램
*/

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <vector>
#include <omp.h>

using namespace std;

// Function to read file lines into a vector
vector<string> readFileLines(const string& filename) {
    ifstream file(filename);
    vector<string> lines;
    string line;

    while (getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

// Function to write differences to a file
void writeDifferencesToFile(const string& filename, const vector<string>& differences) {
    ofstream file(filename);
    for (const auto& diff : differences) {
        file << diff << '\n';
    }
}

int main() {
    
    vector<string> file1Lines = readFileLines("T-result");
    vector<string> file2Lines = readFileLines("V-result");

    if (file1Lines.size() != file2Lines.size()) {
        cerr << "Error: The two files do not have the same number of lines." << endl;
        return 1;
    }

    unordered_set<string> file1Set(file1Lines.begin(), file1Lines.end());
    vector<string> matches;
    vector<string> differences;

    matches.reserve(file2Lines.size());
    differences.reserve(file2Lines.size());

    #pragma omp parallel
    {
        vector<string> localMatches;
        vector<string> localDifferences;

        #pragma omp for nowait schedule(static)
        for (size_t i = 0; i < file2Lines.size(); ++i) {
            if (file1Set.find(file2Lines[i]) != file1Set.end()) {
                localMatches.push_back(file2Lines[i]);
            } else {
                localDifferences.push_back("File1: " + file1Lines[i] + "\tFile2: " + file2Lines[i]);
            }
        }

        #pragma omp critical
        {
            matches.insert(matches.end(), localMatches.begin(), localMatches.end());
            differences.insert(differences.end(), localDifferences.begin(), localDifferences.end());
        }
    }

    double matchPercentage = (static_cast<double>(matches.size()) / file1Lines.size()) * 100.0;

    cout << "Total lines: " << file1Lines.size() << endl;
    cout << "Matched lines: " << matches.size() << endl;
    cout << "Match percentage: " << matchPercentage << "%" << endl;

    writeDifferencesToFile("diff.txt", differences);

    return 0;
}

