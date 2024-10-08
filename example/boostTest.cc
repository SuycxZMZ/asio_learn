#include "boost/lexical_cast.hpp"
#include <iostream>
#include <string>
int main() {
  using namespace std;
  cout << "Enter your weight: ";
  float weight;
  cin >> weight;
  string gain = "A 10% increase raises ";
  string wt = boost::lexical_cast<string>(weight);
  gain = gain + wt + " to "; // string operator()
  weight = 1.1 * weight;
  gain = gain + boost::lexical_cast<string>(weight) + ".";
  cout << gain << endl;
  return 0;
}