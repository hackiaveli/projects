#include <iostream>
#include <fstream>
#include <string>     // std::string, std::stoi
#include <vector>
#include <iomanip>      // std::setprecision

using namespace std;

//col 1 is height
//col 2 is weight
//col 3 is wingspan
//col 4 is reach
//col 5 is no step vert
//col 6 is no step vert reach
//col 7 is running vert
//col 8 is running vert reach
//col 9 is draft or not


string clean(string str)
{
    string str2 = "'";
    std::size_t found = str.find(str2);
    string feetStr = str.substr(0,found);
    double feet = stoi(feetStr);
    // cout << feet << endl;

    string str3 = "\"";
    std::size_t found2 = str.find(str3);

    string inchStr = str.substr(found+1,found2-2);
    double inches = stod(inchStr);
    inches = inches/12.0;
    // cout << inches << endl;

    double total=feet+inches;
    double scale = 0.01;  // i.e. round to nearest one-hundreth
    total = (int)(total / scale) * scale;

    string ret = to_string(total);
    return ret;

  }

int main() {


std::vector<string> myvector;
 ifstream myReadFile;
 myReadFile.open("runVertReach.txt");
 string file;
 if (myReadFile.is_open()) {
 while (!myReadFile.eof()) {


    myReadFile >> file;
    myvector.push_back(file);
 }
}
myReadFile.close();
int si = myvector.size();

cout << si;
ofstream myfile;
myfile.open ("runVertReachClean.txt");

for (std::vector<string>::iterator it = myvector.begin() ; it != myvector.end(); ++it)
  {  
    // std::cout << *it << endl;
    string str = *it;
    string ret = clean(str);
    // cout << ret << endl;
    myfile << ret;
    myfile << endl;

  }


myfile.close();

 


return 0;
}






